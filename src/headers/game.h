#ifndef GAME_H
#define GAME_H

void game(SpriteCollection sprites, WindowsInfos windowsInfos, int level, BITMAP* buffer);

void manageGameKeyboard(Map* map, Characters* character, DebugInfos debugInfos, SpriteCollection sprites, BITMAP* buffer);

int checkPerspective(Map map, Direction direction, int* nextX, int* nextY, int* nextZ);
Point getForegroundBlock(Map map, Point startingBlock, int xOffset, int yOffset, int* foregroundI);
Point getFirstIllusionBlock(Map map, Point startingBloc, int foregroundI, int xOffset, int yOffset, Direction direction);

void manageMovingBlocs(Map* map);

Bool isGameWon(Characters characters);

#endif // GAME_H
