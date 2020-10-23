#ifndef ACTIONS_H
#define ACTIONS_H

void clicOnActionMenu(Sequence* sequence, Characters characters, DebugInfos debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos);
void drawActionMenu(SpriteCollection sprites, Characters characters, MouseInfos mouseInfos, WindowsInfos windowsInfos, BITMAP* buffer);
void dragActionMenu(WindowsInfos* windowsInfos);

void checkIfDraggingMode(MouseInfos* mouseInfos);
void drawDraggingMode(MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer);
void drawDragOnSequence(DragInfos* dragInfos, Sequence* sequence, MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer);
void getDraggingID(Sequence* sequence, DragInfos* dragInfos, WindowsInfos windowsInfos);

#endif // ACTIONS_H
