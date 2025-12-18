#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "soundent.h"
#include "decals.h"
#include "effects.h"

LINK_ENTITY_TO_CLASS(magicattack, CMagicAttack);

extern int gmsgExplosion;
extern int gmsgCreateTrail;
extern int gmsgRemoveTrail;
extern int gmsgAddPoint;

void CMagicAttack::Spawn()
{
	Precache();
	pev->gravity = 0.000001; //magic is not affected by gravity
	
	pev->globalname = MAKE_STRING("magic_attack");
	m_cColor[0] = 175;
	m_cColor[1] = 175;
	m_cColor[2] = 175;
	m_cColor[3] = 200;
	m_fPowerStruggle = false;
	m_pStruggler = NULL;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 254;
	m_fTrail = false;
	pev->effects |= EF_BRIGHTLIGHT;
}

void CMagicAttack::Precache()
{
	PRECACHE_SOUND("weapons/explosion.wav");
	m_sRockGibs = PRECACHE_MODEL("models/rockgibs.mdl");
}

void CMagicAttack::Remove(void)
{
	// silence flysound
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

	MESSAGE_BEGIN(MSG_ALL, gmsgRemoveTrail, NULL);
		WRITE_LONG(entindex());		//entity to remove trail from
	MESSAGE_END();
	
	
	if (m_fTrail)
	{
		// remove all trails from entity
		MESSAGE_BEGIN (MSG_PVS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_KILLBEAM	);	//kill all beams attached to entity
			WRITE_SHORT(entindex());	// short (entity)
		MESSAGE_END();	
	}

	UTIL_Remove(this);
}

void CMagicAttack::AddTrail(int life, int width, short trail)
{
	//add follow beam
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMFOLLOW);				//create a line of decaying beam segments until entity stops moving
		WRITE_SHORT(entindex());				//entity:attachment to follow)
		WRITE_SHORT(trail);						//short (sprite index)
		WRITE_BYTE(life);						//life in 0.1's) 
		WRITE_BYTE(width);						//(line width in 0.1's) 
		WRITE_BYTE(255);						//byte,byte,byte (color): no colorfilter used
		WRITE_BYTE(255);
		WRITE_BYTE(255);
		WRITE_BYTE(254);						//brightness
	MESSAGE_END();
}

/*
do damage to entity hit and move on
used by discs
*/
void CMagicAttack::HitTouch(CBaseEntity *pOther)
{
	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	//don't hit the one who fired the thing
	if (pevOwner == pOther->pev)
		return;

	//don't hit someone twice in a row
	if (m_pPreviousHit != pOther)
	{
		m_pPreviousHit = pOther;
		pOther->TakeDamage(pev, pevOwner, pev->dmg, DMG_MAGIC);
	}
}

/*
create a bubble on touch
*/
void CMagicAttack::BubbleTouch(CBaseEntity *pOther)
{
	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	//don't hit the one who fired the thing
	if (pevOwner == pOther->pev)
		return;

	entvars_t *pevOtherOwner;
	if (pOther->pev->owner)
		pevOtherOwner = VARS(pOther->pev->owner);
	else
		pevOtherOwner = NULL;
	
	//don't hit you're brother (aura, other attacks fired by dad)
	if (pevOtherOwner == pevOwner)
		if (pevOwner) return;

	//don't hit other magic attacks
	if (pOther->pev->globalname == MAKE_STRING("magic_attack"))
		return;
	
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/kiplosion.wav", 1.0, ATTN_NORM);
	::RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, 80, CLASS_NONE, DMG_MAGIC);
	
	pev->velocity = Vector(0, 0, 0);
	pev->scale = 1;
	pev->model = iStringNull;	//invisible
	pev->solid = SOLID_NOT;		// intangible

	pev->takedamage = DAMAGE_NO;
	SetThink(&CMagicAttack::BubbleThink);
	pev->nextthink = gpGlobals->time;
}

