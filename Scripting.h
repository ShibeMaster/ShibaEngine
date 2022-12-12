#pragma once
#include <string>
#include <iostream>
#include <Mono/jit/jit.h>
#include <Mono/metadata/assembly.h>
#include "Engine.h"
#include <fstream>
#include "Transform.h"
#include <rapidjson/prettywriter.h>
#include "InputManager.h"
#include <rapidjson/document.h>
#include <glm/glm.hpp>
#include <mono/metadata/attrdefs.h>
#include "ScriptingTypes.h"
#include <mono/metadata/debug-helpers.h>
#include <unordered_map>
#include "Console.h"
#include "ProjectItem.h"

struct Instance {
	unsigned int entity;
	ClassInstance instance;
	std::unordered_map<std::string, ClassInstance> scripts;
	std::unordered_map<std::string, ClassInstance> coreComponents;
};
struct ScriptingEngineData {
	MonoDomain* rootDomain;
	MonoDomain* appDomain;
	MonoAssembly* appAssembly;
	MonoAssembly* coreAssembly;

	std::unordered_map<std::string, Class> components;
	std::unordered_map<std::string, Class> coreComponentClasses;
	std::unordered_map<unsigned int, Instance> entities;
};
class Utils {
public:
	static char* ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
			return nullptr;

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
			return nullptr;

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}
	static FieldType MonoTypeToFieldType(MonoType* type) {
		std::string name = mono_type_get_name(type);
		// sorry for this shit im about to write
		if (name == "System.Single") return FieldType::Float;
		else if (name == "System.Double") return FieldType::Double;
		else if (name == "System.Bool") return FieldType::Bool;
		else if (name == "System.Char") return FieldType::Char;
		else if (name == "System.String") return FieldType::String;
		else if (name == "System.Byte") return FieldType::Byte;
		else if (name == "System.Int16") return FieldType::Short;
		else if (name == "System.Int32") return FieldType::Int;
		else if (name == "System.Int64") return FieldType::Long;
		else if (name == "System.UByte") return FieldType::UByte;
		else if (name == "System.UInt16") return FieldType::UShort;
		else if (name == "System.UInt32") return FieldType::UInt;
		else if (name == "System.UInt64") return FieldType::ULong;
		else if (name == "System.Numerics.Vector2") return FieldType::Vector2;
		else if (name == "System.Numerics.Vector3") return FieldType::Vector3;
		else if (name == "System.Numerics.Vector4") return FieldType::Vector4;
		else if (name == "ShibaEngineCore.Instance") return FieldType::Instance;
	}
};
class Scripting {
public:
	static ScriptingEngineData data;
	static Class componentClass;
	static Class timeClass;
	static Class coreComponentClass;
	static Class instanceClass;
	static void Initialize() {
		MonoDomain* rootDomain = mono_jit_init("ShibaEngineRuntime");
		if (rootDomain == nullptr) {
			std::cout << "failed to create rootdomain" << std::endl;
			return;
		}
		data.rootDomain = rootDomain;
		data.appDomain = mono_domain_create_appdomain((char*)"ShibaEngine", nullptr);
		mono_domain_set(data.appDomain, true);

		data.coreAssembly = LoadAssembly("C:\\Users\\tombr\\source\\repos\\Shiba Engine\\ShibaEngineCore\\bin\\Debug\\net5.0\\ShibaEngineCore.dll");
		componentClass = Class{ "Component", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Component") };
		SetupClassFields(&componentClass, componentClass.monoClass, true);
		timeClass = Class{ "Time", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Time") };
		coreComponentClass = Class{ "CoreComponent", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "CoreComponent") };
		SetupClassFields(&coreComponentClass, coreComponentClass.monoClass, true);
		instanceClass = Class{ "Instance", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Instance") };
		SetupClassFields(&instanceClass, instanceClass.monoClass, true);
		data.appAssembly = LoadAssembly("C:\\Users\\tombr\\source\\repos\\Shiba Engine\\ShibaEngineCore\\bin\\Debug\\net5.0\\ShibaEngineCore.dll");
		LoadAssemblyClasses();

		mono_add_internal_call("ShibaEngineCore.EngineCalls::AddComponent", AddComponent);
		mono_add_internal_call("ShibaEngineCore.EngineCalls::CreateEntity", CreateEntity);
		mono_add_internal_call("ShibaEngineCore.EngineCalls::PrintMessage", PrintMessage);
		mono_add_internal_call("ShibaEngineCore.EngineCalls::PrintError", PrintError);
		mono_add_internal_call("ShibaEngineCore.Input::GetKeyDown", GetKeyDown);
		mono_add_internal_call("ShibaEngineCore.EngineCalls::FindComponentsInScene", FindComponentsInScene);
		mono_add_internal_call("ShibaEngineCore.Components::UpdateExternCoreComponent", UpdateExternCoreComponent);
		mono_add_internal_call("ShibaEngineCore.Components::UpdateCoreComponent", UpdateCoreComponent);
		mono_add_internal_call("ShibaEngineCore.Components::GetCoreComponent", GetCoreComponent);
		mono_add_internal_call("ShibaEngineCore.Instance::GetName", GetName);
		mono_add_internal_call("ShibaEngineCore.Components::GetEntityInstance", GetInstance);


	}

#pragma region Internal Calls 
	static MonoString* GetName(unsigned int entity) {
		return mono_string_new(data.appDomain, SceneManager::activeScene->items[entity].name.c_str());
	}
	static MonoObject* GetInstance(unsigned int entity) {
		return data.entities[entity].instance.instance;
	}
	static void AddComponent(unsigned int entity, MonoString* name, MonoObject* monoObject) {
		std::string compName = mono_string_to_utf8(name);
		Engine::AddScript(entity, compName);
		data.entities[entity].scripts[compName] = CreateClassInstanceFromMonoObject(compName, monoObject);
	}
	static void UpdateExternCoreComponent(unsigned int entity, MonoString* compName, MonoObject* monoObject) {
		std::string name = mono_string_to_utf8(compName);
		if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
			std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
			return;
		}
		data.entities[entity].coreComponents[name].instance = monoObject;
		Engine::SetCoreComponent(entity, name, &data.entities[entity].coreComponents[name]);
	}
	static void UpdateCoreComponent(unsigned int entity, MonoString* monoName) {
		std::string name = mono_string_to_utf8(monoName);
		if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
			std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
			return;
		}
		Engine::GetCoreComponentObject(entity, name, &data.entities[entity].coreComponents[name]);
	}
	static MonoObject* GetCoreComponent(unsigned int entity, MonoString* monoName) {
		std::string name = mono_string_to_utf8(monoName);
		if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
			std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
			return nullptr;

		}
		Engine::GetCoreComponentObject(entity, name, &data.entities[entity].coreComponents[name]);
		return data.entities[entity].coreComponents[name].instance;
	}
	static bool GetKeyDown(int key) {
		return InputManager::GetKeyDown(key);
	}
	static MonoArray* FindComponentsInScene(MonoString* name) {
		std::vector<unsigned int> entities = Engine::FindScriptInScene(mono_string_to_utf8(name));
		MonoArray* arr = mono_array_new(mono_get_root_domain(), mono_get_uint32_class(), entities.size());
		for (int i = 0; i < entities.size(); i++)
		{
			mono_array_set(arr, uint32_t, i, entities[i]);
		}
		return arr;
	}
	static MonoObject* GetComponent(unsigned int entity, MonoString* name) {
		return data.entities[entity].scripts[mono_string_to_utf8(name)].instance;
	}
	static unsigned int CreateEntity() {
		return Engine::CreateEntity();
	}

	static void PrintMessage(MonoString* message) {
		Console::LogMessage(mono_string_to_utf8(message));
	}
	static void PrintError(MonoString* error) {
		Console::LogError(mono_string_to_utf8(error));
	}

