//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
//added by harSens
#include "in_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "pm_defs.h"

#define EXPLOSION_LAYERS 10 //number of explosion circles
//end harSens add

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
}

#define TEST_IT	1
#if TEST_IT

//Variables added by MoOg
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//Set in view.cpp to the correct view angles (which player->angles doesn't hold)
extern vec3_t v_angles, v_origin; 

vec3_t v_up, v_right, v_forward; //Defined out here so other areas of code can grab them

//added by harSens
HSPRITE		m_hsprCrosshair;		// Used to store the crosshair sprite
extern HSPRITE m_hsprExplosion;		// explosion texture
extern HSPRITE m_hsprExplosionBeam;	// explosion beam texture
extern HSPRITE m_hsprSBCharge;		// spiritbomb charge sprite texture
int m_iCrosshairSize;				// holds the height of the crosshair
extern explosion *tri_exp;
extern charge_sprite chargesb[200];
extern int charge_sb;

//angles for calculation of the explosion sphere
extern float cos_angle[36];
extern float sin_angle[36];
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/*
=================
Draw_Triangles

Example routine.  Draws a sprite offset from the player origin.
=================
*/

/**
* sets the crosshair sprite. overwrites the engine Setcrosshair 
* function
* @author harSens
* @param HSPRITE sprite: the new crosshair sprite, NULL for none
* @param wrect_t size: the size of the new crosshair
* @param int k: if have no clue what those do , added for compatibility with the engine function
* @param int l: see k
* @param int m: see k
*/
void SetCrosshair(HSPRITE sprite, wrect_t size, int k, int l, int m)
{
	m_hsprCrosshair = sprite;
	if (m_hsprCrosshair)
	{
		if (size.bottom - size.top > size.right - size.left)
			m_iCrosshairSize = size.bottom - size.top;
		else
			m_iCrosshairSize = size.right - size.left;
	}
	else
		m_iCrosshairSize = 0;
}

