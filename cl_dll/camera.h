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
// ESFR - Camera point offset, relative to the camera's own axes,
// matching ESF's layout:
// [0] = right/left (cam_xoffset), 
// [1] = extra forward/back added on top of cam_ofs[2] (cam_yoffset), 
// [2] = up/down (cam_zoffset)
extern vec3_t cam_extra_ofs;
// Using third person camera
extern int cam_thirdperson;

// ESFR - View mode constants
#define VIEW_FIRSTPERSON 0
#define VIEW_THIRDPERSON 1


void CAM_Init( void );
void CAM_ClearStates( void );
void CAM_StartMouseMove( void );
void CAM_EndMouseMove( void );
#endif // _CAMERA_H_
