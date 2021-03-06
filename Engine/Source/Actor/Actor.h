#pragma once

#include "../Bombast/interfaces.h"
#include "ActorComponent.h"
#include <map>

class Actor
{
public:
	typedef std::map<ComponentId, ActorComponent*> ActorComponents;

public:
	Actor(ActorId id);
	~Actor();

	bool Initialize(rapidxml::xml_node<>* pData);
	void PostInitialize();

	void Update(const float deltaMs);

	ActorId GetId() const { return m_id; }
	std::string GetType() const { return m_type; }

	std::string ToXML();

	template<class ComponentType>
	ComponentType* GetComponent(ComponentId id) 
	{
		ActorComponents::iterator findItem = m_components.find(id);
		if (findItem != m_components.end())
		{
			ActorComponent* pBase = findItem->second;
			ComponentType* pSub = static_cast<ComponentType*>(pBase); //Cast to Subclass version of pointer
			return pSub;
		}
		
		return NULL;
	}

	template<class ComponentType>
	ComponentType* GetComponent(const char* name)
	{
		ComponentId id = ActorComponent::GetIdFromName(name);
		ActorComponents::iterator findItem = m_components.find(id);
		if (findItem != m_components.end())
		{
			ActorComponent* pBase = findItem->second;
			ComponentType* pSub = static_cast<ComponentType*>(pBase);
			return pSub;
		}

		return NULL;
	}

	const ActorComponents* GetComponents() { return &m_components; }

	void AddComponent(ActorComponent* pComponent);

private:
	ActorId m_id;
	ActorComponents m_components;
	std::string m_type;
};