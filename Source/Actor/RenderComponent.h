#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "RenderComponentInterface.h"
#include "../Utilities/rapidxml.hpp"

class BaseRenderComponent : public RenderComponentInterface
{
public:
	virtual bool VInitialize(rapidxml::xml_node<>* pData) override;
	virtual void VPostInit() override;
	virtual void VOnChanged() override;
	virtual rapidxml::xml_node<>* VGenerateXml() override;
	const Color GetColor() const { return m_color; }

protected:
	virtual bool VDelegateInitialize(rapidxml::xml_node<>* pData) { return true; }
	virtual SceneNode* VCreateSceneNode() = 0;
	Color LoadColor(rapidxml::xml_node<>* pData);

	virtual rapidxml::xml_node<>* VCreateBaseElement(rapidxml::xml_document<>* outDoc) {
		return outDoc->allocate_node(rapidxml::node_element, VGetName());
	}
	virtual void VCreateInheritedXmlElements(rapidxml::xml_node<>* pBaseElement) = 0;

private:
	virtual SceneNode* VGetSceneNode() override;

protected:
	Color m_color;
	SceneNode* m_pSceneNode;
};

#endif