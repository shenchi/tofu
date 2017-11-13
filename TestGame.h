#pragma once

#include "Module.h"

#include "Entity.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "StaticMeshComponent.h"

class TestGame : public tofu::Module
{
public:
	virtual int32_t Init() override;

	virtual int32_t Shutdown() override;

	virtual int32_t Update() override;

private:
	tofu::TransformComponent	tCube;
	tofu::TransformComponent	tCamera;
	float pitch;
	float yaw;
};
