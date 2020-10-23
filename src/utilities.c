#include "main.h"

BITMAP* load_sprite(const char* filename, struct RGB* pal)
{
    /** load_bitmap with error checking **/

    BITMAP* bitmap = NULL;
    char errorMessage[64] = "load_bitmap ";

    bitmap = load_bitmap(filename, pal); // load the bitmap

    if(!bitmap) // if it failed
    {
        strcat(errorMessage, filename); // create the error message
        error(errorMessage); // prints it and exit
    }

    return bitmap; // else return the loaded bitmap
}

void error(char* message)
{
    /** Prints an error message and exit the program **/

    printf("error: %s\n", message);
    exit(EXIT_FAILURE);
}

void projection(int *pxe, int *pye, int x, int y, int z)
{
    /** Projection from cartesian to iso **/

    *pxe = 2*x - 2*y;
    *pye = x + y - 2*z;
}

void printCount(BITMAP* buffer, FONT* font, int count, int x, int y)
{
    /** Prints the stack number of an action next to it in the sequence menu **/

    BITMAP* text;
    char* multiplier; // string containing the count
    int newWidth, newHeight;

    multiplier = malloc(sizeof(char) * 5); // allocate the string
    sprintf(multiplier, "x%d", count); // put the count in the string

    text = create_bitmap(text_length(font,multiplier), text_height(font)); // create a new bitmap to the dimensions of the text

    clear_to_color(text, makecol(255,0,255)); // clear it
    textout(text, font, multiplier, 0, 0,makecol(255,0,0)); // prints the multiplier on it

    newWidth = SCREEN_W * text_length(font, multiplier) / 1600; // calculate the new dimensions depending on the screen width
    newHeight = SCREEN_W * text_height(font) / 1600;

    stretch_sprite(buffer, text, x, y - newHeight / 2, newWidth, newHeight); // stretch the final sprite and draws it

    destroy_bitmap(text); // free the bitmap and the string
    free(multiplier);
}

void draw_trans_stretched_sprite(BITMAP* buffer, BITMAP* sprite, int x, int y, int width, int height)
{
    /** Draws a stretched transparent sprite **/

    BITMAP* stretchedSprite; // create a new bitmap

    stretchedSprite = create_bitmap(width, height); // allocate it to the wanted dimensions
    clear_to_color(stretchedSprite, makecol(255,0,255)); // clear it to transparent color

    stretch_sprite(stretchedSprite, sprite, 0, 0, width, height); // draw the stretched sprite on the new transparent bitmap
    draw_trans_sprite(buffer, stretchedSprite, x, y); // draw the new bitmap containing the stretched transparent sprite on the buffer

    destroy_bitmap(stretchedSprite); // destroy the bitmap
}

Point makePoint(int x, int y, int z)
{
    /** Initialize and return a Point element **/

    Point point;

    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

int convertAxisAndDirectionToInt(Axis axis, AxisDirection direction)
{
    /** Convert an action (axis and direction) to an int **/

    if(axis == xAxis && direction == Forward) return 0;
    else if(axis == xAxis && direction == Backward) return 1;
    else if(axis == yAxis && direction == Forward) return 3;
    else if(axis == yAxis && direction == Backward) return 2;
    else if(axis == zAxis && direction == Forward) return 4;
    else if(axis == zAxis && direction == Backward) return 5;
    else return 0;
}

char* convertAxisAndDirectionToString(Axis axis, AxisDirection direction)
{
    /** Convert an action (axis and direction) to a string in order to print it with text **/

    if(axis == xAxis && direction == Forward) return "Droite";
    else if(axis == xAxis && direction == Backward) return "Gauche";
    else if(axis == yAxis && direction == Forward) return "Avant";
    else if(axis == yAxis && direction == Backward) return "Arriere";
    else if(axis == zAxis && direction == Forward) return "Haut";
    else if(axis == zAxis && direction == Backward) return "Bas";
    else return "Error";
}

void changeBlocColor(SpriteCollection* sprites)
{
    /** Change some colors in the sprites, faster than editing all of them by hand */

    int i,x,y;

    for(i = 0; i < BlocCount; i++)
        for(y = 0; y < 65; y++)
            for(x = 0; x < 65; x++)
                if(getpixel(sprites->blocs[i], x, y) == makecol(155,251,145) || getpixel(sprites->blocs[i], x, y) == makecol(187,252,180) || getpixel(sprites->blocs[i], x, y) == makecol(217,253,213))
                    putpixel(sprites->blocs[i], x, y, makecol(175,175,175));
}