/*
Explode on touch
used by: big bang, kamehameha
*/
void CMagicAttack::ExplodeTouch(CBaseEntity *pOther)
{
	//lost the powerstruggle, give the winner credit
	if (m_fPowerStruggle && pOther->pev->globalname != MAKE_STRING("magic_attack"))
	{
		pev->owner = m_pStruggler->pev->owner;
		Explode();
		return;		
	}
	
	//find out who not to hit
	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	entvars_t *pevOtherOwner;
	if (pOther->pev->owner)
		pevOtherOwner=VARS(pOther->pev->owner);
	else
		pevOtherOwner = NULL;

	//don't hit your owner
	if (pevOwner == pOther->pev)
		return;
	
	//don't hit you're brother (aura, other attacks fired by dad)
	if (pevOtherOwner == pevOwner)
		if (pevOwner) return;
	
	//go into powerstruggle after a collision with other large attacks
	if (pOther->pev->globalname == MAKE_STRING("magic_attack"))
	{
		//ignore discs
		if (pOther->pev->dmg >= 999)
			return;

		//vaporize much smaller attacks
		if ((float)pOther->pev->dmg / (float)pev->dmg < 0.2)
		{
			((CMagicAttack*)pOther)->Remove();
			return;
		}

		//go in powerstruggle with large attacks
		if (!m_fPowerStruggle)
		{
			m_fPowerStruggle = true;
			m_pStruggler = pOther;
			SetThink(&CMagicAttack::PowerStruggleThink);
			pev->nextthink = gpGlobals->time;
		}
		return;	
	}
	Explode();	
}

void CMagicAttack::Explode()
{
	//compensate for the size of the attacking ball
	int radius = pev->dmg * m_iRadius;
	
	TraceResult tr;
	Vector vecBall = pev->velocity.Normalize() * radius;
	UTIL_TraceLine (pev->origin, pev->origin + vecBall ,  ignore_monsters, ENT(pev), &tr);
	Vector vecSrc = tr.vecEndPos;
	
	EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/explosion.wav", 1.0, 0.4);

	//shake ground
	UTIL_ScreenShake(vecSrc, 12, 100, 5, radius);
	
	//explosion sphere
	MESSAGE_BEGIN(MSG_ALL, gmsgExplosion, NULL);
		WRITE_COORD(vecSrc.x);	
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_LONG(radius);			//radius
		WRITE_BYTE(m_cColor[0]);	//red
		WRITE_BYTE(m_cColor[1]);	//green
		WRITE_BYTE(m_cColor[2]);	//blue
		WRITE_BYTE(m_cColor[3]);	//brightness
	MESSAGE_END();
	
	if ((pev->flags | FL_ONGROUND) && tr.flFraction < 1.0)
	{	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
			WRITE_BYTE(TE_BREAKMODEL);					// box of models or sprites
			WRITE_COORD(vecSrc.x);					// coord, coord, coord (position)
			WRITE_COORD(vecSrc.y);
			WRITE_COORD(vecSrc.z);
			WRITE_COORD(50);							// coord, coord, coord (size)
			WRITE_COORD(50);
			WRITE_COORD(50);
			WRITE_COORD(tr.vecPlaneNormal.x * 200);	// coord, coord, coord (velocity)
			WRITE_COORD(tr.vecPlaneNormal.y * 200);
			WRITE_COORD(tr.vecPlaneNormal.z * 200);
			WRITE_BYTE(30);								// byte (random velocity in 10's)
			WRITE_SHORT(m_sRockGibs);					// short (sprite or model index)
			WRITE_BYTE(20);								// byte (count)
			WRITE_BYTE(50);								// byte (life in 0.1 secs)
			WRITE_BYTE(0);								// byte (flags)
		MESSAGE_END();
	}

	entvars_t *pevOwner;
	if (pev->owner)
		pevOwner = VARS(pev->owner);
	else
		pevOwner = NULL;

	::RadiusDamage(pev->origin, pev, pevOwner, pev->dmg, radius, CLASS_NONE, DMG_MAGIC);
	
	if (RANDOM_FLOAT(0, 1) < 0.5)
		UTIL_DecalTrace(&tr, DECAL_SCORCH1);
	else
		UTIL_DecalTrace(&tr, DECAL_SCORCH2);

	Remove();
}

