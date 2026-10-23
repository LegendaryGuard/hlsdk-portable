//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Camera.h  --  defines and such for a 3rd person camera
// NOTE: must include quakedef.h first
#pragma once
#if !defined(_CAMERA_H_)
#define _CAMERA_H_

// pitch, yaw, dist
extern vec3_t cam_ofs;
// ESF - Camera point offset, relative to the camera's own axes,
// matching ESF's layout:
// [0] = right/left (cam_xoffset), 
// [1] = extra forward/back added on top of cam_ofs[2] (cam_yoffset), 
// [2] = up/down (cam_zoffset)
extern vec3_t cam_extra_ofs;
// Using third person camera
extern int cam_thirdperson;

// ESF - View mode constants
#define VIEW_FIRSTPERSON 0
#define VIEW_THIRDPERSON 1


void CAM_Init( void );
void CAM_ClearStates( void );
void CAM_StartMouseMove( void );
void CAM_EndMouseMove( void );

// ESF - Death view
void CAM_SetDeathView( qboolean dead );

// ESF - Auto first person: when the traced camera gets closer than ENTER to its
// focus point it switches to first person, and back to third person above EXIT
#define CAM_FIRSTPERSON_ENTER_EXIT	11.0f
// ESF - Third person as requested (cam_thirdperson or spectating self), ignoring the auto first person
int CAM_IsThirdPersonRequested( void );
void CAM_SetAutoFirstPerson( qboolean enable );

#endif // _CAMERA_H_
