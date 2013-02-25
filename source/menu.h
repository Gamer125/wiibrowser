/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#undef TIME
#undef DEBUG

#include <libwiigui/gui.h>
#include <curl/curl.h>

#include <ogcsys.h>
#include <update.h>
#include <string>

extern "C" {
#include "entities.h"
}

void InitGUIThreads();
void MainMenu (int menuitem);
void StopGUIThreads();

void EnableVideoImg();
bool VideoImgVisible();
bool LoadYouTubeFile(char *newurl, char *data);

extern u8 HWButton;
extern CURL *curl_handle;

enum
{
	MENU_EXIT = -1,
	MENU_NONE,
	MENU_SPLASH,
	MENU_SETTINGS,
	MENU_FAVORITES,
	MENU_BROWSE,
	MENU_HOME,
	MENU_DEFAULT
};

enum
{
    NET_ERR = 1,
    IP_ERR
};

#ifdef __cplusplus
extern "C" {
#endif
    void DisableVideoImg();
    void UpdatePointer();
    void DoMPlayerGuiDraw();

    void ShowAction (const char *msg);
    void CancelAction();
#ifdef __cplusplus
}
#endif

#endif