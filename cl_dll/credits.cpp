#include "hud.h"
#include "parsemsg.h"
#include "cl_util.h"

#include <string.h>

#define CREDITS_XPOS1 (ScreenWidth / 24)
#define CREDITS_XPOS2 (ScreenWidth * 3 / 8)

DECLARE_COMMAND(m_Credits, ShowCredits);

int CHudCredits::Init()
{
	HOOK_COMMAND("credits", ShowCredits);

	//load credits
	char buffer[10000];
	CHudTextMessage::LocaliseTextString("#Credits", buffer,  10000);

	int i = 0;
	int j = 0;
	int left_text = false;
	char string[255];
	for (int n = 0; n < 10000; n++)
	{
		string[j] = buffer[n];
		if (buffer[n] == '\n' || j >= 255 || buffer[n] == 0)
		{
			string[j] = 0;
			strcpy(m_pRightCredits[i], string);
			if (!left_text)
				strcpy(m_pLeftCredits[i], "\0");
			else
				left_text = false;
			i++;
			j = 0;
			if (!buffer[n])
				break;
		}
		else if (buffer[n] == ':')
		{
			left_text = true;
			string[j + 1] = 0;
			strcpy(m_pLeftCredits[i], string);
			j = 0;
			//skip next \n
			while (buffer[n] != '\n')
			{
				n++;
			}
		}
		else
		{
			j++;
		}
	}
	m_iLastCredit = i;

	m_fShowing = false;
	m_iCreditNr = 0;
	m_flYOffset = 0;
	m_flNextScroll = 0;
	gHUD.AddHudElem(this);
	m_iFlags |= HUD_ACTIVE;
	return 1;
}

int CHudCredits::Draw(float flTime)
{
	if (!m_fShowing)
		return 1;

	// scroll one up
	if (flTime > m_flNextScroll + 0.2)
	{
		if (m_flYOffset + gHUD.m_scrinfo.iCharHeight / 4 >= gHUD.m_scrinfo.iCharHeight && m_iCreditNr < m_iLastCredit)
		{
			m_iCreditNr++;
			m_flYOffset = 0;
		}
		else
			m_flYOffset += gHUD.m_scrinfo.iCharHeight / 4;
		
		if (m_iCreditNr >= m_iLastCredit && m_flYOffset >= ScreenHeight)
			m_fShowing = false;

		m_flNextScroll = flTime;
	}

	// draw credits
	int xpos1 = CREDITS_XPOS1;
	int xpos2 = CREDITS_XPOS2;
	int ypos1 = ScreenHeight - gHUD.m_scrinfo.iCharHeight - m_flYOffset;
	int ypos2 = ypos1;
	for (int i = m_iCreditNr; i >= 0; i--)
	{
		for (int j = 0; j < 255 && m_pLeftCredits[i][j]; j++)
		{
			TextMessageDrawChar(xpos1, ypos1, m_pLeftCredits[i][j], 255, 255, 255);
			xpos1 += gHUD.m_scrinfo.charWidths[ m_pLeftCredits[i][j] ];
			if (xpos1>CREDITS_XPOS2 - 2 * gHUD.m_scrinfo.charWidths['W'])
			{
				ypos1 -= gHUD.m_scrinfo.iCharHeight;
				xpos1 = CREDITS_XPOS1;
			}
		}
		for (int k = 0; k < 255 && m_pRightCredits[i][k]; k++)
		{
			TextMessageDrawChar(xpos2, ypos2, m_pRightCredits[i][k], 255, 255, 255);
			xpos2 += gHUD.m_scrinfo.charWidths[ m_pRightCredits[i][k] ];
			if(xpos2 > ScreenWidth - 2 * gHUD.m_scrinfo.charWidths['W'])
			{
				ypos2 -= gHUD.m_scrinfo.iCharHeight;
				xpos2 = CREDITS_XPOS2;
			}
		}
		ypos1 -= gHUD.m_scrinfo.iCharHeight;
		ypos2 = ypos1;
		xpos1 = CREDITS_XPOS1;
		xpos2 = CREDITS_XPOS2;
		if (ypos1 < 0) break;
	}
	return 1;
}

void CHudCredits::UserCmd_ShowCredits()
{
	if (!m_fShowing)
	{
		m_fShowing = true;
		m_iCreditNr = 0;
	}
}