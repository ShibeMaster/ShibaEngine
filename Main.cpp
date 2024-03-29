#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "InputManager.h"
#include "Scene.h"
#include <string>
#include "ProjectManager.h"
#include "Engine.h"
#include "SceneLoader.h"
#include "imgui_stdlib.h"
#include "Physics.h"
#include "BoundingBox.h"
#include "Primitives.h"
#include "Raycast.h"
#include "GameView.h"
#include "SceneView.h"
#include "Console.h"
#include "ScreenLayer.h"
#include "Scripting.h"
#include "Camera.h"
#include <Mono/jit/jit.h>
#include "GUIExtensions.h"
#include "Project.h"
#include <Mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include "Mesh.h"
#include "Display.h"
#include "View.h"
#include "CameraController.h"
#include "FrameBuffer.h"
#include "ViewManager.h"
#include <GLFW/glfw3.h>
#include "EngineTime.h"
#include "Collisions.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include <mono/metadata/mono-config.h>
#include "UIManager.h"
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "MeshCollisionBox.h"
#include "SpriteRenderer.h"
#include "RuntimeManager.h"

GLFWwindow* window;
int clipboardEntity = -1;

unsigned int CreateEntity() {

	unsigned int newEnt = Engine::CreateEntity();
	Engine::AddComponent<Transform>(newEnt, Transform{});
	Scripting::OnEntityCreated(newEnt);
	SceneManager::activeScene->OnCreateEntity(newEnt);
	return newEnt;
}
void ProcessInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, true);
		if (key == GLFW_KEY_TAB) {
			if (InputManager::mouse.inputLocked)
				InputManager::SetMouseUnlocked();
			else
				InputManager::SetMouseLocked();
		}
		if (key == GLFW_KEY_D && mods == GLFW_MOD_CONTROL && UIManager::sceneFrame.selectedEntity > -1 ) {
			unsigned int newEntity = Engine::CreateEntity();
			for (auto& script : Engine::GetEntityScripts(UIManager::sceneFrame.selectedEntity)) {
				Engine::AddScript(newEntity, script);
				Scripting::LoadEntityScript(newEntity, script);
			}
			for (auto& comp : Engine::GetEntityComponents(UIManager::sceneFrame.selectedEntity)) {
				Engine::AddComponent(newEntity, comp);
			}
		}
		// debug instantiation
		if (key == GLFW_KEY_F1) {
			Scripting::InstantiateEntity(1);
		}
		if (key == GLFW_KEY_DELETE && UIManager::sceneFrame.selectedEntity > -1) {
			Engine::DestroyEntity(UIManager::sceneFrame.selectedEntity);
			Scripting::OnEntityDestroyed(UIManager::sceneFrame.selectedEntity);
			SceneManager::OnEntityDestroyed(UIManager::sceneFrame.selectedEntity);
			UIManager::sceneFrame.selectedEntity = -1;
		}
	}
}
void HandleMouseInput(GLFWwindow* window, double xpos, double ypos) {
	InputManager::mouse.MouseCallback(window, xpos, ypos);
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);		
	if (UIManager::viewportFrame.sceneViewFrameOpen)
		ViewManager::sceneView.sceneCam.ProcessCameraMouse();
	if (RuntimeManager::inRuntime && !RuntimeManager::isPaused) {
		for (auto& entity : Engine::FindComponentsInScene<CameraController>()) {
			Engine::GetComponent<CameraController>(entity).ProcessMouse();
		}
	}
}

void SetupDefaultScene() {

	unsigned int light = CreateEntity();
	SceneManager::activeScene->items[light].name = "Light";
	Engine::AddComponent<Light>(light, Light());
}

void RenderScene() {
	Engine::Render();
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = Display::Create();
	glewInit();
	glfwSwapInterval(1);

	mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", "C:\\Program Files (x86)\\Mono\\etc"); 
	mono_config_parse(NULL);

	Scripting::Initialize("");

	UIManager::Initialize();

	Engine::RegisterComponent<Transform>();
	Engine::RegisterComponent<MeshRenderer>();
	Engine::RegisterComponent<Camera>();
	Engine::RegisterComponent<MeshCollisionBox>();
	Engine::RegisterComponent<Physics>();
	Engine::RegisterComponent<SpriteRenderer>();
	Engine::RegisterComponent<Light>();
	Engine::RegisterComponent<CameraController>();
	Engine::RegisterComponent<ScreenLayer>();
	ShaderManager::LoadDefaultShaders();

	ProjectManager::CreateNewProject("C:\\Users\\tombr\\OneDrive\\Desktop\\Downloads\\Test Hierachy\\");
	SceneManager::AddScene("temp.ShbaScene");
	SceneManager::ChangeScene("temp.ShbaScene");
	SceneManager::activeScene->LoadSkybox();
	ViewManager::sceneView.view.framebuffer.Generate();
	ViewManager::gameView.view.framebuffer.Generate();
	SetupDefaultScene();


	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput); 
	glfwSetWindowSizeCallback(window, Display::HandleWindowResize);
	Display::ShowWindow();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		for (auto& source : Engine::FindComponentsInScene<Light>()) {
			auto& light = Engine::GetComponent<Light>(source);
			Renderer::shaderData.lightColour = light.colour;
			Renderer::shaderData.lightPos = light.transform->position;
		}

		Time::currentTime = glfwGetTime() - RuntimeManager::runtimeStartTime;
		Time::deltaTime = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (ProjectManager::projectLoaded) {
			if (UIManager::viewportFrame.sceneViewFrameOpen) {
				Renderer::shaderData.view = ViewManager::sceneView.sceneCam.GetViewMatrix();
				Renderer::shaderData.viewPos = ViewManager::sceneView.sceneCam.transform->position;
				Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), ViewManager::sceneView.view.framebuffer.dimensions.x / ViewManager::sceneView.view.framebuffer.dimensions.y, 0.1f, 100.0f);
				ViewManager::sceneView.view.framebuffer.Bind();

				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ViewManager::sceneView.Update();
				Renderer::ChangeShader("ShibaEngine_Skybox");
				glDepthFunc(GL_LEQUAL);
				SceneManager::activeScene->RenderSkybox();
				glDepthFunc(GL_LESS);
				RenderScene();
				ViewManager::sceneView.view.framebuffer.Unbind();
			}
			if (UIManager::viewportFrame.gameViewFrameOpen) {
				ViewManager::gameView.Update();
				if (ViewManager::gameView.view.hasCamera) {
					Renderer::shaderData.view = ViewManager::gameView.view.camera->GetViewMatrix();
					Renderer::shaderData.viewPos = ViewManager::gameView.view.camera->transform->position;
					Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), (ViewManager::gameView.view.framebuffer.dimensions.x / ViewManager::gameView.view.framebuffer.dimensions.y), 0.1f, 100.0f);
					ViewManager::gameView.view.framebuffer.Bind();
					glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					Renderer::ChangeShader("ShibaEngine_Skybox");
					glDepthFunc(GL_LEQUAL);
					SceneManager::activeScene->RenderSkybox();
					glDepthFunc(GL_LESS);
					RenderScene();
					ViewManager::gameView.view.framebuffer.Unbind();
				}
			}

			if (RuntimeManager::inRuntime && !RuntimeManager::isPaused)
				Collisions::HandleCollision();

			if (RuntimeManager::inRuntime && !RuntimeManager::isPaused) {
				Scripting::Update();
				Engine::Update();
			}

		}
		UIManager::Update();


		glfwSwapBuffers(window);
	}
}