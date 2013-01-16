#include <unistd.h>
#include "liste.h"
#define MAXLEN 256

extern "C" {
#include "urlcode.h"
}

int HandleForm(GuiWindow* parentWindow, GuiWindow* mainWindow, ListaDiBottoni btn, const char* ch);
int HandleMeta(Lista::iterator lista, string *link, struct block *html);

void HandleImgPad(GuiButton *btnup, GuiButton *btndown, GuiImage * image);
void HandleHtmlPad(int *offset, GuiButton *btnup, GuiButton *btndown, Indice ext, Indice Index, GuiWindow *mainWindow);
void HandleMenuBar(string *link, char* url, int *choice, int img, GuiWindow *mainWindow, GuiWindow *parentWindow);

void showBar(GuiWindow *mainWindow, GuiWindow *parentWindow);
void hideBar(GuiWindow *mainWindow, GuiWindow *parentWindow);

extern GuiWindow *statusBar;
extern bool hidden;

int getTime(string);
string getUrl(int *, string);
