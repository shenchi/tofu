#pragma once

#include "Common.h"
#include "Entity.h"

#include <utility>
#include <cassert>

namespace tofu
{
	struct ComponentIndex
	{
		uint32_t	idx;
		
		explicit ComponentIndex() : idx(UINT32_MAX) {}
	};

	// Component is actually like a handle or pointer, 
	// the actuall component T is in Component<T>::components[]
	template<class T>
	class Component
	{
	public:
		typedef T component_data_t;
	protected:
		Entity		entity;
		uint32_t*	compIdx;
	
	public:
		Component() : entity(), compIdx(nullptr) {}

		Component(Entity e) : entity(e), compIdx(nullptr) 
		{
			if (e) compIdx = &(pointers[e.id].idx);
		}

		operator bool() const
		{
			return nullptr != compIdx && *compIdx < numComponents;
		}

		T* operator -> () const
		{
			assert(true == *this);
			return &components[*compIdx];
		}

		inline void Destroy()
		{
			assert(true == *this);
			
			// swap the last element to this location
			// change the pointers
			// then decrease numComponents

			if (numComponents > 1)
			{
				uint32_t lastElemIdx = numComponents - 1;
				Entity lastElemEntity = back_pointers[lastElemIdx];

				components[*compIdx] = std::move(components[lastElemIdx]);

				back_pointers[lastElemIdx] = Entity();
				back_pointers[*compIdx] = lastElemEntity;

				pointers[lastElemEntity.id].idx = *compIdx;
				*compIdx = UINT32_MAX;
			}
			else
			{
				components[*compIdx].~T();
				back_pointers[*compIdx] = Entity();
				*compIdx = UINT32_MAX;
			}

			numComponents--;
		}

	public:
		
		static Component<T> Create(Entity e)
		{
			if (pointers[e.id].idx < numComponents)
				return Component<T>(e);

			uint32_t loc = numComponents++;

			back_pointers[loc] = e;
			pointers[e.id].idx = loc;

			// clear this component in case it was used
			// and prevent resources leaking
			components[loc].~T();
			new (&components[loc]) T(e);

			return Component<T>(e);
		}

		static T* GetAllComponents() { return components; }
		static uint32_t GetNumComponents() { return numComponents; }

	protected:
		// mapping from entity id to component index(location)
		static ComponentIndex pointers[MAX_ENTITIES];

		// mapping from component index(location) to entity id
		static Entity back_pointers[MAX_ENTITIES];

		// array of actual components
		static T components[MAX_ENTITIES];
		static uint32_t numComponents;
	};

	template<class T>
	ComponentIndex Component<T>::pointers[MAX_ENTITIES];

	template<class T>
	Entity Component<T>::back_pointers[MAX_ENTITIES];

	template<class T>
	T Component<T>::components[MAX_ENTITIES];

	template<class T>
	uint32_t Component<T>::numComponents = 0;
}
