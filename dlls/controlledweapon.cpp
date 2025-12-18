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
#include "controlledbeam.h"

extern int gmsgCharge;
extern int gmsgPowerStruggle;

LINK_ENTITY_TO_CLASS(weapon_controlled, CControlledBeam);

BOOL CControlledBeam::Deploy()
{
	InitChargeSprite();

	// set beam start
	char start_sprite[255];
	sprintf(start_sprite, "sprites/%sstart.spr", m_pAttackName);
	m_pBeamStart = CSprite::SpriteCreate(start_sprite, Vector(0, 0, 0), true);
	m_pBeamStart->SetTransparency(kRenderTransAdd, 255, 255, 255, 254, kRenderFxNone);
	m_pBeamStart->pev->framerate = 10;
	m_pBeamStart->pev->effects |= EF_BRIGHTLIGHT;
	m_pBeamStart->TurnOff();

	m_flStartShoot = gpGlobals->time;
	m_eAttackState = ATTACK_IDLE;
	return DefaultDeploy(m_pAttackName);	
}

void CControlledBeam::Precache()
{
	PRECACHE_SOUND("weapons/beamhead.wav");
	PRECACHE_SOUND("weapons/beamfire.wav");
}

void CControlledBeam::Holster(int skiplocal)
{
	if (m_eAttackState == ATTACK_AIM)
	{
		if (UTIL_IsValidEntity(ENT(m_pMagic->pev)))
			m_pMagic->Explode();

		StopAiming();
	}	
	else if (m_eAttackState == ATTACK_CHARGE)
	{
		m_pPlayer->m_flSlowDown = 1.0;
		m_pPlayer->m_fCharging = false;

		//turn of charging sprite
		DisableChargeSprite();

		//turn all sounds off
		SilenceWeaponSounds();
		SilenceVoice();

		//undo drawing charge bar
		MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev);
			WRITE_BYTE(0);	//amount charged in %
		MESSAGE_END();		
	}
	RemoveChargeSprite();
	UTIL_Remove(m_pBeamStart);
}

void CControlledBeam::StopAiming()
{
	m_pMagic = NULL;

	//disable powerstruggle bar
	MESSAGE_BEGIN(MSG_ONE, gmsgPowerStruggle, NULL, m_pPlayer->pev);
		WRITE_CHAR(127);
	MESSAGE_END();

	m_pPlayer->m_flSlowDown = 1.0;
	m_pPlayer->m_fControl = false;
	m_eAttackState=ATTACK_IDLE;

	//disable beamstart
	m_pBeamStart->TurnOff();

	//disable control sound
	SilenceWeaponSounds();
}

void CControlledBeam::WeaponIdle()
{
	if (m_eAttackState == ATTACK_CHARGE)
		Shoot();

	if (m_eAttackState == ATTACK_AIM)
	{
		//magic exploded, back to idle mode
		if(!UTIL_IsValidEntity(ENT(m_pMagic->pev)))
		{
			m_pMagic = NULL;
			StopAiming();
		}
		//show powerstrugglebar
		else if (m_pMagic->m_fPowerStruggle)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgPowerStruggle, NULL, m_pPlayer->pev); 
				WRITE_CHAR(m_pMagic->m_flStruggleRatio * 100);	//struggle ratio in % (-100 till 100 %)
			MESSAGE_END();
		}
	}
}

void CControlledBeam::Shoot()
{
	//disable charge bar
	MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev); 
		WRITE_BYTE(0);	//amount charged in %
	MESSAGE_END();

	//silence charge sounds
	SilenceWeaponSounds();

	m_pPlayer->m_fCharging = false;

	//charge to low?
	if (m_iCharge<m_iMaxCharge / 3)
	{
		m_eAttackState = ATTACK_IDLE;
		m_pPlayer->m_flSlowDown = 1.0;

		SilenceVoice();
		return;
	}
	//turn the charging sprite off
	DisableChargeSprite();

	m_pPlayer->m_fControl = true;

	//do shoot animation
	m_pPlayer->SetAnimation(PLAYER_CONTROL);

	//make shoot/control sounds
	EmitShootSound();
	EmitControlSound();

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 30;
	m_pPlayer->m_flSlowDown = 1.0; //hack hack... needed to get the max speed :)
	Vector vecThrow = gpGlobals->v_forward * m_iSpeed;
	m_pPlayer->m_flSlowDown = 0.0;

	//shoot the magic
	float size = (float)m_iCharge / (float)m_iMaxCharge;
	m_pMagic = CMagicAttack::ShootGuidedMagic(m_pPlayer->pev, vecSrc, vecThrow, m_iCharge * m_iDamage * GetPowerRatio(),m_pAttackName ,size, m_pColor[0], m_pColor[1], m_pColor[2], m_sTrail, m_iRadius);

	//show the charge sprite
	m_pBeamStart->SetScale(size * 0.3);
	UTIL_SetOrigin(m_pBeamStart->pev,vecSrc);
	m_pBeamStart->TurnOn();

	m_eAttackState = ATTACK_AIM;
}

void CControlledBeam::Charge()
{
	if (m_eAttackState == ATTACK_AIM)
		StopAiming();

	//first charge
	if (m_eAttackState!=ATTACK_CHARGE)
	{
		m_pPlayer->m_flSlowDown = 0.0;
		m_eAttackState = ATTACK_CHARGE;
		m_pPlayer->m_fCharging = true;
		m_iCharge = 0;

		//make start charge sound
		EmitChargeSound();

		//set animation
		m_pPlayer->SetAnimation(PLAYER_CHARGE);
	}

	//charge it
	if (m_iCharge < m_iMaxCharge)
	{
		// Check we have some ammo
		if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < m_iKiCost)
		{
			Shoot();
			return;
		}

		// reduce our ammo
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= m_iKiCost;

		if (m_iCharge == m_iMaxCharge / 3)
			EnableChargeSprite();
		
		if (m_iCharge > m_iMaxCharge / 3)
			GrowChargeSprite();

		m_iCharge++;
		MESSAGE_BEGIN(MSG_ONE, gmsgCharge, NULL, m_pPlayer->pev); 
			WRITE_BYTE(m_iCharge * 100 / m_iMaxCharge);	//amount charged in %
		MESSAGE_END(); 
	}
	else
		Shoot();
}

void CControlledBeam::PrimaryAttack()
{
	//delay CHARGETIME secs between shoots
	if (gpGlobals->time - m_flStartShoot < CHARGETIME)
		return;

	m_flStartShoot = gpGlobals->time;

	//don't fire a new one while aiming a controlled beam
	if (m_eAttackState == ATTACK_AIM && UTIL_IsValidEntity(ENT(m_pMagic->pev)))
	{
		if (m_pMagic->m_fPowerStruggle)
		{
			if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > m_iKiCost)
			{
				// reduce our ammo
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= m_iKiCost;
				m_pMagic->pev->dmg += GetPowerRatio() * m_iDamage;
			}
			MESSAGE_BEGIN(MSG_ONE, gmsgPowerStruggle, NULL, m_pPlayer->pev); 
				WRITE_CHAR(m_pMagic->m_flStruggleRatio * 100);	//struggle ratio in % (-100 till 100 %)
			MESSAGE_END(); 
		}
	}
	else
		Charge();
}

void CControlledBeam::SecondaryAttack(void)
{
	if (m_eAttackState == ATTACK_AIM)
	{
		if (UTIL_IsValidEntity(ENT(m_pMagic->pev)))
			m_pMagic->Explode();

		StopAiming();
	}
}

void CControlledBeam::EmitControlSound()
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/beamfire.wav", 1.0, ATTN_NORM);
}