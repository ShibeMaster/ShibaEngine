#pragma once
#include <queue>
#include <string>
#include "imgui.h"
#include "EngineTime.h"
#include "Display.h"
#include <iostream>
#define MAX_MESSAGES 10

struct ConsoleMessage {
	std::string message;
	std::string type;
	ImVec4 color;
	float time;
};
class Console {
private:
	static std::deque<ConsoleMessage> messages;
	static void AddMessage(const std::string& message, const std::string& type, ImVec4 color) {
		messages.push_back({ message, type, color, Time::currentTime });
		if (messages.size() > MAX_MESSAGES)
			messages.pop_front();
	}
public:
	static bool isOpen;

	static void LogMessage(const std::string& message) {
		AddMessage(message, "MSG", ImVec4(1, 1, 1, 1));
	}
	static void LogError(const std::string& message) {
		AddMessage(message, "ERR", ImVec4(1, 0, 0, 1));
	}
	static void Clear() {
		messages.clear();
	}
	static void Render() {
		ImGui::Begin("Console", &isOpen, ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginChild("ScrollingConsole", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar)) {

			for (auto message : messages) {
				ImGui::Text("[%i]", (int)message.time); 
				ImGui::SameLine();
				ImGui::TextColored(message.color, "[%s]", message.type.c_str());
				ImGui::SameLine();
				ImGui::TextColored(message.color, message.message.c_str());
			}
			if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}
};