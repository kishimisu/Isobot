#include "headers/main.h"

int main_menu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer)
{
    /** Main menu */

    MouseInfos mouseInfos;
    ButtonBackGround buttonStatus[3];
    int i, choice = -1;
    Bool wait_ESC_KEY_release = False;
    Bool easter_egg = rand()%100==0;

    for(i = 0; i < 3; i++)
        buttonStatus[i] = Idle;

    clear_to_color(buffer, makecol(0, 200, 200));
    drawMainMenuBackground(sprites, buffer);

    initMouseInfos(&mouseInfos);

    if(key[KEY_ESC])
        wait_ESC_KEY_release = True;

    do
    {
        if(!easter_egg)
            drawRandomTile(sprites, buffer);
        else
            drawMainMenuBackground(sprites, buffer);

        detectMouseOverButton(buttonStatus, windowsInfos, sprites);

        if(mouse_b & 1 && mouseInfos.currentClic == None)
            clicButton(&mouseInfos, buttonStatus, windowsInfos, sprites);
        else if(!(mouse_b & 1) && mouseInfos.currentClic != None)
            choice = releaseButton(&mouseInfos, buttonStatus, windowsInfos, sprites);

        drawMainMenuButtons(sprites, mouseInfos, buttonStatus, windowsInfos, buffer);

        if(key[KEY_H]) helpMenu(sprites, buffer);
        else if(!key[KEY_ESC])
            wait_ESC_KEY_release = False;

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    }while((!key[KEY_ESC] && choice == -1) || wait_ESC_KEY_release);

    return choice;
}

void clicButton(MouseInfos* mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites)
{
    /** Detect if new click is on button */

    int i;

    for(i = 0; i < 3; i++)
    {
        if(mouse_x > SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 && mouse_x < SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonWidth && mouse_y > SCREEN_H * (i+2) / 5 && mouse_y < SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonHeight)
        {
            buttonStatus[i] = Clicked;
            mouseInfos->element = i;
        }
    }

    mouseInfos->currentClic = LeftClic;
}

int releaseButton(MouseInfos* mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites)
{
    /** Manage actions if the mouse is still on the same button when the click began */

    int i;

    for(i = 0; i < 3; i++)
    {
        if(i == mouseInfos->element && mouse_x > SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 && mouse_x < SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonWidth && mouse_y > SCREEN_H * (i+2) / 5 && mouse_y < SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonHeight)
        {
            return i;
        }
    }

    buttonStatus[mouseInfos->element] = Idle;

    mouseInfos->currentClic = None;
    mouseInfos->element = None;

    return -1;
}

void detectMouseOverButton(ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, SpriteCollection sprites)
{
    /** Detect if the mouse is over a button */

    int i;

    for(i = 0; i < 3; i++)
    {
        if(mouse_x > SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 && mouse_x < SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonWidth && mouse_y > SCREEN_H * (i+2) / 5 && mouse_y < SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonHeight)
        {
            if(buttonStatus[i] == Idle)
                buttonStatus[i] = Overlay;
        }
        else if(buttonStatus[i] != Clicked)
            buttonStatus[i] = Idle;
    }
}

