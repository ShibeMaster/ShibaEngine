#include "Scripting.h"
#include <iostream>
#include <Mono/jit/jit.h>
#include <Mono/metadata/assembly.h>
#include "Engine.h"
#include <fstream>
#include <memory>
#include "Transform.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "ScriptingTypes.h"
#include <glm/glm.hpp>
#include <mono/metadata/debug-helpers.h>
#include "SerializationUtils.h"
#include "SceneLoader.h"

#include <mono/metadata/attrdefs.h>
#include "FileExtensions.h"
#include "Console.h"
#include <mono/metadata/mono-config.h>



ScriptingEngineData Scripting::data;
Class Scripting::componentClass;
Class Scripting::timeClass;
Class Scripting::coreComponentClass;
Class Scripting::instanceClass;
Class Scripting::behaviourManager;
Class Scripting::mouseClass;


#pragma region Utils

char* Utils::ReadBytes(const std::string& filepath, uint32_t* outSize)
{
	std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

	if (!stream)
		return nullptr;

	std::streampos end = stream.tellg();
	stream.seekg(0, std::ios::beg);
	auto size = end - stream.tellg();

	if (size == 0)
		return nullptr;

	char* buffer = new char[size];
	stream.read((char*)buffer, size);
	stream.close();

	*outSize = size;
	return buffer;
}
FieldType Utils::MonoTypeToFieldType(MonoType* type) {
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

#pragma endregion
void Scripting::Initialize(const std::string& path) {
	MonoDomain* rootDomain = mono_jit_init("ShibaEngineRuntime");
	if (rootDomain == nullptr) {
		std::cout << "failed to create rootdomain" << std::endl;
		return;
	}
	data.rootDomain = rootDomain;

	Setup(path);

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
	mono_add_internal_call("ShibaEngineCore.Mouse::MouseButtonDown", MouseButtonDown);
	mono_add_internal_call("ShibaEngineCore.Instance::InstantiateInstance", InstantiateInternal);
}
void Scripting::Setup(const std::string& path) {
	mono_config_parse(NULL);

	data.appDomain = mono_domain_create_appdomain((char*)"ShibaEngine", nullptr);
	mono_domain_set(data.appDomain, true);
	data.coreAssemblyPath = "C:\\Users\\tombr\\source\\repos\\Shiba Engine\\ShibaEngineCore\\bin\\Debug\\netcoreapp3.1\\ShibaEngineCore.dll";
	data.coreAssembly = LoadAssembly(data.coreAssemblyPath);
	componentClass = Class{ "Component", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Component") };
	SetupClassFields(&componentClass, componentClass.monoClass, true);
	timeClass = Class{ "Time", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Time") };
	coreComponentClass = Class{ "CoreComponent", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "CoreComponent") };
	SetupClassFields(&coreComponentClass, coreComponentClass.monoClass, true);
	instanceClass = Class{ "Instance", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Instance") };
	SetupClassFields(&instanceClass, instanceClass.monoClass, true);
	behaviourManager = Class{ "BehaviourManager", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "BehaviourManager") };
	mouseClass = Class{ "Mouse", "ShibaEngineCore", GetClass(data.coreAssembly, "ShibaEngineCore", "Mouse") };
	SetupClassFields(&mouseClass, mouseClass.monoClass, false, false);

	LoadCoreAssemblyClasses();
	if (path != "") {
		data.appAssembly = LoadAssembly(path);
		LoadAssemblyClasses();
		FindBehaviours();
	}
}

void Scripting::FindBehaviours() {
	MonoObject* exception = nullptr;

	void* assemblyParams[1];
	assemblyParams[0] = mono_assembly_get_object(data.appDomain, data.appAssembly);
	mono_runtime_invoke(behaviourManager.GetMethod("LoadBehaviours", 1), nullptr, assemblyParams, nullptr);
	MonoObject* countMono = mono_runtime_invoke(behaviourManager.GetMethod("GetBehaviourCount", 0), nullptr, nullptr, nullptr);
	int count = *(int*)mono_object_unbox(countMono);
	MonoMethod* getBehaviourName = behaviourManager.GetMethod("GetBehaviourName", 1);
	MonoMethod* getBehaviourNamespace = behaviourManager.GetMethod("GetBehaviourNamespace", 1);
	MonoMethod* getBehaviourInterval = behaviourManager.GetMethod("GetBehaviourInterval", 1);
	for (int i = 0; i < count; i++) {
		void* params[1];
		params[0] = &i;
		MonoObject* nameObj = mono_runtime_invoke(getBehaviourName, nullptr, params, nullptr);
		MonoObject* namespaceObj = mono_runtime_invoke(getBehaviourNamespace, nullptr, params, nullptr);

		std::string typeName = mono_string_to_utf8(mono_object_to_string(nameObj, nullptr));

		std::string typeNamespace = mono_string_to_utf8(mono_object_to_string(namespaceObj, nullptr));
		MonoClass* monoClass = GetClass(data.appAssembly, typeNamespace.c_str(), typeName.c_str());


		Class klass;
		klass.monoClass = monoClass;
		klass.name = typeName;
		klass.nameSpace = typeNamespace;
		std::cout << "found behaviour: " << klass.name << std::endl;
		SetupClassFields(&klass, monoClass, false, false);
		Behaviour behaviour;
		behaviour.classData = klass;
		float interval = *(float*)mono_object_unbox(mono_runtime_invoke(getBehaviourInterval, nullptr, params, nullptr));
		behaviour.hasInterval = interval != 0;
		behaviour.interval = interval;
		behaviour.update = klass.GetMethod("Update", 0);
		behaviour.hasUpdate = behaviour.update != nullptr;
		data.behaviours[klass.name] = behaviour;
	}
}
void Scripting::UpdateBehaviours() {
	for (auto& behaviour : data.behaviours) {
		if (behaviour.second.ShouldUpdate() && behaviour.second.hasUpdate) {
			mono_runtime_invoke(behaviour.second.update, nullptr, nullptr, nullptr);
			behaviour.second.lastUpdateTime = Time::currentTime;
		}
	}
}
void Scripting::ReloadAssembly(const std::string& path) {
	mono_domain_set(mono_get_root_domain(), false);
	mono_domain_unload(data.appDomain);

	std::cout << path << std::endl;
	Setup(path);
	ReloadAllComponentScripts();
}

#pragma region Internal Calls 
MonoString* Scripting::GetName(unsigned int entity) {
	return mono_string_new(data.appDomain, SceneManager::activeScene->items[entity].name.c_str());
}
MonoObject* Scripting::GetInstance(unsigned int entity) {
	return data.entities[entity].instance.instance;
}
void Scripting::AddComponent(unsigned int entity, MonoString* name, MonoObject* monoObject) {
	std::string compName = mono_string_to_utf8(name);
	Engine::AddScript(entity, compName);
	data.entities[entity].scripts[compName] = CreateClassInstanceFromMonoObject(compName, monoObject);
}
void Scripting::UpdateExternCoreComponent(unsigned int entity, MonoString* compName, MonoObject* monoObject) {
	std::string name = mono_string_to_utf8(compName);
	if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
		std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
		return;
	}
	data.entities[entity].coreComponents[name].instance = monoObject;
	Engine::SetCoreComponent(entity, name, &data.entities[entity].coreComponents[name]);
}
void Scripting::UpdateCoreComponent(unsigned int entity, MonoString* monoName) {
	std::string name = mono_string_to_utf8(monoName);
	if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
		std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
		return;
	}
	Engine::GetCoreComponentObject(entity, name, &data.entities[entity].coreComponents[name]);
}
MonoObject* Scripting::GetCoreComponent(unsigned int entity, MonoString* monoName) {
	std::string name = mono_string_to_utf8(monoName);
	if (data.entities[entity].coreComponents.find(name) == data.entities[entity].coreComponents.end()) {
		std::cout << "core component " << name << " was not added to entity " << entity << std::endl;
		return nullptr;

	}
	Engine::GetCoreComponentObject(entity, name, &data.entities[entity].coreComponents[name]);
	return data.entities[entity].coreComponents[name].instance;
}
bool Scripting::GetKeyDown(int key) {
	return InputManager::GetKeyDown(key);
}
MonoArray* Scripting::FindComponentsInScene(MonoString* name) {
	std::vector<unsigned int> entities = Engine::FindScriptInScene(mono_string_to_utf8(name));
	MonoArray* arr = mono_array_new(mono_get_root_domain(), mono_get_uint32_class(), entities.size());
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		mono_array_set(arr, uint32_t, i, entities[i]);
	}
	return arr;
}
MonoObject* Scripting::GetComponent(unsigned int entity, MonoString* name) {
	return data.entities[entity].scripts[mono_string_to_utf8(name)].instance;
}
unsigned int Scripting::CreateEntity() {
	return Engine::CreateEntity();
}

