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

/**
* DeathballWeapon
* @version 31-10-2001
* @author Herwin 'harSens' van Welbergen
*/
class CDeathBall:public CBasePlayerWeapon
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
	* Get  deathball slot
	* return int: the slot, is 5 for deathball
	*/
	int iItemSlot( void ) { return 5; }

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
	BOOL CanHolster() {return !m_pPlayer->m_fCharging;};

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
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged
	CSprite *m_pChargeSprite;		//the sprites wich appears after charging
};

LINK_ENTITY_TO_CLASS( weapon_deathball, CDeathBall );

void CDeathBall::Spawn( void )
{
	Precache();
	m_iId = WEAPON_DEATHBALL;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	FallInit();// get ready to fall down.
}

void CDeathBall::Precache( void )
{
	PRECACHE_MODEL("sprites/deathball.spr");
	//PRECACHE_SOUND("goku/spiritbomb.wav");	
}

int CDeathBall::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_DEATHBALL;
	p->iWeight = DEATHBALL_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CDeathBall::Deploy()
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/deathball.spr", m_pPlayer->pev->origin, true);
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(),1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();	
	return DefaultDeploy( "deathball");	
}


void CDeathBall::Holster(int skiplocal)
{
	if (m_pPlayer->m_fCharging)
	{
		m_pPlayer->m_flSlowDown=1.0;
		m_pPlayer->m_fCharging=false;
		
		m_pChargeSprite->TurnOff();
		//undo drawing charge bar
		MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();		

		//silence sounds
		//EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
	}
	UTIL_Remove(m_pChargeSprite);
}

void CDeathBall::Shoot()	
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;
	
	//undo drawing charge bar
	MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < DEATHBALL_MAXCHARGETIME/3)
	{
		//silence sounds
		//EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
		return;
	}
	m_pChargeSprite->TurnOff();

	//set the right animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	//calculate startpos and speed
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecThrow = gpGlobals->v_forward * DEATHBALL_SPEED;
	
	CMagicAttack::ShootDeathBall(m_pPlayer->pev, vecSrc, vecThrow, m_iCharge * GetPowerRatio() * DEATHBALL_DAMAGE, m_pChargeSprite->pev->scale);	
}

void CDeathBall::WeaponIdle()
{
	if(m_pPlayer->m_fCharging)
	{
		Shoot();
	}
}

void CDeathBall::PrimaryAttack( void )
{
	//delay CHARGETIME sec between shoots
	if (gpGlobals->time - m_flStartShoot<CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	if (!m_pPlayer->m_fCharging)	//start charging
	{
		//make some sounds :-)
		/*char shoot_sound[255];
		sprintf ( shoot_sound,"%s/spiritbomb.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, shoot_sound, 1.0, ATTN_NORM);
		*/

		m_pPlayer->m_fCharging = true;		
		m_pPlayer->m_flSlowDown=0.0;
		m_iCharge=0;		

		//set charge animation 
		m_pPlayer->SetAnimation( PLAYER_CHARGE );		
	}
	else //increase damage
	{
		// Check we have some ammo
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < DEATHBALL_KI_COST )
		{
			if (m_pPlayer->m_fCharging)
				Shoot();

			PlayEmptySound( );
			return;
		}

		// reduce our ammo
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= DEATHBALL_KI_COST;


		if (m_iCharge > DEATHBALL_MAXCHARGETIME)
		{
			Shoot();
		}
		else
		{
			if (m_iCharge == DEATHBALL_MAXCHARGETIME / 3)
			{
				m_pChargeSprite->TurnOn();
			}			
			m_iCharge++;
			if (m_iCharge > DEATHBALL_MAXCHARGETIME / 3)
			{
				m_pChargeSprite->SetScale( (m_iCharge - DEATHBALL_MAXCHARGETIME / 3) * 0.01 );
			}

			//draw charge bar
			MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
				WRITE_BYTE( m_iCharge * 100 / DEATHBALL_MAXCHARGETIME );	//amount charged in %
			MESSAGE_END();
		}
	}	
}