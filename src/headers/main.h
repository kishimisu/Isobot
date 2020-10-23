#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro.h>
#include <string.h>
#include <math.h>

#include "structures.h"

#include "action.h"
#include "characters.h"
#include "editor.h"
#include "game.h"
#include "map.h"
#include "menu.h"
#include "mouse.h"
#include "sequence.h"
#include "utilities.h"

#define Echelle 16

#define TileWidth 65
#define TileHeight 65

#define CharacterWidth 65
#define CharacterHeight 96
#define CharacterOffset 8

#define DefaultMapZoom 1

#define DefaultActionMenuOffsetX 400
#define DefaultActionMenuOffsetY 450
#define DefaultActionMenuWidth 472
#define DefaultActionMenuHeight 312

#define DefaultSequenceMenuOffsetX 800
#define DefaultSequenceMenuOffsetY 40
#define DefaultSequenceMenuWidth 472
#define DefaultSequenceMenuHeight 745

#define DefaultButtonSize 80

#define None -1

volatile int game_time;

void game_timer();
void initTimer();

void initAllegro();
Bool setWorkingGFXmode();

void initSpriteCollection(SpriteCollection* sprites);
void initWindowsInfos(WindowsInfos* windowsInfos);

#endif // MAIN_H
