#include "headers/main.h"

void game(SpriteCollection sprites, WindowsInfos windowsInfos, int level, BITMAP* buffer)
{
    /** Game main loop and engine **/

    DebugInfos debugInfos;
    MouseInfos mouseInfos;
    DragInfos dragInfos;
    Map map;
    Characters characters;
    Sequence* sequences;
    Bool gameWon = False;

    // initialize every structure
    initMouseInfos(&mouseInfos);
    loadMapFromFile(&map, level);

    debugInfos.status = Stop;
    initCharacters(&characters, &debugInfos, map);
    sequences = malloc(characters.count * sizeof(Sequence));
    initSequences(sequences, characters);

    resetCharacters(&characters);

    while(!key[KEY_ESC] && !gameWon) // main loop
    {
        //clear_bitmap(buffer); // clear the buffer
        clear_to_color(buffer, makecol(0,150,230));

        while(game_time > 0) // "physics" game engine
        {
            if((mouse_b & 1 || mouse_b & 2) && mouseInfos.currentClic == None) // if a new click occur and the mouse wasn't already clicked
            {
                // updates the mouse infos depending on the click
                if(mouse_b & 1) mouseInfos.currentClic = LeftClic;
                else if(mouse_b & 2) mouseInfos.currentClic = RightClic;

                // handle this new click
                detectMouseFocus(&sequences[characters.current], &characters, map, &debugInfos, &mouseInfos, &windowsInfos);
            }
            else
            {   // else if there is no click but there was one before
                if((mouseInfos.currentClic == LeftClic && !(mouse_b & 1)) || (mouseInfos.currentClic == RightClic && !(mouse_b & 2)))
                {
                    // handle the release of the mouse
                    releaseMouse(sequences, &characters, &map, &debugInfos, &mouseInfos, &dragInfos, windowsInfos);

                    // updates the mouse infos
                    mouseInfos.currentClic = None;
                    mouseInfos.clicFocus = None;
                }
                else if(mouse_b & 1 && mouseInfos.clicFocus == ActionMenu && mouseInfos.element != None && mouseInfos.clicFocus != Dragging && debugInfos.status == Stop)
                    checkIfDraggingMode(&mouseInfos);

                gameWon = isGameWon(characters);
            }

            // manage dragging the different windows
            if(windowsInfos.actionMenu.drag)
                dragActionMenu(&windowsInfos);
            else if(windowsInfos.sequenceMenu.drag)
                dragSequenceMenu(&windowsInfos);
            dragMap(&map, &mouseInfos);

            manageMovingBlocs(&map);

            // manage the sequence execution
            if(debugInfos.status == Run || debugInfos.status == Step)
                executeAllSequences(sequences, &debugInfos, &map, &characters);

            // manage the rest of the game
            manageGameKeyboard(&map, &characters, debugInfos, sprites, buffer);

            game_time--;
        }

        // "graphic" game engine

        drawGameBackground(map, sprites, buffer);
        drawGameMap(map, characters, sprites, buffer); // draw the game map

        // draw the sequence and action menu depending on the draw priority
        if(windowsInfos.actionMenu.drawPriority == 1)
        {
            drawSequenceMenu(sequences[characters.current], characters, debugInfos, mouseInfos, windowsInfos, sprites, buffer);
            if(debugInfos.status == Stop)
                drawActionMenu(sprites, characters, mouseInfos, windowsInfos, buffer);
        }
        else
        {
            if(debugInfos.status == Stop)
                drawActionMenu(sprites, characters, mouseInfos, windowsInfos, buffer);
            drawSequenceMenu(sequences[characters.current], characters, debugInfos, mouseInfos, windowsInfos, sprites, buffer);
        }

        if(mouseInfos.clicFocus == Dragging)
        {
            drawDragOnSequence(&dragInfos, &sequences[characters.current], mouseInfos, windowsInfos, sprites, buffer);
            drawDraggingMode(mouseInfos, windowsInfos, sprites, buffer);
        }
        if(key[KEY_H]) helpMenu(sprites, buffer);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // blit the buffer to the screen
    }

    if(gameWon)
        victoryMenu(sprites, windowsInfos, buffer);
}

