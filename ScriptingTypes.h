#pragma once
#include <string>
#include <iostream>
#include <mono/metadata/object.h>
#include <unordered_map>

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
	Vector4
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
	std::unordered_map<std::string, Field> fields;

	MonoMethod* GetMethod(const std::string& name, int parameterCount) {
		return mono_class_get_method_from_name(monoClass, name.c_str(), parameterCount);
	}
};
struct ClassInstance {
	Class classData;
	MonoObject* instance;

	MonoMethod* startMethod;
	bool hasStart = false;
	MonoMethod* updateMethod;
	bool hasUpdate = false;

	ClassInstance() {}
	ClassInstance(Class klass, bool core = false) {
		this->classData = klass;
		if (!core) {
			startMethod = classData.GetMethod("Start", 0);
			hasStart = startMethod != nullptr;
			updateMethod = classData.GetMethod("Update", 0);
			hasUpdate = updateMethod != nullptr;
		}
	}
	void InvokeMethod(MonoMethod* method, int parameterCount, void** params = nullptr) {
		MonoObject* exception = nullptr;
		mono_runtime_invoke(method, instance, params, &exception);
	}



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