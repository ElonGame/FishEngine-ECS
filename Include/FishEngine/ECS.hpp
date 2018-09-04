#pragma once
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include "Math.hpp"
#include "Engine.hpp"
#include "Object.hpp"

class Transform;
class TransformSystem;

namespace ECS
{

	class Scene;
	class GameObject;

	using EntityID = uint32_t;

	class Component
	{
	public:
		EntityID entityID;
		
		virtual std::type_index GetTypeIndex() = 0;
		
		template<class T>
		bool Is() const
		{
			const T* t = dynamic_cast<const T*>(this);
			return t != nullptr;
		}
		
		template<class T>
		T* As()
		{
			return dynamic_cast<T*>(this);
		}

		inline Transform* GetTransform() const;
		
	protected:
		Component() = default;

		// temp
		GameObject* m_GameObject = nullptr;
	};


	#define COMPONENT(T)                            \
	protected:                                      \
		T() = default;                              \
	private:                                        \
		friend class ECS::Scene;                    \
		inline static std::vector<T*> components;   \
		std::type_index GetTypeIndex() override     \
		{ return std::type_index(typeid(T)); }      \
		static T* Create() { T* t = new T(); components.push_back(t); return t; }               \


	

	class GameObject : public Object
	{
		friend class Scene;
	public:

		EntityID GetID() const { return ID; }
		
		Transform* GetTransform() const { return m_Transform; }
		//EntityID GetParentID() const { return m_ParentID; }

		template<class T>
		T* GetComponent() {
			for (auto comp : m_Components)
			{
				T* t = comp->As<T>();
				if (t != nullptr)
					return t;
			}
			return nullptr;
		}
		
		Scene* GetScene() { return m_Scene; }

		std::string m_Name;

	protected:
		GameObject(EntityID entityID, Scene* scene);

	protected:
		std::vector<Component*> m_Components;
		Transform* m_Transform = nullptr;
		//EntityID m_ParentID = 0;
		//int m_RootOrder = 0;		// index in parent's children array
		Scene* m_Scene = nullptr;
	private:
		EntityID ID;
	};


	class ISystem
	{
		friend Scene;
	public:
		virtual void OnAdded() {}
		virtual void Start() {}
		virtual void Update() = 0;
		virtual void PostUpdate() {};

		int m_Priority = 0;

	protected:
		Scene * m_Scene = nullptr;
	};


	class SingletonComponent
	{
		friend Scene;
	protected:
		SingletonComponent() = default;
	};


	class Scene
	{
	public:
		//EntityID CreateGameObject();
		GameObject* CreateGameObject();
		
		template<class T>
		T* GameObjectAddComponent(GameObject* go)
		{
			T* comp = T::Create();
			go->m_Components.push_back(comp);
			comp->entityID = go->ID;
			comp->m_GameObject = go;
			return comp;
		}
		
		template<class T>
		T* GameObjectAddComponent(EntityID id)
		{
			return GameObjectAddComponent<T>(GetGameObjectByID(id));
		}
		
	//	void GameObjectSetParent(EntityID child, EntityID parent)
	//	{
	//		auto c = GetGameObjectByID(child);
	////		auto p = GetGameObjectByID(parent);
	//		c->m_ParentID = parent;
	//	}
		
		void All(std::function<void(GameObject*)> func)
		{
			for (auto& pair : m_GameObjects)
			{
				func(pair.second);
			}
		}
		
		template<class T>
		void ForEach(std::function<void(GameObject*, T*)> func)
		{
			for (T* t : T::components)
			{
				GameObject* go = GetGameObjectByID(t->entityID);
				func(go, t);
			}
		}
		
		
		template<class T1, class T2>
		void ForEach(std::function<void(GameObject*, T1*, T2*)> func)
		{
			for (auto& pair : m_GameObjects)
			{
				T1* t1 = nullptr;
				T2* t2 = nullptr;
				GameObject* go = pair.second;
				for (Component* comp : go->m_Components)
				{
					if (t1 != nullptr && comp->Is<T1>())
					{
						t1 = (T1*)comp;
					}
					else if (t2 != nullptr && comp->Is<T2>())
					{
						t2 = (T2*)comp;
					}
					
					if (t1 != nullptr && t2 != nullptr)
					{
						func(go, t1, t2);
						return;
					}
				}
			}
		}
		
		
		template<class T>
		T* FindComponent() const
		{
			if (T::components.empty())
				return nullptr;
			return T::components.front();
		}

		template<class T>
		T* AddSingletonComponent()
		{
			auto typeidx = std::type_index(typeid(T));
			auto it = m_SingletonComponents.find(typeidx);
			if (it != m_SingletonComponents.end())
			{
				abort();
			}

			T* t = new T();
			m_SingletonComponents[typeidx] = t;
			return t;
		}

		template<class T>
		T* GetSingletonComponent()
		{
			auto it = m_SingletonComponents.find(std::type_index(typeid(T)));
			if (it == m_SingletonComponents.end())
				return nullptr;
			return (T*)it->second;
		}
		
		void AddSystem(ISystem* system)
		{
			m_Systems.push_back(system);
			system->m_Scene = this;
			system->OnAdded();
		}

		template<class T>
		T* GetSystem()
		{
			for (ISystem* s : m_Systems)
			{
				T* t = dynamic_cast<T*>(s);
				if (t != nullptr)
					return t;
			}
			return nullptr;
		}

		void Start()
		{
			std::sort(m_Systems.begin(), m_Systems.end(), [](ISystem* a, ISystem* b) {
				return a->m_Priority < b->m_Priority;
			});

			for (ISystem* s : m_Systems)
			{
				s->Start();
			}
		}
		
		void Update()
		{
			for (ISystem* s : m_Systems)
			{
				s->Update();
			}
		}

		void PostUpdate()
		{
			for (ISystem* s : m_Systems)
			{
				s->PostUpdate();
			}
		}
		
		GameObject* GetGameObjectByID(EntityID id)
		{
			auto it = m_GameObjects.find(id);
			if (it == m_GameObjects.end())
				return nullptr;
			return it->second;
		}


		bool m_Cleaning = false;
		std::vector<Transform*> m_RootTransforms;
		const auto& GetRootTransforms() { return m_RootTransforms; }
		void AddRootTransform(Transform* t);
		void RemoveRootTransform(Transform* t);
		
	protected:
		std::unordered_map<EntityID, GameObject*> m_GameObjects;
		std::vector<ISystem*> m_Systems;
		std::unordered_map<std::type_index, SingletonComponent*> m_SingletonComponents;
		
	private:
		EntityID m_LastEntityID = 0;
	};


	// inline

	Transform* ECS::Component::GetTransform() const
	{
		return m_GameObject->GetTransform();
	}

} // namespace ECS
