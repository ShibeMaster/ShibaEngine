#pragma once
class Component {
public:
	unsigned int entity;
	virtual void Start(){}
	virtual void Update(){}
	virtual void DrawGui(){}
};