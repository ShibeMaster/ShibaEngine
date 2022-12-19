#include "UIManager.h"

int UIManager::selectedEntity = -1;
bool UIManager::sceneViewActive = true;
bool UIManager::sceneViewFrameOpen = true;
bool UIManager::gameViewFrameOpen = false;
glm::vec2 UIManager::viewportSize;