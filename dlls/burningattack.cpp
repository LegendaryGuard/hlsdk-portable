#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "gamerules.h"
#include "effects.h"
#include "controlledbeam.h"

/**
* Burning Attack Weapon
* @version : 31-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CBurningAttack:public CControlledBeam
{	
public:
	/**
	* Init function
	*/
	void Spawn();

	/**
	* Precaches sprites and sounds
	*/
	void Precache();

	/**
	* Get burningattack slot
	* return int: the slot, is 3 for burning attack
	*/
	int iItemSlot( void ) { return 3; }

	/**
	* Get final flash item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

private:
	/**
	* Emits charge sound
	*/
	virtual void EmitChargeSound();

	/**
	* Emits shoot sound
	*/
	virtual void EmitShootSound();
};

LINK_ENTITY_TO_CLASS( weapon_burningattack, CBurningAttack );

void CBurningAttack::Spawn( void )
{
	Precache();
	m_eAttackState = ATTACK_IDLE; //not firing
	m_iCharge = 0;
	m_pMagic = NULL;
	m_iId = WEAPON_BURNINGATTACK;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "sprites/burningattack.spr");
	m_iClip = -1;

	m_iMaxCharge = BURNINGATTACK_MAXCHARGETIME;
	m_iSpeed = BURNINGATTACK_SPEED;
	m_iKiCost = BURNINGATTACK_KI_COST;
	m_iDamage = BURNINGATTACK_DAMAGE;
	m_iRadius = BURNINGATTACK_RADIUS;
	strcpy(m_pAttackName,"burningattack");

	//explosion color
	m_pColor[0] = 255;
	m_pColor[1] = 255;
	m_pColor[2] = 0;

	FallInit();// get ready to fall down.
}

void CBurningAttack::Precache( void )
{
	PRECACHE_MODEL("sprites/burningattack.spr");
	PRECACHE_MODEL("sprites/burningattackstart.spr");
	//PRECACHE_SOUND("vegeta/finalflash.wav");
	//PRECACHE_SOUND("weapons/kame_charge.wav");
	m_sTrail = PRECACHE_MODEL("sprites/burningattacktrail.spr");

	CControlledBeam::Precache();
}

int CBurningAttack::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_BURNINGATTACK;
	p->iWeight = BURNINGATTACK_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CBurningAttack::EmitShootSound()
{	/*
	char shoot_sound[255];
	sprintf( shoot_sound,"%s/finalflash.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);
	*/
}

void CBurningAttack::EmitChargeSound()
{	
}