#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "effects.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "effects.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "gamerules.h"
#include "../engine/shake.h"

extern int gmsgCharge;

/**
* Solarflare
* @version redo: 14-8-2001
* @author Herwin 'harSens' van Welbergen
*/
class CSolarFlare:public CBasePlayerWeapon
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
	* Get solarflare slot
	* return int: the slot, is 5 for solarflare
	*/
	int iItemSlot( void ) { return 5; }

	/**
	* Get solarflare item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* shoot beam if charging
	*/
	void WeaponIdle( void );

	/**
	* charges a solarflare
	*/
	void PrimaryAttack( void );

	/**
	* Called when sbc is selected as weapon
	* @return bool: true, if deploy succeeded
	*/
	BOOL Deploy();

	/**
	* Can the sbc holster?
	* @return bool: you can holster if you're not charging
	*/
	BOOL CanHolster() {return !m_pPlayer->m_fCharging;};

	/**
	* Holsters the sbc
	* @param int skiplocal:???
	*/
	void Holster(int skiplocal);

	/**
	* Shoots a sbc
	*/
	void Shoot( void );

private:
	int m_fFiring;
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged	
};

LINK_ENTITY_TO_CLASS( weapon_solarflare, CSolarFlare );

void CSolarFlare::Spawn( void )
{
	Precache();
	m_iId = WEAPON_SOLARFLARE;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	m_fFiring = false;
	FallInit();// get ready to fall down.
}

void CSolarFlare::Precache( void )
{
	PRECACHE_SOUND("goku/solarflare.wav");
	PRECACHE_SOUND("cell/solarflare.wav");
	PRECACHE_SOUND("krillin/solarflare.wav");
}

int CSolarFlare::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;					//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_SOLARFLARE;
	p->iWeight = SOLARFLARE_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CSolarFlare::Deploy()
{
	m_flStartShoot = gpGlobals->time;
	return DefaultDeploy("solarflare");
}

void CSolarFlare::Holster(int skiplocal)
{
	if (m_pPlayer->m_fCharging)
	{
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fCharging = false;
		
		//undo drawing charge bar
		MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();
	}	
}

void CSolarFlare::Shoot()	
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;
	
	//undo drawing charge bar
	MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < SOLARFLARE_MAXCHARGETIME / 3)
		return;

	//set the right animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	// make shoot sound
	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/solarflare.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);
	
	//do flash damage
	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	float radius = GetPowerRatio() * m_iCharge * 25;
	RadiusDamage( m_pPlayer->pev->origin + gpGlobals->v_forward * (radius/2 + 40), m_pPlayer->pev, m_pPlayer->pev, 0, radius, CLASS_NONE, DMG_FLASH );

	//fade the screen to white for a short time if not already blinded
	if (!m_pPlayer->m_fBlinded)
		UTIL_ScreenFade( m_pPlayer, Vector(255, 255, 255), 1.0 , 0.5 , 255, FFADE_IN );			

	m_fFiring=false;
	
	//only one solarflare for each player :)
	m_pPlayer->pev->weapons &= ~(1<<WEAPON_SOLARFLARE);
	RetireWeapon();
}

void CSolarFlare::WeaponIdle()
{
	if(m_pPlayer->m_fCharging)
		Shoot();
}		

void CSolarFlare::PrimaryAttack( void )
{
	//delay CHARGETIME secs between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < SOLARFLARE_KI_COST )
	{
		if (m_pPlayer->m_fCharging)
			Shoot();

		PlayEmptySound();
		return;
	}

	// reduce our ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= SOLARFLARE_KI_COST;
	
	if (!m_pPlayer->m_fCharging)	//start charging
	{
		m_pPlayer->m_fCharging = true;		
		m_pPlayer->m_flSlowDown = 0.0;
		m_iCharge = 0;
		m_pPlayer->SetAnimation( PLAYER_CHARGE );
	}
	else //increase damage
	{
		if (m_iCharge > SOLARFLARE_MAXCHARGETIME)
		{
			Shoot();
		}
		else
		{
			m_iCharge++;

			//draw charge bar
			MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
				WRITE_BYTE( m_iCharge * 100 / SOLARFLARE_MAXCHARGETIME );	//amount charged in %
			MESSAGE_END();
		}
	}
}