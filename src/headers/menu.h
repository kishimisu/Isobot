#ifndef MENU_H
#define MENU_H

int main_menu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer);

void clicButton(MouseInfos* mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites);
int releaseButton(MouseInfos* mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites);
void detectMouseOverButton(ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites);

void drawMainMenuButtons(SpriteCollection sprites, MouseInfos mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, BITMAP* buffer);
void drawMainMenuBackground(SpriteCollection sprites, BITMAP* buffer);
void drawRandomTile(SpriteCollection sprites, BITMAP* buffer);

void levelSelectMenu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer);
int updateBackgroundColor(Bool init);

void victoryMenu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer);
void helpMenu(SpriteCollection sprites, BITMAP* buffer);

#endif // MENU_H
