//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "kbutton.h"
#include "cvardef.h"
#include "usercmd.h"
#include "const.h"
#include "in_defs.h"
#include "pm_defs.h"
#include "event_api.h" 

#if XASH_WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define WIN32_EXTRA_LEAN
#define HSPRITE WINDOWS_HSPRITE
#include <windows.h>
#undef HSPRITE
#else
typedef struct point_s
{
	int x;
	int y;
} POINT;
#define GetCursorPos(x)
#define SetCursorPos(x,y)
#endif

float CL_KeyState( kbutton_t *key );

extern "C"
{
	void DLLEXPORT CAM_Think( void );
	int DLLEXPORT CL_IsThirdPerson( void );
	void DLLEXPORT CL_CameraOffset( float *ofs );
	// ESFR - Extra offsets
	void DLLEXPORT CL_CameraExtraOffset( float *ofs );
}

extern cl_enginefunc_t gEngfuncs;

//-------------------------------------------------- Constants

#define CAM_DIST_DELTA 1.0f
#define CAM_ANGLE_DELTA 2.5f
#define CAM_ANGLE_SPEED 2.5f
#define CAM_MIN_DIST 0.0f	// ESFR - Before 30.0f
#define CAM_ANGLE_MOVE 0.5f
#define MAX_ANGLE_DIFF 10.0f
#define PITCH_MAX 90.0f
#define PITCH_MIN 0.0f
#define YAW_MAX  135.0f
#define YAW_MIN	 -135.0f

enum ECAM_Command
{
	CAM_COMMAND_NONE = 0,
	CAM_COMMAND_TOTHIRDPERSON = 1,
	CAM_COMMAND_TOFIRSTPERSON = 2
};

//-------------------------------------------------- Global Variables

cvar_t	*cam_command;
cvar_t	*cam_snapto;
cvar_t	*cam_idealyaw;
cvar_t	*cam_idealpitch;
cvar_t	*cam_idealdist;
cvar_t	*cam_contain;

// ESFR - Camera point offset along the camera's own right/up/forward axes
cvar_t	*cam_xoffset;
cvar_t	*cam_yoffset;
cvar_t	*cam_zoffset;

cvar_t	*c_maxpitch;
cvar_t	*c_minpitch;
cvar_t	*c_maxyaw;
cvar_t	*c_minyaw;
cvar_t	*c_maxdistance;
cvar_t	*c_mindistance;

// pitch, yaw, dist
vec3_t cam_ofs;

// ESFR - Camera point offset (right, up, extra-forward)
vec3_t cam_extra_ofs;

// ESFR - Global camera angles
vec3_t g_CameraAngles;

// ESFR - Which view mode is actively requested
int cam_idealview = VIEW_THIRDPERSON;

// In third person
int cam_thirdperson;
int cam_mousemove; //true if we are moving the cam with the mouse, False if not
int iMouseInUse = 0;
int cam_distancemove;
extern int mouse_x, mouse_y;  //used to determine what the current x and y values are
int cam_old_mouse_x, cam_old_mouse_y; //holds the last ticks mouse movement
POINT cam_mouse;
//-------------------------------------------------- Local Variables

static kbutton_t cam_pitchup, cam_pitchdown, cam_yawleft, cam_yawright;
static kbutton_t cam_in, cam_out, cam_move;

//-------------------------------------------------- Prototypes

void CAM_ToThirdPerson(void);
void CAM_ToFirstPerson(void);
void CAM_StartDistance(void);
void CAM_EndDistance(void);

//-------------------------------------------------- Local Functions

float MoveToward( float cur, float goal, float maxspeed )
{
	if( cur != goal )
	{
		if( fabs( cur - goal ) > 180.0f )
		{
			if( cur < goal )
				cur += 360.0f;
			else
				cur -= 360.0f;
		}

		if( cur < goal )
		{
			if( cur < goal - 1.0f )
				cur += ( goal - cur ) * 0.25f;
			else
				cur = goal;
		}
		else
		{
			if( cur > goal + 1.0f )
				cur -= ( cur - goal ) * 0.25f;
			else
				cur = goal;
		}
	}

	// bring cur back into range
	if( cur < 0.0f )
		cur += 360.0f;
	else if( cur >= 360.0f )
		cur -= 360.0f;

	return cur;
}

