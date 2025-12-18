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
#include "controlledbeam.h"

/**
* Masenko Weapon
* @version 31-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CMasenko:public CControlledBeam
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
	* Get masenko slot
	* return int: the slot, is 5 for masenko
	*/
	int iItemSlot( void ) { return 5; }

	/**
	* Get masenko item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

private:
	CSprite *m_pChargeSprite;		//the sprites wich appears after charging

	/**
	* Emits charge sound
	*/
	virtual void EmitChargeSound();

	/**
	* Emits shoot sound
	*/
	virtual void EmitShootSound();

	/**
	* Inits charge sprite
	*/
	virtual void InitChargeSprite();

	/**
	* Grow charge sprite
	*/
	virtual void GrowChargeSprite(){m_pChargeSprite->SetScale((float)m_iCharge * 0.015);};

	/**
	* Enable charge sprite
	*/
	virtual void EnableChargeSprite(){m_pChargeSprite->TurnOn();};

	/**
	* Disable charge sprite
	*/
	virtual void DisableChargeSprite(){m_pChargeSprite->TurnOff();};

	/**
	* Remove charge sprite
	*/
	virtual void RemoveChargeSprite(){UTIL_Remove(m_pChargeSprite);};
};

LINK_ENTITY_TO_CLASS( weapon_masenko, CMasenko);

void CMasenko::Spawn( void )
{
	Precache();
	m_eAttackState = ATTACK_IDLE; //not firing
	m_iCharge = 0;
	m_pMagic = NULL;
	m_iId = WEAPON_MASENKO;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "sprites/masenko.spr");
	m_iClip = -1;

	m_iMaxCharge = MASENKO_MAXCHARGETIME;
	m_iSpeed = MASENKO_SPEED;
	m_iKiCost = MASENKO_KI_COST;
	m_iDamage = MASENKO_DAMAGE;
	m_iRadius = MASENKO_RADIUS;
	strcpy(m_pAttackName, "masenko");

	//explosion color
	m_pColor[0] = 200;
	m_pColor[1] = 200;
	m_pColor[2] = 0;

	FallInit();// get ready to fall down.
}

void CMasenko::Precache( void )
{
	PRECACHE_MODEL("sprites/masenko.spr");
	PRECACHE_MODEL("sprites/masenkocharge.spr");
	PRECACHE_MODEL("sprites/masenkostart.spr");
	PRECACHE_SOUND("piccolo/masenko.wav");
	PRECACHE_SOUND("piccolo/ha.wav");
	PRECACHE_SOUND("weapons/mase_charge.wav");
	m_sTrail = PRECACHE_MODEL("sprites/masenkotrail.spr");

	CControlledBeam::Precache();
}

int CMasenko::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_MASENKO;
	p->iWeight = MASENKO_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CMasenko::InitChargeSprite( void )
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/masenkocharge.spr", m_pPlayer->pev->origin, true );
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(), 1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->SetScale(0.1);
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();	
}

void CMasenko::EmitShootSound()
{
	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/ha.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);
}

void CMasenko::EmitChargeSound()
{
	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/masenko.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/mase_charge.wav", 1.0, ATTN_NORM);
}