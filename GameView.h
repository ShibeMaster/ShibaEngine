#pragma once
#include "View.h"
#include "Display.h"
#include "Camera.h"
#include "Time.h"
#include "Engine.h"
class GameView {
public:
	View view = View(glm::vec2(0.0f), glm::vec2(Display::width, Display::height));

	void Update() {
		if (!view.hasCamera) {
			auto cams = Engine::FindComponentsInScene<Camera>();
			if (cams.size() > 0) {
				view.camera = Engine::GetComponentPointer<Camera>(cams[0]);
				view.hasCamera = true;

			}

		}
	}
};