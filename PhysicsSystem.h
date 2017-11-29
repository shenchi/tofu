#pragma once

#include "Module.h"

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace tofu
{
	class PhysicsSystem : public Module
	{
		SINGLETON_DECL(PhysicsSystem)

	public:
		PhysicsSystem()
			:
			config(nullptr),
			dispatcher(nullptr),
			pairCache(nullptr),
			solver(nullptr),
			world(nullptr)
		{}

	public:
		int32_t Init() override;

		int32_t Shutdown() override;

		int32_t Update() override;

	private:
		btDefaultCollisionConfiguration*		config;
		btCollisionDispatcher*					dispatcher;
		btBroadphaseInterface*					pairCache;
		btSequentialImpulseConstraintSolver*	solver;
		btDiscreteDynamicsWorld*				world;
	};
}
