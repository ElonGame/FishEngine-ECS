#pragma once

#include "../Engine.hpp"
#include "../ISerializable.hpp"

#include <typeindex>

namespace FishEditor
{
	class EditorSystem;
}

namespace FishEngine
{
	class GameObject;
	class Transform;
	class Scene;

	class Component : public ISerializabe
	{
	public:
		EntityID entityID;
		bool m_Enabled = true;
		
		virtual std::type_index GetTypeIndex() = 0;
		
		constexpr static const char* CLASS_NAME = "Component";
		virtual const char* GetClassName() { return CLASS_NAME; }
		
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
		
		
		virtual void OnDrawGizmos() const { }
		virtual void OnDrawGizmosSelected() const { }
		
		void Deserialize(InputArchive& archive) override;
		void Serialize(OutputArchive& archive) const override;
		
	protected:
		Component() = default;
		
	public:
		// temp
		GameObject* m_GameObject = nullptr;
		Transform* GetTransform() const;
	};


#define COMPONENT(T)                            \
	protected:                                      \
		T() = default;                              \
	private:                                        \
		friend class FishEngine::Scene;                    \
		friend class FishEditor::EditorSystem;					\
		inline static std::vector<T*> components;										\
		std::type_index GetTypeIndex() override { return std::type_index(typeid(T)); }	\
		constexpr static const char* CLASS_NAME = #T; \
		const char* GetClassName() override { return CLASS_NAME; }						\
		static T* Create() { T* t = new T(); components.push_back(t); return t; }		\
		void Deserialize(FishEngine::InputArchive& archive) override; \
		void Serialize(FishEngine::OutputArchive& archive) const override;

}
