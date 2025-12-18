/*
*	Client speed control
*	With special thanks to Adrian "Fireball" LaVallee
*/

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Speed, Speed);

int CHudSpeed::Init()
{
	HOOK_MESSAGE(Speed);

	gHUD.AddHudElem(this);

	return 1;
}

void CHudSpeed::SetSpeed(int speed)
{
	char cmd[50];

	sprintf(cmd, "cl_forwardspeed %d\n", speed);
	ClientCmd(cmd);
	sprintf(cmd, "cl_backspeed %d\n", speed);
	ClientCmd(cmd);
	sprintf(cmd, "cl_sidespeed %d\n", speed);
	ClientCmd(cmd);
	sprintf(cmd, "cl_movespeedkey %f\n", 0.5);
	ClientCmd(cmd);
}

int CHudSpeed::MsgFunc_Speed(const char *pszName,  int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int x = READ_SHORT();

	SetSpeed(x);

	return 1;
}