#pragma endregion 

	static void OnAddComponent(unsigned int entity, const std::string& name) {
		if (data.coreComponentClasses.find(name) == data.coreComponentClasses.end()) {
			std::cout << "class does not exist" << std::endl;
			return;
		}
		auto instance = ClassInstance(data.coreComponentClasses[name], true);
		instance.instance = Instantiate(instance.classData.monoClass);
		instance.SetFieldValue<unsigned int>("entity", entity);
		data.entities[entity].coreComponents[name] = instance;

	}

	static void OnEntityDestroyed(unsigned int entity) {
		if (data.entities.find(entity) != data.entities.end())
			data.entities.erase(entity);
	}
	static void OnEntityCreated(unsigned int entity) {
		data.entities[entity] = { entity, CreateClassInstance(entity, instanceClass) };
		OnAddComponent(entity, "Transform");
	}
	static MonoAssembly* LoadAssembly(const std::string& path) {
		uint32_t size = 0;
		char* fileData = Utils::ReadBytes(path, &size);
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, size, 1, &status, 0);
		if (status != MONO_IMAGE_OK) {
			std::cout << mono_image_strerror(status) << std::endl;
			return nullptr;
		}
		MonoAssembly* assembly = mono_assembly_load_from_full(image, path.c_str(), &status, 0);
		mono_image_close(image);
		delete[] fileData;
		return assembly;
	}
	static ClassInstance CreateClassInstanceFromMonoObject(const std::string& name, MonoObject* monoObject) {
		ClassInstance instance = ClassInstance(data.components[name]);
		instance.instance = monoObject;
		return instance;
	}
	static MonoClass* GetClass(MonoAssembly* assembly, const char* nameSpace, const char* name) {
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, nameSpace, name);
		if (klass == nullptr) {
			std::cout << "failed to get class" << std::endl;
			return nullptr;
		}

		return klass;
	}
	static void SetupClassFields(Class* klass, MonoClass* classData, bool getParents = false, bool getPrivate = false) {
		int fieldCount = mono_class_num_fields(classData);
		void* iterator = nullptr;
		while (MonoClassField* field = mono_class_get_fields(classData, &iterator)) {
			const char* fieldName = mono_field_get_name(field);
			uint32_t flags = mono_field_get_flags(field);
			
			if (flags & MONO_FIELD_ATTR_PUBLIC || getPrivate) {
				MonoType* monoType = mono_field_get_type(field);
				FieldType type = Utils::MonoTypeToFieldType(monoType);
				klass->fields[std::string(fieldName)] = Field{ std::string(fieldName), type, field };
			}
		}
		if (mono_class_get_parent(classData) != nullptr && getParents)
			SetupClassFields(klass, mono_class_get_parent(classData), getPrivate);
	}
	static ClassInstance CreateClassInstance(unsigned int entity, Class klass) {
		ClassInstance instance = ClassInstance(klass);
		instance.instance = Instantiate(klass.monoClass);
		MonoClassField* entityField = mono_class_get_field_from_name(klass.monoClass, "entity");
		if (entityField == nullptr)
			std::cout << "failed to find entity field" << std::endl;
		mono_field_set_value(instance.instance, entityField, &entity);
		return instance;
	}
	static void LoadAssemblyClasses() {
		data.components.clear();
		MonoImage* image = mono_assembly_get_image(data.appAssembly);
		const MonoTableInfo* typeDefTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefTable);
		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefTable, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			std::string fullname;
			if (strlen(nameSpace))
				fullname = nameSpace + std::string(".") + name;
			else
				fullname = name;

			std::cout << fullname << std::endl;
			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			if (monoClass == componentClass.monoClass || monoClass == coreComponentClass.monoClass)
				continue;
			
			bool isComponent = mono_class_is_subclass_of(monoClass, componentClass.monoClass, false);
			if (isComponent) {

				Class klass = Class{ name, nameSpace, monoClass };

				SetupClassFields(&klass, klass.monoClass);
				data.components[fullname] = klass;
			}
			else if (mono_class_is_subclass_of(monoClass, coreComponentClass.monoClass, false)) {
				Class klass = Class{ name, nameSpace, monoClass };
				SetupClassFields(&klass, klass.monoClass, true, true);
				data.coreComponentClasses[name] = klass;
			}
			else
				continue;


		}
	}
	static std::unordered_map<std::string, ClassInstance>& GetEntityComponentInstances(unsigned int entity) {
		return data.entities[entity].scripts;
	}
	static void ReloadEntityScripts(unsigned int entity) {
		data.entities[entity].scripts.clear();
		auto scripts = Engine::GetEntityScripts(entity);
		for (auto script : scripts) {
			if (data.components.find(script) != data.components.end())
				data.entities[entity].scripts[script] = CreateClassInstance(entity, data.components[script]);
		}
	}
	static void RemoveScriptInstance(unsigned int entity, const std::string& script) {
		if (data.entities[entity].scripts.find(script) != data.entities[entity].scripts.end())
			data.entities[entity].scripts.erase(script);
	}
	static void LoadEntityScript(unsigned int entity, const std::string& script) {

		if (data.components.find(script) != data.components.end()) {
			data.entities[entity].scripts[script] = CreateClassInstance(entity, data.components[script]);
			data.entities[entity].scripts[script].InvokeMethod(componentClass.GetMethod("Initialize", 0), 0);
		}
	}
	static void OnRuntimeStart() {
		for (auto& comp : data.entities) {
			for (auto& instance : comp.second.scripts) {
				instance.second.InvokeMethod(instance.second.startMethod, 0);
			}
		}
	}
	static std::string GetNameFromInstance(MonoObject* instance) {
		unsigned int entity;
		mono_field_get_value(instance, Scripting::instanceClass.fields["entity"].classField, &entity);
		return SceneManager::activeScene->items[entity].name;
	}
	static void SerializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		json->Key(field.name.c_str());
		// sorry for this
		switch (field.type)
		{
		case FieldType::Float: json->Double(instance.GetFieldValue<float>(field.name)); break;
		case FieldType::Double: json->Double(instance.GetFieldValue<double>(field.name)); break;
		case FieldType::Bool: json->Bool(instance.GetFieldValue<bool>(field.name)); break;
		case FieldType::UInt: json->Int(instance.GetFieldValue<unsigned int>(field.name)); break;
		case FieldType::Vector2: SerializationUtils::SerializeVec2(instance.GetFieldValue<glm::vec2>(field.name), json); break;
		case FieldType::Vector3: SerializationUtils::SerializeVec3(instance.GetFieldValue<glm::vec3>(field.name), json); break;
		case FieldType::Int: json->Int(instance.GetFieldValue<unsigned int>(field.name)); break;
		case FieldType::Instance: json->String(GetNameFromInstance(instance.GetFieldValue<MonoObject*>(field.name)).c_str()); break;
		default: break;
		}
	}
	static void SerializeEntityScripts(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
		for (auto& instance : data.entities[entity].scripts) {
			json->Key(instance.second.classData.name.c_str());
			json->StartObject();
			for (auto& field : instance.second.classData.fields) {
				SerializeEntityScriptField(entity, instance.second, field.second, json);
			}
			json->EndObject();
		}
	}
	static void DeserializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::Value& obj) {
		switch (field.type)
		{
		case FieldType::Float: instance.SetFieldValue<float>(field.name, obj.GetDouble()); break;
		case FieldType::Double: instance.SetFieldValue<double>(field.name, obj.GetDouble()); break;
		case FieldType::Bool: instance.SetFieldValue<bool>(field.name, obj.GetBool()); break;
		case FieldType::UInt: instance.SetFieldValue<unsigned int>(field.name, obj.GetInt()); break;
		case FieldType::Vector2: instance.SetFieldValue<glm::vec2>(field.name, SerializationUtils::DeserializeVec2(obj)); break;
		case FieldType::Vector3: instance.SetFieldValue<glm::vec3>(field.name, SerializationUtils::DeserializeVec3(obj)); break;
		case FieldType::Int: instance.SetFieldValue<int>(field.name, obj.GetInt()); break;
		default: break;
		}
	}
	static void DeserializeEntityScripts(unsigned int entity, rapidjson::Value& obj) {
		
		for (auto& script : data.components) {
			if (obj.HasMember(script.first.c_str())) {
				Engine::AddScript(entity, script.first);
				Scripting::LoadEntityScript(entity, script.first);
				for (auto& field : script.second.fields) {
					if(obj[script.first.c_str()].HasMember(field.first.c_str()))
						DeserializeEntityScriptField(entity, data.entities[entity].scripts[script.first], field.second, obj[script.first.c_str()][field.first.c_str()]);

				}
			}
		}
	}
	static void Update() {

		void* params[] = {
			&Time::deltaTime,
			&Time::currentTime
		};
		MonoObject* exception = nullptr;
		mono_runtime_invoke(timeClass.GetMethod("UpdateTime", 2), nullptr, params, &exception);
		for (auto& comp : data.entities) {
			for (auto& instance : comp.second.scripts) {
				instance.second.InvokeMethod(instance.second.updateMethod, 0);
			}
		}
	}
	static MonoObject* Instantiate(MonoClass* klass) {
		MonoObject* instance = mono_object_new(data.appDomain, klass);
		if (instance == nullptr) {
			std::cout << "failed to instantiate class" << std::endl;
			return nullptr;
		}
		mono_runtime_object_init(instance);
		return instance;
	}
};