void CMagicAttack::Grow()
{
	pev->scale += 0.02;
}

void CMagicAttack::RandomRotateThink(void)
{
	int new_avelocity = 10 * RANDOM_LONG(-100, 100);
	pev->avelocity.z = 0.6 * new_avelocity + pev->avelocity.z * 0.4;
	NormalThink();
}

void CMagicAttack::NormalThink(void)
{
	//remove magic if it has moved out of the world
	if (!IsInWorld())
	{
		Remove();
		return;
	}

	//put in bubble trail if underwater
	if (pev->waterlevel == 3)	
	{	
		UTIL_BubbleTrail(pev->origin - pev->velocity * 0.1, pev->origin, 4);
	} 
	
	//animate sprites
	int iMaxFrames = MODEL_FRAMES(pev->modelindex) - 1;
	if (iMaxFrames>0)
	{
		pev->frame ++;
		if (pev->frame > iMaxFrames)
			pev->frame = 0;
	}

	pev->nextthink = gpGlobals->time + 0.2;
}

void CMagicAttack::BubbleThink(void)
{
	//remove magic if it has moved out of the world
	if (!IsInWorld())
	{
		Remove();
		return;
	}

	if (pev->scale > 0.25)
		pev->scale *= 0.5;
	else
	{
		Remove();
		return;
	}
	pev->nextthink = gpGlobals->time + 0.2;
}

void CMagicAttack::FollowThink(void )
{
	entvars_t *pevOwner;

	//check of magic has an owner
	if (pev->owner)
	{
		Vector vecOldVelocity = pev->velocity;
		
		//calculate crosshair position
		pevOwner=VARS(pev->owner);
		UTIL_MakeVectors(pevOwner->v_angle);
		Vector vecSrc = pevOwner->origin + pevOwner->view_ofs;
		Vector vecAiming = gpGlobals->v_forward;
		TraceResult tr;
		UTIL_TraceLine (vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, pev->owner, &tr);
		tr.vecEndPos = tr.vecEndPos - 10 * gpGlobals->v_forward;//move it a little from edges of the hitpoint

		UTIL_MakeAimVectors(pev->angles);
		Vector vecTarget = gpGlobals->v_forward;
		
		float flDist, flMax, flDot;
		flMax = 4096;
		
		Vector vecDir = tr.vecEndPos - pev->origin;
		flDist = vecDir.Length();
		vecDir = vecDir.Normalize();
		flDot = DotProduct(gpGlobals->v_forward, vecDir);
		if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
			vecTarget = vecDir;
		
		pev->velocity = (pev->velocity).Normalize() * 0.8 * m_flSpeed + vecTarget * m_flSpeed * 0.2;
		pev->angles = UTIL_VecToAngles(pev->velocity);

		//add a trail if necesary
		if (m_fTrail)
		{
			int velocity_change = (vecOldVelocity - pev->velocity).Length();
			if (velocity_change > 10) 
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgAddPoint, NULL);
					WRITE_COORD(pev->origin.x);	
					WRITE_COORD(pev->origin.y);
					WRITE_COORD(pev->origin.z);
					WRITE_LONG(entindex());	//radius				
				MESSAGE_END();
			}
		}
	}
	else
		SetThink(&CMagicAttack::NormalThink);

	NormalThink();//call normal think function
	pev->nextthink = gpGlobals->time + 0.3;
}

void CMagicAttack::PowerStruggleThink()
{
	// check if struggler still exists
	if (!m_pStruggler || !UTIL_IsValidEntity(ENT(m_pStruggler->pev)))
	{
		UTIL_MakeAimVectors(pev->angles);
		
		//weenie opponent blew his attack up, triple speed
		m_flSpeed *= 3;

		pev->velocity = gpGlobals->v_forward * m_flSpeed;
		m_fPowerStruggle = false;
		SetThink(&CMagicAttack::NormalThink);
		pev->nextthink = gpGlobals->time;
		return;
	}

	if (pev->dmg >= m_pStruggler->pev->dmg)
	{
		//we're winning, or it's a draw
		UTIL_MakeAimVectors(pev->angles);
		pev->velocity = gpGlobals->v_forward * (pev->dmg - m_pStruggler->pev->dmg) / pev->dmg * m_flSpeed;		
		m_flStruggleRatio = pev->velocity.Length() / m_flSpeed;
	}
	else
	{
		pev->velocity = m_pStruggler->pev->velocity;
		m_flStruggleRatio = -m_pStruggler->pev->velocity.Length() / ((CMagicAttack*)m_pStruggler)->m_flSpeed;
	}
	pev->nextthink = gpGlobals->time + 0.3;
}

