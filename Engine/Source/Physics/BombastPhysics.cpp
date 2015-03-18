#include "BombastPhysics.h"
#include "../Actor/TransformComponent.h"
#include "../Resources/XmlResource.h"
#include "../Events/Events.h"

const float BombastPhysics::Timestep = 1.00f / 60.0f;

const float BE_PHYSICS_GRAVITY = -9.91f;

BombastPhysics::BombastPhysics()
	: m_pFoundation(nullptr), m_pPhysicsCore(nullptr)
{
}

BombastPhysics::~BombastPhysics()
{
	SAFE_DELETE(m_pFoundation);
	SAFE_DELETE(m_pPhysicsCore);
}

bool BombastPhysics::VInitialize()
{
	VLoadPhysicsConfigXml();

	m_pFoundation = BE_NEW BpFoundation;
	m_pPhysicsCore = BE_NEW BpPhysicsCore;

	if (!m_pPhysicsCore) 
	{
		BE_ERROR("Error Creating Bombast Physics Device");
		return false;
	}

	BpSceneDesc sceneDesc;
	sceneDesc.gravity = BpVec3(0.0f, -9.91f, 0.0f);
	sceneDesc.bounceThresholdVelocity = BE_PHYSICS_GRAVITY * -0.2f;

	m_pScene = m_pPhysicsCore->CreateScene(sceneDesc);

	return true;
}

PhysicsMaterialData BombastPhysics::LookupMaterialData(const std::string& materialString)
{
	auto it = m_materialTable.find(materialString);
	if (it != m_materialTable.end())
	{
		return it->second;
	}
	else
	{
		return PhysicsMaterialData(0, 0);
	}
}

float BombastPhysics::LookupDensity(const std::string& densityString)
{
	float density = 1.0f;
	auto it = m_densityTable.find(densityString);
	if (it != m_densityTable.end())
	{
		density = it->second;
	}

	return density;
}

void BombastPhysics::VLoadPhysicsConfigXml()
{
	rapidxml::xml_node<>* pRoot = XmlResourceLoader::LoadAndReturnRootXmlElement("Config\\physicsConfig.xml");
	BE_ASSERT(pRoot);

	rapidxml::xml_node<>* pMatNode = pRoot->first_node("PhysicsMaterials");
	BE_ASSERT(pMatNode);
	for (rapidxml::xml_node<>* pNode = pMatNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		double restitution = 0;
		double friction = 0;
		restitution = atof(pNode->first_attribute("restitution")->value());
		friction = atof(pNode->first_attribute("friction")->value());
		m_materialTable.insert(std::make_pair(pNode->name(), PhysicsMaterialData((float)restitution, (float)friction)));
	}

	rapidxml::xml_node<>* pDensityNode = pRoot->first_node("DensityTable");
	BE_ASSERT(pDensityNode);
	for (rapidxml::xml_node<>* pNode = pDensityNode->first_node(); pNode; pNode = pNode->next_sibling())
	{
		m_densityTable.insert(std::make_pair(pNode->name(), (float)atof(pNode->value())));
	}
}

void BombastPhysics::VOnUpdate(float const deltaMs)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VSyncVisibleScene()
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VAddSphere(float radius, Actor* gameActor, const std::string& densityStr, const std::string& physicsMaterial, bool gravityEnabled, float linearDamping, float angularDamping)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VRemoveActor(ActorId id)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VRenderDiagnostics()
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VApplyForce(const Vec3 &dir, float newtons, ActorId actorId)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VApplyTorque(const Vec3 &dir, float newtons, ActorId actorId)
{
	throw "Function not yet implemented.";
}

bool BombastPhysics::VMove(const Mat4x4 &mat, ActorId actorId)
{
	throw "Function not yet implemented.";
	return false;
}

void BombastPhysics::VStopActor(ActorId actorId)
{
	throw "Function not yet implemented.";
}

Vec3 BombastPhysics::VGetVelocity(ActorId actorId)
{
	throw "Function not yet implemented.";
	return Vec3::g_InvalidVec3;
}

void BombastPhysics::VSetVelocity(ActorId actorId, const Vec3& vel)
{
	throw "Function not yet implemented.";
}

Vec3 BombastPhysics::VGetAngularVelocity(ActorId actorId)
{
	throw "Function not yet implemented.";
	return Vec3::g_InvalidVec3;
}

void BombastPhysics::VSetAngularVelocity(ActorId actorId, const Vec3& vel)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VTranslate(ActorId actorId, const Vec3& vec)
{
	throw "Function not yet implemented.";
}

void BombastPhysics::VSetTransform(const ActorId id, const Mat4x4& mat)
{
	throw "Function not yet implemented.";
}

Mat4x4 BombastPhysics::VGetTransform(const ActorId id)
{
	throw "Function not yet implemented.";
	return Mat4x4::g_Identity;
}

void BombastPhysics::Mat4x4ToBpMat4x4(const Mat4x4& input, BpMat4x4* output)
{
	output->col0.x = input._11;
	output->col0.y = input._12;
	output->col0.z = input._13;
	output->col0.w = input._14;

	output->col1.x = input._21;
	output->col1.y = input._22;
	output->col1.z = input._23;
	output->col1.w = input._24;

	output->col2.x = input._31;
	output->col2.y = input._32;
	output->col2.z = input._33;
	output->col2.w = input._34;

	output->col3.x = input._41;
	output->col3.y = input._42;
	output->col3.z = input._43;
	output->col3.w = input._44;
}

void BombastPhysics::BpMat4x4ToMat4x4(const BpMat4x4& input, Mat4x4* output)
{
	output->_11 = input.col0.x;
	output->_12 = input.col0.y;
	output->_13 = input.col0.z;
	output->_14 = input.col0.w;

	output->_21 = input.col1.x;
	output->_22 = input.col1.y;
	output->_23 = input.col1.z;
	output->_24 = input.col1.w;
	
	output->_31 = input.col2.x;
	output->_32 = input.col2.y;
	output->_33 = input.col2.z;
	output->_34 = input.col2.w;
	
	output->_41 = input.col3.x;
	output->_42 = input.col3.y;
	output->_43 = input.col3.z;
	output->_44 = input.col3.w;
}

void BombastPhysics::BpVec3ToVec3(const BpVec3& input, Vec3* output)
{
	output->x = input.x;
	output->y = input.y;
	output->z = input.z;
}