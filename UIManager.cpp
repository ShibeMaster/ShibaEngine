#include "UIManager.h"

int UIManager::selectedEntity = -1;
bool UIManager::sceneViewActive = true;
bool UIManager::sceneViewFrameOpen = true;
bool UIManager::gameViewFrameOpen = false;
std::string UIManager::addingScriptName = "NewScript";
glm::vec2 UIManager::viewportSize;