//harSens add
void DrawExplosion (explosion *exp)
{
	//calculate positions
	vec3_t circle[EXPLOSION_LAYERS + 1][36];
	float height = (float)exp->radius / (float)EXPLOSION_LAYERS;
	float radius;
	int z, n;

	for (z = 0; z <= EXPLOSION_LAYERS; z++)
	{
		if (z == EXPLOSION_LAYERS)
			radius = 0; //calculation of this one is often wrong...
		else
			radius = sqrt((exp->radius * exp->radius) - (height * z * height * z));

		for (int n = 0; n < 36; n++)
		{
			circle[z][n].x = cos_angle[n] * radius;
			circle[z][n].y = sin_angle[n] * radius;
			circle[z][n].z = height * z;
		}
	}

	vec3_t temporg;

	for (z = 0; z < EXPLOSION_LAYERS; z++)
	{
		float bright_up = 0.5 + 0.5 * (float)(z + 1) / (float)EXPLOSION_LAYERS;
		float bright_dn = 0.5 - 0.5 * (float)(z + 1) / (float)EXPLOSION_LAYERS;
		bright_up = 1.0;
		bright_dn = 1.0;

		if (!gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(m_hsprExplosion), 0))
			return;

		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->Color4ub(exp->r,exp->g,exp->b,exp->a);

		for (n = 0; n < 36; n++)
		{
			//calculate 'right' circle point
			int right;
			if (n < 35)
				right = n + 1;
			else
				right = 0;

			//upper circle half
			gEngfuncs.pTriAPI->Brightness(bright_up);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			temporg.x = exp->pos.x + circle[z][n].x;
			temporg.y = exp->pos.y + circle[z][n].y;
			temporg.z = exp->pos.z + circle[z][n].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);

			gEngfuncs.pTriAPI->Brightness(bright_up);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			temporg.x = exp->pos.x + circle[z + 1][n].x;
			temporg.y = exp->pos.y + circle[z + 1][n].y;
			temporg.z = exp->pos.z + circle[z + 1][n].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);
		
			gEngfuncs.pTriAPI->Brightness(bright_up);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			temporg.x = exp->pos.x + circle[z + 1][right].x;
			temporg.y = exp->pos.y + circle[z + 1][right].y;
			temporg.z = exp->pos.z + circle[z + 1][right].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);

			gEngfuncs.pTriAPI->Brightness(bright_up);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			temporg.x = exp->pos.x + circle[z][right].x;
			temporg.y = exp->pos.y + circle[z][right].y;
			temporg.z = exp->pos.z + circle[z][right].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);

			//lower circle half
			gEngfuncs.pTriAPI->Brightness(bright_dn);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			temporg.x = exp->pos.x + circle[z][n].x;
			temporg.y = exp->pos.y + circle[z][n].y;
			temporg.z = exp->pos.z - circle[z][n].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);

			gEngfuncs.pTriAPI->Brightness(bright_dn);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			temporg.x = exp->pos.x + circle[z + 1][n].x;
			temporg.y = exp->pos.y + circle[z + 1][n].y;
			temporg.z = exp->pos.z - circle[z + 1][n].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);
		
			gEngfuncs.pTriAPI->Brightness(bright_dn);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			temporg.x = exp->pos.x + circle[z + 1][right].x;
			temporg.y = exp->pos.y + circle[z + 1][right].y;
			temporg.z = exp->pos.z - circle[z + 1][right].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);

			gEngfuncs.pTriAPI->Brightness(bright_dn);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			temporg.x = exp->pos.x + circle[z][right].x;
			temporg.y = exp->pos.y + circle[z][right].y;
			temporg.z = exp->pos.z - circle[z][right].z;
			gEngfuncs.pTriAPI->Vertex3fv(temporg);
		}
		gEngfuncs.pTriAPI->End();

		//draw beams
		if (!gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(m_hsprExplosionBeam), 0))
			return;

		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->Color4ub(exp->r, exp->g, exp->b, exp->a);

		for (n = 0; n < 36; n++)
		{
			//draw upper beam?
			if (gEngfuncs.pfnRandomFloat(0, 20) > 19)
			{
				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				temporg.x = exp->pos.x + circle[z][n].x;
				temporg.y = exp->pos.y + circle[z][n].y;
				temporg.z = exp->pos.z + circle[z][n].z;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);

				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				temporg.x = exp->pos.x + circle[z + 1][n].x;
				temporg.y = exp->pos.y + circle[z + 1][n].y;
				temporg.z = exp->pos.z + circle[z + 1][n].z;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);
		
				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				temporg.x = exp->pos.x + circle[z + 1][n].x * 2 + circle[z][n].x;
				temporg.y = exp->pos.y + circle[z + 1][n].y * 2 + circle[z][n].y; 
				temporg.z = exp->pos.z + circle[z + 1][n].z * 2 + circle[z][n].z;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);

				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				temporg.x = exp->pos.x + circle[z + 1][n].x + circle[z][n].x * 2;
				temporg.y = exp->pos.y + circle[z + 1][n].y + circle[z][n].y * 2; 
				temporg.z = exp->pos.z + circle[z + 1][n].z + circle[z][n].z * 2;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);
			}

			//draw lower beam?
			if (gEngfuncs.pfnRandomFloat(0, 20) > 19)
			{
				gEngfuncs.pTriAPI->Brightness(bright_dn);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				temporg.x = exp->pos.x + circle[z][n].x;
				temporg.y = exp->pos.y + circle[z][n].y;
				temporg.z = exp->pos.z - circle[z][n].z;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);

				gEngfuncs.pTriAPI->Brightness(bright_dn);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				temporg.x = exp->pos.x + circle[z + 1][n].x;
				temporg.y = exp->pos.y + circle[z + 1][n].y;
				temporg.z = exp->pos.z - circle[z + 1][n].z;
				gEngfuncs.pTriAPI->Vertex3fv(temporg);
		
				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				temporg.x = exp->pos.x + circle[z + 1][n].x * 2 + circle[z][n].x;
				temporg.y = exp->pos.y + circle[z + 1][n].y * 2 + circle[z][n].y; 
				temporg.z = exp->pos.z - (circle[z + 1][n].z * 2 + circle[z][n].z);
				gEngfuncs.pTriAPI->Vertex3fv(temporg);

				gEngfuncs.pTriAPI->Brightness(bright_up);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				temporg.x = exp->pos.x + circle[z + 1][n].x + circle[z][n].x * 2;
				temporg.y = exp->pos.y + circle[z + 1][n].y + circle[z][n].y * 2; 
				temporg.z = exp->pos.z - (circle[z + 1][n].z + circle[z][n].z * 2);
				gEngfuncs.pTriAPI->Vertex3fv(temporg);
			}
		}
		gEngfuncs.pTriAPI->End();
	}
}

