#pragma once
#include <mono/metadata/object.h>
#include <rapidjson/PrettyWriter.h>
#include "ScriptingTypes.h"
#include "Transform.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
class Component {
public:
	unsigned int entity;
	Transform* transform;
	virtual void Start(){};
	virtual void Update(){};
	/// <summary>
	/// Called after the entity variable is set, just allows the component to set up variables that require the entity value
	/// </summary>
	virtual void Initialize() {
		transform = Engine::GetComponentPointer<Transform>(entity);
	}
	static void DrawGUI(unsigned int selectedEntity) {};
	/// <summary>
	/// Serializes the component to save into the scene
	/// </summary>
	/// <param name="entity"></param>
	/// <param name="json"></param>
	virtual void Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* json) {};
	/// <summary>
	/// Deserializes the values in json object to the values in the component
	/// </summary>
	/// <param name="obj"></param>
	virtual void Deserialize(rapidjson::Value& obj) {};
	/// <summary>
	/// This is called when the scripting system requests a core component of a specific type, it basically just loads the latest data of this object into the c# equivalent.
	/// </summary>
	/// <param name="instance"></param>
	virtual void GetObject(ClassInstance* instance) {};
	/// <summary>
	/// This is called when the scripting system makes a change to the equivalent component object, so it updates the values accordingly.
	/// </summary>
	/// <param name="instance"></param>
	virtual void SetObject(ClassInstance* instance) {};
	/// <summary>
	/// This is called only for components that need to be rendered in and outside of runtime
	/// </summary>
	virtual void Render() {};
};