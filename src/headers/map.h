#ifndef MAP_H
#define MAP_H

Map createMap(int width, int height, int depth);
Case*** mallocMapCases(int width, int height, int depth);

void loadMap(Map* map);
void loadMapFromFile(Map* map, int level);
void initCases(Map* map);
void saveMap(Map map);
void resetMap(Map* map);

void dragMap(Map* map, MouseInfos* mouseInfos);
void drawGameMap(Map map, Characters characters, SpriteCollection sprites, BITMAP* buffer);
void drawGameBackground(Map map, SpriteCollection sprites, BITMAP* buffer);
void drawEditorMap(Map map, EditorSelection selection, SpriteCollection sprites, BITMAP* buffer);
void drawLevelSelectMap(Map map, SpriteCollection sprites, BITMAP* buffer);

void rotateMap(Map* map, Direction direction);
void rotateCoordinates(int* x, int* y, Map map, Direction direction);
void rotateAction(MoveAction* action, Direction direction);

#endif // MAP_H
