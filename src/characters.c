#include "headers/main.h"

void initCharacters(Characters* characters, DebugInfos* debugInfos, Map map)
{
    /** Initializes the characters structure and all the characters properties */

    int current = 0, charactersCount = 0;
    int x, y, z;

    // loop through each case in order to get the number of characters of the level
    for(z = 0; z < map.depth; z++)
        for(y = 0; y < map.height; y++)
            for(x = 0; x < map.width; x++)
                if(map.cases[x][y][z].type == Start) // if it's a "start" bloc
                    charactersCount++; // increment the character counter

    characters->current = 0; // initialize the structure properties
    characters->count = charactersCount;
    characters->entity = malloc(charactersCount * sizeof(Character)); // allocate memory for all the characters

    debugInfos->count = malloc(charactersCount * sizeof(int));
    debugInfos->currentAction = malloc(charactersCount * sizeof(int));

    if(characters->entity == NULL || debugInfos->count == NULL || debugInfos->currentAction == NULL) error("malloc in initCharacters()");

    // loop through each case
    for(z = 0; z < map.depth; z++)
        for(y = 0; y < map.height; y++)
            for(x = 0; x < map.width; x++)
                if(map.cases[x][y][z].type == Start) // if it's a "Start' bloc
                {
                    characters->entity[current].defaultPos.x = x; // initialize a new character
                    characters->entity[current].defaultPos.y = y;
                    characters->entity[current].defaultPos.z = z + 1;
                    characters->entity[current].direction = Right;
                    characters->entity[current].didWin = False;

                    current++;
                }
}

void resetCharacters(Characters* characters)
{
    /** Reset all the characters to their default position */

    int i;

    for(i = 0; i < characters->count; i++) // for each character
    {
        characters->entity[i].x = characters->entity[i].defaultPos.x; // reset its properties
        characters->entity[i].y = characters->entity[i].defaultPos.y;
        characters->entity[i].z = characters->entity[i].defaultPos.z;
        characters->entity[i].direction = Right;
        characters->entity[i].didWin = False;
    }
}

void moveCharacter(Character* character, Map map, Direction direction)
{
    /** Move the character by trying to check if there is a perspective illusion, else the standard way */

    // variables to make the code breathe
    int x, y, z;
    //Bool validAction = False;
    Case* newActiveBloc, *actionBloc;

    int i;
    int nextX = character->x, nextY = character->y, nextZ = character->z;

    nextZ -= 1; // the next Z is under the character, not the character's Z itself

    if(!checkPerspective(map, direction, &nextX, &nextY, &nextZ)) // if no perspective illusion has been found
    {
        nextZ += 1;

        // move the character the standard way
        switch(direction)
        {
            case Left:
                nextX -= 1;
            break;
            case Right:
                nextX += 1;
            break;
            case Up:
                nextY -= 1;
            break;
            case Down:
                nextY += 1;
            break;
        }
    }

    // if the next position isn't out of bound
    if(nextX >= 0 && nextX < map.width && nextY >= 0 && nextY < map.height && nextZ >= 0 && nextZ < map.depth+1)
    {
        // if there is a bloc under the destination bloc and two empty blocs on top of it
        if(map.cases[nextX][nextY][nextZ].type == None && map.cases[nextX][nextY][nextZ-1].type != None && map.cases[nextX][nextY][nextZ+1].type == None)
        {
            // update the character's position
            character->x = nextX;
            character->y = nextY;
            character->z = nextZ;
            //validAction = True;
        }

        // if the user walked on an action bloc
        if(map.cases[nextX][nextY][nextZ-1].actionBloc)
        {
            actionBloc = &map.cases[nextX][nextY][nextZ-1]; // "breathe variable"

            // for every linked bloc
            for(i = 0; i < actionBloc->actionNumber; i++)
            {
                x = actionBloc->linkedCases[i].x; // "breathe" variables
                y = actionBloc->linkedCases[i].y;
                z = actionBloc->linkedCases[i].z;

                newActiveBloc = &map.cases[x][y][z];

                // initialize and start the linked bloc's action
                newActiveBloc->action = actionBloc->action;
                newActiveBloc->action.current = 0;
                newActiveBloc->action.startingTime = clock();
                newActiveBloc->action.isMoving = True;
            }
        }
    }

    character->direction = direction;
}

void rotateCharacters(Characters* characters, Map* map, Direction direction)
{
    /** Rotate all the characters when the map is rotated */

    int i;

    // for every character
    for(i = 0; i < characters->count; i++)
    {
        rotateCoordinates(&characters->entity[i].x, &characters->entity[i].y, *map, direction); // rotate its coordinates

        // update the character direction
        if(direction == Right) characters->entity[i].direction = characters->entity[i].direction + 1;
        else if(characters->entity[i].direction == 0) characters->entity[i].direction = 3;
        else characters->entity[i].direction = characters->entity[i].direction - 1;
        if(characters->entity[i].direction > 3) characters->entity[i].direction = 0;
    }
}

void drawCharacter(Character character, Map map, CharacterArea area, Bool mainCharacter, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the lower or the upper part of the character's sprite depending on the z position in the drawMap function */

    int x = character.x;
    int y = character.y;
    int z = character.z;

    int newX , newY;

    BITMAP* sprite;

    if(mainCharacter)
        sprite = sprites.mainCharacter[character.direction];
    else
        sprite = sprites.character[character.direction];

    if(area == Top) z++;

    projection(&newX, &newY, x*Echelle*map.zoom, y*Echelle*map.zoom, z*Echelle*map.zoom);

    // offset the new x and y
    newX += map.xOffset - CharacterWidth / 2;
    newY += map.yOffset - (CharacterHeight / 2) * map.zoom + TileHeight * (map.zoom - 1);

    if(area == Bottom) // draw the correct area
        masked_stretch_blit(sprite, buffer, 0, CharacterHeight / 2, CharacterWidth, CharacterHeight / 2, newX, newY - CharacterOffset * map.zoom, CharacterWidth * map.zoom, CharacterHeight/2 * map.zoom);
    else
        masked_stretch_blit(sprite, buffer, 0, 0, CharacterWidth, CharacterHeight / 2, newX, newY - CharacterWidth / 4 * map.zoom - CharacterOffset * map.zoom, CharacterWidth * map.zoom, CharacterHeight/2 * map.zoom);
}

int isClickOnCharacter(Characters characters, Map map)
{
    /** Returns the id of the clicked character, or -1 if the click isn't on any character */

    int i;
    int isoX, isoY;

    for(i = 0; i < characters.count; i++) // for every character
    {
        projection(&isoX, &isoY, characters.entity[i].x*Echelle*map.zoom, characters.entity[i].y*Echelle*map.zoom, characters.entity[i].z*Echelle*map.zoom);

        isoX += map.xOffset - CharacterWidth / 2;
        isoY += map.yOffset - CharacterHeight * map.zoom + TileHeight * (map.zoom - 1);

        // if the click is on it
        if(mouse_x > isoX && mouse_x < isoX + CharacterWidth && mouse_y > isoY && mouse_y < isoY + CharacterHeight)
            return i; //  returns the character id
    }

    return -1; // the click isn't on any character, returns -1
}
