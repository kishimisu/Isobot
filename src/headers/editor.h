#ifndef EDITOR_H
#define EDITOR_H

void editor(SpriteCollection sprites, BITMAP* buffer);

void initEditorSelection(EditorSelection* selection);
void manageEditorKeyboard(Map* map, EditorSelection* selection, SpriteCollection sprites, BITMAP* snapshot, BITMAP* buffer);

void drawWalls(SpriteCollection sprites, Map map, BITMAP* buffer);
void drawSelection(EditorSelection selection, Map map, SpriteCollection sprites, BITMAP* buffer);
void drawHelpers(SpriteCollection sprites, EditorSelection selection, Map map, int x, int y, int z, BITMAP* buffer);

void addLinkedCase(Case* linkageCase, Point newLink);
void removeLinkedCase(Case* linkageCase, Point newLink);

Bool isAlreadyLinked(Case linkSource, Point newLink);
void linkageMenu(Case* linkage, FONT* customFont, BITMAP* snapshot, BITMAP* buffer);
Bool manageLinkageMenuKeyboard(Case* linkage, int *row);

#endif // EDITOR_H
