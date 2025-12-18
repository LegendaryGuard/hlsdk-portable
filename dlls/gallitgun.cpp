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
* Gallit Gun Weapon
* @version : 22-7-2001
* @author Herwin 'harSens' van Welbergen
*/
class CGallitGun:public CControlledBeam
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
	* Get gallit gun slot
	* return int: the slot, is 2 for gallit gun
	*/
	int iItemSlot( void ) { return 3; }

	/**
	* Get gallit gun item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

private:
	CSprite *m_pChargeSprite;		//the sprites wich appears after charging

	/**
	* Emits shoot sound
	*/
	virtual void EmitShootSound();

	/**
	* Emits charge sound
	*/
	virtual void EmitChargeSound();

	/**
	* Inits charge sprite
	*/
	virtual void InitChargeSprite();

	/**
	* Grow charge sprite
	*/
	virtual void GrowChargeSprite() { m_pChargeSprite->SetScale((float)m_iCharge * 0.015); };

	/**
	* Enable charge sprite
	*/
	virtual void EnableChargeSprite() { m_pChargeSprite->TurnOn(); };

	/**
	* Disable charge sprite
	*/
	virtual void DisableChargeSprite() { m_pChargeSprite->TurnOff(); };

	/**
	* Remove charge sprite
	*/
	virtual void RemoveChargeSprite() { UTIL_Remove(m_pChargeSprite); };
};

LINK_ENTITY_TO_CLASS( weapon_gallitgun, CGallitGun );

void CGallitGun::Spawn( void )
{
	Precache();
	m_eAttackState = ATTACK_IDLE; //not firing
	m_iCharge = 0;
	m_pMagic = NULL;
	m_iId = WEAPON_GALLITGUN;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "sprites/gallitgun.spr");
	m_iClip = -1;

	m_iMaxCharge = GALLITGUN_MAXCHARGETIME;
	m_iSpeed = GALLITGUN_SPEED;
	m_iKiCost = GALLITGUN_KI_COST;
	m_iDamage = GALLITGUN_DAMAGE;
	m_iRadius = GALLITGUN_RADIUS;
	strcpy(m_pAttackName,"gallitgun");
	

	//explosion color
	m_pColor[0] = 200;
	m_pColor[1] = 100;
	m_pColor[2] = 100;

	FallInit();// get ready to fall down.
}

void CGallitGun::Precache( void )
{
	PRECACHE_MODEL("sprites/gallitgun.spr");
	PRECACHE_MODEL("sprites/kamecharge.spr");
	PRECACHE_MODEL("sprites/gallitgunstart.spr");
	PRECACHE_SOUND("cell/gallitgun.wav");	
	PRECACHE_SOUND("vegeta/gallitgun.wav");
	PRECACHE_SOUND("weapons/kame_charge.wav");
	m_sTrail = PRECACHE_MODEL("sprites/gallitguntrail.spr");

	CControlledBeam::Precache();
}

int CGallitGun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_GALLITGUN;
	p->iWeight = GALLITGUN_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CGallitGun::InitChargeSprite( void )
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/kamecharge.spr", m_pPlayer->pev->origin, true);
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(), 1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->SetScale(0.2);
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();	
}

void CGallitGun::EmitShootSound()
{
	char shoot_sound[255];
	sprintf ( shoot_sound, "%s/gallitgun.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);	
}

void CGallitGun::EmitChargeSound()
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/kame_charge.wav", 1.0, ATTN_NORM);
}