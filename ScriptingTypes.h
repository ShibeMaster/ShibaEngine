#pragma once
#include <string>
#include <mono/metadata/object.h>
#include <iostream>
#include <map>

enum class FieldType {
	None,
	Float,
	Double,
	Bool,
	Char,
	String,
	Byte,
	Short,
	Int,
	Long,
	UByte,
	UShort,
	UInt,
	ULong,
	Vector2,
	Vector3,
	Vector4,
	Instance
};

struct Field {
	std::string name;
	FieldType type;
	MonoClassField* classField;
};
struct Class {
	std::string name;
	std::string nameSpace;
	MonoClass* monoClass;
	std::map<std::string, Field> fields;

	MonoMethod* GetMethod(const std::string& name, int parameterCount);
};
struct ClassInstance {
	Class classData;
	MonoObject* instance;

	MonoMethod* startMethod;
	bool hasStart = false;
	MonoMethod* updateMethod;
	bool hasUpdate = false;

	ClassInstance() {}
	ClassInstance(Class klass, bool core = false);
	void InvokeMethod(MonoMethod* method, int parameterCount, void** params = nullptr);

	template<typename T>
	T GetFieldValue(const std::string& fieldName) {
		if (classData.fields.find(fieldName) == classData.fields.end())
			return T();

		T value;

		mono_field_get_value(instance, classData.fields[fieldName].classField, &value);

		return value;
	}

	template<typename T>
	void SetFieldValue(const std::string& fieldName, T value) {
		if (classData.fields.find(fieldName) == classData.fields.end()) {
			std::cout << "field does not exist" << std::endl;
			return;
		}

		mono_field_set_value(instance, classData.fields[fieldName].classField, &value);
	}

	template<typename T>
	void SetFieldValue(const std::string& fieldName, T* value) {
		if (classData.fields.find(fieldName) == classData.fields.end()) {
			std::cout << "field does not exist" << std::endl;
			return;
		}

		mono_field_set_value(instance, classData.fields[fieldName].classField, value);
	}

};
/// <summary>
/// A behaviour is going to be a static class that isn't attached to any entity, it'll also be able to have the update method called at a specified interval instead of being called every frame
/// It's pretty much going to be an alternative to components but mostly for "game manager" type systems.
/// </summary>
struct Behaviour {
	Class classData;
	float interval;
	float lastUpdateTime;
	bool hasInterval;

	MonoMethod* update;
	bool hasUpdate;

	bool ShouldUpdate();
};