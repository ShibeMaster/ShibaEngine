#pragma once
#include <mono/metadata/object.h>
#include "ScriptingTypes.h"
class Component {
public:
	unsigned int entity;
	virtual void Start(){};
	virtual void Update(bool inRuntime){};
	static void DrawGUI(unsigned int selectedEntity) {};
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
};