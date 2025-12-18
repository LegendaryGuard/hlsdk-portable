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
* Final Flash Weapon
* @version : 14-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CFinalFlash:public CControlledBeam
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
	* Get finalflash slot
	* return int: the slot, is 5 for final flash
	*/
	int iItemSlot( void ) { return 5; }

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

	/**
	* Inits charge sprite
	*/
	virtual void InitChargeSprite();

	/**
	* Enable charge sprite
	*/
	virtual void EnableChargeSprite();

	/**
	* Grows the charge sprite
	*/
	virtual void GrowChargeSprite();

	/**
	* Disable charge sprite
	*/
	virtual void DisableChargeSprite();

	/**
	* Remove charge sprite
	*/
	virtual void RemoveChargeSprite() { UTIL_Remove(m_pChargeSprite); };

	CSprite *m_pChargeSprite;		//the sprites wich appears after charging
};

LINK_ENTITY_TO_CLASS( weapon_finalflash, CFinalFlash );

void CFinalFlash::Spawn( void )
{
	Precache();
	m_eAttackState = ATTACK_IDLE; //not firing
	m_iCharge = 0;
	m_pMagic = NULL;
	m_iId = WEAPON_FINALFLASH;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "sprites/finalflash.spr");
	m_iClip = -1;

	m_iMaxCharge = FINALFLASH_MAXCHARGETIME;
	m_iSpeed = FINALFLASH_SPEED;
	m_iKiCost = FINALFLASH_KI_COST;
	m_iDamage = FINALFLASH_DAMAGE;
	m_iRadius = FINALFLASH_RADIUS;
	strcpy(m_pAttackName, "finalflash");

	//explosion color
	m_pColor[0] = 255;
	m_pColor[1] = 255;
	m_pColor[2] = 0;

	FallInit(); // get ready to fall down.
}

void CFinalFlash::Precache( void )
{
	PRECACHE_MODEL("sprites/finalflash.spr");
	PRECACHE_MODEL("sprites/finalflashcharge.spr");
	PRECACHE_MODEL("sprites/finalflashstart.spr");
	PRECACHE_SOUND("vegeta/finalflash.wav");
	//PRECACHE_SOUND("weapons/kame_charge.wav");
	m_sTrail = PRECACHE_MODEL("sprites/finalflashtrail.spr");

	CControlledBeam::Precache();
}

int CFinalFlash::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_FINALFLASH;
	p->iWeight = FINALFLASH_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CFinalFlash::InitChargeSprite( void )
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/finalflashcharge.spr", Vector(0, 0, 0), true);
	//m_pChargeSprite->SetAttachment(m_pPlayer->edict(), 1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->SetScale(0.01);
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();	
}

void CFinalFlash::DisableChargeSprite()
{
	m_pChargeSprite->TurnOff();
	m_pPlayer->pev->renderfx = kRenderFxNone;
}

void CFinalFlash::EnableChargeSprite()
{
	m_pPlayer->pev->renderfx = kRenderFxGlowShell;
	m_pPlayer->pev->rendercolor = Vector (255, 255, 0);
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	UTIL_SetOrigin(m_pChargeSprite->pev, m_pPlayer->GetGunPosition() + 30 * gpGlobals->v_forward);
	m_pChargeSprite->TurnOn();
}

void CFinalFlash::GrowChargeSprite()
{
	m_pChargeSprite->SetScale(0.5 * ((float)m_iCharge / (float)FINALFLASH_MAXCHARGETIME));
}

void CFinalFlash::EmitShootSound()
{
	char shoot_sound[255];
	sprintf( shoot_sound,"%s/finalflash.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);
}

void CFinalFlash::EmitChargeSound()
{	
}