void manageGameKeyboard(Map* map, Characters* characters, DebugInfos debugInfos, SpriteCollection sprites, BITMAP* buffer)
{
    /** Manage all the possible key presses of the game mode **/

    static Bool keyPressed = False; // avoid the keyboard repetition

    if(!keyPressed) // if no key was previously pressed
    {
        // move the character (debug feature for now)
        if(key[KEY_A]) moveCharacter(&characters->entity[0], *map, Left);
        else if(key[KEY_D]) moveCharacter(&characters->entity[0], *map, Right);
        else if(key[KEY_W]) moveCharacter(&characters->entity[0], *map, Up);
        else if(key[KEY_S]) moveCharacter(&characters->entity[0], *map, Down);

        // rotate the map and the character coordinates
        else if(key[KEY_LEFT] && debugInfos.status == Stop)
        {
            rotateMap(map, Left);
            rotateCharacters(characters, map, Left);
        }
        else if(key[KEY_RIGHT] && debugInfos.status == Stop)
        {
            rotateMap(map, Right);
            rotateCharacters(characters, map, Right);
        }
    }

    // if one of the key was pressed, set keyPressed to true. The user will need to release all keys in order to press a new one
    if(key[KEY_A] || key[KEY_D] || key[KEY_W] || key[KEY_S] || key[KEY_LEFT] || key[KEY_RIGHT])
        keyPressed = True;
    else // else all keys are released, a new action can be executed
        keyPressed = False;
}

int checkPerspective(Map map, Direction direction, int* nextX, int* nextY, int* nextZ)
{
    /** Returns true and the coordinates of the bloc if a perspective illusion is detected in the direction of the move **/

    int xOffset=0, yOffset=0;
    Point startingBlock, foregroundBlock, illusionBlock;
    int foreGroundI;

    startingBlock.x = *nextX; // initializes the starting bloc coordinates
    startingBlock.y = *nextY;
    startingBlock.z = *nextZ;

    // depending on the direction of the move, the algorithm starts at a specific offset
    if(direction == Right)
        xOffset = 1;
    else if (direction == Down)
        yOffset = 1;
    else if (direction == Left)
        xOffset = -1;
    else
        yOffset = -1;

    // get the first bloc that COULD BE an illusion and from where the algorithm will start
    foregroundBlock = getForegroundBlock(map, startingBlock, xOffset, yOffset, &foreGroundI);

    if(foregroundBlock.valid) // if there is a foreground bloc
    {
        /* printf("foregroundBlock: %d %d %d\n", foregroundBlock.x, foregroundBlock.y, foregroundBlock.z); */

        // get the first bloc that IS an illusion
        illusionBlock = getFirstIllusionBlock(map, startingBlock, foreGroundI, xOffset, yOffset, direction);

        if(illusionBlock.valid) // if there is a valid illusion bloc
        {
            // updates the next x, y and z position
            *nextX = illusionBlock.x;
            *nextY = illusionBlock.y;
            *nextZ = illusionBlock.z + 1;

            return 1; // a valid illusion bloc has been found !
        }
    }
    /* else printf("foregroundBlock: Invalid\n"); */

    return 0; // no valid illusion bloc found, handle the character movement normally on a 2d map
}

