#include "headers/main.h"

void editor(SpriteCollection sprites, BITMAP* buffer)
{
    /** Editor main loop and engine */

    BITMAP* bufferSnapshot;
    EditorSelection selection;
    MouseInfos mouseInfos;
    Map map;

    map = createMap(10, 10, 10); // initialize and allocate memory for a new map
    bufferSnapshot = create_bitmap(SCREEN_W, SCREEN_H); // keep a snapshot of the previous blit for the linkage menu

    if(bufferSnapshot == NULL) error("create_bitmap");

    // initialize the structures
    loadMapFromFile(&map, LevelCount-1);
    initMouseInfos(&mouseInfos);
    initEditorSelection(&selection);

    while(!key[KEY_ESC]) // main loop
    {
        if(selection.linkage.active)
            clear_to_color(buffer, makecol(100,230,100));
        else
            clear_to_color(buffer, makecol(40,100,230));
            //clear_bitmap(buffer); // clear the buffer

        while(game_time > 0) // "physics" engine
        {
            dragMap(&map, &mouseInfos); // manage the drag of the map

            if(!(mouse_b & 1)) // reset the mouse click focus if the click is released
                mouseInfos.clicFocus = None;

            // manage the user key presses
            manageEditorKeyboard(&map, &selection, sprites, bufferSnapshot, buffer);

            game_time--;
        }

        // "graphic" engine

        // draw the walls and the map
        drawWalls(sprites, map, buffer);
        drawEditorMap(map, selection, sprites, buffer);

        // debug x, y and z current position
        textprintf_ex(buffer, font, 0, 0, makecol(255,255,255), makecol(255,0,255), "x: %d, y: %d, z: %d", selection.x, selection.y, selection.z);
        if(selection.linkage.active)
            textout_centre(buffer, sprites.customFont, "Linkage Mode", SCREEN_W / 2, 0, makecol(255,50,50));
        else
            textout_centre(buffer, sprites.customFont, "Edition Mode", SCREEN_W / 2, 0, makecol(0,0,0));

        if(key[KEY_H]) helpMenu(sprites, buffer);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // blit the buffer to the screen

        blit(buffer, bufferSnapshot, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // blit the buffer to the bufferSnapshot to keep the last frame of the game
    }

    saveMap(map);
}

void initEditorSelection(EditorSelection* selection)
{
    /** Initializes the editor selection properties **/

    selection->x = 0;
    selection->y = 0;
    selection->z = 0;
    selection->bloc = 0;
    selection->helper = True;
    selection->linkage.active = False;
}
Point makePoint(int x, int y, int z);

void manageEditorKeyboard(Map* map, EditorSelection* selection, SpriteCollection sprites, BITMAP* snapshot, BITMAP* buffer)
{
    /** Manage all the possible key presses of the editor mode **/

    static Bool keyPressed = False; // avoid the keyboard repetition
    Case* linkageBloc, *currentBloc; // variables to let the code breathe
    Point currentPoint;

    if(!keyPressed) // if no key was previously pressed
    {
        if(selection->linkage.active) // set the "breathe" variables
            linkageBloc = &map->cases[selection->linkage.x][selection->linkage.y][selection->linkage.z];
        currentBloc = &map->cases[selection->x][selection->y][selection->z];
        currentPoint = makePoint(selection->x,selection->y,selection->z);

        // Move the current selected bloc
        if(key[KEY_A] && selection->x > 0) selection->x--;
        else if(key[KEY_D] && selection->x < map->width-1) selection->x++;
        else if(key[KEY_W] && selection->y > 0) selection->y--;
        else if(key[KEY_S] && selection->y < map->height-1) selection->y++;
        else if(key[KEY_UP] && selection->z < map->depth-1) selection->z++;
        else if(key[KEY_DOWN] && selection->z > 0) selection->z--;

        // Show or not the helpers
        else if(key[KEY_R]) selection->helper = !selection->helper;

        // Add a new bloc or link a bloc
        else if(key[KEY_ENTER])
        {
            if(selection->linkage.active) // if the linkage mode is on
            {
                if(!currentBloc->actionBloc) // we can't move an action bloc !
                {
                    if(isAlreadyLinked(*linkageBloc, currentPoint)) // if the current bloc is already linked
                        removeLinkedCase(linkageBloc, currentPoint); // remove it from the linked cases
                    else
                        addLinkedCase(linkageBloc, currentPoint); // else add it to the lined cases
                }
            }
            else // if it's the normal edition mode
                currentBloc->type = selection->bloc; // replace the selected bloc with the current selection bloc
        }
        // Delete the current bloc
        else if(key[KEY_BACKSPACE]) map->cases[selection->x][selection->y][selection->z].type = None;

        // Change the selected bloc type
        else if(key[KEY_Q])
        {
            selection->bloc--; // decrease the current bloc

            if(selection->bloc == None) // check if the new value is not out of bound
                selection->bloc = BlocCount-1;

        }
        else if(key[KEY_E])
        {
            selection->bloc++; // increase the current bloc

            if(selection->bloc > BlocCount-1) // check if the new value is not out of bound
                selection->bloc = 0;
        }

        // rotate the map
        else if(key[KEY_LEFT])
        {
            rotateMap(map, Left); // rotate the map, the selection coordinates and the linked bloc coordinates
            rotateCoordinates(&selection->x, &selection->y, *map, Left);

            if(selection->linkage.active) // if the linkage mode is on
                rotateCoordinates(&selection->linkage.x, &selection->linkage.y, *map, Left);
        }
        else if(key[KEY_RIGHT])
        {
            rotateMap(map, Right);
            rotateCoordinates(&selection->x, &selection->y, *map, Right);

            if(selection->linkage.active)
                rotateCoordinates(&selection->linkage.x, &selection->linkage.y, *map, Right);
        }

        // enter / exit the linkage mode
        else if(key[KEY_O])
        {
            if(selection->linkage.active) // if the linkage mode was already on
            {
                selection->linkage.active = False; // set it to false (exit linkage mode)

                if(linkageBloc->actionNumber == 0) // if the linkage bloc has no action
                    linkageBloc->actionBloc = False; // it's not an action bloc
            }
            else if(currentBloc->type != None) // else if the selection isn't on an empty bloc
            {
                if(!currentBloc->actionBloc) // if that bloc isn't an action bloc
                {
                    currentBloc->actionBloc = True; // it becomes an action bloc
                    currentBloc->actionNumber = 0; // initialize all the default properties
                    currentBloc->action.axis = yAxis;
                    currentBloc->action.direction = Forward;
                    currentBloc->action.length = 1;
                }

                selection->linkage.x = selection->x; // store the coordinates of the linkage bloc
                selection->linkage.y = selection->y;
                selection->linkage.z = selection->z;

                selection->linkage.active = True; // linkage mode set to true (enter linkage mode)
            }
        }
        // show the linkage menu if the user is in linkage mode
        else if(key[KEY_P] && selection->linkage.active)
        {
            linkageMenu(linkageBloc, sprites.customFont, snapshot, buffer);
        }
    }

    // if one of the key was pressed, set keyPressed to true. The user will need to release all keys in order to press a new one
    if(key[KEY_A] || key[KEY_D] || key[KEY_W] || key[KEY_S] || key[KEY_UP] || key[KEY_DOWN] || key[KEY_R] || key[KEY_ENTER] || key[KEY_BACKSPACE] || key[KEY_Q] || key[KEY_E] || key[KEY_LEFT] || key[KEY_RIGHT] || key[KEY_O] || key[KEY_P])
        keyPressed = True;
    else // else all keys are released, a new action can be executed
        keyPressed = False;
}

Bool isAlreadyLinked(Case linkSource, Point newLink)
{
    /** Return true if the newLink point is already linked to the linkSource **/

    int i;

    if(linkSource.actionNumber > 0) // if the link source has at least one linked case
    {
        for(i = 0; i < linkSource.actionNumber; i++) // for every linked case
        {
            if(newLink.x == linkSource.linkedCases[i].x && // if it matches the new link
               newLink.y == linkSource.linkedCases[i].y &&
               newLink.z == linkSource.linkedCases[i].z)
            {
                return True; // return true
            }
        }
    }

    return False; // else return false
}

void drawHelpers(SpriteCollection sprites, EditorSelection selection, Map map, int x, int y, int z, BITMAP* buffer)
{
    /** Draws a gray transparent 3-axis helper bloc in order to help the user visualize his position on the map **/

    int isoX, isoY;
    int helper;

    // at certain specific x, y and z, the sprite of the helper bloc has to change or it creates visual glitches
    if( (y == map.height-1 && x == selection.x && z == selection.z) ||
        (x == map.width-1 && y == selection.y && z == selection.z) ||
        (z == map.depth-1 && x == selection.x && y == selection.y))
        helper = 0;
    else if(x == selection.x && z == selection.z)
        helper = 1;
    else if(y == selection.y && z == selection.z)
        helper = 2;
    else if(x == selection.x && y == selection.y)
        helper = 3;
    if(x == selection.x && y == selection.y && z == selection.z)
        helper = -1;

    projection(&isoX, &isoY, x * Echelle * map.zoom, y * Echelle * map.zoom, z * Echelle * map.zoom);

    // draw the helper bloc depending on its position on the screen
    if(helper != -1)
        draw_trans_stretched_sprite(buffer, sprites.helper[helper], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth * map.zoom, TileHeight * map.zoom);
}

void drawWalls(SpriteCollection sprites, Map map, BITMAP* buffer)
{
    /** Draw the background walls that help visualize the size of the editor area **/

    int x = -1, y, z;
    int isoX, isoY;

    // Left wall
    for(y = -1; y < map.height; y++) // go through y and z while x is at -1
        for(z = 0; z < map.depth; z++)
        {
            projection(&isoX, &isoY, x * Echelle * map.zoom, y * Echelle * map.zoom, z * Echelle * map.zoom);

            stretch_sprite(buffer, sprites.blocs[1], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
        }

    y = -1;

    // Back wall
    for(x = 0; x < map.width; x++) // go through x and z while y is at -1
        for(z = 0; z < map.depth; z++)
        {
            projection(&isoX, &isoY, x * Echelle * map.zoom, y * Echelle * map.zoom, z * Echelle * map.zoom);

            stretch_sprite(buffer, sprites.blocs[1], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
        }
}

void drawSelection(EditorSelection selection, Map map, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the selection bloc and the red box on top of it **/

    int isoX, isoY;

    projection(&isoX, &isoY, selection.x * Echelle * map.zoom, selection.y * Echelle * map.zoom, selection.z * Echelle * map.zoom);

    // selection bloc
    stretch_sprite(buffer, sprites.blocs[selection.bloc], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
    // red helper box
    stretch_sprite(buffer, sprites.selection, isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
}

void addLinkedCase(Case* linkageCase, Point newLink)
{
    /** Link a case to an action bloc **/

    // if it's the first linked case
    if(linkageCase->actionNumber == 0)
        linkageCase->linkedCases = malloc(sizeof(Point)); // malloc one new case
    else // else realloc for one more case
        linkageCase->linkedCases = realloc(linkageCase->linkedCases, sizeof(Point) * (linkageCase->actionNumber + 1));

    if(linkageCase->linkedCases == NULL) error("malloc or realloc linkageCase->linkedCases");

    linkageCase->linkedCases[linkageCase->actionNumber] = newLink; // add the new case
    linkageCase->actionNumber += 1; // increase the number of linked case
}

void removeLinkedCase(Case* linkageCase, Point newLink)
{
    /** remove a linked case from an action bloc **/

    int removed = 0;
    int i;
    Point* newArray = malloc((linkageCase->actionNumber-1) * sizeof(Point));

    if(newArray == NULL) error("newArray = malloc((linkageCase->actionNumber-1) * sizeof(Point))");

    // for every linked bloc
    for(i = 0; i < linkageCase->actionNumber; i++)
    {
        // if it's not the bloc to remove
        if(newLink.x != linkageCase->linkedCases[i].x ||
           newLink.y != linkageCase->linkedCases[i].y ||
           newLink.z != linkageCase->linkedCases[i].z)
        {
            newArray[i-removed] = linkageCase->linkedCases[i]; // add it to the new array
        }
        else
            removed = 1; // indicate that we passed the removed bloc
    }

    free(linkageCase->linkedCases); // free the current array
    linkageCase->linkedCases = newArray; // replace the array

    linkageCase->actionNumber -= 1; // decrease the action number
}

void linkageMenu(Case* linkage, FONT* customFont, BITMAP* snapshot, BITMAP* buffer)
{
    /** Linkage menu to modify the direction and the length of an actionBloc's action **/

    int width = text_length(customFont, ">              <"); // width and height properties of the texts
    int height = text_height(customFont);
    int row = 0; // current row

    do
    {
        clear_bitmap(buffer); // clear the screen

        blit(snapshot, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // print the last snapshot of the screen

        // background rectangle
        rectfill(buffer, SCREEN_W/2 - width / 2, SCREEN_H/2 - height * 2, SCREEN_W/2 + width / 2, SCREEN_H/2 + height * 2,makecol(255,255,255));

        // selection arrows
        textout_centre_ex(buffer, customFont, ">              <",SCREEN_W / 2,SCREEN_H / 2 + height * row - 2*height,makecol(0,0,0),makecol(255,255,255));

        // Direction, Distance and Exit rows
        textprintf_centre_ex(buffer, customFont, SCREEN_W / 2, SCREEN_H / 2 - 2*height, makecol(0,0,0), makecol(255,255,255), "Direction : %s", convertAxisAndDirectionToString(linkage->action.axis, linkage->action.direction));
        textprintf_centre_ex(buffer, customFont, SCREEN_W / 2, SCREEN_H / 2+height - 2*height, makecol(0,0,0) + height, makecol(255,255,255), "Distance : %d", linkage->action.length);
        textout_centre_ex(buffer, customFont, "Quitter",SCREEN_W / 2,SCREEN_H / 2 + 3* height - 2*height,makecol(0,0,0),makecol(255,255,255));

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H); // print the buffer onto the screen

    }while(!manageLinkageMenuKeyboard(linkage, &row)); // while the user didn't press Enter
}

Bool manageLinkageMenuKeyboard(Case* linkage, int *row)
{
    /** Manage all the possible key presses of the linkage menu **/

    static Bool keyPressed; // avoid the keyboard repetition
    int lastRow;

    if(!keyPressed) // if no key was previously pressed
    {
        lastRow = *row; // keep track of the last row

        // if the user presses enter, exits the menu
        if(key[KEY_ENTER]) return True;

        // move the row up and down
        else if (key[KEY_UP]) *row -= 1;
        else if(key[KEY_DOWN]) *row += 1;

        // if the user wants to edit a property
        else if(key[KEY_LEFT]) // left key
        {
            if(*row == 0) // if the current row is the direction row
            {
                // change the axis and direction to keep the order "Forward, Backward, Up, Down, Right, Left"
                if(linkage->action.direction == Backward)
                    linkage->action.direction = Forward;
                else
                {
                    linkage->action.direction = Backward;

                    if(linkage->action.axis == 0)
                        linkage->action.axis = 2;
                    else
                        linkage->action.axis -= 1;
                }
            }
            else if(*row == 1) // else if it's the length row
                linkage->action.length -= 1; // decrease the length
        }
        else if(key[KEY_RIGHT]) // right key
        {
            if(*row == 0) // if the current row is the direction row
            {
                // change the axis and direction to keep the order "Forward, Backward, Up, Down, Right, Left"
                if(linkage->action.direction == Forward)
                    linkage->action.direction = Backward;
                else
                {
                    linkage->action.direction = Forward;
                    linkage->action.axis += 1;
                }
            }
            else if(*row == 1) // if the current row is the length row
                linkage->action.length += 1; // increase the length
        }

        if(*row == 2) // 2 is in between "length" and "exit" so it's not a valid position
        {
            if(lastRow == 1) *row = 3; // update the row position according to the previous one
            else *row = 1;
        }

        // if the row is out of bounds, replace it
        if(*row < 0) *row = 3;
        else if(*row > 3) *row = 0;

        // if the current direction is out out bounds, replace it
        if(linkage->action.axis < 0) linkage->action.axis = 2;
        else if(linkage->action.axis > 2) linkage->action.axis = 0;

        // if the current length is out of bounds, replace it
        if(linkage->action.length < 1) linkage->action.length = 9;
        else if(linkage->action.length > 9) linkage->action.length = 1;
    }

    // if one of the key was pressed, set keyPressed to true. The user will need to release all keys in order to press a new one
    if(!key[KEY_UP] && !key[KEY_DOWN] && !key[KEY_LEFT] && !key[KEY_RIGHT] && !key[KEY_ENTER])
        keyPressed = False;
    else // else all keys are released, a new action can be executed
        keyPressed = True;

    return False;
}