void Draw_WorldSprite(HSPRITE sprite, vec3_t pos, float size, int RenderMode)
{
	vec3_t temporg;

	if (!gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(sprite), 0))
		return;

	// Create a triangle, sigh
	gEngfuncs.pTriAPI->RenderMode(RenderMode);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	// Overload p->color with index into tracer palette, p->packedColor with brightness
	gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);
	// UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 0);
	temporg = pos + (v_right * (-size / 2)) + (v_up * (-size / 2));
	gEngfuncs.pTriAPI->Vertex3f(temporg.x, temporg.y, temporg.z);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(0, 1);
	temporg = pos + (v_right * (-size / 2)) + (v_up * (size / 2));
	gEngfuncs.pTriAPI->Vertex3f(temporg.x, temporg.y, temporg.z);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 1);
	temporg = pos + (v_right * (size / 2)) + (v_up * (size / 2));
	gEngfuncs.pTriAPI->Vertex3f(temporg.x, temporg.y, temporg.z);

	gEngfuncs.pTriAPI->Brightness(1);
	gEngfuncs.pTriAPI->TexCoord2f(1, 0);
	temporg = pos + (v_right * (size / 2)) + (v_up * (-size / 2));
	gEngfuncs.pTriAPI->Vertex3f(temporg.x, temporg.y, temporg.z);

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}

void Draw_Triangles(void)
{
	//added by harSens to draw explosions
	explosion *current = tri_exp;
	while (current)
	{
		DrawExplosion(current);
		current = current->next;
	}

	//added by harSens to draw spiritbomb charge sprites
	if (charge_sb)
	{
		for (int n = 0; n < 200; n++)
		{
			chargesb[n].origin = chargesb[n].origin + chargesb[n].direction * .01;
			Draw_WorldSprite(m_hsprSBCharge, chargesb[n].origin, 1, kRenderTransAdd);
		}
	}

	//added by harSens to draw the crosshair
	if (!m_hsprCrosshair)
		return;

	cl_entity_t *player;

	// Load it up with some bogus data
	player = gEngfuncs.GetLocalPlayer();
	if (!player)
		return;

	vec3_t org = player->origin;
	vec3_t view_ofs;
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
	org = org + view_ofs;
	// Done here since v_angles will definitely be holding the players angles at this point
	AngleVectors(v_angles, v_forward, v_right, v_up);
	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(player->index - 1);
	gEngfuncs.pEventAPI->EV_PlayerTrace(org, org + v_forward * 8192, PM_NORMAL, -1, &tr);

	org = tr.endpos - 5 * v_forward; //pull it a little off the hitpoint

	Draw_WorldSprite(m_hsprCrosshair, org, m_iCrosshairSize, kRenderTransAdd);
}
//end harSens add
#endif

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	gHUD.m_Spectator.DrawOverview();
#if TEST_IT
//	Draw_Triangles();
#endif
}

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
	/*removed by harSens
#if TEST_IT
	*/
	Draw_Triangles();
	/*removed by harSens
#endif
	*/
}
