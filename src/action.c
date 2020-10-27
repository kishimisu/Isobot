#include "headers/main.h"

void clicOnActionMenu(Sequence* sequence, Characters characters, DebugInfos debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos)
{
    /** Update all the informations structures depending on the exact position of the clic in the window */

    int i;
    int numberOfButtons = ActionButtonCount;
    windowsInfos->actionMenu.drawPriority = 1; // change the draw priority of the windows
    windowsInfos->sequenceMenu.drawPriority = 0;

    if(characters.current != 0) numberOfButtons -= 2;

    for(i = 0; i < numberOfButtons; i++) // for each action button
    {
        if(isMouseOnButton(ActionMenu, i, *windowsInfos) && debugInfos.status == Stop) // if the mouse is on the button and the debug isn't running
        {
            mouseInfos->clicFocus = ActionMenu; // update the clic focus
            mouseInfos->element = i; // update the selected element
        }
    }

    // if the mouse is located on the top of the window, indicating a drag action
    if(mouse_x > windowsInfos->actionMenu.offsetX && mouse_x < windowsInfos->actionMenu.offsetX + windowsInfos->actionMenu.width && mouse_y > windowsInfos->actionMenu.offsetY && mouse_y < windowsInfos->actionMenu.offsetY + windowsInfos->dragOffset)
        windowsInfos->actionMenu.drag = True; // we set the drag property to true
}

void drawActionMenu(SpriteCollection sprites, Characters characters, MouseInfos mouseInfos, WindowsInfos windowsInfos, BITMAP* buffer)
{
    /** Draws the action menu and every element in it */

    int i;
    int numberOfButtons = ActionButtonCount;

    int actionMenuOffsetX = windowsInfos.actionMenu.offsetX; // set some variables so the code isn't too heavy
    int actionMenuOffsetY = windowsInfos.actionMenu.offsetY;

    int xOffset = windowsInfos.offsetFromLeft;
    int yOffset = windowsInfos.offsetFromTop;

    int buttonSize = windowsInfos.buttonSize;
    int distanceBetweenButtons = windowsInfos.distanceBetweenButtons;

    // draw the background
    stretch_sprite(buffer, sprites.actionBackground, actionMenuOffsetX, actionMenuOffsetY, windowsInfos.actionMenu.width, windowsInfos.actionMenu.height);

    if(characters.current != 0) numberOfButtons -= 2;

    for(i = 0; i < numberOfButtons; i++) // for every action button
    {
        // if the mouse is on the button and the clic focus correspond to the same button, draw a darker background
        if(mouseInfos.clicFocus == ActionMenu && mouseInfos.element == i && mouse_x > actionMenuOffsetX + xOffset + distanceBetweenButtons * (i%4) && mouse_x < actionMenuOffsetX + xOffset + distanceBetweenButtons * (i%4) + buttonSize && mouse_y > actionMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4) && mouse_y < actionMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4) + buttonSize)
            stretch_sprite(buffer, sprites.buttonBackground[Clicked], actionMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), actionMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
        else // else draw the normal background
            stretch_sprite(buffer, sprites.buttonBackground[Idle], actionMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), actionMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);

        // draw the button
        stretch_sprite(buffer, sprites.actionButton[i], actionMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), actionMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
    }
}

void dragActionMenu(WindowsInfos* windowsInfos)
{
    /** Manage the position of the action window when the user is dragging it */

    static int startingX, startingY, startingXOffset, startingYOffset;
    static Bool firstClic = True;

    if(mouse_b & 1) // if the user is still dragging it
    {
        if(firstClic) // if it's the first click on the window
        {
            // initialize the starting coordinates
            startingX = mouse_x;
            startingY = mouse_y;
            startingXOffset = windowsInfos->actionMenu.offsetX;
            startingYOffset = windowsInfos->actionMenu.offsetY;

           firstClic = False; // not the first click anymore
        }

        // draw the window at the new location depending on the current mouse location and the starting coordinates
        windowsInfos->actionMenu.offsetX = startingXOffset - (startingX - mouse_x);
        windowsInfos->actionMenu.offsetY = startingYOffset - (startingY - mouse_y);
    }
    else // else reset the firstClic property and indicates that the user isn't dragging the window anymore
    {
        firstClic = True;
        windowsInfos->actionMenu.drag = False;
    }
}

