#include "PhysXCharacterControllerComponent.h"
#include "TransformComponent.h"

#include "../Physics/PhysXPhysics.h"

#include <characterkinematic/PxCapsuleController.h>
#include <characterkinematic/PxControllerManager.h>
#include <PxRigidDynamic.h>

using namespace physx;

PhysXCharacterControllerDesc::PhysXCharacterControllerDesc() :
	mPosition(PxExtendedVec3(0, 0, 0)),
	mSlopeLimit(0.0f),
	mContactOffset(0.0f),
	mStepOffset(0.0f),
	mInvisibleWallHeight(0.0f),
	mMaxJumpHeight(0.0f),
	mRadius(1.0f),
	mHeight(5.0f),
	mCrouchHeight(2.0f),
	mProxyDensity(10.0f),
	mProxyScale(0.9f),
	mVolumeGrowth(1.5f),
	mMaterial(nullptr),
	mReportCallback(nullptr),
	mBehaviorCallback(nullptr)
{}

const char* PhysXCharacterControllerComponent::g_Name = "PhysXCharacterControllerComponent";

PhysXCharacterControllerComponent::PhysXCharacterControllerComponent()
	: m_pController(nullptr), m_pPhysXPhysics(nullptr), m_materialName(nullptr)
{
}

PhysXCharacterControllerComponent::~PhysXCharacterControllerComponent()
{
	m_pController->release();
}


bool PhysXCharacterControllerComponent::VInitialize(rapidxml::xml_node<>* pData)
{
	rapidxml::xml_node<>* pMaterial = pData->first_node("PhysicsMaterial");
	if (pMaterial)
	{
		m_materialName = pMaterial->value();
	}

	m_controllerDesc.mReportCallback = this;
	m_controllerDesc.mBehaviorCallback = this;
	return true;
}

void PhysXCharacterControllerComponent::VPostInit()
{
	m_pPhysXPhysics = static_cast<PhysXPhysics*>(g_pApp->m_pGame->VGetGamePhysics());

	//Try and load physicsmaterial, if cannot find specified name load default
	if (m_materialName != nullptr)
	{
		PxMaterial* material = m_pPhysXPhysics->GetPxMaterialByName(m_materialName);
		m_controllerDesc.mMaterial = material;
	}

	if (m_controllerDesc.mMaterial == nullptr)
	{
		m_controllerDesc.mMaterial = m_pPhysXPhysics->GetDefaultPxMaterial();
	}

	PxCapsuleControllerDesc controllerDesc;

	controllerDesc.height = m_controllerDesc.mHeight;
	controllerDesc.radius = m_controllerDesc.mRadius;
	controllerDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	controllerDesc.density = m_controllerDesc.mProxyDensity;
	controllerDesc.scaleCoeff = m_controllerDesc.mProxyScale;
	controllerDesc.position = m_controllerDesc.mPosition;
	controllerDesc.slopeLimit = m_controllerDesc.mSlopeLimit;
	controllerDesc.stepOffset = m_controllerDesc.mStepOffset;
	controllerDesc.invisibleWallHeight = m_controllerDesc.mInvisibleWallHeight;
	controllerDesc.maxJumpHeight = m_controllerDesc.mMaxJumpHeight;
	controllerDesc.reportCallback = m_controllerDesc.mReportCallback;
	controllerDesc.behaviorCallback = m_controllerDesc.mBehaviorCallback;
	controllerDesc.volumeGrowth = m_controllerDesc.mVolumeGrowth;
	controllerDesc.material = m_controllerDesc.mMaterial;

	PxController* controller = m_pPhysXPhysics->GetPxControllerManager()->createController(controllerDesc);
	PxRigidDynamic* controllerActor = controller->getActor();
	if (controllerActor != nullptr)
	{
		if (controllerActor->getNbShapes() > 0)
		{
			PxShape* controllerShape;
			controllerActor->getShapes(&controllerShape, 1);
			controllerShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		}
	}

	m_pController = controller;
}

void PhysXCharacterControllerComponent::VUpdate(const float deltaMs)
{
	TransformComponent* pTransformComponent = m_pOwner->GetComponent<TransformComponent>(TransformComponent::g_Name);

	if (!pTransformComponent)
	{
		BE_ERROR("No Transform Component!");
		return;
	}

	const PxControllerFilters filters(nullptr, nullptr, nullptr);
	PxVec3 disp;
	PhysXPhysicsHelpers::Vec3ToPxVec(m_targetDisplacement, &disp);

	m_pController->move(disp, 0.0f, deltaMs, filters);

	PxExtendedVec3 pos = m_pController->getPosition();
	Vec3 bPos;
	PhysXPhysicsHelpers::PxExtendedVecToVec3(pos, &bPos);

	pTransformComponent->SetPosition(bPos);
}

void PhysXCharacterControllerComponent::SetTargetDisplacement(Vec3 newDisplacement)
{
	m_targetDisplacement = newDisplacement;
}

void PhysXCharacterControllerComponent::onShapeHit(const PxControllerShapeHit& hit)
{
}

PxControllerBehaviorFlags PhysXCharacterControllerComponent::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysXCharacterControllerComponent::getBehaviorFlags(const PxController&)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags PhysXCharacterControllerComponent::getBehaviorFlags(const PxObstacle&)
{
	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}