void CMagicAttack::HomeThink(void)
{
	entvars_t *pevOwner;

	//check of magic has an owner
	if (pev->owner)
	{
		pevOwner = VARS(pev->owner);
		Vector vecHome = pevOwner->origin+pevOwner->view_ofs;
		vecHome.z += 20;
		Vector vecDir = vecHome - pev->origin;
		Vector vecTargetVel = vecDir.Normalize() * m_flSpeed;
		pev->velocity = pev->velocity * 0.2 + vecTargetVel * 0.8;
		pev->angles = UTIL_VecToAngles(pev->velocity);
	}
	else
		SetThink(&CMagicAttack::NormalThink);

	NormalThink();//call normal think function
	pev->nextthink = gpGlobals->time + 0.3;
}

void CMagicAttack::OrbitThink(void)
{
	entvars_t *pevAimEnt;

	//check of magic has an owner
	if (pev->chain)
	{
		pevAimEnt = VARS(pev->chain);

		//owner does not exist explode
		if(!UTIL_IsValidEntity(ENT(pevAimEnt)))
		{
			Explode();
			return;
		}

		Vector vecIdealVelocity = pevAimEnt->velocity;
		Vector vecRadius = pev->origin - pevAimEnt->origin;

		//orbit around the owner
		Vector vecOrbit = CrossProduct(vecRadius, pevAimEnt->velocity).Normalize() * m_flSpeed * 0.20;
		vecIdealVelocity = vecIdealVelocity + vecOrbit;

		//keep at a certain distance from the owner
		float Dist = vecRadius.Length();
		if (Dist > 10)
			vecIdealVelocity = vecIdealVelocity - vecRadius * 12;
		else
			vecIdealVelocity = vecIdealVelocity + vecRadius;

		pev->velocity = vecIdealVelocity * 0.5 + pev->velocity * 0.5;		
	}
	else
	{
		//no aiment: Explode
		Explode();
		return;
	}
	NormalThink();//call normal think function
	pev->nextthink = gpGlobals->time + 0.2;
}

