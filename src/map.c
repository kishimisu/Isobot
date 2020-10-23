#include "main.h"

Map createMap(int width, int height, int depth)
{
    /** Create, initialize all properties and allocate memory for a new map **/

    Map map;

    map.width = width;
    map.height = height;
    map.depth = depth;

    map.xOffset = SCREEN_W / 2;
    map.yOffset = SCREEN_H / 4;

    map.zoom = DefaultMapZoom;
    map.orientation = North;

    map.cases = mallocMapCases(width, height, depth); // allocate memory for the tiles

    return map; // return the new map
}

Case*** mallocMapCases(int width, int height, int depth)
{
    /** Allocate memory for the tiles represented by a 3D dynamic array,
    * add 2 "ghost" rows on top of the map to avoid out of bound checking
    * and initialize all the cases                                    **/

    Case*** cases;
    int i, j, x, y, z;

    cases = malloc(width * sizeof(Case**));

    if(cases == NULL) error("cases = malloc(width * sizeof(Case**))");

    for(i = 0; i < width; i++)
    {
        cases[i] = malloc(height * sizeof(Case*));

        if(cases[i] == NULL) error("cases[i] = malloc(height * sizeof(Case*))");

        for(j = 0; j < height; j++)
        {
            cases[i][j] = malloc((depth+2) * sizeof(Case)); // "+ 2" inaccessible ghost rows

            if(cases[i][j] == NULL) error("cases[i][j] = malloc((depth+2) * sizeof(Case))");
        }
    }

    // loop through every tile in order to initialize it
    for(z = 0; z < depth + 2; z++)
        for(y = 0; y < height; y++)
            for(x = 0; x < width; x++)
            {
                cases[x][y][z].type = None;

                cases[x][y][z].action.isMoving = False;
                cases[x][y][z].actionNumber = 0;
                cases[x][y][z].actionBloc = False;
            }

    return cases;
}

void loadMap(Map* map)
{
    /** Old maps created the hard way before the existence of the editor **/

    int i, j, k;

    for(k = 0; k < map->depth; k++)
        for(j = 0; j < map->width; j++)
            for(i = 0; i < map->height; i++)
                {
                    /** CUBE
                    if((k == 0 || k == 9) && (i == 0 || i == 9 || j == 0 || j == 9))
                        map->cases[i][j][k] = 0;
                    else if(k != 0 && k != 9 && (i == 0 || i == 9) && (j == 0 || j == 9))
                        map->cases[i][j][k] = 1;
                    else
                        map->cases[i][j][k] = None;*/

                    /** GROUND*/

                    if(k == 0)
                        map->cases[i][j][k].type = 0;
                    else
                        map->cases[i][j][k].type = None;

                    //map->cases[i][j][k] = None;
                }

    /** TRIANGLE ILLUSION */

    map->cases[5][4][1].type = 1;
    map->cases[5][3][1].type = 2;

    map->cases[5][5][1].type = 1;
    map->cases[6][5][1].type = 1;
    map->cases[7][5][1].type = 1;
    map->cases[8][5][1].type = 1;

    map->cases[8][5][2].type = 1;
    map->cases[8][5][3].type = 1;
}

