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
* Melee attack system
* @version 24-6-2001
* @author Herwin 'harSens' van Welbergen
*/
class CMelee:public CBasePlayerWeapon
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
	* Get melee slot
	* return int: the slot, is 1 for melee
	*/
	int iItemSlot( void ) { return 1; }

	/**
	* Get melee item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* Punches
	*/
	void PrimaryAttack( void );

	/**
	* Kicks
	*/
	void SecondaryAttack( void );

	/**
	* Deploys the melee attack
	*/
	BOOL Deploy();

private:
	
	float m_flStartShoot;			//shot starting time
	float m_flNextAttack;
};

LINK_ENTITY_TO_CLASS( weapon_melee, CMelee );

void CMelee::Spawn( void )
{
	Precache();
	m_iId = WEAPON_MELEE;
	m_iDefaultAmmo = 0;
	m_iClip = -1;

	FallInit();// get ready to fall down.
}

void CMelee::Precache( void )
{
	PRECACHE_SOUND("weapons/meleemiss.wav");
	PRECACHE_SOUND("weapons/kickhit.wav");
	PRECACHE_SOUND("weapons/punchhit.wav");	
}

int CMelee::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";	//no ki required
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_MELEE;
	p->iWeight = MELEE_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CMelee::Deploy( void )
{
	m_flStartShoot = gpGlobals->time;
	m_flNextAttack = 0;
	return DefaultDeploy( "melee" );	
}


void CMelee::PrimaryAttack( void )
{
	//delay m_flNextAttack secs between shoots
	if (gpGlobals->time - m_flStartShoot < m_flNextAttack)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < MELEE_KI_COST )
	{
		PlayEmptySound();
		return;
	}
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= MELEE_KI_COST;

	if (m_pPlayer->pev->button & IN_BACK)
	{
		m_pPlayer->SetAnimation(PLAYER_BACKFLIP);
		m_flNextAttack = 2.0;
	}
	else
	{
		m_pPlayer->SetAnimation(PLAYER_PUNCH);
		m_flNextAttack = 0.4;

		//check end pos
		TraceResult tr;
		UTIL_MakeVectors (m_pPlayer->pev->v_angle);
		Vector vecSrc	= m_pPlayer->GetGunPosition( );
		Vector vecEnd	= vecSrc + gpGlobals->v_forward * 32;
		UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

		//missed
		if ( tr.flFraction >= 1.0 )
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/meleemiss.wav", 1.0, ATTN_NORM);	
		}
		//hit something
		else
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/punchhit.wav", 1.0, ATTN_NORM);	
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
			//do damage
			pEntity->TakeDamage(m_pPlayer->pev, m_pPlayer->pev, PUNCH_DAMAGE * GetPowerRatio(), DMG_PUNCH);			
		}
	}
}

void CMelee::SecondaryAttack( void )
{
	//delay m_flNextAttack secs between shoots
	if (gpGlobals->time - m_flStartShoot < m_flNextAttack)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < MELEE_KI_COST )
	{
		PlayEmptySound();
		return;
	}
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= MELEE_KI_COST;

	m_pPlayer->SetAnimation(PLAYER_KICK);
	m_flNextAttack = 1;

	//check end pos
	TraceResult tr;
	UTIL_MakeVectors (m_pPlayer->pev->v_angle);
	Vector vecSrc	= m_pPlayer->GetGunPosition( );
	Vector vecEnd	= vecSrc + gpGlobals->v_forward * 64;
	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	//missed
	if ( tr.flFraction >= 1.0 )
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/meleemiss.wav", 1.0, ATTN_NORM);
	}
	//hit something
	else
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/kickhit.wav", 1.0, ATTN_NORM);	
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		//do damage
		pEntity->TakeDamage(m_pPlayer->pev, m_pPlayer->pev, KICK_DAMAGE * GetPowerRatio(), DMG_KICK);		
	}	
}

