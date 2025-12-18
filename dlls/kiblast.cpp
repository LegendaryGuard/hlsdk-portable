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
* Ki-blast Weapon
* @version 9-7-2001
* @author Herwin 'harSens' van Welbergen
*/
class CKiBlast:public CBasePlayerWeapon
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
	* Get kiblast slot
	* return int: the slot, is 2 for kiblast
	*/
	int iItemSlot( void ) { return 2; }

	/**
	* Get kiblast item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* Shoots a kiblast attack
	*/
	void PrimaryAttack( void );

	/**
	* Shoots a secundairy kiblast
	*/
	void SecondaryAttack(void);

	/**
	* Called when kiblast is selected as weapon
	*/
	BOOL Deploy();

private:
	float m_flStartShoot;			//shot starting time
};

LINK_ENTITY_TO_CLASS( weapon_kiblast, CKiBlast );

void CKiBlast::Spawn( void )
{
	Precache();
	m_iId = WEAPON_KIBLAST;
	m_iDefaultAmmo = 0;
	m_iClip = -1;

	FallInit();// get ready to fall down.
}

void CKiBlast::Precache( void )
{
	PRECACHE_MODEL("sprites/kiblast.spr");
	PRECACHE_SOUND("weapons/kiblast.wav");
	PRECACHE_SOUND("weapons/kiplosion.wav");
}

int CKiBlast::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_KIBLAST;
	p->iWeight = KIBLAST_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CKiBlast::Deploy( void )
{
	m_flStartShoot = gpGlobals->time;
	return DefaultDeploy( "kiblast" );	
}


void CKiBlast::PrimaryAttack( void )
{
	//delay 0.6 sec between shoots
	if (gpGlobals->time - m_flStartShoot < 0.6)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < KIBLAST_KI_COST )
	{
		PlayEmptySound();
		return;
	}

	// reduce our ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= KIBLAST_KI_COST;

	//emit sound
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/kiblast.wav", 1.0, ATTN_NORM);

	//play animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	//shoot it
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 10; 
	Vector vecThrow = gpGlobals->v_forward * KIBLAST_SPEED;
	CMagicAttack::ShootKiBlast( m_pPlayer->pev, vecSrc, vecThrow, KIBLAST_DAMAGE * GetPowerRatio());		
}

void CKiBlast::SecondaryAttack( void )
{
	//delay 0.4 sec between shoots
	if (gpGlobals->time - m_flStartShoot < 0.4)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < KIBLAST_KI_COST )
	{
		PlayEmptySound();
		return;
	}

	// reduce our ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= KIBLAST_KI_COST;

	//emit sound
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/kiblast.wav", 1.0, ATTN_NORM);

	//shoot it
	int right = RANDOM_LONG(-30, 30);
	int up = RANDOM_LONG(-30, 30);

	//choose shoot animation, shoot origin
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc;
	if (right > 0)
	{
		vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * 10; 
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	}
	else
	{
		vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_right * -10; 
		m_pPlayer->SetAnimation( PLAYER_ATTACK2 );
	}
	
	Vector vecThrow = gpGlobals->v_forward * KIBLAST_SPEED + gpGlobals->v_right * right + gpGlobals->v_up * up;
	CMagicAttack::ShootKiBlast( m_pPlayer->pev, vecSrc, vecThrow, KIBLAST_DAMAGE * GetPowerRatio());
}