Point getForegroundBlock(Map map, Point startingBlock, int xOffset, int yOffset, int* foregroundI)
{
    /** Returns the coordinates, if valid, of the foreground bloc depending on the starting bloc **/

    int i;

    Point current; // "route" bloc that will take the position of every bloc up to the foreground bloc

    current.x = startingBlock.x; // initializes the route bloc
    current.y = startingBlock.y;
    current.z = startingBlock.z;
    current.valid = True; // assume it's a valid one

    // the route bloc will take the position of every bloc until it reaches the out of the box
    for(i = 0; current.x >= 0 && current.x < map.width && current.y >= 0 && current.y < map.height && current.z >= 0 && current.z < map.depth; i++)
    {
        current.x = startingBlock.x + i + xOffset;
        current.y = startingBlock.y + i + yOffset;
        current.z = startingBlock.z + i;
    }

    i-=2;

    // updates the coordinates of the foreground bloc
    current.x = startingBlock.x + i + xOffset;
    current.y = startingBlock.y + i + yOffset;
    current.z = startingBlock.z + i;

    *foregroundI = i; // keep track of the number of step needed to get to the foreground bloc

    if(i < 0) current.valid = False; // the bloc isn't valid if the starting bloc was too close to the border of the map

    return current; // return the foreground bloc
}

Point getFirstIllusionBlock(Map map, Point startingBloc, int foregroundI, int xOffset, int yOffset, Direction direction)
{
    /** Returns the coordinates, if valid, of the foreground bloc depending on the starting bloc **/

    int i;

    Point current, illusionBlock;
    Bool checkIfIllusionIsMasked = False;
    Bool illusionToStartMode = False;

    // for 2 directions, the algorithm will start at the starting bloc, so when the foregroundI is equal to 0, otherwise it will start at the foreground bloc
    if(direction == Left || direction == Up) foregroundI = 0;

    // initializes the "route" bloc that will take the position of every bloc up to the foreground bloc
    current.x = startingBloc.x + foregroundI + xOffset;
    current.y = startingBloc.y + foregroundI + yOffset;
    current.z = startingBloc.z + foregroundI;

    illusionBlock.valid = False; // assume that the bloc isn't a valid one

    // manage the route bloc position until an illusion bloc has been found or the route bloc is out of bounds
    for(i = foregroundI; current.x >= 0 && current.x < map.width && current.y >= 0 && current.y < map.height && current.z >= 0 && current.z < map.depth; illusionToStartMode ? i++ : i--)
    {
        /* printf("try: %d %d %d\n", current.x, current.y, current.z); */

        // we will need to check some blocs around the route bloc, verify those blocs aren't out of bounds
        if(current.x - 1 >= 0 && current.x + 1 < map.width && current.y - 1 >= 0 && current.y + 1 < map.height && current.z >= 0 && current.z + 1 < map.depth)
            // if we're in checkIfIllusionIsMasked mode and if, depending on the direction, one of those conditions is true, then a bloc is blocking the illusion
            if((( checkIfIllusionIsMasked == True && map.cases[current.x][current.y][current.z+1].type != None ) || ((
                  direction == Down  && map.cases[current.x+1][current.y][current.z+1].type != None )) || (
                  direction == Up && (map.cases[current.x+1][current.y][current.z+1].type != None || map.cases[current.x][current.y+1][current.z+1].type != None || map.cases[current.x+1][current.y+1][current.z+1].type != None) ) || (
                  direction == Left && (map.cases[current.x+1][current.y][current.z+1].type != None /*|| map.cases[current.x][current.y+1][current.z+1] != None || map.cases[current.x+1][current.y+1][current.z+1] != None*/) ) || (
                  direction == Right && (map.cases[current.x-1][current.y][current.z+1].type != None || (map.cases[current.x-1][current.y][current.z].type != None && current.z > startingBloc.z)))))
            {
                checkIfIllusionIsMasked = False; // exit the checkIfIllusionIsMasked mode
                illusionBlock.valid = False; // illusion bloc isn't valid anymore

                /* printf("blocking Block: %d %d %d [%d] or %d %d %d [%d]\n", current.x, current.y, current.z+1, map.cases[current.x][current.y][current.z+1], current.x+1, current.y, current.z+1, map.cases[current.x+1][current.y][current.z+1]); */
            }

        // if the route bloc isn't on an empty bloc and checkIfIllusionIsMasked is false, so the algorithm is looking for an illusion bloc
        if(map.cases[current.x][current.y][current.z].type != None && checkIfIllusionIsMasked == False)
        {
            illusionBlock.valid = True; // for now it's a valid bloc
            checkIfIllusionIsMasked = True; // stop searching for illusion blocs and start searching for blocking blocs
            illusionBlock.x = current.x; // update illusion bloc coordinates
            illusionBlock.y = current.y;
            illusionBlock.z = current.z;

            if(direction == Left || direction == Up) // for those two directions, the route bloc isn't going toward the camera but toward the bottom of the map
            {
                 illusionToStartMode = True; // the route bloc will now go toward the camera, from its current position to the starting bloc
                 i += 1;

                 /* printf("illusionToStartMode = True\n"); */
            }

            /* printf("illusionBlock: %d %d %d\n", current.x, current.y, current.z); */
        }

        // if the route bloc is out of bounds, break
        if((current.z <= startingBloc.z && (direction == Right || direction == Down)) || (current.z >= startingBloc.z-1 && illusionToStartMode))
            break;

        // update the route bloc coordinates
        current.x = startingBloc.x + i + xOffset;
        current.y = startingBloc.y + i + yOffset;
        current.z = startingBloc.z + i;
    }

    // if the illusion bloc is valid, check if there are two empty block on top of it so the character can fit [MAY BE USELESS, ALREADY CHECKED IN MOVECHARACTER]
   /* if(illusionBlock.valid)
            if(map.cases[illusionBlock.x][illusionBlock.y][illusionBlock.z + 1].type != None || map.cases[illusionBlock.x][illusionBlock.y][illusionBlock.z + 2].type != None)
                illusionBlock.valid = False;*/

    return illusionBlock; // return the illusion bloc
}

