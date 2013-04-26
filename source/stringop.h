#ifndef _STRINGOP_H_
#define _STRINGOP_H_

#include "common.h"

extern "C" {
#include "entities.h"
}

void downloadPath(struct block *html, char *url, char *path);
void correctPath(char *path, char *arg, int which);

enum
{
    PAGE,
    IMAGES,
    SCREENSHOT,
    OTHER
};

#endif