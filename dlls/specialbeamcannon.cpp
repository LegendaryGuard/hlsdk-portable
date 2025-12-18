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

extern int gmsgCharge;

/**
* Special Beam Cannon Weapon
* @version 17-6-2001
* @author Herwin 'harSens' van Welbergen
*/
class CSpecialBeamCannon:public CBasePlayerWeapon
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
	* Get specialbeamcannon slot
	* return int: the slot, is 3 for special beam cannon
	*/
	int iItemSlot( void ) { return 3; }

	/**
	* Get special beam cannon item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* shoot beam if charging
	*/
	void WeaponIdle( void );

	/**
	* charges a special beam cannon attack
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
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged
	CMagicAttack *m_pBeamCannon;	//the magic attack created after firing	
	CSprite *m_pChargeSprite;		//the sprites wich appears after charging
	short m_sTrail;					//the trail sprite
};

LINK_ENTITY_TO_CLASS( weapon_specialbeamcannon, CSpecialBeamCannon );

void CSpecialBeamCannon::Spawn( void )
{
	Precache();
	m_iId = WEAPON_SPECIALBEAMCANNON;
	m_iDefaultAmmo = 0;
	SET_MODEL(ENT(pev), "models/w_beamcannon.mdl");	
	m_iClip = -1;
	FallInit();// get ready to fall down.
}

void CSpecialBeamCannon::Precache( void )
{
	PRECACHE_MODEL("models/w_beamcannon.mdl");
	PRECACHE_SOUND("piccolo/specialbeamcannon.wav");
	PRECACHE_MODEL("sprites/sbccharge.spr");
	m_sTrail = PRECACHE_MODEL("sprites/sbctrail.spr");
}

int CSpecialBeamCannon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_SPECIALBEAMCANNON;
	p->iWeight = SPECIALBEAMCANNON_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CSpecialBeamCannon::Deploy()
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/sbccharge.spr", m_pPlayer->pev->origin, true);
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(),1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->SetScale(0.2);
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();
	m_flStartShoot = gpGlobals->time;
	return DefaultDeploy( "specialbeamcannon");
}

void CSpecialBeamCannon::Holster(int skiplocal)
{
	if (m_pPlayer->m_fCharging)
	{
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fCharging = false;
		
		//turn of charging sprite
		m_pChargeSprite->TurnOff();

		//undo drawing charge bar
		MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();
	}
	UTIL_Remove(m_pChargeSprite);
}

void CSpecialBeamCannon::Shoot()	
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;
	
	//undo drawing charge bar
	MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < SPECIALBEAMCANNON_MAXCHARGETIME / 3)
		return;

	//turn the charging sprite off
	m_pChargeSprite->TurnOff();

	//set the right animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	//make some sounds :-)
	char shoot_sound[255];
	sprintf ( shoot_sound,"%s/specialbeamcannon.wav", CBaseClass::GetName(m_pPlayer->m_pClass->Classify()) );
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, shoot_sound, 1.0, ATTN_NORM);

	//calculate startpos and speed
	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 30;
	Vector vecThrow = gpGlobals->v_forward * SPECIALBEAMCANNON_SPEED;
	
	//shoot the magic
	m_pBeamCannon = CMagicAttack::ShootWave( m_pPlayer->pev, vecSrc, vecThrow, m_iCharge * 0.5 * SPECIALBEAMCANNON_DAMAGE * GetPowerRatio(),"beamcannon", 225, 125, 0, m_sTrail, SPECIALBEAMCANNON_RADIUS);
	CMagicAttack::ShootOrbitBall( m_pBeamCannon->pev, m_pPlayer->pev, vecSrc + gpGlobals->v_right * 10, vecThrow, m_iCharge * 0.5 * SPECIALBEAMCANNON_DAMAGE * GetPowerRatio(),225, 125, 0, m_sTrail, SPECIALBEAMCANNON_RADIUS);
}

void CSpecialBeamCannon::WeaponIdle()
{
	if (m_pPlayer->m_fCharging)
		Shoot();
}		

void CSpecialBeamCannon::PrimaryAttack( void )
{
	//delay CHARGETIME secs between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < SPECIALBEAMCANNON_KI_COST )
	{
		if (m_pPlayer->m_fCharging)
			Shoot();

		PlayEmptySound( );
		return;
	}

	// reduce our ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= SPECIALBEAMCANNON_KI_COST;
	
	if (!m_pPlayer->m_fCharging)	//start charging
	{
		m_pPlayer->m_fCharging = true;		
		m_pPlayer->m_flSlowDown = 0.0;
		m_iCharge = 0;
		m_pPlayer->SetAnimation( PLAYER_CHARGE );
	}
	else //increase damage
	{
		if (m_iCharge > SPECIALBEAMCANNON_MAXCHARGETIME)
		{
			Shoot();
		}
		else
		{	m_iCharge++;
			
			//draw charge bar
			MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
				WRITE_BYTE( m_iCharge * 100 / SPECIALBEAMCANNON_MAXCHARGETIME );	//amount charged in %
			MESSAGE_END(); 

			if (m_iCharge == SPECIALBEAMCANNON_MAXCHARGETIME / 3)
			{
				m_pChargeSprite->TurnOn();
			}
		}
	}
}