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

extern int gmsgCharge;
extern int gmsgChargeSB;
extern int gmsgStopSBCharge;

/**
* Spirit Bomb Weapon
* @version 23-7-2001
* @author Herwin 'harSens' van Welbergen
*/
class CSpiritBomb:public CBasePlayerWeapon
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
	* Get  spiritbomb slot
	* return int: the slot, is 4 for special beam cannon
	*/
	int iItemSlot( void ) { return 4; }

	/**
	* Get spirit bomb item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* shoot beam if charging
	*/
	void WeaponIdle( void );

	/**
	* charges a spirit bomb attack
	*/
	void PrimaryAttack( void );

	/**
	* Called when spirit bomb is selected as weapon
	* @return bool: true, if deploy succeeded
	*/
	BOOL Deploy();

	/**
	* Can the spirit bomb holster?
	* @return bool: you can holster if you're not charging
	*/
	BOOL CanHolster() { return !m_pPlayer->m_fCharging; };

	/**
	* Holsters the sbc
	* @param int skiplocal:???
	*/
	void Holster(int skiplocal);

	/**
	* Shoots a spirit bomb
	*/
	void Shoot( void );

private:
	//short m_sCharger;				//the trail for the spirit bomb loading points
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged
	CMagicAttack* m_pSpiritBomb;	//the spirit bomb attack
};

LINK_ENTITY_TO_CLASS( weapon_spiritbomb, CSpiritBomb );

void CSpiritBomb::Spawn( void )
{
	Precache();
	m_iId = WEAPON_SPIRITBOMB;
	m_pSpiritBomb = NULL;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	FallInit();// get ready to fall down.
}

void CSpiritBomb::Precache( void )
{
	PRECACHE_MODEL("sprites/spiritbomb.spr");
	PRECACHE_SOUND("goku/spiritbomb.wav");	
}

int CSpiritBomb::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_SPIRITBOMB;
	p->iWeight = SPIRITBOMB_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CSpiritBomb::Deploy()
{
	return DefaultDeploy( "spiritbomb" );
}


void CSpiritBomb::Holster(int skiplocal)
{
	if (m_pPlayer->m_fCharging)
	{
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fCharging = false;

		//undo drawing charge bar
		MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();

		//stop the movement of the sprites
		MESSAGE_BEGIN( MSG_ONE, gmsgStopSBCharge, NULL, m_pPlayer->pev );			
		MESSAGE_END();

		//remove the bomb sprite
		m_pSpiritBomb->Remove();
		m_pSpiritBomb = NULL;

		//silence sounds
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
	}
}

void CSpiritBomb::Shoot()
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;
	
	//undo drawing charge bar
	MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//stop the movement of the sprites
	MESSAGE_BEGIN( MSG_ONE, gmsgStopSBCharge, NULL, m_pPlayer->pev );			
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < SPIRITBOMB_MAXCHARGETIME/3)
	{	m_pSpiritBomb->Remove();
		m_pSpiritBomb = NULL;
		
		//silence sounds
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
		return;
	}

	//set the right animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	//calculate crosshair pos
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition();
	TraceResult tr;
	UTIL_TraceLine (vecSrc,vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(pev), &tr );
	
	vecAiming = tr.vecEndPos - m_pSpiritBomb->pev->origin;

	m_pSpiritBomb->ShootSpiritBomb(vecAiming.Normalize() * SPIRITBOMB_SPEED, m_iCharge * GetPowerRatio() * SPIRITBOMB_DAMAGE);
}

void CSpiritBomb::WeaponIdle()
{
	if (m_pPlayer->m_fCharging)
		Shoot();
}		

void CSpiritBomb::PrimaryAttack( void )
{
	//delay CHARGETIME sec between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	if (!m_pPlayer->m_fCharging)	//start charging
	{
		//check if we spirit bomb pos is free
		UTIL_MakeVectors( m_pPlayer->pev->angles );
		Vector vecBombPos = m_pPlayer->pev->origin + Vector(0,0,200);
		TraceResult tr;
		UTIL_TraceLine(m_pPlayer->GetGunPosition(),vecBombPos,ignore_monsters,ENT(m_pPlayer->pev),&tr);
		if (tr.flFraction < 1)
			return;

		//make some sounds :-)
		char shoot_sound[255];
		sprintf ( shoot_sound,"%s/spiritbomb.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);

		m_pPlayer->m_fCharging = true;		
		m_pPlayer->m_flSlowDown = 0.0;
		m_iCharge = 0;

		m_pSpiritBomb = CMagicAttack::CreateSpiritBomb(m_pPlayer->pev, vecBombPos);

		//enable charge sprites
		MESSAGE_BEGIN( MSG_ONE, gmsgChargeSB, NULL, m_pPlayer->pev);
			WRITE_COORD(m_pSpiritBomb->pev->origin.x);
			WRITE_COORD(m_pSpiritBomb->pev->origin.y);
			WRITE_COORD(m_pSpiritBomb->pev->origin.z);
		MESSAGE_END();
		
		//set charge animation
		m_pPlayer->SetAnimation( PLAYER_CHARGE );		
	}
	else //increase damage
	{
		// Check we have some ammo
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < SPIRITBOMB_KI_COST )
		{
			if (m_pPlayer->m_fCharging)
				Shoot();

			PlayEmptySound();
			return;
		}

		// reduce our ammo
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= SPIRITBOMB_KI_COST;

		if (m_iCharge > SPIRITBOMB_MAXCHARGETIME)
		{
			Shoot();
		}
		else
		{
			if(m_iCharge % 2) 
			{
				m_pSpiritBomb->Grow(); 
			}

			m_iCharge++;
			
			//draw charge bar
			MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
				WRITE_BYTE( m_iCharge * 100 / SPIRITBOMB_MAXCHARGETIME );	//amount charged in %
			MESSAGE_END();
		}
	}
}