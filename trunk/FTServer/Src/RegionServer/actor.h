/*****************************************************************************************
	filename:	actor.h
	created:	10/19/2012
	author:		chen
	purpose:	actor on the map

*****************************************************************************************/

#ifndef _H_ACTOR
#define _H_ACTOR

#include "server_common.h"
#include "memory_object.h"

class Actor : public MemoryObject
{
public:
	// cstr and dstr
	Actor();
	virtual ~Actor(){}

	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(const Vector3& position);

	void SetRotation(float fX, float fY, float fZ);
	void SetRotation(const Vector3& rotation);

	void SetVelocity(float fX, float fY, float fZ);
	void SetVelocity(const Vector3& velocity);

	// check if other actors near this one
	bool IsWithinDistance(Actor* pActor, uint32 iDistance);

public:
	uint32 m_iActorId;
	Vector3 m_vPosition;
	Vector3 m_vRotation;
	Vector3 m_vVelocity;
};

#endif