void drawLevelSelectMap(Map map, SpriteCollection sprites, BITMAP* buffer)
{
    int x, y, z;
    int isoX, isoY;

    // loop through every bloc of the map
    for(z = 0; z < map.depth; z++)
        for(y = 0; y < map.height; y++)
            for(x = 0; x < map.width; x++)
                {
                    // if the bloc isn't empty
                    if (map.cases[x][y][z].type != None)
                    {
                        projection(&isoX, &isoY, Echelle*x*map.zoom, Echelle*y*map.zoom, Echelle*z*map.zoom);

                        // draw it
                        stretch_sprite(buffer, sprites.blocs[map.cases[x][y][z].type], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);

                        if(map.cases[x][y][z].actionBloc) // if it's an action bloc, draw the overlay indicating the direction of the movement
                            stretch_sprite(buffer, sprites.blocDirection[convertAxisAndDirectionToInt(map.cases[x][y][z].action.axis, map.cases[x][y][z].action.direction)], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
                    }
                }
}

void drawGameMap(Map map, Characters characters, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the entire game map **/

    int x, y, z, i;
    int isoX, isoY;

    // loop through every bloc of the map
    for(z = 0; z < map.depth+2; z++)
        for(y = 0; y < map.height; y++)
            for(x = 0; x < map.width; x++)
                {
                    // if the bloc isn't empty
                    if (map.cases[x][y][z].type != None)
                    {
                        projection(&isoX, &isoY, Echelle*x*map.zoom, Echelle*y*map.zoom, Echelle*z*map.zoom);

                        // draw it
                        stretch_sprite(buffer, sprites.blocs[map.cases[x][y][z].type], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);

                        if(map.cases[x][y][z].actionBloc) // if it's an action bloc, draw the overlay indicating the direction of the movement
                            stretch_sprite(buffer, sprites.blocDirection[convertAxisAndDirectionToInt(map.cases[x][y][z].action.axis, map.cases[x][y][z].action.direction)], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
                    }

                    // if the current x, y and z matches the top or the bottom of one of the character's position, draw it
                    for(i = 0; i < characters.count; i++)
                    {
                        if(x == characters.entity[i].x && y == characters.entity[i].y && z == characters.entity[i].z)
                            drawCharacter(characters.entity[i], map, Bottom, i == 0, sprites, buffer);
                        else if(x == characters.entity[i].x && y == characters.entity[i].y && z == characters.entity[i].z+1)
                            drawCharacter(characters.entity[i], map, Top, i == 0, sprites, buffer);
                    }
                }

    if(characters.count > 1)
        for(i = 0; i < characters.count; i++)
            if(i == characters.current)
            {
                projection(&isoX, &isoY, Echelle*characters.entity[i].x*map.zoom, Echelle*characters.entity[i].y*map.zoom, Echelle*(characters.entity[i].z+1)*map.zoom);
                stretch_sprite(buffer, sprites.actualCharacter, isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * 1.5 * map.zoom);
            }

    //draw_sprite(buffer, sprites.arrows[map.orientation], 0, 0);
}

void drawGameBackground(Map map, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the game background (inaccessible floor under the level) */

    int x, y, z = -1;
    int isoX, isoY;

    for(y = -2; y < map.height + 2; y++)
        for(x = -2; x < map.width + 2; x++)
    {
        projection(&isoX, &isoY, Echelle*x*map.zoom, Echelle*y*map.zoom, Echelle*z*map.zoom);

        stretch_sprite(buffer, sprites.blocs[13], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
    }
}

void drawEditorMap(Map map, EditorSelection selection, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the entire editor map **/

    Case linkSource; // variable to let the code breathe
    int x, y, z, i;
    int isoX, isoY;

    // loop through every bloc of the map
    for(z = 0; z < map.depth; z++)
        for(y = 0; y < map.height; y++)
            for(x = 0; x < map.width; x++)
                {
                    // if the bloc isn't empty
                    if (map.cases[x][y][z].type != None)
                    {
                        projection(&isoX, &isoY, Echelle*x*map.zoom, Echelle*y*map.zoom, Echelle*z*map.zoom);

                        // draw it
                        stretch_sprite(buffer, sprites.blocs[map.cases[x][y][z].type], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);

                        if(map.cases[x][y][z].actionBloc) // if it's an action bloc, draw the overlay indicating the direction of the movement
                            stretch_sprite(buffer, sprites.blocDirection[convertAxisAndDirectionToInt(map.cases[x][y][z].action.axis, map.cases[x][y][z].action.direction)], isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
                    }

                    // if the x, y and z is the same as the selection's coordinates
                    if(x == selection.x && y == selection.y && z == selection.z)
                        drawSelection(selection, map, sprites, buffer); // draw the selection bloc
                    // if the bloc is on the path of the helper lines and the helper mode is active
                    if(selection.helper == True && ((x == selection.x && y == selection.y) || (x == selection.x && z == selection.z) || (y == selection.y && z == selection.z)))
                        drawHelpers(sprites, selection, map, x, y, z, buffer); // draw the helper bloc
                }

    if(selection.linkage.active) // if the linkage mode is active
    {
        linkSource = map.cases[selection.linkage.x][selection.linkage.y][selection.linkage.z]; // initialize the "breathe variable"

        // for every linked bloc
        for(i = 0; i < linkSource.actionNumber; i++)
        {
            projection(&isoX, &isoY, Echelle*linkSource.linkedCases[i].x*map.zoom, Echelle*linkSource.linkedCases[i].y*map.zoom, Echelle*linkSource.linkedCases[i].z*map.zoom);

            // draw a red overlay on top of it
            stretch_sprite(buffer, sprites.selection, isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);
        }

        projection(&isoX, &isoY, Echelle*selection.linkage.x*map.zoom, Echelle*selection.linkage.y*map.zoom, Echelle*selection.linkage.z*map.zoom);

        // draw a green overlay on top of the main linkage bloc
        stretch_sprite(buffer, sprites.linkage, isoX-TileWidth/2 + map.xOffset, isoY - TileHeight + map.yOffset, TileWidth*map.zoom, TileHeight * map.zoom);

    }
}

void rotateMap(Map* map, Direction direction)
{
    /** Rotate the map by an angle of 45 degrees depending on the direction of the rotation **/

    Map tmpMap = createMap(map->width, map->height, map->depth); // temporary array
    int x, y, z, i;
    int newX, newY;

    // loop through every bloc of the map
    for(z = 0; z < map->depth; z++)
        for(y = 0; y < map->height; y++)
            for(x = 0; x < map->width; x++)
            {
                // if it's an action bloc or is a moving bloc
                if(map->cases[x][y][z].actionBloc || map->cases[x][y][z].action.isMoving)
                    rotateAction(&map->cases[x][y][z].action, direction); // rotate the action properties

                // for every linked case of the bloc
                for(i = 0; i < map->cases[x][y][z].actionNumber; i++)
                    rotateCoordinates(&map->cases[x][y][z].linkedCases[i].x, &map->cases[x][y][z].linkedCases[i].y, *map, direction); // rotate its coordinates

                // update new x and new y depending on the direction of the rotation
                if(direction == Right)
                {
                    newX = map->width - 1 - y;
                    newY = x;
                }
                else
                {
                    newX = y;
                    newY = map->height - 1 - x;
                }

                tmpMap.cases[newX][newY][z] = map->cases[x][y][z]; // store the current bloc in the temporary array at the correct new x and y
            }

    free(map->cases); // free the old array
    map->cases = tmpMap.cases; // replace the array

    // update the map orientation
    if(direction == Right) map->orientation = map->orientation + 1;
    else if(map->orientation == 0) map->orientation = 3;
    else map->orientation = map->orientation - 1;
    if(map->orientation > 3) map->orientation = 0;
}

void rotateAction(MoveAction* action, Direction direction)
{
    /** Rotate the properties of an action depending on the direction of the rotation **/

    if(action->axis != zAxis) // if the axis of the action isn't the z axis (thus the rotation won't change anything)
    {
        // depending on the direction of the rotation, change the axis and direction properties of the action
        if(direction == Left)
        {
            if(action->axis == xAxis)
            {
                action->axis = yAxis;
            }
            else
            {
                action->axis = xAxis;

                if(action->direction == Forward)
                    action->direction = Backward;
                else action->direction = Forward;
            }
        }
        else
        {
            if(action->axis == xAxis)
            {
                action->axis = yAxis;

                if(action->direction == Forward)
                    action->direction = Backward;
                else action->direction = Forward;
            }
            else
            {
                action->axis = xAxis;
            }
        }
    }
}

void rotateCoordinates(int* x, int* y, Map map, Direction direction)
{
    /** Rotate x and y coordinates depending on the direction of the rotation **/

    int newX, newY;

    // Calculate new x and y positions
    if(direction == Right)
    {
        newX = map.width - 1 - *y;
        newY = *x;
    }
    else
    {
        newX = *y;
        newY = map.height - 1 - *x;
    }

    // update new x and y positions
    *x = newX;
    *y = newY;
}

void dragMap(Map* map, MouseInfos* mouseInfos)
{
    /** Manage the drawing offset of the map when the user is dragging it */

    static int startingX, startingY, startingXOffset, startingYOffset;
    static Bool spaceKeyPressed = False;

    if(mouseInfos->clicFocus == None && mouse_b & 1) // if the user starts dragging it
    {
        // initialize the starting coordinates
        startingX = mouse_x;
        startingY = mouse_y;
        startingXOffset = map->xOffset;
        startingYOffset = map->yOffset;

        mouseInfos->clicFocus = Camera;
    }
    else if(mouseInfos->clicFocus == Camera) // else if the user is still dragging it, calculate new x and y offsets
    {
        map->xOffset = startingXOffset - (startingX - mouse_x);
        map->yOffset = startingYOffset - (startingY - mouse_y);
    }

    // switch between 1.0 and 2.0 zoom levels
    if((key[KEY_SPACE] && !spaceKeyPressed))
    {
        map->zoom += 1;

        if(map->zoom > 2)
            map->zoom = 1;

        spaceKeyPressed = True;
    }
    else if(!key[KEY_SPACE])
        spaceKeyPressed = False;
}

void loadMapFromFile(Map* map, int level)
{
    /** Load a map from a .dat file **/

    FILE* file;
    char fileName[64];
    int i, j, k, d;
    static int lastLevel;

    if(level != -1)
        lastLevel = level;
    else level = lastLevel;

    sprintf(fileName, "ressources/lvl/level_%d.dat", level);
    file = fopen(fileName,"rb"); // open the file in read mode

    if(file != NULL) // if the file has been loaded
    {
        fread(map, sizeof(Map), 1, file); // read the map informations

        map->cases = mallocMapCases(map->width, map->height, map->depth); // allocate memory for the cases

        // loop through every case
        for(k = 0; k < map->depth; k++)
            for(j = 0; j < map->height; j++)
                for(i = 0; i < map->width; i++)
                {
                    fread(&map->cases[i][j][k], sizeof(Case), 1, file); // read the current case

                    // if it has at least one linked case
                    if(map->cases[i][j][k].actionNumber > 0)
                    {
                        map->cases[i][j][k].linkedCases = malloc(map->cases[i][j][k].actionNumber * sizeof(Point)); // allocate memory for the linked cases

                        if(map->cases[i][j][k].linkedCases == NULL) error("map->cases[i][j][k].linkedCases = malloc(map->cases[i][j][k].actionNumber * sizeof(Point))");

                        // for every linked case
                        for(d = 0; d < map->cases[i][j][k].actionNumber; d++)
                            fread(&map->cases[i][j][k].linkedCases[d], sizeof(Point), 1, file); // read it
                    }
                }

        fclose(file); // close the file
    }
    else
        error("fopen(\"level_x.dat\",\"rb\")");

    map->xOffset = SCREEN_W / 2;
    map->yOffset = SCREEN_H - (map->width + 5 - 4) * TileHeight / 2;

    map->zoom = DefaultMapZoom;
    map->orientation = North;

   // (map->width + 5) * TileHeight / 2;
}

void saveMap(Map map)
{
    /** Save a map to a .dat file **/

    FILE* file;
    int i, j, k, d;

    file = fopen("ressources/lvl/level_9.dat","wb"); // open the file in write mode

    if(file != NULL) // if the file has been loaded
    {
        fwrite(&map, sizeof(Map), 1, file); // write the map properties

        // loop through every case
        for(k = 0; k < map.depth; k++)
            for(j = 0; j < map.height; j++)
                for(i = 0; i < map.width; i++)
                {
                    fwrite(&map.cases[i][j][k], sizeof(Case), 1, file); // write the case

                    if(map.cases[i][j][k].actionNumber > 0) // if the current bloc has linked cases
                    {
                        // for every linked case
                        for(d = 0; d < map.cases[i][j][k].actionNumber; d++)
                            fwrite(&map.cases[i][j][k].linkedCases[d], sizeof(Point), 1, file); // write it
                    }
                }

        fclose(file); // close the file
    }
    else
        error("fopen(\"map.dat\",\"wb\")");
}

void resetMap(Map* map)
{
    int lastOffsetX = map->xOffset;
    int lastOffsetY = map->yOffset;

    loadMapFromFile(map, -1);

    map->xOffset = lastOffsetX;
    map->yOffset = lastOffsetY;
}