CMagicAttack* CMagicAttack::ShootDestructoDisc(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{	CMagicAttack *pMagicAttack=GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();
	
	pMagicAttack->pev->movetype=MOVETYPE_NOCLIP;//go through walls
	pMagicAttack->pev->solid=SOLID_TRIGGER;//only touch players

	//set model and sound
	SET_MODEL(ENT(pMagicAttack->pev), "models/w_destructodisc.mdl");
	

	UTIL_SetSize(pMagicAttack->pev, Vector(-15, 6 , -15),Vector (15, 9, 15));

	pMagicAttack->pev->dmg = 999; //instant death
	UTIL_SetOrigin(pMagicAttack->pev,vecStart);
	pMagicAttack->pev->velocity = vecVelocity;
	pMagicAttack->m_flSpeed = vecVelocity.Length();
	pMagicAttack->pev->angles = UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("Destructo Disc");

	//emit flying sound
	EMIT_SOUND(ENT(pMagicAttack->pev), CHAN_WEAPON, "weapons/rotating.wav", 1.0, ATTN_NORM);	

	//make them transparant
	pMagicAttack->pev->rendermode = kRenderTransAdd;
	pMagicAttack->pev->renderamt = 90;

	pMagicAttack->SetTouch(&CMagicAttack::HitTouch);
	pMagicAttack->SetThink(&CMagicAttack::RandomRotateThink);
	pMagicAttack->pev->nextthink = gpGlobals->time + 0.25;//start flying straight
	return pMagicAttack;
}

void CMagicAttack::ShootSpiritBomb(Vector vecVelocity, int damage)
{
	pev->solid = SOLID_TRIGGER;	//touches everything	
	pev->dmg = damage;
	pev->velocity = vecVelocity;
	m_flSpeed = vecVelocity.Length();
	pev->angles = UTIL_VecToAngles(pev->velocity);

	int iAttackSize = (pev->scale * SPIRITBOMB_SIZE) / 2;
	UTIL_SetSize(pev, Vector(-iAttackSize, -iAttackSize, -iAttackSize), Vector(iAttackSize, iAttackSize, iAttackSize));
	SetTouch(&CMagicAttack::ExplodeTouch);

	SetThink(&CMagicAttack::NormalThink);
	pev->nextthink = gpGlobals->time;	
}

CMagicAttack *CMagicAttack::ShootDeathBall(entvars_t *pevOwner,Vector vecSrc,Vector vecVelocity, int damage, float size)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();
	//set model
	SET_MODEL(ENT(pMagicAttack->pev), "sprites/deathball.spr");
	
	UTIL_SetOrigin(pMagicAttack->pev,vecSrc);
	
	pMagicAttack->m_iRadius = DEATHBALL_RADIUS;
	pMagicAttack->m_cColor[0] = 100;
	pMagicAttack->m_cColor[1] = 0;
	pMagicAttack->m_cColor[2] = 0;
	pMagicAttack->m_cColor[3] = 200;

	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("Death Ball");	

	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything	
	pMagicAttack->pev->dmg = damage; 
	pMagicAttack->pev->velocity = vecVelocity;
	pMagicAttack->m_flSpeed = vecVelocity.Length();
	pMagicAttack->pev->angles = UTIL_VecToAngles(pMagicAttack->pev->velocity);
	
	int iAttackSize = (size * DEATHBALL_SIZE) / 2;
	UTIL_SetSize(pMagicAttack->pev, Vector(-iAttackSize, -iAttackSize, -iAttackSize), Vector(iAttackSize, iAttackSize, iAttackSize));
	pMagicAttack->SetTouch(&CMagicAttack::ExplodeTouch);

	pMagicAttack->SetThink(&CMagicAttack::NormalThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;	

	return pMagicAttack;
}

CMagicAttack* CMagicAttack::ShootGuidedMagic(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int damage,char *attack, float size, int r, int g, int b, short trail, int radius)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	
	
	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything

	//set model
	char buffer[255];
	sprintf(buffer, "sprites/%s.spr", attack);
	SET_MODEL(ENT(pMagicAttack->pev), buffer);
	pMagicAttack->pev->scale = size;
	
	pMagicAttack->m_iRadius = radius;
	//init colors
	pMagicAttack->m_cColor[0] = r;
	pMagicAttack->m_cColor[1] = g;
	pMagicAttack->m_cColor[2] = b;
	pMagicAttack->m_cColor[3] = 254;

	//add follow beam
	pMagicAttack->m_fTrail = true;
	MESSAGE_BEGIN(MSG_ALL, gmsgCreateTrail, NULL);
		WRITE_COORD(vecStart.x);	
		WRITE_COORD(vecStart.y);
		WRITE_COORD(vecStart.z);
		WRITE_LONG(pMagicAttack->entindex());			//entity to follow
		WRITE_SHORT(trail);							//trail sprite
		WRITE_BYTE(size * GUIDEDATTACK_SIZE * 1.5);	//width
	MESSAGE_END();

	pMagicAttack->pev->dmg = damage; 
	UTIL_SetOrigin(pMagicAttack->pev,vecStart);
	pMagicAttack->pev->velocity=vecVelocity;
	pMagicAttack->m_flSpeed=vecVelocity.Length();
	pMagicAttack->pev->angles=UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner=ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING(attack);
	
	int iAttackSize = (size * GUIDEDATTACK_SIZE)/2;
	UTIL_SetSize(pMagicAttack->pev, Vector(-iAttackSize, -iAttackSize, -iAttackSize), Vector(iAttackSize, iAttackSize, iAttackSize));
	pMagicAttack->SetTouch(&CMagicAttack::ExplodeTouch);

	pMagicAttack->SetThink(&CMagicAttack::FollowThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;

	EMIT_SOUND(ENT(pMagicAttack->pev), CHAN_WEAPON, "weapons/beamhead.wav", 1.0, ATTN_NORM);
	return pMagicAttack;
}


CMagicAttack* CMagicAttack::ShootFriezaDisc(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	
	
	pMagicAttack->pev->movetype=MOVETYPE_NOCLIP;//go through walls
	pMagicAttack->pev->solid=SOLID_TRIGGER;//only touch players

	//set model 
	SET_MODEL(ENT(pMagicAttack->pev), "models/w_friezadisc.mdl");
	
	UTIL_SetSize(pMagicAttack->pev, Vector(-15, 6 , -15),Vector (15, 9, 15));

	pMagicAttack->pev->dmg=999; //instant death
	UTIL_SetOrigin(pMagicAttack->pev,vecStart);
	pMagicAttack->pev->velocity=vecVelocity;
	pMagicAttack->m_flSpeed=vecVelocity.Length();
	pMagicAttack->pev->angles=UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner=ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("Frieza Disc");

	//emit fly sound
	EMIT_SOUND(ENT(pMagicAttack->pev), CHAN_WEAPON, "weapons/rotating.wav", 1.0, ATTN_NORM);

	pMagicAttack->SetTouch(&CMagicAttack::HitTouch);
	// make monsters/players afraid of it while in the air
	pMagicAttack->SetThink(&CMagicAttack::FollowThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;

	return pMagicAttack;
}

CMagicAttack* CMagicAttack::ShootWave(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int damage, char *attack, int r, int g, int b, short trail, int radius)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	
	
	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything

	//set model
	char buffer[255];
	sprintf(buffer,"models/w_%s.mdl",attack);
	SET_MODEL(ENT(pMagicAttack->pev), buffer);
	
	pMagicAttack->m_iRadius = radius;
	pMagicAttack->m_cColor[0]=r;
	pMagicAttack->m_cColor[1]=g;
	pMagicAttack->m_cColor[2]=b;
	pMagicAttack->m_cColor[3]=254;

	//add follow beam
	pMagicAttack->AddTrail(128,15,trail);
	
	pMagicAttack->pev->dmg = damage; 
	UTIL_SetOrigin(pMagicAttack->pev,vecStart);
	pMagicAttack->pev->velocity=vecVelocity;
	pMagicAttack->m_flSpeed=vecVelocity.Length();
	pMagicAttack->pev->angles=UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner=ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING(attack);

	UTIL_SetSize(pMagicAttack->pev, Vector(-32, -32, -32), Vector(32, 32, 32));
	pMagicAttack->SetTouch(&CMagicAttack::ExplodeTouch);

	// make monsters/players afraid of it while in the air
	pMagicAttack->SetThink(&CMagicAttack::NormalThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;
	
	return pMagicAttack;
}

CMagicAttack* CMagicAttack::ShootKiBlast(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int damage)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	
	
	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything

	//set model
	SET_MODEL(ENT(pMagicAttack->pev), "sprites/kiblast.spr");

	pMagicAttack->pev->dmg = damage;
	UTIL_SetOrigin(pMagicAttack->pev,vecStart);
	pMagicAttack->pev->velocity = vecVelocity;
	pMagicAttack->m_flSpeed = vecVelocity.Length();
	pMagicAttack->pev->angles = UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("kiblast");
	pMagicAttack->pev->scale = 0.2;

	UTIL_SetSize(pMagicAttack->pev, Vector(-8, -8, -8), Vector(8, 8, 8));
	pMagicAttack->SetTouch(&CMagicAttack::BubbleTouch);	

	// make monsters/players afraid of it while in the air
	pMagicAttack->SetThink(&CMagicAttack::NormalThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;
	
	return pMagicAttack;
}

CMagicAttack* CMagicAttack::ShootOrbitBall(entvars_t *pevAimEnt,entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int damage, int r, int g, int b, short trail, int radius)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	

	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything

	//set model and sound
	SET_MODEL(ENT(pMagicAttack->pev), "models/w_beamcannon.mdl");

	pMagicAttack->m_iRadius = radius;
	pMagicAttack->m_cColor[0] = r;
	pMagicAttack->m_cColor[1] = g;
	pMagicAttack->m_cColor[2] = b;
	pMagicAttack->m_cColor[3] = 254;

	//add following trail
	pMagicAttack->AddTrail(128, 12, trail);

	pMagicAttack->pev->dmg = damage;
	UTIL_SetOrigin(pMagicAttack->pev, vecStart);
	pMagicAttack->pev->velocity = vecVelocity;
	pMagicAttack->m_flSpeed = vecVelocity.Length();
	pMagicAttack->pev->angles = UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->chain = ENT(pevAimEnt);
	pMagicAttack->pev->classname = MAKE_STRING("Special Beam Cannon");

	UTIL_SetSize(pMagicAttack->pev, Vector(-16, -16, -16), Vector(16, 16, 16));
	pMagicAttack->SetTouch(&CMagicAttack::ExplodeTouch);

	pMagicAttack->SetThink(&CMagicAttack::OrbitThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;

	return pMagicAttack;
}

CMagicAttack* CMagicAttack::CreateSpiritBomb(entvars_t *pevOwner, Vector vecStart)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();	

	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_NOT;//touches nothing
	//set model
	SET_MODEL(ENT(pMagicAttack->pev), "sprites/spiritbomb.spr");

	pMagicAttack->m_iRadius = SPIRITBOMB_RADIUS;
	pMagicAttack->m_cColor[0] = 100;
	pMagicAttack->m_cColor[1] = 100;
	pMagicAttack->m_cColor[2] = 200;
	pMagicAttack->m_cColor[3] = 200;

	UTIL_SetOrigin(pMagicAttack->pev, vecStart);
	pMagicAttack->pev->velocity = Vector(0, 0, 0);
	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("Spirit Bomb");
	pMagicAttack->pev->scale = 0.01;

	pMagicAttack->pev->dmg = 0;

	pMagicAttack->SetThink(&CMagicAttack::NormalThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;

	return pMagicAttack;
}

CMagicAttack* CMagicAttack::ShootBigBang(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, int damage, float size)
{
	CMagicAttack *pMagicAttack = GetClassPtr((CMagicAttack *)NULL);
	pMagicAttack->Spawn();
	
	pMagicAttack->pev->movetype = MOVETYPE_FLY;
	pMagicAttack->pev->solid = SOLID_TRIGGER;	//touches everything

	//set model
	SET_MODEL(ENT(pMagicAttack->pev), "sprites/bigbang.spr");

	pMagicAttack->m_cColor[0] = 75;
	pMagicAttack->m_cColor[1] = 75;
	pMagicAttack->m_cColor[2] = 200;
	pMagicAttack->m_cColor[3] = 200;

	pMagicAttack->m_iRadius = BIGBANG_RADIUS;

	pMagicAttack->pev->dmg = damage; 
	UTIL_SetOrigin(pMagicAttack->pev, vecStart);
	pMagicAttack->pev->velocity = vecVelocity;
	pMagicAttack->m_flSpeed = vecVelocity.Length();
	pMagicAttack->pev->angles = UTIL_VecToAngles(pMagicAttack->pev->velocity);
	pMagicAttack->pev->owner = ENT(pevOwner);
	pMagicAttack->pev->classname = MAKE_STRING("bigbang");
	pMagicAttack->pev->scale = size;

	int iAttackSize = (BIGBANGATTACK_SIZE * size) / 2;
	UTIL_SetSize(pMagicAttack->pev, Vector(-iAttackSize, -iAttackSize, -iAttackSize), Vector(iAttackSize, iAttackSize, iAttackSize));
	pMagicAttack->SetTouch(&CMagicAttack::ExplodeTouch);

	// make monsters/players afraid of it while in the air
	pMagicAttack->SetThink(&CMagicAttack::NormalThink);
	pMagicAttack->pev->nextthink = gpGlobals->time;
	
	return pMagicAttack;
}