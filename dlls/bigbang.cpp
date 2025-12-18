#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "effects.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "aura.h"
#include "classes.h"
#include "player.h"
#include "gamerules.h"

extern int gmsgCharge;

/**
* Big Bang Weapon
* @version 15-8-2001
* @author Herwin 'harSens' van Welbergen
*/
class CBigBang:public CBasePlayerWeapon
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
	* Get bigbang slot
	* return int: the slot, is 4 for special beam cannon
	*/
	int iItemSlot( void ) { return 4; }

	/**
	* Get bigbang item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* attack iddle
	*/
	void WeaponIdle( void );

	/**
	* charges a bigbang attack
	*/
	void PrimaryAttack( void );

	/**
	* Called when bigbang is selected as weapon
	* @return bool: true, if deploy succeeded
	*/
	BOOL Deploy();

	/**
	* Can the bigbang holster?
	* @return bool: you can holster if you're not charging
	*/
	BOOL CanHolster() { return !m_pPlayer->m_fCharging; }

	/**
	* Holsters the bigbang
	* @param int skiplocal:???
	*/
	void Holster(int skiplocal);

	/**
	* Shoots a bigbang
	*/
	void Shoot( void );

private:
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged	
};

LINK_ENTITY_TO_CLASS( weapon_bigbang, CBigBang );

void CBigBang::Spawn()
{
	Precache();
	m_iId = WEAPON_BIGBANG;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	FallInit(); // get ready to fall down.
}

void CBigBang::Precache()
{
	PRECACHE_MODEL("sprites/bigbang.spr");
	PRECACHE_SOUND("vegeta/bigbang.wav");
}

int CBigBang::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_BIGBANG;
	p->iWeight = BIGBANG_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CBigBang::Deploy()
{
	return DefaultDeploy("bigbang");
}


void CBigBang::Holster(int skiplocal)
{
	if (m_pPlayer->m_fCharging)
	{
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fCharging = false;

		//undo drawing charge bar
		MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev); 
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();
	}
}

void CBigBang::Shoot()
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;

	//undo drawing charge bar
	MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < BIGBANG_MAXCHARGETIME / 3)
		return;

	//set the right animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	//make some sounds :-)
	char shoot_sound[255];
	sprintf(shoot_sound, "%s/bigbang.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()));
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);

	//shoot the attack
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 10;
	Vector vecThrow = gpGlobals->v_forward * BIGBANG_SPEED;
	CMagicAttack::ShootBigBang(m_pPlayer->pev, vecSrc, vecThrow, m_iCharge * BIGBANG_DAMAGE * GetPowerRatio(), (float)m_iCharge / (float)BIGBANG_MAXCHARGETIME);
}

void CBigBang::WeaponIdle()
{
	if (m_pPlayer->m_fCharging)
		Shoot();
}

void CBigBang::PrimaryAttack( void )
{
	//delay CHARGETIME sec between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	if (!m_pPlayer->m_fCharging)	//start charging
	{
		m_pPlayer->m_fCharging = true;
		m_pPlayer->m_flSlowDown = 0.0;
		m_iCharge = 0;

		m_pPlayer->SetAnimation(PLAYER_CHARGE);
	}
	else //increase damage
	{
		// Check we have some ammo
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < BIGBANG_KI_COST)
		{
			if (m_pPlayer->m_fCharging)
				Shoot();

			PlayEmptySound( );
			return;
		}

		// reduce our ammo
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= BIGBANG_KI_COST;


		if (m_iCharge>BIGBANG_MAXCHARGETIME)
			Shoot();
		else
		{
			m_iCharge++;

			//draw charge bar
			MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev); 
				WRITE_BYTE(m_iCharge * 100 / BIGBANG_MAXCHARGETIME);	//amount charged in %
			MESSAGE_END();			
		}
	}	
}