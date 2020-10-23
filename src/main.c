#include "main.h"

/// 1781 lines : 05/04/18 - 03h25
/// 2482 lines : 11/04/18 - 14h58
/// 2780 lines : 23/04/18 - 00h34

int main(int argc, char* argv[])
{
    SpriteCollection sprites;
    WindowsInfos windowsInfos;
    BITMAP* buffer;
    int exit = 0;

    srand(time(NULL));

    initAllegro();

    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    initSpriteCollection(&sprites);
    initWindowsInfos(&windowsInfos);

    do
    {
        switch(main_menu(sprites, windowsInfos, buffer))
        {
            case 0: levelSelectMenu(sprites, windowsInfos, buffer); break;
                //game(sprites, windowsInfos, 2, buffer); break;
            case 1: editor(sprites, buffer); break;
            default: exit = 1; break;
        }
    }while(!exit);

    return 0;
}END_OF_MAIN();

void initAllegro()
{
    allegro_init();

    if(install_keyboard() != 0) error("install_keyboard");
    if(install_mouse() == -1) error("install_mouse");
    show_os_cursor(MOUSE_CURSOR_ARROW);

    initTimer();

    set_color_depth(32);
    text_mode(-1);

    if(!setWorkingGFXmode())
        error("set_gfx_mode");
}

Bool setWorkingGFXmode()
{
    /** Brute force different dimensions of the allegro window to find the best one for the current screen size **/

    int i;

    int x[3] = {1920, 1000, 800}; // array of window dimensions
    int y[3] = {1280, 700, 600};

    for(i = 0; i < 3; i++) // for every dimension
    {
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, x[i], y[i], 0, 0) == 0) // if the allegro windows has been created
            return True; // exit with success
    }

    return False; // else if every dimension failed, exit with failure
}

void game_timer()
{
    /** Function called each millisecond that increment the game timer **/

    game_time++;
}END_OF_FUNCTION(game_timer);

void initTimer()
{
    /** Initialize the game timer **/

    install_timer(); // install the timer

    if(install_int(game_timer, 1) < 0) // if it failed, exit
        error("install_int");

    game_time = 0; // reset the time counter

    LOCK_VARIABLE(game_time); // lock the allegro functions
    LOCK_FUNCTION(game_timer);
}

