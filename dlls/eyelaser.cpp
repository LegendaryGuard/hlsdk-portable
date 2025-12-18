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
#include "animation.h"
#include "decals.h"
#include "effects.h"
#include "customentity.h"

#define ATTACH_LEFTEYE 2
#define ATTACH_RIGHTEYE 3

/**
* Piccolo's eyelaser
* @version 8-6-2001
* @author Herwin 'harSens' van Welbergen
*/
class CEyelaser:public CBasePlayerWeapon
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
	* Get eyelaser slot
	* return int: the slot, is 4 for eyelaser
	*/
	int iItemSlot( void ) { return 4; }

	/**
	* Get eyelaser item info
	* @param ItemInfo *p: The ItemInfo gets stored here
	* @return int: Always returns 1 (why???)
	*/
	int GetItemInfo(ItemInfo *p);

	/**
	* Shoots a eyelaser attack
	*/
	void PrimaryAttack( void );

	/**
	* Stops shooting the eyelaser
	*/
	void SecondaryAttack( void );

	/**
	* Called when eyelaser is selected as weapon
	*/
	BOOL Deploy();

	/**
	* Holsters the eyelaser
	* @param int skiplocal: ??
	*/
	void Holster(int skiplocal);

	/**
	* Weapon idle function
	*/
	void WeaponIdle();
private:
	/**
	* Shoots a laser between begin and en position
	* @param laser: the laser beam
	* @param Vector begin: begin position for the laser
	* @param Vector end: end position for the laser
	*/
	void ShootLaser(CBeam *laser, Vector begin, Vector end);

	/**
	* Creates the laser beam
	* @param attachment:to wich attachment to add?
	*/
	CBeam *CreateLaser(int attachment);

	/**
	* Destroy a laser beam
	* @param laser:the laser to destroy
	*/
	void DestroyLaser(CBeam *laser);

	/**
	* Set red eyes instead of beam
	* @param laser:the laser to turn into red eye status
	*/
	void SetRedEye(CBeam *laser);

	/**
	* Set laser beam instead of red eyes
	* @param laser:the laser to turn into beam status
	*/
	void SetBeam(CBeam *laser);

	CBeam *m_pLeftLaser;		//eyelaser beams
	CBeam *m_pRightLaser;		//
	float m_flStartShoot;		//shot starting time
	BOOL m_fShooting;
};

LINK_ENTITY_TO_CLASS( weapon_eyelaser, CEyelaser );

void CEyelaser::Spawn( void )
{
	Precache();
	m_iId = WEAPON_EYELASER;
	m_iDefaultAmmo = 0;
	m_iClip = -1;
	m_pLeftLaser = NULL;
	m_pRightLaser = NULL;
	m_fShooting = false;
	FallInit();// get ready to fall down.
}

void CEyelaser::Precache( void )
{
	PRECACHE_MODEL("sprites/trail.spr");
	PRECACHE_SOUND("weapons/melt.wav");		
}

int CEyelaser::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ki";
	p->iMaxAmmo1 = -1;	//max ammo is weapon independent
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EYELASER;
	p->iWeight = EYELASER_WEIGHT;
	p->iFlags = 0;
	return 1;
}

BOOL CEyelaser::Deploy( void )
{
	m_flStartShoot = gpGlobals->time;
	m_pLeftLaser = CreateLaser(ATTACH_LEFTEYE);
	m_pRightLaser = CreateLaser(ATTACH_RIGHTEYE);
	
	//create beam effect between the eyes for red eye fx
	m_pLeftLaser->SetStartAttachment(ATTACH_RIGHTEYE);
	m_pRightLaser->SetStartAttachment(ATTACH_LEFTEYE);

	return DefaultDeploy( "eyelaser" );	
}

void CEyelaser::WeaponIdle()
{
	if (m_fShooting)
	{
		//color the eyes red
		SetRedEye(m_pLeftLaser);
		SetRedEye(m_pRightLaser);
		m_fShooting = false;
	}	
}

