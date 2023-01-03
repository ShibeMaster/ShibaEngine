#pragma once
#include <glm/glm.hpp>
#include <string>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

class SerializationUtils {
public:
	static void SerializeVec3(const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void SerializeVec3(const char* name, const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void SerializeVec2(const glm::vec2& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static glm::vec2 DeserializeVec2(rapidjson::Value& obj);
	static glm::vec3 DeserializeVec3(rapidjson::Value& obj);
	static void SerializeUniformInformation(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json, const std::string& type, const std::string& name);
	static std::string ReadFileBinary(const std::string& path);
	static std::string ReadFile(const std::string& path);
};

