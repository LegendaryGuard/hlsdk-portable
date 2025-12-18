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
extern int gmsgExplosion;
/**
* Finger Laser weapon
* @version 8-9-2001
* @author Herwin 'harSens' van Welbergen
*/
class CFingerLaser:public CBasePlayerWeapon
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
	* Get fingerlaser slot
	* return int: the slot, is 3 for fingerlaser
	*/
	int iItemSlot( void ) { return 3; }

	/**
	* Get fingerlaser cannon item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* shoot fingerlaser if charging
	*/
	void WeaponIdle( void );

	/**
	* charges a fingerlaser attack
	*/
	void PrimaryAttack( void );

	/**
	* Called when fingerlaser is selected as weapon
	* @return bool: true, if deploy succeeded
	*/
	BOOL Deploy();

	/**
	* Can the fingerlaser holster?
	* @return bool: you can holster if you're not charging
	*/
	BOOL CanHolster() {return !m_pPlayer->m_fCharging;};

	/**
	* Holsters the fingerlaser
	* @param int skiplocal:???
	*/
	void Holster(int skiplocal);

	/**
	* Shoots the laser
	*/
	void Shoot( void );

private:
	float m_flStartShoot;			//shot starting time
	int m_iCharge;					//how far is it charged
	CSprite *m_pChargeSprite;		//the sprites wich appears after charging	
};

LINK_ENTITY_TO_CLASS( weapon_fingerlaser, CFingerLaser );

void CFingerLaser::Spawn( void )
{
	Precache();
	m_iId = WEAPON_FINGERLASER;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	FallInit();// get ready to fall down.
}

void CFingerLaser::Precache( void )
{
	PRECACHE_MODEL("sprites/fingerlasercharge.spr");
	PRECACHE_SOUND("weapons/fingerlaser.wav");
	PRECACHE_SOUND("weapons/fingerlasercharge.wav");
}

int CFingerLaser::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 2;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_FINGERLASER;
	p->iWeight = FINGERLASER_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CFingerLaser::Deploy( void )
{
	m_pChargeSprite = CSprite::SpriteCreate( "sprites/fingerlasercharge.spr", m_pPlayer->pev->origin, true);
	m_pChargeSprite->SetAttachment(m_pPlayer->edict(), 1);
	m_pChargeSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 254, kRenderFxGlowShell );
	m_pChargeSprite->pev->framerate = 10;
	m_pChargeSprite->TurnOff();
	m_flStartShoot = gpGlobals->time;
	return DefaultDeploy( "fingerlaser");
}

void CFingerLaser::Holster(int skiplocal)
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

		//silence sounds
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
	}
	UTIL_Remove(m_pChargeSprite);
}

void CFingerLaser::Shoot()	
{
	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fCharging = false;
	
	//undo drawing charge bar
	MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
		WRITE_BYTE(0);	//disable it
	MESSAGE_END();

	//prevent mass shooting
	if (m_iCharge < FINGERLASER_MAXCHARGETIME/3)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
		return;
	}

	//turn the charging sprite off
	m_pChargeSprite->TurnOff();

	//set the right animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	//make some noize :-)
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/fingerlaser.wav", 1.0, ATTN_NORM);

	//calculate startpos and speed
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition();
	TraceResult tr;
	UTIL_TraceLine (vecSrc,vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(pev), &tr );

	//shoot the laser
	CBeam *beam = CBeam::BeamCreate("sprites/trail.spr", 3);
	beam->PointEntInit( tr.vecEndPos, m_pPlayer->entindex());
	beam->SetEndAttachment(1);
	beam->SetColor(200, 100, 100);
	beam->SetBrightness(200);	
	beam->LiveForTime( 0.75 );

	int damage = FINGERLASER_DAMAGE * m_iCharge * GetPowerRatio();
	RadiusDamage( tr.vecEndPos, m_pPlayer->pev, m_pPlayer->pev, damage, damage * 2.5, CLASS_NONE, DMG_MAGIC );

	//make explosion
	MESSAGE_BEGIN( MSG_ALL, gmsgExplosion, NULL );
		WRITE_COORD( tr.vecEndPos.x );	
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_LONG (2.5 * damage);	//radius
		WRITE_BYTE (200);			//red
		WRITE_BYTE (100);			//green
		WRITE_BYTE (100);			//blue
		WRITE_BYTE (200);			//brightness
	MESSAGE_END();
}

void CFingerLaser::WeaponIdle()
{
	if(m_pPlayer->m_fCharging)
		Shoot();
}

void CFingerLaser::PrimaryAttack( void )
{
	//delay CHARGETIME secs between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < FINGERLASER_KI_COST )
	{
		if (m_pPlayer->m_fCharging)
			Shoot();

		PlayEmptySound();
		return;
	}

	// reduce our ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= FINGERLASER_KI_COST;

	if (!m_pPlayer->m_fCharging)	//start charging
	{
		m_pPlayer->m_fCharging = true;
		m_pPlayer->m_flSlowDown = 0.0;
		m_iCharge = 0;
		m_pPlayer->SetAnimation( PLAYER_CHARGE );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/fingerlasercharge.wav", 1.0, ATTN_NORM);
	}
	else //increase damage
	{
		if (m_iCharge > FINGERLASER_MAXCHARGETIME)
		{
			Shoot();
		}
		else
		{
			m_iCharge++;
			m_pChargeSprite->SetScale( 0.1 * (float)m_iCharge / (float)FINGERLASER_MAXCHARGETIME);
			//draw charge bar
			MESSAGE_BEGIN( MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev ); 
				WRITE_BYTE( m_iCharge * 100 / FINGERLASER_MAXCHARGETIME);	//amount charged in %
			MESSAGE_END();

			if (m_iCharge == FINGERLASER_MAXCHARGETIME / 3)
			{
				m_pChargeSprite->TurnOn();
			}
		}
	}	
}