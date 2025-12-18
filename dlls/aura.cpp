#include "extdll.h" 
#include "decals.h" 
#include "util.h" 
#include "cbase.h" 
#include "monsters.h" 
#include "weapons.h" 
#include "soundent.h" 
#include "shake.h" 
#include "gamerules.h" 
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"

const Vector vecAuraColor[] =
{
	Vector(0, 0, 255),
	Vector(255, 0, 0),
	Vector(255, 255, 0),
	Vector(0, 255, 0),
	Vector(200, 0, 255),
};

LINK_ENTITY_TO_CLASS(aura, CAura);

void CAura::Spawn()
{
	Precache();
	SetTouch(NULL);	
	SET_MODEL(ENT(pev), "models/aura.mdl");

	pev->velocity = Vector(0, 0, 0);	
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;

	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 100;
	//pev->rendercolor = Vector(0, 0, 100);
	//pev->renderfx = kRenderFxGlowShell;

	//add sprite
	m_pAuraSprite =	CSprite::SpriteCreate("sprites/aura.spr", Vector(0, 0, 0), true);
	m_pAuraSprite->SetAttachment(ENT(pev), 0);
	m_pAuraSprite->SetTransparency(kRenderTransAdd, 230, 230, 230, 75, kRenderFxPulseFast);
	m_pAuraSprite->SetScale(2.0);

	pev->gaitsequence = 0;
	pev->sequence = 0;
	pev->frame = 0;

	ResetSequenceInfo();	
	m_fHasTrail = false;
}

void CAura::Precache()
{
	PRECACHE_MODEL("models/aura.mdl");
	PRECACHE_MODEL("sprites/aura.spr");
	PRECACHE_SOUND("weapons/aura.wav");
	m_sTrailSprite = PRECACHE_MODEL("sprites/auratrail.spr");
}

void CAura::RemoveThink()
{
	entvars_t *pevOwner;
	if (pev->owner)
	{
		pevOwner = VARS(pev->owner);
		if (pevOwner->solid == SOLID_NOT)
			Destroy();
	}
	else
		Destroy();

	if (m_pPlayer->m_fFlying)
	{
		pev->angles = m_pPlayer->pev->v_angle;
		if (!m_fHasTrail)
		{
			m_fHasTrail = true;
			Vector vecTrailColor = vecAuraColor[pev->skin];
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(TE_BEAMFOLLOW);			//create a line of decaying beam segments until entity stops moving
				WRITE_SHORT(entindex() | 0x1000);		//entity:attachment to follow)
				WRITE_SHORT(m_sTrailSprite);			//short (sprite index)
				WRITE_BYTE(25);						//life in 0.1's) 
				WRITE_BYTE(25);						//(line width in 0.1's) 
				WRITE_BYTE(vecTrailColor.x);			//byte,byte,byte (color)
				WRITE_BYTE(vecTrailColor.y);			
				WRITE_BYTE(vecTrailColor.z);
				WRITE_BYTE(254);						//brightness
			MESSAGE_END();
		}
	}

	if (!m_pPlayer->m_fFlying && m_fHasTrail)
	{
		m_fHasTrail = false;
		// remove all trails from entity
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_KILLBEAM);	//kill all beams attached to entity
			WRITE_SHORT(m_pPlayer->entindex());	// short (entity)
		MESSAGE_END();
	}

	pev->nextthink=gpGlobals->time + 0.5;
}

void CAura::SetAnimation(const char *animation)
{
	int animDesired = LookupSequence(animation);
	if (animDesired == -1)
		animDesired = 0;			

	// Already using the desired animation?
	if (pev->sequence == animDesired)
		return;

	pev->gaitsequence = 0;
	pev->sequence = animDesired;
	pev->frame = 0;
	ResetSequenceInfo();
}

CAura *CAura::Create(CBasePlayer *pevOwner)
{
	CAura *pAura = GetClassPtr((CAura *)NULL);
	pAura->Spawn();
	UTIL_SetOrigin(pAura->pev, pevOwner->pev->origin);
	pAura->m_pPlayer = pevOwner;
	pAura->pev->owner = pevOwner->edict();
	pAura->SetThink(&CAura::RemoveThink);
	pAura->pev->nextthink = gpGlobals->time + 0.5;

	pAura->pev->aiment = pevOwner->edict();
	pAura->pev->movetype = MOVETYPE_FOLLOW;
	pAura->pev->skin = pAura->m_pPlayer->m_pClass->GetAuraSkin();

	//emit aura sound
	EMIT_SOUND(ENT(pAura->pev), CHAN_WEAPON, "weapons/aura.wav", 1, ATTN_NORM);
	return pAura;	
}

void CAura::Destroy()
{
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", 1, ATTN_NORM);
	if (m_fHasTrail)
	{
		// remove all trails from entity
		MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_KILLBEAM);	//kill all beams attached to entity
			WRITE_SHORT(m_pPlayer->entindex());	// short (entity)
		MESSAGE_END();
	}
	UTIL_Remove(m_pAuraSprite);
	UTIL_Remove(this);
}