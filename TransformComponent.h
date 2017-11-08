#pragma once

#include "Component.h"
#include "Transform.h"
#include <vector>

namespace tofu
{
	class TransformComponentData;

	typedef Component<TransformComponentData> TransformComponent;

	class TransformComponentData
	{
	public:
		TransformComponentData() : TransformComponentData(Entity()) {}

		TransformComponentData(Entity e)
			: 
			entity(e),
			parent(),
			dirty(1)
		{}

		TF_INLINE void					SetParent(TransformComponent parent)
		{
			// TODO
			UpdateTransfromInHierachy();
		}

		TF_INLINE TransformComponent	GetParent() const { return parent; }

		TF_INLINE const Transform&		GetLocalTransform() const { return localTransform; }

		const Transform&				GetWorldTransform() const { return worldTransform; }

	public:

		// auxiliary functions

		TF_INLINE void					SetLocalPosition(const math::float3& pos)
		{
			localTransform.SetTranslation(pos);
			UpdateTransfromInHierachy();
		}

		TF_INLINE void					Translate(const math::float3& t)
		{
			localTransform.SetTranslation(localTransform.GetTranslation() + t);
			UpdateTransfromInHierachy();
		}

		TF_INLINE void					SetLocalRotation(const math::quat& quat)
		{
			localTransform.SetRotation(quat);
			UpdateTransfromInHierachy();
		}

		// get position coordinates in world space
		TF_INLINE math::float3			GetWorldPosition() const
		{
			return GetWorldTransform().TransformPosition(math::float3());
		}

		// get right vector in world space
		TF_INLINE math::float3			GetRightVector() const
		{
			return GetWorldTransform().TransformVector(math::float3{ 1, 0, 0 });
		}

		// get up vector in world space
		TF_INLINE math::float3			GetUpVector() const
		{
			return GetWorldTransform().TransformVector(math::float3{ 0, 1, 0 });
		}

		// get forward vector in world space
		TF_INLINE math::float3			GetForwardVector() const
		{
			return GetWorldTransform().TransformVector(math::float3{ 0, 0, 1 });
		}


	private:

		void							UpdateTransfromInHierachy();

	private:
		Entity							entity;
		TransformComponent				parent;
		std::vector<TransformComponent>	children;

		Transform						localTransform;
		Transform						worldTransform;
		uint32_t						dirty : 1;
	};

}