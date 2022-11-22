#pragma once
class Component {
public:
	unsigned int entity;
	virtual void Start(){}
	virtual void Update(bool inRuntime){}
	static void DrawGUI(unsigned int selectedEntity){}
};