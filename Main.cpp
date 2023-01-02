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
#include "Time.h"
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

// This is whether the application is going to be running the game or being used as an engine
#define SHIBAENGINE_RUNTIME 1

// I'm basically just gonna write out an explanation of the idea that I'm going to be using for compiling and running games
// The way that I'm going to be handling this in the engine certainly isn't the best or safest in any way, but I'm just trying to do it including a compiler in the engine
// The engine is going to need to be installed twice, one with this variable as 1 and another with 0 (it definitely would work better with using seperate project but theres so much interlinking shit in here that I'm not bothered trying to seperate all the files need for runtime from the engine)
// When a game is compiled from the engine, it'll create a zip file which includes the project information, all the assets in the same layout as in the project directory (probably should try to encrypt them later lol), the project assembly with a specific name and finally a copy of the engine with the runtime variable enabled
// If the runtime preprocessor definiton is enabled, the engine is expecting that all of these things are in the correct places
// The runtime application will not show anything of the actual engine and pretty much just run the game normally using all of the existing data
// probably explained this pretty poorly, but I hope anyone reading this will get the basic idea

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

#if SHIBAENGINE_RUNTIME
	ProjectManager::LoadProject("GameData\\Project.SHBAPROJ");
	SceneLoader::LoadScene(ProjectManager::activeProject.settings.startingScenePath);
	RuntimeManager::StartRuntime(true);
#else 
	ProjectManager::CreateNewProject("C:\\Users\\tombr\\OneDrive\\Desktop\\Downloads\\Test Hierachy\\");
	SceneManager::AddScene("temp.ShbaScene");
	SceneManager::ChangeScene("temp.ShbaScene");
	SceneManager::activeScene->LoadSkybox();
	ViewManager::sceneView.view.framebuffer.Generate();
	ViewManager::gameView.view.framebuffer.Generate();
#endif

	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput); 
	glfwSetWindowSizeCallback(window, Display::HandleWindowResize);
	Display::ShowWindow();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetupDefaultScene();
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
#if SHIBAENGINE_RUNTIME
		ViewManager::gameView.Update();
		if (ViewManager::gameView.view.hasCamera) {
			Renderer::shaderData.view = ViewManager::gameView.view.camera->GetViewMatrix();
			Renderer::shaderData.viewPos = ViewManager::gameView.view.camera->transform->position;
			Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), ViewManager::gameView.view.framebuffer.dimensions.x / ViewManager::gameView.view.framebuffer.dimensions.y, 0.1f, 100.0f);
			Renderer::ChangeShader("ShibaEngine_Skybox");
			glDepthFunc(GL_LEQUAL);
			SceneManager::activeScene->RenderSkybox();
			glDepthFunc(GL_LESS);
			RenderScene();
		}
		if (RuntimeManager::inRuntime && !RuntimeManager::isPaused)
			Collisions::HandleCollision();

		if (RuntimeManager::inRuntime && !RuntimeManager::isPaused) {
			Scripting::Update();
			Engine::Update();
		}
#else
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
					Renderer::shaderData.projection = glm::perspective(glm::radians(45.0f), ViewManager::gameView.view.framebuffer.dimensions.x / ViewManager::gameView.view.framebuffer.dimensions.y, 0.1f, 100.0f);
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

#endif

		glfwSwapBuffers(window);
	}
}