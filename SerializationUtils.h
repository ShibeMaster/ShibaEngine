#pragma once
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/stringbuffer.h>

class SerializationUtils {
public:
	static void SerializeVec3(const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->StartArray();
		json->Double(val.x);
		json->Double(val.y);
		json->Double(val.z);
		json->EndArray();
	}

	static void SerializeVec3(const char* name, const glm::vec3& val, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key(name);
		SerializationUtils::SerializeVec3(val, json);
	}
	static glm::vec3 DeserializeVec3(rapidjson::Value& obj) {
		glm::vec3 val;
		auto arr = obj.GetArray();
		val.x = arr[0].GetDouble();
		val.y = arr[1].GetDouble();
		val.z = arr[2].GetDouble();
		return val;
	}
	static std::string ReadFile(const std::string& path) {
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


};