void Scripting::PrintMessage(MonoString* message) {
	Console::LogMessage(mono_string_to_utf8(message));
}
void Scripting::PrintError(MonoString* error) {
	Console::LogError(mono_string_to_utf8(error));
}
bool Scripting::MouseButtonDown(int button) {
	return InputManager::MouseButtonDown(button);
}
#pragma endregion 

void Scripting::OnAddComponent(unsigned int entity, const std::string& name) {
	if (data.coreComponentClasses.find(name) == data.coreComponentClasses.end()) {
		std::cout << "class does not exist" << std::endl;
		return;
	}
	auto instance = ClassInstance(data.coreComponentClasses[name], true);
	instance.instance = Instantiate(instance.classData.monoClass);
	instance.SetFieldValue<unsigned int>("entity", entity);
	data.entities[entity].coreComponents[name] = instance;

}

void Scripting::OnEntityDestroyed(unsigned int entity) {
	if (data.entities.find(entity) != data.entities.end())
		data.entities.erase(entity);
}
void Scripting::OnEntityCreated(unsigned int entity) {
	data.entities[entity] = { entity, CreateClassInstance(entity, instanceClass) };
	OnAddComponent(entity, "Transform");
}
MonoObject* Scripting::InstantiateInternal(unsigned int entity) {
	InstantiateEntity(entity);
	return data.entities[entity].instance.instance;
}
MonoAssembly* Scripting::LoadAssembly(const std::string& path) {
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
ClassInstance Scripting::CreateClassInstanceFromMonoObject(const std::string& name, MonoObject* monoObject) {
	ClassInstance instance = ClassInstance(data.components[name]);
	instance.instance = monoObject;
	return instance;
}
MonoClass* Scripting::GetClass(MonoAssembly* assembly, const char* nameSpace, const char* name) {
	MonoImage* image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, nameSpace, name);
	if (klass == nullptr) {
		std::cout << "failed to get class" << std::endl;
		return nullptr;
	}

	return klass;
}
void Scripting::SetupClassFields(Class* klass, MonoClass* classData, bool getParents, bool getPrivate) {
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
ClassInstance Scripting::CreateClassInstance(unsigned int entity, Class klass) {
	ClassInstance instance = ClassInstance(klass);
	instance.instance = Instantiate(klass.monoClass);
	MonoClassField* entityField = mono_class_get_field_from_name(klass.monoClass, "entity");
	if (entityField == nullptr)
		std::cout << "failed to find entity field" << std::endl;
	mono_field_set_value(instance.instance, entityField, &entity);
	return instance;
}
void Scripting::CreateVSProject(const std::string& path) {
	std::string data = R"XML(
<Project Sdk="Microsoft.NET.Sdk">

  <PropertyGroup>
    <TargetFramework>netcoreapp3.1</TargetFramework>
  </PropertyGroup>

  <ItemGroup>
    <Reference Include="ShibaEngineCore">
      <HintPath>)XML" + Scripting::data.coreAssemblyPath + R"XML(</HintPath>
    </Reference>
  </ItemGroup>

</Project>
)XML";
	std::cout << path << std::endl;
	FileExtensions::CreateAndWriteToFile(path, data);

}
void Scripting::LoadCoreAssemblyClasses() {
	data.coreComponentClasses.clear();
	MonoImage* image = mono_assembly_get_image(data.coreAssembly);
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

		MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

		if (monoClass == coreComponentClass.monoClass)
			continue;

		bool isComponent = mono_class_is_subclass_of(monoClass, coreComponentClass.monoClass, false);
		if (isComponent) {
			Class klass = Class{ name, nameSpace, monoClass };
			SetupClassFields(&klass, klass.monoClass, true, true);
			data.coreComponentClasses[name] = klass;
		}
		else
			continue;
	}

}
void Scripting::LoadAssemblyClasses() {
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

		if (monoClass == componentClass.monoClass)
			continue;

		bool isComponent = mono_class_is_subclass_of(monoClass, componentClass.monoClass, false);
		if (isComponent) {

			Class klass = Class{ name, nameSpace, monoClass };

			SetupClassFields(&klass, klass.monoClass);
			data.components[fullname] = klass;
		}
		else
			continue;


	}
}
std::map<std::string, ClassInstance>& Scripting::GetEntityComponentInstances(unsigned int entity) {
	return data.entities[entity].scripts;
}
void Scripting::ReloadEntityScripts(unsigned int entity) {
	data.entities[entity].scripts.clear();
	auto scripts = Engine::GetEntityScripts(entity);
	for (auto script : scripts) {
		if (data.components.find(script) != data.components.end())
			data.entities[entity].scripts[script] = CreateClassInstance(entity, data.components[script]);
	}
}
void Scripting::RemoveScriptInstance(unsigned int entity, const std::string& script) {
	if (data.entities[entity].scripts.find(script) != data.entities[entity].scripts.end())
		data.entities[entity].scripts.erase(script);
}
void Scripting::LoadEntityScript(unsigned int entity, const std::string& script) {

	if (data.components.find(script) != data.components.end()) {
		data.entities[entity].scripts[script] = CreateClassInstance(entity, data.components[script]);
		data.entities[entity].scripts[script].InvokeMethod(componentClass.GetMethod("Initialize", 0), 0);
	}
}
void Scripting::OnRuntimeStart() {
	for (auto& comp : data.entities) {
		for (auto& instance : comp.second.scripts) {
			if(instance.second.hasStart)
				instance.second.InvokeMethod(instance.second.startMethod, 0);
		}
	}
}
void Scripting::ReloadAllComponentScripts() {
	for (auto& entity : data.entities) {
		entity.second.instance = CreateClassInstance(entity.first, instanceClass);
		auto comps = Engine::GetEntityComponents(entity.first);
		for (auto& comp : comps) {
			entity.second.coreComponents.erase(comp);
			OnAddComponent(entity.first, comp);
		}
		for (auto& script : Engine::GetEntityScripts(entity.first)) {
			entity.second.scripts.erase(script);
			LoadEntityScript(entity.first, script);
		}
	}
}
std::string Scripting::GetNameFromInstance(MonoObject* instance) {
	unsigned int entity;
	mono_field_get_value(instance, Scripting::instanceClass.fields["entity"].classField, &entity);
	return SceneManager::activeScene->items[entity].name;
}
void Scripting::SerializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
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
	case FieldType::Int: json->Int(instance.GetFieldValue<int>(field.name)); break;
	case FieldType::Instance: json->String(GetNameFromInstance(instance.GetFieldValue<MonoObject*>(field.name)).c_str()); break;
	default: json->String("No support for this type of variable"); break;
	}
}
void Scripting::SerializeEntityScripts(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {
	for (auto& instance : data.entities[entity].scripts) {
		json->Key(instance.second.classData.name.c_str());
		json->StartObject();
		for (auto& field : instance.second.classData.fields) {
			SerializeEntityScriptField(entity, instance.second, field.second, json);
		}
		json->EndObject();
	}
}
void Scripting::DeserializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::Value& obj) {
	switch (field.type)
	{
	case FieldType::Float: instance.SetFieldValue<float>(field.name, (float)obj.GetDouble()); break;
	case FieldType::Double: instance.SetFieldValue<double>(field.name, obj.GetDouble()); break;
	case FieldType::Bool: instance.SetFieldValue<bool>(field.name, obj.GetBool()); break;
	case FieldType::UInt: instance.SetFieldValue<unsigned int>(field.name, obj.GetInt()); break;
	case FieldType::Vector2: instance.SetFieldValue<glm::vec2>(field.name, SerializationUtils::DeserializeVec2(obj)); break;
	case FieldType::Vector3: instance.SetFieldValue<glm::vec3>(field.name, SerializationUtils::DeserializeVec3(obj)); break;
	case FieldType::Int: instance.SetFieldValue<int>(field.name, obj.GetInt()); break;
	case FieldType::Instance: instance.SetFieldValue<MonoObject>(field.name, data.entities[SceneManager::activeScene->GetInstanceFromName(obj.GetString())].instance.instance); break;
	default: break;
	}
}
void Scripting::DeserializeEntityScripts(unsigned int entity, rapidjson::Value& obj) {

	for (auto& script : data.components) {
		if (obj.HasMember(script.first.c_str())) {
			Engine::AddScript(entity, script.first);
			Scripting::LoadEntityScript(entity, script.first);
			for (auto& field : script.second.fields) {
				if (obj[script.first.c_str()].HasMember(field.first.c_str()))
					DeserializeEntityScriptField(entity, data.entities[entity].scripts[script.first], field.second, obj[script.first.c_str()][field.first.c_str()]);

			}
		}
	}
}
void Scripting::Update() {

	void* params[] = {
		&Time::deltaTime,
		&Time::currentTime
	};
	MonoObject* exception = nullptr;
	mono_runtime_invoke(timeClass.GetMethod("UpdateTime", 2), nullptr, params, &exception);
	mono_field_set_value(nullptr, mouseClass.fields["position"].classField, &InputManager::mouse.position);
	mono_field_set_value(nullptr, mouseClass.fields["frameOffset"].classField, &InputManager::mouse.frameOffset);
	UpdateBehaviours();
	for (auto& comp : data.entities) {
		for (auto& instance : comp.second.scripts) {
			instance.second.InvokeMethod(instance.second.updateMethod, 0);
		}
	}
}
void Scripting::Clear() {
	data.entities.clear();
	data.behaviours.clear();
}
unsigned int Scripting::InstantiateEntity(unsigned int entity) {
	// to make a pretty good copy of the entity, we're going to want to get the sceneitem to serialize and deserialize it
	rapidjson::StringBuffer str;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> json = rapidjson::PrettyWriter<rapidjson::StringBuffer>(str);
	SceneLoader::SerializeSceneHierachyNode(&SceneManager::activeScene->items[entity], &json);
	rapidjson::Document doc;
	doc.Parse(str.GetString());
	auto item = SceneLoader::DeserializeSceneHierachyNode(SceneManager::activeScene, doc);
	SceneLoader::DeserializeSceneHierachyNodeScripts(SceneManager::activeScene, item, doc);
	SceneManager::activeScene->hierachy.push_back(item);
	return item->entity;
}
MonoObject* Scripting::Instantiate(MonoClass* klass) {
	MonoObject* instance = mono_object_new(data.appDomain, klass);
	if (instance == nullptr) {
		std::cout << "failed to instantiate class" << std::endl;
		return nullptr;
	}
	mono_runtime_object_init(instance);
	return instance;
}