void checkIfDraggingMode(MouseInfos* mouseInfos)
{
    /** Detect if the user wants to drag a selected action menu into the sequence menu */

    if(sqrt((mouse_x - mouseInfos->startingX) * (mouse_x - mouseInfos->startingX) + (mouse_y - mouseInfos->startingY) * (mouse_y - mouseInfos->startingY)) > TileWidth)
        mouseInfos->clicFocus = Dragging;
}

void drawDraggingMode(MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the dragged button at the mouse's position */

    stretch_sprite(buffer, sprites.buttonBackground[Idle], mouse_x, mouse_y, windowsInfos.buttonSize, windowsInfos.buttonSize);
    stretch_sprite(buffer, sprites.actionButton[mouseInfos.element], mouse_x, mouse_y, windowsInfos.buttonSize, windowsInfos.buttonSize);
}

void drawDragOnSequence(DragInfos* dragInfos, Sequence* sequence, MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer)
{
    /** Update the sequence menu with animations depending on where the new action is dragged */

    int gap = (SCREEN_W * 422 / 1600 - windowsInfos.buttonSize * 4)/5;
    int color;
    int offset = None;

    int sequenceMenuOffsetX = windowsInfos.sequenceMenu.offsetX; // set some variables so the code isn't too heavy
    int sequenceMenuOffsetY = windowsInfos.sequenceMenu.offsetY;

    int xOffset = windowsInfos.offsetFromLeft;
    int yOffset = windowsInfos.offsetFromTop;

    int buttonSize = windowsInfos.buttonSize;
    int distanceBetweenButtons = windowsInfos.distanceBetweenButtons;

    getDraggingID(sequence, dragInfos, windowsInfos);

    if(dragInfos->id != None)
    {
        if(sequence->action[dragInfos->id].type == mouseInfos.element)
            offset = 0;
        else if(dragInfos->id-1 >= 0)
            if (sequence->action[dragInfos->id-1].type == mouseInfos.element)
                offset = 1;

        if(sequence->count == SequenceLimit && offset == None)
            color = makecol(255,0,0);
        else
            color = makecol(0,0,0);

        rectfill(buffer, dragInfos->x * windowsInfos.distanceBetweenButtons + windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft - 4 - gap/2, dragInfos->y * windowsInfos.distanceBetweenButtons + windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTop,  dragInfos->x * windowsInfos.distanceBetweenButtons + windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft + 4 - gap / 2, dragInfos->y * windowsInfos.distanceBetweenButtons + windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTop + windowsInfos.buttonSize, color);

        if(offset != None)
        {
            int x = dragInfos->x;
            int y = dragInfos->y;

            if(x == 4 && offset == 0)
            {
                x = 0;
                y++;
            }
            else if(x == 0 && offset == 1 && y != 0)
            {
                x = 3;
                y--;
            }
            else if(offset == 1)
                x--;

            stretch_sprite(buffer, sprites.add, sequenceMenuOffsetX + xOffset + distanceBetweenButtons * x + buttonSize/2, sequenceMenuOffsetY + yOffset + distanceBetweenButtons * y + buttonSize/2, buttonSize/2, buttonSize/2);
        }
    }
}

void getDraggingID(Sequence* sequence, DragInfos* dragInfos, WindowsInfos windowsInfos)
{
    /** Get the ID where the new action is supposed to be placed in the sequence depending on the mouse's position */

    if(mouse_x > windowsInfos.sequenceMenu.offsetX && mouse_x < windowsInfos.sequenceMenu.offsetX + windowsInfos.sequenceMenu.width && mouse_y > windowsInfos.sequenceMenu.offsetY && mouse_y < windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTopDebug)
    {
        dragInfos->x = (mouse_x - windowsInfos.sequenceMenu.offsetX - windowsInfos.offsetFromLeft - windowsInfos.buttonSize/2 + windowsInfos.distanceBetweenButtons) / windowsInfos.distanceBetweenButtons;
        dragInfos->y = (mouse_y - windowsInfos.sequenceMenu.offsetY - windowsInfos.offsetFromTop - windowsInfos.buttonSize/2 + windowsInfos.distanceBetweenButtons/2) / windowsInfos.distanceBetweenButtons;

        if(dragInfos->x + 4 * dragInfos->y > SequenceLimit-1)
            dragInfos->id = None;
        else if(dragInfos->x + 4 * dragInfos->y > sequence->count)
        {
            dragInfos->id = sequence->count;
            dragInfos->x = dragInfos->id%4;
            dragInfos->y = floor(dragInfos->id/4);
        }
        else
            dragInfos->id = dragInfos->x + 4 * dragInfos->y;
    }
    else dragInfos->id = None;
}
