#pragma once

#include "Component.h"

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* gameObject);
	virtual ~ComponentMaterial();

	void Update() override;
	void OnEditor() override;

	void AddMaterial(int id);

public:
	int textureId;
};