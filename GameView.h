#pragma once
#include "View.h"
#include "Display.h"
#include "Camera.h"
#include "Time.h"
#include "Engine.h"
class GameView {
public:
	View view = View(glm::vec2(Display::width * 0.3f, Display::height * 0.25f), glm::vec2(Display::width * 0.45f, Display::height * 0.7f));

	void Update(bool inRuntime) {
		if (!view.hasCamera) {
			auto cams = Engine::FindComponentsInScene<Camera>();
			if (cams.size() > 0) {
				view.camera = Engine::GetComponentPointer<Camera>(cams[0]);
				view.hasCamera = true;

			}

		}
		// view.Update(inRuntime);
		if (inRuntime && view.hasCamera) {
			view.camera->transform->position += view.camera->forward * InputManager::MoveVert() * Time::deltaTime;
			view.camera->transform->position += view.camera->right * InputManager::MoveHorz() * Time::deltaTime;
			view.camera->transform->position += view.camera->worldUp * InputManager::MoveUpDown() * Time::deltaTime;
			view.camera->UpdateCameraVectors();
		}
	}
};