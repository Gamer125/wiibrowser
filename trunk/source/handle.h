#include <unistd.h>
#include "liste.h"
#define MAXLEN 100

extern "C" {
#include "urlcode.h"
}

int HandleForm(GuiWindow* parentWindow, GuiWindow* mainWindow, ListaDiBottoni btn);
void HandleImgPad(GuiButton *btnup, GuiButton *btndown, GuiImage * image);

void HandleHtmlPad(int *offset, GuiButton *btnup, GuiButton *btndown, Indice ext, Indice Index, GuiWindow *mainWindow);
void HandleMenuBar(string *link, int *choice, int img, GuiWindow *mainWindow, GuiWindow *parentWindow);

void showBar(GuiWindow *mainWindow, GuiWindow *parentWindow);
void hideBar(GuiWindow *mainWindow, GuiWindow *parentWindow);
void delAllElements();

extern GuiWindow *statusBar;
extern bool hidden;

int getTime(string);
string getUrl(int *, string);