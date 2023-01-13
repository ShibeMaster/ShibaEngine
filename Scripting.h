#pragma once
#include <string>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include "ScriptingTypes.h"
#include <mono/metadata/object.h>
#include <map>


struct Instance {
	unsigned int entity;
	ClassInstance instance;
	std::map<std::string, ClassInstance> scripts;
	std::map<std::string, ClassInstance> coreComponents;
};
struct ScriptingEngineData {
	MonoDomain* rootDomain;
	MonoDomain* appDomain;
	MonoAssembly* appAssembly;
	MonoAssembly* coreAssembly;

	std::string coreAssemblyPath;
	std::map<std::string, Class> components;
	std::map<std::string, Class> coreComponentClasses;
	std::map<unsigned int, Instance> entities;
	std::map<std::string, Behaviour> behaviours;
};
class Utils {
public:
	static char* ReadBytes(const std::string& filepath, uint32_t* outSize);
	static FieldType MonoTypeToFieldType(MonoType* type);
};
class Scripting {
private:
	static Class componentClass;
	static Class timeClass;
	static Class coreComponentClass;
	static Class behaviourManager;
	static Class mouseClass;
	static void LoadCoreAssemblyClasses();
	static void LoadAssemblyClasses();
	static void FindBehaviours();
	static MonoAssembly* LoadAssembly(const std::string& path);
	static ClassInstance CreateClassInstanceFromMonoObject(const std::string& name, MonoObject* monoObject);
	static void SetupClassFields(Class* klass, MonoClass* classData, bool getParents = false, bool getPrivate = false);
	static std::string GetNameFromInstance(MonoObject* instance);
	static void ReloadAllComponentScripts();

	static void SerializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void DeserializeEntityScriptField(unsigned int entity, ClassInstance& instance, Field& field, rapidjson::Value& obj);


#pragma region Internal Calls 
	static MonoString* GetName(unsigned int entity);
	static MonoObject* GetInstance(unsigned int entity);
	static void AddComponent(unsigned int entity, MonoString* name, MonoObject* monoObject);
	static void UpdateExternCoreComponent(unsigned int entity, MonoString* compName, MonoObject* monoObject);
	static void UpdateCoreComponent(unsigned int entity, MonoString* monoName);
	static MonoObject* GetCoreComponent(unsigned int entity, MonoString* monoName);
	static bool GetKeyDown(int key);
	static MonoArray* FindComponentsInScene(MonoString* name);
	static MonoObject* GetComponent(unsigned int entity, MonoString* name);
	static unsigned int CreateEntity();
	static void PrintMessage(MonoString* message);
	static void PrintError(MonoString* error);
	static bool MouseButtonDown(int button);
	static MonoObject* InstantiateInternal(unsigned int entity);
#pragma endregion 

public:
	static ScriptingEngineData data;
	static Class instanceClass;
	static void Initialize(const std::string& path);
	static void Setup(const std::string& path);
	static void UpdateBehaviours();
	static void ReloadAssembly(const std::string& path);
	static void OnAddComponent(unsigned int entity, const std::string& name);
	static void OnEntityDestroyed(unsigned int entity);
	static void OnEntityCreated(unsigned int entity);
	static MonoClass* GetClass(MonoAssembly* assembly, const char* nameSpace, const char* name);
	static ClassInstance CreateClassInstance(unsigned int entity, Class klass);
	static void CreateVSProject(const std::string& path);
	static std::map<std::string, ClassInstance>& GetEntityComponentInstances(unsigned int entity);
	static void ReloadEntityScripts(unsigned int entity);
	static void RemoveScriptInstance(unsigned int entity, const std::string& script);
	static void LoadEntityScript(unsigned int entity, const std::string& script);
	static void OnRuntimeStart();
	static void SerializeEntityScripts(unsigned int entity, rapidjson::PrettyWriter<rapidjson::StringBuffer>* json);
	static void DeserializeEntityScripts(unsigned int entity, rapidjson::Value& obj);
	static void Update();
	static void Clear();
	static unsigned int InstantiateEntity(unsigned int entity);
	static MonoObject* Instantiate(MonoClass* klass);
};
