#pragma once
#include <glm/glm.hpp>
#include <string>
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


};

