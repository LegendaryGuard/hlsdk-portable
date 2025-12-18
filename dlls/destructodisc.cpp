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
/**
* Destructo Disc Weapon
* @version 13-4-2001
* @author Herwin 'harSens' van Welbergen
*/
class CDestructoDisc:public CBasePlayerWeapon
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
	* Get destructo disc slot
	* return int: the slot, is 3 for destructo disc
	*/
	int iItemSlot( void ) { return 4; }

	/**
	* Get destructodisc item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* Shoots a destructo disc
	*/
	void PrimaryAttack( void );
	
	/**
	* Creates a destructo disc
	*/
	void SecondaryAttack( void );

	/**
	* When the weapon is in idle :)
	*/
	void WeaponIdle();

	/**
	* Called when destructo disc is selected as weapon
	*/
	BOOL Deploy();

private:
	float m_flStartCharging;
	enum {DISC_IDLE,DISC_CREATING,DISC_READY} m_eDiscStatus;
};

LINK_ENTITY_TO_CLASS( weapon_destructodisc, CDestructoDisc );

void CDestructoDisc::Spawn( void )
{
	Precache();
	m_iId = WEAPON_DESTRUCTODISC;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "models/w_destructodisc.mdl");
	m_iClip = -1;

	FallInit();// get ready to fall down.
}

void CDestructoDisc::Precache( void )
{
	PRECACHE_MODEL("models/w_destructodisc.mdl");
	PRECACHE_SOUND("krillin/destructodisc.wav");
	PRECACHE_SOUND("weapons/rotating.wav");
}

int CDestructoDisc::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_DESTRUCTODISC;
	p->iWeight = DESTRUCTODISC_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CDestructoDisc::Deploy()
{
	m_eDiscStatus = DISC_IDLE;
	//return DefaultDeploy( "models/v_destructodisc.mdl", "models/p_destructodisc.mdl", DESTRUCTODISC_DRAW, "destructodisc" );
	return true;
}

void CDestructoDisc::WeaponIdle()
{
	if (m_eDiscStatus == DISC_CREATING)
	{
		// wait 3 secs, before full charge
		if (gpGlobals->time-m_flStartCharging>3)
		{
			m_flStartCharging=gpGlobals->time;
			// increase speed again
			m_pPlayer->m_flSlowDown = 0.5;
			m_eDiscStatus = DISC_READY;
		}
	}	
}
void CDestructoDisc::PrimaryAttack( void )
{
	//if not ready, we can't shoot it :)
	if (m_eDiscStatus != DISC_READY)
		return;

	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/destructodisc.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);
	m_pPlayer->m_flSlowDown = 1.0;
	
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc = m_pPlayer->GetGunPosition();

	Vector vecThrow = gpGlobals->v_forward * DESTRUCTODISC_SPEED;
	CMagicAttack::ShootDestructoDisc( m_pPlayer->pev, vecSrc, vecThrow);		
	m_eDiscStatus = DISC_IDLE;
}

void CDestructoDisc::SecondaryAttack( void )
{
	if (m_eDiscStatus == DISC_READY)
		return;

	if (m_eDiscStatus == DISC_IDLE)
	{
		// Check we have some ammo
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < DESTRUCTODISC_KI_COST )
		{
			PlayEmptySound( );
			return;
		}
	
		// reduce our ammo
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= DESTRUCTODISC_KI_COST;
		m_eDiscStatus = DISC_CREATING;
		m_pPlayer->m_flSlowDown = 0.0;
		m_flStartCharging = gpGlobals->time;
	}
	else if (m_eDiscStatus == DISC_CREATING)
	{
		// wait 3 secs, before full charge
		if (gpGlobals->time-m_flStartCharging>3)
		{
			m_flStartCharging=gpGlobals->time;
			// increase speed again
			m_pPlayer->m_flSlowDown = 0.5;
			m_eDiscStatus = DISC_READY;
		}
	}		
}