void initSpriteCollection(SpriteCollection* sprites)
{
    /** Initialize all the sprites and store them in a structure only once for each instance of the game **/

    int i;
    char fileName[64];

    // the following lines are used to manage sprites with transparency
    COLOR_MAP global_trans_table;
    PALETTE p ;

    set_palette(p);
    create_trans_table(&global_trans_table, p, 128, 128, 128, NULL);
    set_trans_blender(128, 128, 128, 128);

    // loading of all the sprites

    for(i = 0; i < BlocCount; i++)
    {
        sprintf(fileName, "ressources/img/blocs/iso_cube%d.bmp", i+5);
        sprites->blocs[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i < 6; i++)
    {
        sprintf(fileName, "ressources/img/blocs/blocDirection_%d.bmp", i);
        sprites->blocDirection[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i< 4; i++)
    {
        sprintf(fileName, "ressources/img/characters/character_%d.bmp", i);
        sprites->character[i] = load_sprite(fileName, NULL);

        sprintf(fileName, "ressources/img/characters/mainCharacter_%d.bmp", i);
        sprites->mainCharacter[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i < ActionButtonCount; i++)
    {
        sprintf(fileName, "ressources/img/buttons/actionButton_%d.bmp", i);
        sprites->actionButton[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i < DebugButtonCount; i++)
    {
        sprintf(fileName, "ressources/img/buttons/debugButton_%d.bmp", i);
        sprites->debugButton[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i < ButtonBackgroundCount; i++)
    {
        sprintf(fileName, "ressources/img/buttons/buttonBackground_%d.bmp", i);
        sprites->buttonBackground[i] = load_sprite(fileName, NULL);
    }

    for(i = 0; i < 3; i ++)
    {
        sprintf(fileName, "ressources/img/menus/mainMenuButton_%d.bmp", i);
        sprites->mainMenuButton[i] = load_sprite(fileName, NULL);
    }

    sprites->mainMenuTitle = load_sprite("ressources/img/menus/mainMenuTitle.bmp", NULL);
    sprites->victory = load_sprite("ressources/img/menus/victory.bmp", NULL);
    sprites->levelSelect = load_sprite("ressources/img/menus/levelSelect.bmp", NULL);
    sprites->levelSelectArrow[0] = load_sprite("ressources/img/menus/levelSelectArrow_0.bmp", NULL);
    sprites->levelSelectArrow[1] = load_sprite("ressources/img/menus/levelSelectArrow_1.bmp", NULL);

    sprites->actualCharacter = load_sprite("ressources/img/characters/actualCharacter.bmp", NULL);
    sprites->actionBackground = load_sprite("ressources/img/menus/actionBackground.bmp", NULL);
    sprites->add = load_sprite("ressources/img/buttons/add.bmp", NULL);
    sprites->sequenceBackground = load_sprite("ressources/img/menus/sequenceBackground.bmp", NULL);
    sprites->selection = load_sprite("ressources/img/editor/iso_selection.bmp", NULL);
    sprites->wall[0] = load_sprite("ressources/img/editor/wall0.bmp", NULL);
    sprites->wall[1] = load_sprite("ressources/img/editor/wall1.bmp", NULL);

    sprites->linkage = load_sprite("ressources/img/editor/linkage.bmp", NULL);

    for(i = 0; i < HelperCount; i++)
    {
        sprintf(fileName, "ressources/img/editor/helper%d.bmp", i);
        sprites->helper[i] = load_sprite(fileName,p);
    }

    sprites->customFont = load_font("ressources/font.pcx", NULL, NULL);

    changeBlocColor(sprites);
}

void initWindowsInfos(WindowsInfos* windowsInfos)
{
    /** Init all the windows properties, scaling them according to the screen width **/

    windowsInfos->buttonSize = SCREEN_W * DefaultButtonSize / 1600;
    windowsInfos->distanceBetweenButtons = SCREEN_W * 422 / 1600 - windowsInfos->buttonSize * 4;
    windowsInfos->dragOffset = SCREEN_W * 56 / 1600;
    windowsInfos->separationHeight = SCREEN_W * 100 / 1600;

    windowsInfos->offsetFromLeft = SCREEN_W * 23 / 1600 + windowsInfos->distanceBetweenButtons / 5;
    windowsInfos->offsetFromTop = SCREEN_W * 57 / 1600 + windowsInfos->distanceBetweenButtons / 5;
    windowsInfos->offsetFromTopDebug = SCREEN_W * 637 / 1600 + (SCREEN_W * 97 / 1600 - windowsInfos->buttonSize) / 2;

    windowsInfos->actionMenu.width = SCREEN_W * DefaultActionMenuWidth / 1600;
    windowsInfos->actionMenu.height = SCREEN_W * DefaultActionMenuHeight / 1600;
    windowsInfos->actionMenu.offsetX = 0;
    windowsInfos->actionMenu.offsetY = SCREEN_H - windowsInfos->actionMenu.height;

    windowsInfos->actionMenu.drag = False;
    windowsInfos->actionMenu.drawPriority = 1;


    windowsInfos->sequenceMenu.width = SCREEN_W * DefaultSequenceMenuWidth / 1600;
    windowsInfos->sequenceMenu.height = SCREEN_W * DefaultSequenceMenuHeight / 1600;
    windowsInfos->sequenceMenu.offsetX = SCREEN_W - windowsInfos->sequenceMenu.width;
    windowsInfos->sequenceMenu.offsetY = 0;

    windowsInfos->sequenceMenu.drag = False;
    windowsInfos->sequenceMenu.drawPriority = 1;

    windowsInfos->sequenceMenu.drag = False;
    windowsInfos->sequenceMenu.drawPriority = 0;

    windowsInfos->mainMenuButtonWidth = SCREEN_W * 483 / 1600;
    windowsInfos->mainMenuButtonHeight = SCREEN_W * 179 / 1600;
    windowsInfos->mainMenuButtonOffset = SCREEN_W * 15 / 1600;

    windowsInfos->mainMenuTitleWidth = SCREEN_W * 946 / 1600;
    windowsInfos->mainMenuTitleHeight = SCREEN_W * 261 / 1600;

    windowsInfos->victoryOffsetX = SCREEN_W * 31 / 1600;
    windowsInfos->victoryOffsetY = SCREEN_W * 50 / 1600;

    windowsInfos->levelRenderHeight = windowsInfos->mainMenuTitleWidth * 678 / 946;
}