//-------------------------------------------------- Gobal Functions

typedef struct
{
	vec3_t		boxmins, boxmaxs;// enclose the test object along entire move
	float		*mins, *maxs;	// size of the moving object
	vec3_t		mins2, maxs2;	// size when clipping against mosnters
	float		*start, *end;
	trace_t		trace;
	int		type;
	edict_t		*passedict;
	qboolean	monsterclip;
} moveclip_t;

extern trace_t SV_ClipMoveToEntity( edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end );

void DLLEXPORT CAM_Think( void )
{
	vec3_t origin;
	vec3_t ext, pnt, camForward, camRight, camUp;
	moveclip_t	clip;
	float dist;
	vec3_t camAngles;
	float flSensitivity;
#if LATER
	int i;
#endif
	vec3_t viewangles;

	// ESFR - Don't force the camera on multiplayer
	// if( gEngfuncs.GetMaxClients() > 1 && CL_IsThirdPerson() )
	//	CAM_ToFirstPerson();

	switch( (int)cam_command->value )
	{
		case CAM_COMMAND_TOTHIRDPERSON:
			CAM_ToThirdPerson();
			break;
		case CAM_COMMAND_TOFIRSTPERSON:
			CAM_ToFirstPerson();
			break;
		case CAM_COMMAND_NONE:
		default:
			break;
	}

	if( !cam_thirdperson )
		return;
#if LATER
	if( cam_contain->value )
	{
		gEngfuncs.GetClientOrigin( origin );
		ext[0] = ext[1] = ext[2] = 0.0f;
	}
#endif
	camAngles[PITCH] = cam_idealpitch->value;
	camAngles[YAW] = cam_idealyaw->value;
	dist = cam_idealdist->value;

	//
	//movement of the camera with the mouse
	//
	if( cam_mousemove )
	{
		//get windows cursor position
		GetCursorPos( &cam_mouse );

		//check for X delta values and adjust accordingly
		//eventually adjust YAW based on amount of movement
		//don't do any movement of the cam using YAW/PITCH if we are zooming in/out the camera
		if( !cam_distancemove )
		{
			//keep the camera within certain limits around the player (ie avoid certain bad viewing angles)  
			if( cam_mouse.x>gEngfuncs.GetWindowCenterX() )
			{
				//if( ( camAngles[YAW] >= 225.0f ) || ( camAngles[YAW] < 135.0f ) )
				if( camAngles[YAW] < c_maxyaw->value )
				{
					camAngles[YAW] += CAM_ANGLE_MOVE * ( ( cam_mouse.x - gEngfuncs.GetWindowCenterX() ) / 2 );
				}
				if( camAngles[YAW] > c_maxyaw->value )
				{
					camAngles[YAW] = c_maxyaw->value;
				}
			}
			else if( cam_mouse.x<gEngfuncs.GetWindowCenterX() )
			{
				//if( ( camAngles[YAW] <= 135.0f ) || ( camAngles[YAW] > 225.0f ) )
				if( camAngles[YAW] > c_minyaw->value )
				{
					camAngles[YAW] -= CAM_ANGLE_MOVE * ( ( gEngfuncs.GetWindowCenterX() - cam_mouse.x ) / 2 );
				}
				if( camAngles[YAW] < c_minyaw->value )
				{
					camAngles[YAW] = c_minyaw->value;
				}
			}

			//check for y delta values and adjust accordingly
			//eventually adjust PITCH based on amount of movement
			//also make sure camera is within bounds
			if( cam_mouse.y>gEngfuncs.GetWindowCenterY() )
			{
				if( camAngles[PITCH] < c_maxpitch->value )
				{
					camAngles[PITCH] += CAM_ANGLE_MOVE * ( ( cam_mouse.y - gEngfuncs.GetWindowCenterY() ) / 2 );
				}
				if( camAngles[PITCH] > c_maxpitch->value )
				{
					camAngles[PITCH] = c_maxpitch->value;
				}
			}
			else if( cam_mouse.y<gEngfuncs.GetWindowCenterY() )
			{
				if( camAngles[PITCH] > c_minpitch->value )
				{
					camAngles[PITCH] -= CAM_ANGLE_MOVE * ( ( gEngfuncs.GetWindowCenterY() - cam_mouse.y ) / 2 );
				}
				if( camAngles[PITCH] < c_minpitch->value )
				{
					camAngles[PITCH] = c_minpitch->value;
				}
			}

			//set old mouse coordinates to current mouse coordinates
			//since we are done with the mouse
			if( ( flSensitivity = gHUD.GetSensitivity() ) != 0 )
			{
				cam_old_mouse_x = cam_mouse.x * flSensitivity;
				cam_old_mouse_y = cam_mouse.y * flSensitivity;
			}
			else
			{
				cam_old_mouse_x = cam_mouse.x;
				cam_old_mouse_y = cam_mouse.y;
			}
			SetCursorPos( gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY() );
		}
	}

	//Nathan code here
	if( CL_KeyState( &cam_pitchup ) )
		camAngles[PITCH] += CAM_ANGLE_DELTA;
	else if( CL_KeyState( &cam_pitchdown ) )
		camAngles[PITCH] -= CAM_ANGLE_DELTA;

	if( CL_KeyState( &cam_yawleft ) )
		camAngles[YAW] -= CAM_ANGLE_DELTA;
	else if( CL_KeyState( &cam_yawright ) )
		camAngles[YAW] += CAM_ANGLE_DELTA;

	if( CL_KeyState( &cam_in ) )
	{
		dist -= CAM_DIST_DELTA;
		if( dist < CAM_MIN_DIST )
		{
			// If we go back into first person, reset the angle
			camAngles[PITCH] = 0;
			camAngles[YAW] = 0;
			dist = CAM_MIN_DIST;
		}
	}
	else if( CL_KeyState( &cam_out ) ) {
		dist += CAM_DIST_DELTA;
		// ESFR - Limit to 128
		if( dist > 128.0f )
			dist = 128.0f;
	}

	if( cam_distancemove )
	{
		if( cam_mouse.y > gEngfuncs.GetWindowCenterY() )
		{
			if( dist < c_maxdistance->value )
			{
			    dist += CAM_DIST_DELTA * ( ( cam_mouse.y - gEngfuncs.GetWindowCenterY() ) / 2);
			}
			if( dist > c_maxdistance->value )
			{
				dist = c_maxdistance->value;
			}
		}
		else if( cam_mouse.y < gEngfuncs.GetWindowCenterY() )
		{
			if( dist > c_mindistance->value )
			{
			   dist -= CAM_DIST_DELTA * ( ( gEngfuncs.GetWindowCenterY() - cam_mouse.y ) / 2 );
			}
			if ( dist < c_mindistance->value )
			{
				dist = c_mindistance->value;
			}
		}
		//set old mouse coordinates to current mouse coordinates
		//since we are done with the mouse
		cam_old_mouse_x = cam_mouse.x * gHUD.GetSensitivity();
		cam_old_mouse_y = cam_mouse.y * gHUD.GetSensitivity();
		SetCursorPos( gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY() );
	}
#if LATER
	if( cam_contain->value )
	{
		// check new ideal
		VectorCopy( origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for( i = 0; i < 3; i++ )
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset( &clip, 0, sizeof(moveclip_t) );
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if( clip.trace.fraction == 1.0 )
		{
			// update ideal
			cam_idealpitch->value = camAngles[PITCH];
			cam_idealyaw->value = camAngles[YAW];
			cam_idealdist->value = dist;
		}
	}
	else
#endif
	{
		// update ideal
		cam_idealpitch->value = camAngles[PITCH];
		cam_idealyaw->value = camAngles[YAW];
		cam_idealdist->value = dist;
	}

	// Move towards ideal
	VectorCopy( cam_ofs, camAngles );

	gEngfuncs.GetViewAngles( (float *)viewangles );

	if( cam_snapto->value )
	{
		camAngles[YAW] = cam_idealyaw->value + viewangles[YAW];
		camAngles[PITCH] = cam_idealpitch->value + viewangles[PITCH];
		camAngles[2] = cam_idealdist->value;
	}
	else
	{
		if( camAngles[YAW] - viewangles[YAW] != cam_idealyaw->value )
			camAngles[YAW] = MoveToward( camAngles[YAW], cam_idealyaw->value + viewangles[YAW], CAM_ANGLE_SPEED );

		if( camAngles[PITCH] - viewangles[PITCH] != cam_idealpitch->value )
			camAngles[PITCH] = MoveToward( camAngles[PITCH], cam_idealpitch->value + viewangles[PITCH], CAM_ANGLE_SPEED );

		if( fabs( camAngles[2] - cam_idealdist->value ) < 2.0f )
			camAngles[2] = cam_idealdist->value;
		else
			camAngles[2] += ( cam_idealdist->value - camAngles[2] ) * 0.25f;
	}

	// ESFR - Trace from the camera pivot
	{
		pmtrace_t tr;
		vec3_t camForward, camRight, camUp;
		AngleVectors( camAngles, camForward, camRight, camUp );

		// Defaults: nothing blocking, use the full distance
		float tracedDist = dist;
		float tracedFactor = 1.0f;
		cl_entity_t *localPlayer = gEngfuncs.GetLocalPlayer();
		if( localPlayer && gEngfuncs.pEventAPI )
		{
			vec3_t view_ofs;
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight( view_ofs );

			vec3_t pivot;
			VectorAdd( localPlayer->origin, view_ofs, pivot );

			vec3_t fullOfs;
			VectorScale( camForward, -dist, fullOfs );
			VectorMA( fullOfs, cam_xoffset->value, camRight, fullOfs );
			VectorMA( fullOfs, cam_yoffset->value, camForward, fullOfs );
			VectorMA( fullOfs, cam_zoffset->value, camUp, fullOfs );

			vec3_t camPos;
			VectorAdd( pivot, fullOfs, camPos );

			gEngfuncs.pEventAPI->EV_SetSolidPlayers( localPlayer->index - 1 );
			gEngfuncs.pEventAPI->EV_SetTraceHull( 0 );	// point-sized probe for the camera
			gEngfuncs.pEventAPI->EV_PlayerTrace( pivot, camPos, PM_NORMAL, -1, &tr );

			float frac = 1.0f;
			if( tr.startsolid || tr.allsolid )
				frac = 0.0f;
			else if( tr.fraction < 1.0f )
				frac = tr.fraction;

			// Real length of the ray so the skin doesn't depend on the forward
			// component alone
			float totalLen = sqrt( DotProduct( fullOfs, fullOfs ) );
			if( totalLen < 1e-4f )
				totalLen = 1e-4f;

			// Pull in a few units from the impact so the near plane doesn't clip
			float hitLen = frac * totalLen - 4.0f;
			if( hitLen < 0.0f )
				hitLen = 0.0f;

			// Minimum distance from the pivot
			const float MIN_CAM_DIST = 16.0f;
			if( hitLen < MIN_CAM_DIST )
				hitLen = MIN_CAM_DIST;

			// hitLen is measured along the fullOfs ray
			float forwardScale = dist / totalLen;
			tracedDist = hitLen * forwardScale;
			tracedFactor = hitLen / totalLen;
		}

		// Smooth: snap in
		static float smoothedDist = -1.0f;
		static float smoothedFactor =  1.0f;
		if( smoothedDist < 0.0f )
		{
			smoothedDist = tracedDist;
			smoothedFactor = tracedFactor;
		}
		else if( tracedDist < smoothedDist )
		{
			smoothedDist = tracedDist;
			smoothedFactor = tracedFactor;
		}
		else
		{
			smoothedDist += ( tracedDist - smoothedDist ) * 0.1f;
			smoothedFactor += ( tracedFactor - smoothedFactor ) * 0.1f;
		}

		camAngles[2] = smoothedDist;

		// scale the extra offsets
		cam_extra_ofs[0] = smoothedFactor * cam_xoffset->value;
		cam_extra_ofs[1] = smoothedFactor * cam_yoffset->value;
		cam_extra_ofs[2] = smoothedFactor * cam_zoffset->value;
	}
	// synchronize distance by trace
	dist = camAngles[2];

#if LATER
	if( cam_contain->value )
	{
		// Test new position
		dist = camAngles[ROLL];
		camAngles[ROLL] = 0;

		VectorCopy( origin, pnt );
		AngleVectors( camAngles, camForward, camRight, camUp );
		for( i = 0; i < 3; i++ )
			pnt[i] += -dist * camForward[i];

		// check line from r_refdef.vieworg to pnt
		memset( &clip, 0, sizeof(moveclip_t) );
		ext[0] = ext[1] = ext[2] = 0.0f;
		clip.trace = SV_ClipMoveToEntity( sv.edicts, r_refdef.vieworg, ext, ext, pnt );
		if( clip.trace.fraction != 1.0f )
			return;
	}
#endif

	cam_ofs[0] = camAngles[0];
	cam_ofs[1] = camAngles[1];
	cam_ofs[2] = dist;

	// ESFR - Save camera angles so that other modules can read them
	VectorCopy( camAngles, g_CameraAngles );
	g_CameraAngles[2] = 0.0f;
}

extern void KeyDown( kbutton_t *b );	// HACK
extern void KeyUp( kbutton_t *b );	// HACK

void CAM_PitchUpDown( void )
{
	KeyDown( &cam_pitchup );
}

void CAM_PitchUpUp( void )
{
	KeyUp( &cam_pitchup );
}

void CAM_PitchDownDown( void )
{
	KeyDown( &cam_pitchdown );
}

void CAM_PitchDownUp( void )
{
	KeyUp( &cam_pitchdown );
}

void CAM_YawLeftDown( void )
{
	KeyDown( &cam_yawleft );
}

void CAM_YawLeftUp( void )
{
	KeyUp( &cam_yawleft );
}

void CAM_YawRightDown( void )
{
	KeyDown( &cam_yawright );
}

void CAM_YawRightUp( void )
{
	KeyUp( &cam_yawright );
}

void CAM_InDown( void )
{
	KeyDown( &cam_in );
}

void CAM_InUp( void )
{
	KeyUp( &cam_in );
}

void CAM_OutDown( void )
{
	KeyDown( &cam_out );
}

void CAM_OutUp( void )
{
	KeyUp( &cam_out );
}

void CAM_ToThirdPerson( void )
{
	vec3_t viewangles;

	// ESFR - Allow third person camera on multiplayer
	//#if !_DEBUG
	//	if( gEngfuncs.GetMaxClients() > 1 )
	//	{
	//		// no thirdperson in multiplayer.
	//		return;
	//	}
	//#endif
	cam_idealview = VIEW_THIRDPERSON;

	gEngfuncs.GetViewAngles( (float *)viewangles );

	if( !cam_thirdperson )
	{
		cam_thirdperson = 1; 

		cam_ofs[YAW] = viewangles[YAW]; 
		cam_ofs[PITCH] = viewangles[PITCH]; 
		cam_ofs[2] = CAM_MIN_DIST; 
	}

	gEngfuncs.Cvar_SetValue( "cam_command", 0 );
}

void CAM_ToFirstPerson( void ) 
{ 
	cam_thirdperson = 0;
	cam_idealview = VIEW_FIRSTPERSON;
	
	gEngfuncs.Cvar_SetValue( "cam_command", 0 );
}

void CAM_ToggleSnapto( void )
{ 
	cam_snapto->value = !cam_snapto->value;
}

void CAM_Init( void )
{
	gEngfuncs.pfnAddCommand( "+campitchup", CAM_PitchUpDown );
	gEngfuncs.pfnAddCommand( "-campitchup", CAM_PitchUpUp );
	gEngfuncs.pfnAddCommand( "+campitchdown", CAM_PitchDownDown );
	gEngfuncs.pfnAddCommand( "-campitchdown", CAM_PitchDownUp );
	gEngfuncs.pfnAddCommand( "+camyawleft", CAM_YawLeftDown );
	gEngfuncs.pfnAddCommand( "-camyawleft", CAM_YawLeftUp );
	gEngfuncs.pfnAddCommand( "+camyawright", CAM_YawRightDown );
	gEngfuncs.pfnAddCommand( "-camyawright", CAM_YawRightUp );
	gEngfuncs.pfnAddCommand( "+camin", CAM_InDown );
	gEngfuncs.pfnAddCommand( "-camin", CAM_InUp );
	gEngfuncs.pfnAddCommand( "+camout", CAM_OutDown );
	gEngfuncs.pfnAddCommand( "-camout", CAM_OutUp );
	gEngfuncs.pfnAddCommand( "thirdperson", CAM_ToThirdPerson );
	gEngfuncs.pfnAddCommand( "firstperson", CAM_ToFirstPerson );
	gEngfuncs.pfnAddCommand( "+cammousemove",CAM_StartMouseMove);
	gEngfuncs.pfnAddCommand( "-cammousemove",CAM_EndMouseMove);
	gEngfuncs.pfnAddCommand( "+camdistance", CAM_StartDistance );
	gEngfuncs.pfnAddCommand( "-camdistance", CAM_EndDistance );
	gEngfuncs.pfnAddCommand( "snapto", CAM_ToggleSnapto );

	cam_command			= gEngfuncs.pfnRegisterVariable( "cam_command", "0", 0 );	 // tells camera to go to thirdperson
	cam_snapto			= gEngfuncs.pfnRegisterVariable( "cam_snapto", "0", 0 );	 // snap to thirdperson view
	cam_idealyaw			= gEngfuncs.pfnRegisterVariable( "cam_idealyaw", "0", 0 );	 // thirdperson yaw
	cam_idealpitch			= gEngfuncs.pfnRegisterVariable( "cam_idealpitch", "0", 0 );	 // thirperson pitch
	cam_idealdist			= gEngfuncs.pfnRegisterVariable( "cam_idealdist", "40", 0 );	// ESFR - Modified to 40, before: 128	 // thirdperson distance
	cam_contain			= gEngfuncs.pfnRegisterVariable( "cam_contain", "0", 0 );	// contain camera to world

	// ESFR - Camera point offset along the camera's own axes, matching
	// layout: x = right/left, y = extra forward/back, z = up/down
	cam_xoffset			= gEngfuncs.pfnRegisterVariable( "cam_xoffset", "0", 0 );	// right/left
	cam_yoffset			= gEngfuncs.pfnRegisterVariable( "cam_yoffset", "0", 0 );	// extra forward/back
	cam_zoffset			= gEngfuncs.pfnRegisterVariable( "cam_zoffset", "10", 0 );	// up/down

	c_maxpitch			= gEngfuncs.pfnRegisterVariable( "c_maxpitch", "90.0", 0 );
	c_minpitch			= gEngfuncs.pfnRegisterVariable( "c_minpitch", "0.0", 0 );
	c_maxyaw			= gEngfuncs.pfnRegisterVariable( "c_maxyaw", "135.0", 0 );
	c_minyaw			= gEngfuncs.pfnRegisterVariable( "c_minyaw", "-135.0", 0 );
	c_maxdistance			= gEngfuncs.pfnRegisterVariable( "c_maxdistance", "200.0", 0 );
	c_mindistance			= gEngfuncs.pfnRegisterVariable( "c_mindistance", "30.0", 0 );
}

void CAM_ClearStates( void )
{
	vec3_t viewangles;

	gEngfuncs.GetViewAngles( (float *)viewangles );

	cam_pitchup.state = 0;
	cam_pitchdown.state = 0;
	cam_yawleft.state = 0;
	cam_yawright.state = 0;
	cam_in.state = 0;
	cam_out.state = 0;

	// ESFR - Force third person camera at the start
	cam_thirdperson = 1;
	cam_command->value = 0;
	cam_mousemove=0;

	cam_snapto->value = 0;
	cam_distancemove = 0;

	cam_ofs[0] = 0.0;
	cam_ofs[1] = 0.0;
	cam_ofs[2] = 40.0f;

	// ESFR - Extra offsets
	cam_extra_ofs[0] = cam_xoffset ? cam_xoffset->value : 0.0f;
	cam_extra_ofs[1] = cam_yoffset ? cam_yoffset->value : 0.0f;
	cam_extra_ofs[2] = cam_zoffset ? cam_zoffset->value : 0.0f;

	cam_idealpitch->value = viewangles[PITCH];
	cam_idealyaw->value = viewangles[YAW];
	cam_idealdist->value = 40.0f;

	// ESFR - Force third person camera at the start
	gEngfuncs.Cvar_SetValue( "cam_command", CAM_COMMAND_TOTHIRDPERSON );
	cam_idealview = VIEW_THIRDPERSON;
}

void CAM_StartMouseMove( void )
{
	float flSensitivity;

	//only move the cam with mouse if we are in third person.
	if( cam_thirdperson )
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if( !cam_mousemove )
		{
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos( &cam_mouse );

			if( ( flSensitivity = gHUD.GetSensitivity() ) != 0 )
			{
				cam_old_mouse_x = cam_mouse.x * flSensitivity;
				cam_old_mouse_y = cam_mouse.y * flSensitivity;
			}
			else
			{
				cam_old_mouse_x = cam_mouse.x;
				cam_old_mouse_y = cam_mouse.y;
			}

			// ESFR - Recenter immediately
			SetCursorPos( gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY() );
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{   
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndMouseMove( void )
{
	cam_mousemove = 0;
	iMouseInUse = 0;
}

//----------------------------------------------------------
//routines to start the process of moving the cam in or out 
//using the mouse
//----------------------------------------------------------
void CAM_StartDistance( void )
{
	//only move the cam with mouse if we are in third person.
	if( cam_thirdperson )
	{
		//set appropriate flags and initialize the old mouse position
		//variables for mouse camera movement
		if( !cam_distancemove )
		{
			cam_distancemove = 1;
			cam_mousemove = 1;
			iMouseInUse = 1;
			GetCursorPos( &cam_mouse );
			cam_old_mouse_x = cam_mouse.x * gHUD.GetSensitivity();
			cam_old_mouse_y = cam_mouse.y * gHUD.GetSensitivity();

			// ESFR - Recenter immediately
			SetCursorPos( gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY() );
		}
	}
	//we are not in 3rd person view..therefore do not allow camera movement
	else
	{
		cam_distancemove = 0;
		cam_mousemove = 0;
		iMouseInUse = 0;
	}
}

//the key has been released for camera movement
//tell the engine that mouse camera movement is off
void CAM_EndDistance( void )
{
	cam_distancemove = 0;
	cam_mousemove = 0;
	iMouseInUse = 0;
}

int DLLEXPORT CL_IsThirdPerson( void )
{
	return ( cam_thirdperson ? 1 : 0 ) || ( g_iUser1 && ( g_iUser2 == gEngfuncs.GetLocalPlayer()->index ) );
}

void DLLEXPORT CL_CameraOffset( float *ofs )
{
	VectorCopy( cam_ofs, ofs );
}

// ESFR - Extra offsets
void DLLEXPORT CL_CameraExtraOffset( float *ofs )
{
	VectorCopy( cam_extra_ofs, ofs );
}
