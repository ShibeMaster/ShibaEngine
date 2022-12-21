#include "EditorFrame.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Scripting.h"
#include "Engine.h"
class SceneHierachyFrame : public EditorFrame {
private:

	void CreateEntityDragAndDrop(SceneItem* item) {
		unsigned int entity;
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			selectedEntity = item->entity;
		}
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("DRAG_DROP_Entity", &item->entity, sizeof(unsigned int));
			ImGui::Text(item->name.c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_Entity")) {
				entity = *(unsigned int*)payload->Data;
				SceneManager::activeScene->MoveEntityToChild(entity, item->entity);
			}
			ImGui::EndDragDropTarget();
		}
	}
	void RenderSceneItem(SceneItem* item) {
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		if (item->children.size() > 0) {
			bool open = ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanFullWidth);
			CreateEntityDragAndDrop(item);

			for (auto& child : item->children)
				RenderSceneItem(child);
			ImGui::TreePop();
		}
		else {
			ImGui::TreeNodeEx(item->name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
			CreateEntityDragAndDrop(item);
		}
	}

public:
	unsigned int selectedEntity;

	void Render() {
		if (ImGui::Begin("Entities", &isOpen, ImGuiWindowFlags_NoCollapse)) {
			ImGui::TextUnformatted("Scene");

			if (ImGui::BeginTable("EntitiesTable", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody, ImVec2(ImGui::GetWindowSize().x - 15, -30))) {
				float textWidth = ImGui::CalcTextSize("A").x;
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
				ImGui::TableHeadersRow();
				for (auto& item : SceneManager::activeScene->hierachy) {
					RenderSceneItem(item);
				}
				ImGui::EndTable();
			}
			if (ImGui::Button("New Entity")) {
				CreateEntity();
			}
			ImGui::End();
		}
	}
	static unsigned int CreateEntity() {

		unsigned int newEnt = Engine::CreateEntity();
		Engine::AddComponent<Transform>(newEnt, Transform{});
		Scripting::OnEntityCreated(newEnt);
		SceneManager::activeScene->OnCreateEntity(newEnt);
		return newEnt;
	}
};