#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "imgui.h"
#include "Shaders.h"
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
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include "Time.h"
#include "Collisions.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <glm/ext/matrix_transform.hpp>
#include "UIManager.h"
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include "MeshCollisionBox.h"
#include "SpriteRenderer.h"

GLFWwindow* window;

const char* defaultVertexSource = R"GLSL(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;


out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool billboard;

void main(){
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
	if(billboard){
		mat4 modelView = view * model;
		modelView[0][0] = 1.0;
		modelView[0][1] = 0.0;
		modelView[0][2] = 0.0;

		modelView[1][0] = 0.0;
		modelView[1][1] = 1.0;
		modelView[1][2] = 0.0;
		
		modelView[2][0] = 0.0;
		modelView[2][1] = 0.0;
		modelView[2][2] = 1.0;

		gl_Position = projection * modelView * vec4(position, 1.0);

	} else{
		gl_Position = projection * view * model * vec4(position, 1.0);
	}
}

)GLSL";

const char* defaultFragmentSource = R"GLSL(
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;


uniform bool hasTexture;
uniform sampler2D texture_diffuse1;

uniform vec3 lightColour;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform bool lightEffected;


void main(){
	vec4 colour = hasTexture ? texture(texture_diffuse1, TexCoords) : vec4(1.0, 1.0, 1.0, 1.0);

	if(colour.a < 0.1)
		discard;
	if(lightEffected){


		float ambientStrength = 0.1;
		vec3 ambient = ambientStrength * lightColour;

		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColour;

		vec3 result = (ambient + diffuse) * vec3(colour);
		FragColor = vec4(result, 1.0);
	} else {
		FragColor = colour;
	}


}
)GLSL";
bool inRuntime = false;
float runtimeStartTime = 0.0f;
SceneView sceneView;
GameView gameView;
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
		if (key == GLFW_KEY_F1) {
			std::vector<RayHit> outHits;
			std::cout << Raycast(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 10.0f, &outHits) << std::endl;
		}
		if (key == GLFW_KEY_F5) {
			std::string path;
			std::getline(std::cin, path);
			SceneLoader::LoadScene(path);
			for (auto& item : SceneManager::activeScene->hierachy) {
				std::cout << item->name << std::endl;
			}
		}
		if (key == GLFW_KEY_F3) {
			Scripting::OnRuntimeStart();
			inRuntime = true;
			Console::LogMessage("Runtime Started");
			runtimeStartTime = glfwGetTime();
		}

		if (key == GLFW_KEY_D && mods == GLFW_MOD_CONTROL && UIManager::selectedEntity > -1 ) {
			unsigned int newEntity = Engine::CreateEntity();
			for (auto& script : Engine::GetEntityScripts(UIManager::selectedEntity)) {
				Engine::AddScript(newEntity, script);
				Scripting::LoadEntityScript(newEntity, script);
			}
			for (auto& comp : Engine::GetEntityComponents(UIManager::selectedEntity)) {
				Engine::AddComponent(newEntity, comp);
			}
		}
		if (key == GLFW_KEY_DELETE && UIManager::selectedEntity > -1) {
			Engine::DestroyEntity(UIManager::selectedEntity);
			Scripting::OnEntityDestroyed(UIManager::selectedEntity);
			SceneManager::OnEntityDestroyed(UIManager::selectedEntity);
			UIManager::selectedEntity = -1;
		}
	}
}
void HandleMouseInput(GLFWwindow* window, double xpos, double ypos) {
	InputManager::mouse.MouseCallback(window, xpos, ypos);
	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);		
	if (UIManager::sceneViewActive)
		sceneView.sceneCam.ProcessCameraMouse();
	else if (inRuntime)
		gameView.view.camera->ProcessCameraMouse();
}

void SetupDefaultScene() {

	unsigned int light = CreateEntity();
	SceneManager::activeScene->items[light].name = "Light";
	Engine::AddComponent<Light>(light, Light());
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = Display::CreateWindow();

	glewInit();
	glfwSwapInterval(1);

	mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", "C:\\Program Files (x86)\\Mono\\etc");

	Scripting::Initialize();

	UIManager::Initialize();

	Engine::RegisterComponent<Transform>();
	Engine::RegisterComponent<MeshRenderer>();
	Engine::RegisterComponent<Camera>();
	Engine::RegisterComponent<MeshCollisionBox>();
	Engine::RegisterComponent<Physics>();
	Engine::RegisterComponent<SpriteRenderer>();
	Engine::RegisterComponent<Light>();

	ProjectManager::CreateNewProject("C:\\Users\\tombr\\OneDrive\\Desktop\\Downloads\\Test Hierachy");
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	Shaders::activeShader = Shader(defaultVertexSource, defaultFragmentSource);
	Shaders::activeShader.Use();
	SceneManager::AddScene("temp.ShbaScene");
	SceneManager::ChangeScene("temp.ShbaScene");
	SceneManager::activeScene->LoadSkybox();
	Engine::Start();
	glfwSetKeyCallback(window, ProcessInput);
	glfwSetCursorPosCallback(window, HandleMouseInput); 

	Display::ShowWindow();
	

	bool gameViewOpen;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetupDefaultScene();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		for (auto& source : Engine::FindComponentsInScene<Light>()) {
			auto& light = Engine::GetComponent<Light>(source);
			Shaders::activeShader.SetVec3("lightColour", light.colour);
			Shaders::activeShader.SetVec3("lightPos", light.transform->position);
		}

		Time::currentTime = glfwGetTime() - runtimeStartTime;
		Time::deltaTime = glfwGetTime() - Time::lastFrameTime;
		Time::lastFrameTime = glfwGetTime();


		glClearColor(color[0], color[1], color[2], color[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (UIManager::sceneViewActive) {
			sceneView.Update(inRuntime);
			glDepthFunc(GL_LEQUAL);
			SceneManager::activeScene->shader.Use();
			SceneManager::activeScene->shader.SetMat4("view", glm::mat4(glm::mat3(sceneView.sceneCam.GetViewMatrix())));
			SceneManager::activeScene->shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), sceneView.view.dimensions.x / sceneView.view.dimensions.y, 0.1f, 100.0f));
			SceneManager::activeScene->RenderSkybox();
			Shaders::activeShader.Use();
			glDepthFunc(GL_LESS);
		}
		else
		{
			gameView.Update(inRuntime);

			SceneManager::activeScene->shader.Use();
			SceneManager::activeScene->shader.SetMat4("view", glm::mat4(glm::mat3(gameView.view.camera->GetViewMatrix())));
			SceneManager::activeScene->shader.SetMat4("projection", glm::perspective(glm::radians(45.0f), gameView.view.dimensions.x / gameView.view.dimensions.y, 0.1f, 100.0f));
			SceneManager::activeScene->RenderSkybox();
			Shaders::activeShader.Use();
		}

		if(inRuntime)
			Scripting::Update();

		Engine::Update(inRuntime);
		if(inRuntime)
			Collisions::HandleCollision();
		
		UIManager::Update();

		glfwSwapBuffers(window);
	}
}