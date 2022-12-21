#include "UIManager.h"

int UIManager::selectedEntity = -1;
bool UIManager::sceneViewActive = true;
bool UIManager::sceneViewFrameOpen = true;
bool UIManager::gameViewFrameOpen = false;
std::string UIManager::addingScriptName = "NewScript";
float UIManager::addingBehaviourInterval = 0.0f;
bool UIManager::isAddingBehaviour = false;
glm::vec2 UIManager::viewportSize;