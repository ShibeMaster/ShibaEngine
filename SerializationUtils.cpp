#include "SerializationUtils.h"
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
void SerializationUtils::SerializeVec3(const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	json->StartArray();
	json->Double(val.x);
	json->Double(val.y);
	json->Double(val.z);
	json->EndArray();
}

void SerializationUtils::SerializeVec3(const char* name, const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	json->Key(name);
	SerializationUtils::SerializeVec3(val, json);
}
void SerializationUtils::SerializeVec2(const glm::vec2& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	json->StartArray();
	json->Double(val.x);
	json->Double(val.y);
	json->EndArray();
}
glm::vec2 SerializationUtils::DeserializeVec2(rapidjson::Value& obj) {
	glm::vec2 val;
	auto arr = obj.GetArray();
	val.x = (float)arr[0].GetDouble();
	val.y = (float)arr[1].GetDouble();
	return val;
}
glm::vec3 SerializationUtils::DeserializeVec3(rapidjson::Value& obj) {
	glm::vec3 val;
	auto arr = obj.GetArray();
	val.x = (float)arr[0].GetDouble();
	val.y = (float)arr[1].GetDouble();
	val.z = (float)arr[2].GetDouble();
	return val;
}
 std::string SerializationUtils::ReadFile(const std::string& path) {
	std::ifstream file(path);
	std::cout << path << std::endl;
	std::string data;
	if (file) {
		std::ostringstream ss;
		ss << file.rdbuf();
		data = ss.str();
	}
	file.close();
	return data;
}