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
* Kamehameha Weapon
* @version redo: 13-7-2001
* @author Herwin 'harSens' van Welbergen
*/
class CKamehameha:public CControlledBeam
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
	* Get kamehameha slot
	* return int: the slot, is 2 for kamehameha
	*/
	int iItemSlot( void ) { return 3; }

	/**
	* Get kamehameha item info
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
	* Enable charge sprite
	*/
	virtual void EnableChargeSprite(){m_pChargeSprite->TurnOn();};

	/**
	* Disable charge sprite
	*/
	virtual void DisableChargeSprite(){m_pChargeSprite->TurnOff();};

	/**
	* Grow charge sprite
	*/
	virtual void GrowChargeSprite(){m_pChargeSprite->SetScale((float)m_iCharge * 0.015);};

	/**
	* Remove charge sprite
	*/
	virtual void RemoveChargeSprite(){UTIL_Remove(m_pChargeSprite);};
};

LINK_ENTITY_TO_CLASS( weapon_kamehameha, CKamehameha );

void CKamehameha::Spawn( void )
{
	Precache();
	m_eAttackState = ATTACK_IDLE; //not firing
	m_iCharge = 0;
	m_pMagic = NULL;
	m_iId = WEAPON_KAMEHAMEHA;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "sprites/kamehameha.spr");
	m_iClip = -1;

	m_iMaxCharge = KAMEHAMEHA_MAXCHARGETIME;
	m_iSpeed = KAMEHAMEHA_SPEED;
	m_iKiCost = KAMEHAMEHA_KI_COST;
	m_iDamage = KAMEHAMEHA_DAMAGE;
	m_iRadius = KAMEHAMEHA_RADIUS;
	strcpy(m_pAttackName,"kamehameha");

	//explosion color
	m_pColor[0] = 100;
	m_pColor[1] = 100;
	m_pColor[2] = 200;

	FallInit();// get ready to fall down.
}

void CKamehameha::Precache( void )
{
	PRECACHE_MODEL("sprites/kamehameha.spr");
	PRECACHE_MODEL("sprites/kamecharge.spr");
	PRECACHE_MODEL("sprites/kamehamehastart.spr");
	PRECACHE_SOUND("goku/kamehame.wav");
	PRECACHE_SOUND("goku/ha.wav");
	PRECACHE_SOUND("weapons/kame_charge.wav");
	m_sTrail = PRECACHE_MODEL("sprites/kamehamehatrail.spr");
	
	CControlledBeam::Precache();
}

int CKamehameha::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_KAMEHAMEHA;
	p->iWeight = KAMEHAMEHA_WEIGHT;
	p->iFlags = 0;
	return 1;
}

void CKamehameha::InitChargeSprite( void )
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/kamecharge.spr", m_pPlayer->pev->origin, true);
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(),1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->SetScale(0.1);
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();	
}

void CKamehameha::EmitShootSound()
{
	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/ha.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()));
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);
}

void CKamehameha::EmitChargeSound()
{
	char charge_sound[255];
	sprintf ( charge_sound,"%s/kamehame.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()));
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, charge_sound, 1.0, ATTN_NORM);	
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/kame_charge.wav", 1.0, ATTN_NORM);
}