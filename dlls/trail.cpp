#include "extdll.h"
#include "decals.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "soundent.h"
#include "shake.h"
#include "gamerules.h"
#include "trail.h"

void CTrail::Spawn()
{
	m_pTrailList = new TrailList;
	m_pTrailList->next = NULL;
}

void CTrail::DrawThink()
{
	TrailList *current = m_pTrailList;
	Vector vecDir = Vector(0, 0, 0);
	Vector vecStart;
	Vector vecEnd;
	//draw trail inbetween points
	while (current->next)
	{
		vecEnd = current->next->vecPoint;
		vecStart = current->vecPoint;
		
		//adjust start of trail, to fill out holes
		if (vecDir.Length() > 0.5 * m_iWidth)
			vecStart = vecStart - vecDir.Normalize() * 0.5 * m_iWidth;

		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_BEAMPOINTS);				// beam effect between two points
			WRITE_COORD(vecStart.x);				// coord coord coord (start position) 
			WRITE_COORD(vecStart.y);
			WRITE_COORD(vecStart.z);
			WRITE_COORD(vecEnd.x);					// coord coord coord (end position) 
			WRITE_COORD(vecEnd.y);
			WRITE_COORD(vecEnd.z);
			WRITE_SHORT(m_sTrailSprite);			// short (sprite index) 
			WRITE_BYTE(0);							// byte (starting frame) 
			WRITE_BYTE(10);							// byte (frame rate in 0.1's) 
			WRITE_BYTE(2);							// byte (life in 0.1's) 
			WRITE_BYTE(m_iWidth);					// byte (line width in 0.1's) 
			WRITE_BYTE(0);							// byte (noise amplitude in 0.01's) 
			WRITE_BYTE(255);						// byte,byte,byte (color): no color filters
			WRITE_BYTE(255);
			WRITE_BYTE(255);
			WRITE_BYTE(254);						//byte (brightness)
			WRITE_BYTE(10);							// byte (scroll speed in 0.1's)
		MESSAGE_END();
		vecDir = vecEnd - vecStart;
		current = current->next;
	}

	//draw trail to entity
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMENTPOINT);			// beam effect between point and entity
		WRITE_SHORT(m_iTarget);					// short (start entity) 
		WRITE_COORD(current->vecPoint.x);		// coord coord coord (end position) 
		WRITE_COORD(current->vecPoint.y);
		WRITE_COORD(current->vecPoint.z);
		WRITE_SHORT(m_sTrailSprite);			// short (sprite index) 
		WRITE_BYTE(0);							// byte (starting frame)	
		WRITE_BYTE(10);							// byte (frame rate in 0.1's) 
		WRITE_BYTE(2);							// byte (life in 0.1's) 
		WRITE_BYTE(m_iWidth);					// byte (line width in 0.1's) 
		WRITE_BYTE(0);							// byte (noise amplitude in 0.01's) 
		WRITE_BYTE(255);						// byte,byte,byte (color)
		WRITE_BYTE(255);
		WRITE_BYTE(255);
		WRITE_BYTE(254);						// byte (brightness)
		WRITE_BYTE(10);							// byte (scroll speed in 0.1's)
	MESSAGE_END();
	pev->nextthink = gpGlobals->time + 0.2;
}

void CTrail::AddPoint(Vector point)
{
	TrailList *newpoint = new TrailList;
	newpoint->next = NULL;
	newpoint->vecPoint = point;

	//find last point
	TrailList *last = m_pTrailList;
	while (last->next)
	{
		last = last->next;
	}
	last->next = newpoint;
}

void CTrail::Remove()
{
	TrailList *current = m_pTrailList;
	TrailList *temp;
	while (current->next)
	{
		temp = current->next;
		delete current;
		current = temp;
	}

	UTIL_Remove(this);
}

CTrail *CTrail::CreateTrail(Vector start, int target, short sprite, int width)
{
	CTrail *pTrail = GetClassPtr((CTrail *)NULL);
	pTrail->Spawn();
	pTrail->m_pTrailList->vecPoint = start;
	pTrail->m_sTrailSprite = sprite;
	pTrail->m_iWidth = width;
	pTrail->m_iTarget = target;

	pTrail->SetThink(&CTrail::DrawThink);
	pTrail->pev->nextthink = gpGlobals->time;

	return pTrail;
}