void drawMainMenuButtons(SpriteCollection sprites, MouseInfos mouseInfos, ButtonBackGround buttonStatus[3], WindowsInfos windowsInfos, BITMAP* buffer)
{
    /** Draw the main menu buttons */

    int i;
    int bgColor;

    stretch_sprite(buffer, sprites.mainMenuTitle, SCREEN_W / 2 - windowsInfos.mainMenuTitleWidth / 2, 0, windowsInfos.mainMenuTitleWidth, windowsInfos.mainMenuTitleHeight);

    for(i = 0; i < 3; i++)
    {
        bgColor = 255;

        if(mouse_x > SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 && mouse_x < SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonWidth && mouse_y > SCREEN_H * (i+2) / 5 && mouse_y < SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonHeight)
        {
            if(buttonStatus[i] == Overlay)
                bgColor = 155;
            else if(buttonStatus[i] == Clicked && mouseInfos.element == i)
                bgColor = 55;
        }

        bgColor = makecol(bgColor, bgColor, bgColor);

        rectfill(buffer, SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonOffset, SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonOffset, SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2 + windowsInfos.mainMenuButtonWidth - windowsInfos.mainMenuButtonOffset, SCREEN_H * (i+2) / 5 + windowsInfos.mainMenuButtonHeight - windowsInfos.mainMenuButtonOffset, bgColor);
        stretch_sprite(buffer, sprites.mainMenuButton[i], SCREEN_W / 2 - windowsInfos.mainMenuButtonWidth / 2, SCREEN_H * (i+2) / 5, windowsInfos.mainMenuButtonWidth, windowsInfos.mainMenuButtonHeight);
    }
}

void drawMainMenuBackground(SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the main menu background */

    int x, y;
    int isoX, isoY;

    for(y = 0; y < SCREEN_H/TileHeight*4+TileHeight; y++)
    {
        for(x = 0; x < SCREEN_W/TileWidth + TileWidth; x++)
        {
            isoX = TileWidth * x - TileWidth / 2;
            isoY = TileHeight / 4 * y - TileHeight / 4;

            if(y%2 == 1)
                isoX += TileWidth / 2;

            draw_sprite(buffer, sprites.blocs[rand()%(BlocCount-2)], isoX, isoY);
        }
    }
}

void drawRandomTile(SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw a random tile to animate the background */

    int x = rand()%(SCREEN_W/TileWidth + TileWidth);
    int y = rand()%(SCREEN_H/TileHeight*4+TileHeight);
    int isoX, isoY;

    isoX = TileWidth * x - TileWidth / 2;
    isoY = TileHeight / 4 * y - TileHeight / 4;

    if(y%2 == 1)
        isoX += TileWidth / 2;

    draw_sprite(buffer, sprites.blocs[rand()%(BlocCount-2)], isoX, isoY);
}

void victoryMenu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer)
{
    /** Menu that appear when the player win a level */

    int color;

    do
    {
        if(clock()%500 > 250)
            color = makecol(0,255,0);
        else color = makecol(255,100,0);

        rectfill(buffer, SCREEN_W / 2 - windowsInfos.mainMenuTitleWidth / 2 + windowsInfos.victoryOffsetX, SCREEN_H / 2 - windowsInfos.mainMenuTitleHeight / 2 + windowsInfos.victoryOffsetY, SCREEN_W / 2 + windowsInfos.mainMenuTitleWidth / 2 - windowsInfos.victoryOffsetX, SCREEN_H / 2 + windowsInfos.mainMenuTitleHeight / 2 - windowsInfos.victoryOffsetY, color);
        stretch_sprite(buffer, sprites.victory, SCREEN_W / 2 - windowsInfos.mainMenuTitleWidth / 2, SCREEN_H / 2 - windowsInfos.mainMenuTitleHeight / 2, windowsInfos.mainMenuTitleWidth, windowsInfos.mainMenuTitleHeight);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }while(!key[KEY_ESC]);
}