void manageMovingBlocs(Map* map)
{
    /** Manage every moving bloc depending on its action properties **/

    Case* current, *next; // variables to let the code breathe
    time_t currentTime;

    int x, y, z;

    // loop through every bloc on the map
    for(z = 0; z < map->depth; z++)
        for(y = 0; y < map->height; y++)
            for(x = 0; x < map->width; x++)
            {
                current = &map->cases[x][y][z]; // set "breathe" variables
                next = NULL;

                if(current->action.isMoving) // if the bloc is moving
                {
                    if(current->action.current == current->action.length) // if the bloc is arrived at its destination
                        current->action.isMoving = False; // stop the bloc movement
                    else
                    {
                        // get the address of the next case depending on the direction and the axis of the movement
                        if(current->action.axis == xAxis && current->action.direction == Forward && x < map->width-1) next = &map->cases[x+1][y][z];
                        else if(current->action.axis == xAxis && current->action.direction == Backward && x > 0) next = &map->cases[x-1][y][z];
                        else if(current->action.axis == yAxis && current->action.direction == Forward && y > 0) next = &map->cases[x][y-1][z];
                        else if(current->action.axis == yAxis && current->action.direction == Backward && y < map->height-1) next = &map->cases[x][y+1][z];
                        else if(current->action.axis == zAxis && current->action.direction == Forward && z < map->depth-1) next = &map->cases[x][y][z+1];
                        else if(current->action.axis == zAxis && current->action.direction == Backward && z > 0) next = &map->cases[x][y][z-1];

                        // get the time elapsed since the start of the action
                        currentTime = clock() - current->action.startingTime;

                        if(next != NULL) // if the next bloc is valid (not out of bounds)
                        {
                            if(currentTime > 1000 && next->type == None) // if more than 1 second has elapsed since the beginning of the action and the next bloc is empty
                            {
                                current->action.startingTime = clock(); // reset the starting time
                                current->action.current += 1; // increase the current action tracker

                                *next = *current; // move the bloc to the next position

                                // reset the current bloc properties
                                current->type = None;
                                current->action.isMoving = False;
                            }
                        }
                        else // if the next position is out of bounds
                            current->action.isMoving = False; // stop the movement
                    }
                }
            }
}

Bool isGameWon(Characters characters)
{
    int i;

    for(i = 0; i < characters.count; i++)
        if(!characters.entity[i].didWin)
            return False;

    return True;
}
