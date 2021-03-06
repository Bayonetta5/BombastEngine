#pragma once

#include "../Utilities/String.h"

class ActorComponent{
	friend class ActorFactory;

public:
	virtual ~ActorComponent() { m_pOwner = 0; };

	//Meant to be overwriten by implementation classes
	virtual bool VInitialize(rapidxml::xml_node<>* pData) = 0;
	virtual void VPostInit() {};
	virtual void VUpdate(const float deltaMs){}
	virtual void VOnChanged() {};


	virtual rapidxml::xml_node<>* VGenerateXml(rapidxml::xml_document<> &outDoc) = 0;

	//Overewritten by interface class
	virtual ComponentId VGetId() const { return GetIdFromName(VGetName()); }
	virtual const char* VGetName() const = 0;
	static ComponentId GetIdFromName(const char* componentStr)
	{
		return static_cast<ComponentId>(beStringHash(componentStr));
	}

protected:
	Actor* m_pOwner;

private:
	void SetOwner(Actor* pOwner) { m_pOwner = pOwner; }
};