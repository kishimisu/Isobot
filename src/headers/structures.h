#ifndef STRUCTURES_H
#define STRUCTURES_H

#define BlocCount 14
#define LevelCount 10
#define HelperCount 4

#define DebugButtonCount 4
#define ActionButtonCount 7
#define ButtonBackgroundCount 3

#define SequenceLimit 20

typedef enum MainMenuButtons
{
    Play, Editor, Quit
}MainMenuButtons;

typedef enum CharacterArea
{
    Bottom, Top
}CharacterArea;

typedef enum ButtonBackGround
{
    Debugging, Idle, Clicked, Failed, Overlay
}ButtonBackGround;

typedef enum DebugAction
{
    Run, Pause, Step, Stop,
}DebugAction;

typedef enum ActionType
{
    MoveForward, RotateCharacterLeft, RotateCharacterRight, Nothing, Finish, RotateCameraLeft, RotateCameraRight
}ActionType;

typedef enum ClicFocus
{
    SequenceMenu, ActionMenu, DebugMenu, Camera, Dragging
}ClicFocus;

typedef enum Bool
{
    False, True
}Bool;

typedef enum Direction
{
    Up, Right, Down, Left
}Direction;

typedef enum Orientation
{
    North, East, South, West
}Orientation;

typedef enum CaseType
{
    Start, Validated = 3, End = 8
}CaseType;

typedef enum Clic
{
    LeftClic, RightClic
}Clic;

typedef enum Axis
{
    xAxis, yAxis, zAxis
}Axis;

typedef enum AxisDirection
{
   Backward, Forward
}AxisDirection;

typedef struct Point
{
    /** Structure used for perspective and illusion checking **/

    int x, y, z; // position
    Bool valid;  // if the block is a valid illusion block
}Point;

typedef struct Linkage
{
    Bool active;
    int x, y, z;
}Linkage;

typedef struct EditorSelection
{
    /** Manages the cursor in the editor **/

    int x, y, z; // position
    CaseType bloc;   // current bloc
    Bool helper; // true if the helpers blocs are visible
    Linkage linkage;
}EditorSelection;

typedef struct MouseInfos
{
    /** Manages informations relative to the mouse in "game" mode **/

    ClicFocus clicFocus; // area where the first click occurred
    int element;         // button number in the area where the first click occurred
    Clic currentClic;    // if a click is pressed, is it the left or right one ?

    int startingX, startingY;
}MouseInfos;

typedef struct WindowProperties
{
    /** Store all the properties of a window **/

    int offsetX, offsetY;   // offset from the left and the top of the screen
    int width, height;      // dimensions
    Bool drag;              // true if the user is dragging the current windows
    int drawPriority;       // windows with a higher priority will appear on top of the ones with a lower priority
}WindowProperties;

typedef struct DragInfos
{
    int id;
    int x, y;
    int link;
}DragInfos;

typedef struct WindowsInfos
{
    /** Store all the properties of all the windows **/

    WindowProperties actionMenu, sequenceMenu;              // List of the different windows
    int offsetFromLeft, offsetFromTop, offsetFromTopDebug;  // offset from the left and the top of the windows to place the buttons
    int buttonSize, distanceBetweenButtons;                 // properties relative to the buttons
    int dragOffset;                                         // the user can drag a window if he clicks between the top of the window and the dragOffset
    int separationHeight;

    int mainMenuButtonWidth, mainMenuButtonHeight, mainMenuButtonOffset;
    int mainMenuTitleWidth, mainMenuTitleHeight;
    int victoryOffsetX, victoryOffsetY;
    int levelRenderHeight;
}WindowsInfos;

typedef struct DebugInfos
{
    /** Manages informations when the user enters the debug mode **/

    int* currentAction;       // the current action for each sequence
    int* count;              // if the current action is stacked, the position in the stack for each sequence
    int longestSequence;
    Bool firstStep, newStep;                 // booleans used to have a smooth transition between "Run" and "Step" modes
    DebugAction status;                      // the current status of the debugging (pause, run, step, stop...)
}DebugInfos;

typedef struct Action
{
    /** Represents a single action used in the sequence **/

    ActionType type; // type of action
    int count;      // if it's stacked, the number of actions
}Action;

typedef struct Sequence
{
    /** The sequence of actions the user created to solve a level **/

    int count;                     // the number of active actions
    Action action[SequenceLimit];   // an array containing all the different actions
}Sequence;

typedef struct MoveAction
{
    /** Store all the properties of an action **/

    Bool isMoving;              // true if the bloc is moving
    time_t startingTime;       // the time when the action started
    int length, current;       // the length of the action and the current step
    Axis axis;                 // the axis of the action
    AxisDirection direction;   // the direction of the action
}MoveAction;

typedef struct Case
{
    /** Store all the informations for a single bloc on the map **/

    CaseType type;          // the type of the bloc

    MoveAction action;      // if any, the action properties of the bloc

    Bool actionBloc;        // true if this is an action bloc (a bloc that can move other blocs)
    int actionNumber;       // number of linked blocs
    Point* linkedCases;     // dynamic array of linked blocs
}Case;

typedef struct Map
{
    /** Manages every information concerning the map **/

    int width, height, depth;   // dimensions
    int xOffset, yOffset;       // offset from the left and the top of the screen
    int zoom;                   // the current level of zoom (1.0 or 2.0)
    Orientation orientation;    // the current orientation of the map
    Case*** cases;              // a 3D dynamic array containing all the tiles of the map
}Map;

typedef struct Character
{
    /** Properties of the character **/

    int x, y, z;            // current position
    Point defaultPos;       // default position
    Direction direction;    // direction
    Bool didWin;
}Character;

typedef struct Characters
{
    /** Structures that manages all the characters of the level */

    Character* entity; // dynamic array containing all characters
    int current, count; // current character, and count of characters
}Characters;

typedef struct SpriteCollection
{
    /** A structure that contains all the sprites used in the game. Initialized and destroyed only one time per instance of the game **/

    BITMAP* blocs[BlocCount];                           // all different blocks
    BITMAP* blocDirection[6];                           // direction overlay for the action blocs
    BITMAP* mainCharacter[4];                           // main character sprites
    BITMAP* character[4];                               // character sprites
    BITMAP* actualCharacter;                            // green overlay for the selected character
    BITMAP* actionBackground;                           // action menu background
    BITMAP* add;                                        // "+" button for drag & drop in sequence
    BITMAP* sequenceBackground;                         // sequence menu background
    BITMAP* debugButton[DebugButtonCount];              // debug menu buttons
    BITMAP* actionButton[ActionButtonCount];            // action menu buttons
    BITMAP* buttonBackground[ButtonBackgroundCount];    // background of the buttons
    BITMAP* mainMenuButton[3];
    BITMAP* mainMenuTitle;
    BITMAP* victory;
    BITMAP* levelSelect;
    BITMAP* levelSelectArrow[2];

    BITMAP* selection;                                  // selection (editor)
    BITMAP* wall[2];                                    // walls (editor)
    BITMAP* helper[HelperCount];                        // helper blocks (editor)
    BITMAP* linkage;                                    // linkage bloc overlay (editor, linkage mode)

    FONT* customFont;                                   // custom font used for the stack number in the sequence

}SpriteCollection;

#endif // STRUCTURES_H
