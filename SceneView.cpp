#include "SceneView.h"
#include "Engine.h"
#include "Time.h"
#include "Renderer.h"
#include "Light.h"

SceneView::SceneView() {
	sceneCam = Camera(&cameraTransform);
	view = View(glm::vec2(0.0f), glm::vec2(Display::width, Display::height), &this->sceneCam);
}
void SceneView::Update(bool inRuntime) {
	// view.Update(inRuntime);
	RenderSceneSprites();
	sceneCam.transform->position += sceneCam.forward * InputManager::MoveVert() * Time::deltaTime;
	sceneCam.transform->position += sceneCam.right * InputManager::MoveHorz() * Time::deltaTime;
	sceneCam.transform->position += sceneCam.worldUp * InputManager::MoveUpDown() * Time::deltaTime;
	sceneCam.UpdateCameraVectors();
}
void SceneView::RenderSceneSprites() {
	for (auto cam : Engine::FindComponentsInScene<Camera>()) {
		auto& camera = Engine::GetComponent<Camera>(cam);
		Renderer::ChangeShader(camera.icon.shader);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, camera.transform->position);
		Renderer::SetModel(model);
		camera.icon.Render();
	}
	for (auto ligh : Engine::FindComponentsInScene<Light>()) {
		auto& light = Engine::GetComponent<Light>(ligh);
		Renderer::ChangeShader(light.icon.shader);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, light.transform->position);
		Renderer::SetModel(model);
		light.icon.Render();
	}
}