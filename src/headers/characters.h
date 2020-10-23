#ifndef CHARACTERS_H
#define CHARACTERS_H

void initCharacters(Characters* characters, DebugInfos* debugInfos, Map map);
void resetCharacters(Characters* characters);

void rotateCharacters(Characters* characters, Map* map, Direction direction);
void moveCharacter(Character* character, Map map, Direction direction);

void drawCharacter(Character character, Map map, CharacterArea area, Bool mainCharacter, SpriteCollection sprites, BITMAP* buffer);

int isClickOnCharacter(Characters characters, Map map);

#endif // CHARACTERS_H
