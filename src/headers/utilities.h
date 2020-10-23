#ifndef UTILITIES_H
#define UTILITIES_H

BITMAP* load_sprite(const char* filename, struct RGB* pal);
void error(char* message);

void projection(int *pxe, int *pye, int x, int y, int z);
void printCount(BITMAP* buffer, FONT* font, int count, int x, int y);
void draw_trans_stretched_sprite(BITMAP* buffer, BITMAP* sprite, int x, int y, int width, int height);

int convertAxisAndDirectionToInt(Axis axis, AxisDirection direction);
char* convertAxisAndDirectionToString(Axis axis, AxisDirection direction);

void changeBlocColor(SpriteCollection* sprites);

#endif // UTILITIES_H
