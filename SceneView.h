#pragma once
#include "View.h"
#include "Transform.h"
class SceneView {
public:
	View view;
	Transform cameraTransform;
	Camera sceneCam;

	SceneView();
	void Update();
	void RenderSceneSprites();
};