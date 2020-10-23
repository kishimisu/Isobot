#ifndef MOUSE_H
#define MOUSE_H

void initMouseInfos(MouseInfos* mouseInfos);

void detectMouseFocus(Sequence* sequence, Characters* characters, Map map, DebugInfos* debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos);
Bool isMouseOnButton(ClicFocus window, int buttonNumber, WindowsInfos windowsInfos);

void releaseMouse(Sequence* sequences, Characters* characters, Map* map, DebugInfos* debugInfos, MouseInfos* mouseInfos, DragInfos* dragInfos, WindowsInfos windowsInfos);

#endif // MOUSE_H