void CEyelaser::ShootLaser(CBeam *laser, Vector begin, Vector end)
{
	//calculate real end position
	TraceResult tr;
	Vector vecAiming = end - begin;
	vecAiming.Normalize();
	UTIL_TraceLine (begin, begin + vecAiming * 8192, dont_ignore_monsters, ENT(pev), &tr );

	//draw laser beam
	laser->SetStartPos(tr.vecEndPos);
		
	//draw smoke clouds
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tr.vecEndPos );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( 2 ); // scale * 10
		WRITE_BYTE( 5 ); // framerate
	MESSAGE_END();

	//make holes in da ground
	UTIL_DecalTrace( &tr, DECAL_GUNSHOT1 );

	//do damage if hit a player
	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
	pEntity->TakeDamage(m_pPlayer->pev,m_pPlayer->pev,EYELASER_DAMAGE * GetPowerRatio(),DMG_MAGIC);	

	//emit melting sound
	EMIT_SOUND(ENT(pEntity->pev), CHAN_WEAPON, "weapons/melt.wav", 1.0, ATTN_NORM);
}

void CEyelaser::SetRedEye(CBeam *laser)
{
	laser->SetType( BEAM_ENTS );
	laser->RelinkBeam();
}

void CEyelaser::SetBeam(CBeam *laser)
{
	laser->SetType( BEAM_ENTPOINT );
	laser->RelinkBeam();
}

CBeam *CEyelaser::CreateLaser(int attachment)
{
	CBeam *pLaser = CBeam::BeamCreate("sprites/trail.spr",3);
	pLaser->SetColor(200,0,0);
	pLaser->SetBrightness(200);
	pLaser->SetEndEntity(m_pPlayer->entindex());
	pLaser->SetEndAttachment(attachment);
	pLaser->SetStartEntity(m_pPlayer->entindex());
	SetRedEye(pLaser);
	return pLaser;
}

void CEyelaser::DestroyLaser(CBeam *laser)
{
	if ( laser )
	{
		UTIL_Remove( laser );
		laser = NULL;
	}
}

void CEyelaser::Holster(int skiplocal)
{
	DestroyLaser(m_pLeftLaser);
	DestroyLaser(m_pRightLaser);
}

void CEyelaser::PrimaryAttack( void )
{
	if (gpGlobals->time - m_flStartShoot < 0.2)
		return;

	m_flStartShoot = gpGlobals->time;
	
	// Check we have some ammo
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < EYELASER_KI_COST )
	{
		PlayEmptySound();
		if (m_fShooting)
		{
			//color the eyes red
			SetRedEye(m_pLeftLaser);
			SetRedEye(m_pRightLaser);
			m_fShooting = false;
		}
		return;
	}

	//reduce ammo
	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]-=EYELASER_KI_COST; 
	
	if (!m_fShooting)
	{
		SetBeam(m_pLeftLaser);
		SetBeam(m_pRightLaser);
		m_fShooting = true;
	}

	//calculate eye position and eye width
	Vector vecLeftEyePos = m_pLeftLaser->GetEndPos();
	Vector vecRightEyePos = m_pRightLaser->GetEndPos();
	
	//calculate crosshair pos
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = m_pPlayer->GetGunPosition();
	TraceResult tr;
	UTIL_TraceLine (vecSrc,vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(pev), &tr );
	Vector vecLeftLaser = tr.vecEndPos - gpGlobals->v_right * 2;
	Vector vecRightLaser = tr.vecEndPos + gpGlobals->v_right * 2;

	//shoot the 2 lasers
	ShootLaser(m_pLeftLaser,vecLeftEyePos,vecLeftLaser);
	ShootLaser(m_pRightLaser,vecRightEyePos,vecRightLaser);
}

void CEyelaser::SecondaryAttack()
{
	//ignore pressing of secondary attack.
	WeaponIdle();
}


