#include "actor.h"

Actor::Actor() : m_iActorId(0)
{

}

void Actor::SetPosition(float fX, float fY, float fZ)
{
	m_vPosition.m_fX = fX;
	m_vPosition.m_fY = fY;
	m_vPosition.m_fZ = fZ;
}

void Actor::SetPosition(const Vector3 &position)
{
	m_vPosition = position;
}

void Actor::SetRotation(float fX, float fY, float fZ)
{
	m_vRotation.m_fX = fX;
	m_vRotation.m_fY = fY;
	m_vRotation.m_fZ = fZ;
}

void Actor::SetRotation(const Vector3 &rotation)
{
	m_vRotation = rotation;
}

void Actor::SetVelocity(float fX, float fY, float fZ)
{
	m_vVelocity.m_fX = fX;
	m_vVelocity.m_fY = fY;
	m_vVelocity.m_fZ = fZ;
}

void Actor::SetVelocity(const Vector3& velocity)
{
	m_vVelocity = velocity;
}

bool Actor::IsWithinDistance(Actor *pActor, uint32 iDistance)
{
	float x = pActor->m_vPosition.m_fX - m_vPosition.m_fX;
	float y = pActor->m_vPosition.m_fY - m_vPosition.m_fY;

	return (x*x + y*y) <= (float)(iDistance * iDistance);
}