void levelSelectMenu(SpriteCollection sprites, WindowsInfos windowsInfos, BITMAP* buffer)
{
    Map map;
    int currentLevel = 0;
    Bool keyPressed = False;

    loadMapFromFile(&map, currentLevel);
    updateBackgroundColor(True);

    do
    {
        //clear_bitmap(buffer);
        clear_to_color(buffer, updateBackgroundColor(False));

        if(!keyPressed)
        {
            if(key[KEY_LEFT] && currentLevel > 0)
            {
                currentLevel--;
                loadMapFromFile(&map, currentLevel);
            }
            else if(key[KEY_RIGHT] && currentLevel < LevelCount - 1)
            {
                currentLevel++;
                loadMapFromFile(&map, currentLevel);
            }
            else if(key[KEY_ENTER])
                game(sprites, windowsInfos, currentLevel, buffer);
        }

        if(key[KEY_LEFT] || key[KEY_RIGHT] || key[KEY_ENTER])
            keyPressed = True;
        else
            keyPressed = False;

        drawGameBackground(map,sprites,buffer);
        drawLevelSelectMap(map, sprites, buffer);

        stretch_sprite(buffer, sprites.levelSelect, SCREEN_W / 2 - windowsInfos.mainMenuTitleWidth / 2, 0, windowsInfos.mainMenuTitleWidth, windowsInfos.mainMenuTitleHeight);

        if(currentLevel > 0)
            stretch_sprite(buffer, sprites.levelSelectArrow[0], 0, SCREEN_H / 2 - windowsInfos.mainMenuButtonHeight, windowsInfos.mainMenuButtonHeight*2, windowsInfos.mainMenuButtonHeight*2);
        if(currentLevel < LevelCount - 1)
            stretch_sprite(buffer, sprites.levelSelectArrow[1], SCREEN_W - windowsInfos.mainMenuButtonHeight*2, SCREEN_H / 2 - windowsInfos.mainMenuButtonHeight, windowsInfos.mainMenuButtonHeight*2, windowsInfos.mainMenuButtonHeight*2);

        if(key[KEY_H]) helpMenu(sprites, buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }while(!key[KEY_ESC]);
}

int updateBackgroundColor(Bool init)
{
    static time_t timeWhenLastColorChanged;
    static int targetR, targetG, targetB;
    static int currentR, currentG, currentB;

    if(init)
    {
        currentR = rand()%256;
        currentG = rand()%256;
        currentB = rand()%256;
        targetR = rand()%256;
        targetG = rand()%256;
        targetB = rand()%256;
        timeWhenLastColorChanged = clock();
    }

    if(clock() - timeWhenLastColorChanged > 10)
    {
        timeWhenLastColorChanged = clock();

        if(currentR > targetR) currentR--;
        else if(currentR < targetR) currentR++;

        if(currentG > targetG) currentG--;
        else if(currentG < targetG) currentG++;

        if(currentB > targetB) currentB--;
        else if(currentB < targetB) currentB++;

        if(currentR == targetR && currentG == targetG && currentB == targetB)
        {

            targetR = rand()%256;
            targetG = rand()%256;
            targetB = rand()%256;
        }
    }

    return makecol(currentR, currentG, currentB);
}

void helpMenu(SpriteCollection sprites, BITMAP* buffer)
{
    FILE* file;
    char buf[1024];
    int currentLine = 0;
    BITMAP* snapshot;
    BITMAP* window;
    int maxWidth = 0;
    int release = False;

    snapshot = create_bitmap(SCREEN_W, SCREEN_H);
    blit(buffer, snapshot, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    clear_bitmap(buffer);

    file = fopen("ressources/help.txt","r");

    if(file != NULL)
    {
        while(fgets(buf, sizeof(buf), file) != NULL)
        {
            buf[strlen(buf) - 1] = '\0';

            currentLine++;
            if(text_length(font, buf) > maxWidth)
                maxWidth = text_length(font, buf);
        }

        window = create_bitmap(maxWidth + 40, (currentLine+2) * text_height(font));

        currentLine = 0;
        fseek(file, SEEK_SET, 0);

        while(fgets(buf, sizeof(buf), file) != NULL)
        {
            buf[strlen(buf) - 1] = '\0';

            textout(window, font, buf, 20, (currentLine+1) * text_height(font), makecol(255,255,255));

            currentLine++;
        }

        blit(snapshot, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        blit(window, buffer, 0, 0, SCREEN_W / 2 - window->w / 2, SCREEN_H / 2 - window->h / 2, window->w, window->h);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        do
        {
            if(!key[KEY_H])
                release = True;
        }while(!key[KEY_H] && !release);

        destroy_bitmap(window);
        fclose(file);
    }

    destroy_bitmap(snapshot);
}
