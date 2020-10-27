#include "headers/main.h"

void initMouseInfos(MouseInfos* mouseInfos)
{
    /** Initializes the mouse parameters */

    mouseInfos->clicFocus = None;
    mouseInfos->element = None;
    mouseInfos->currentClic = None;
}

void detectMouseFocus(Sequence* sequence, Characters* characters, Map map, DebugInfos* debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos)
{
    /** Updates the mouse informations when a new click occurs */

    int newCharacter;

    if(windowsInfos->actionMenu.drawPriority == 1) // depending on the window that has the highest draw priority
    {
        // if the mouse is in the action window
        if(mouse_x > windowsInfos->actionMenu.offsetX && mouse_x < windowsInfos->actionMenu.offsetX + windowsInfos->actionMenu.width && mouse_y > windowsInfos->actionMenu.offsetY && mouse_y < windowsInfos->actionMenu.offsetY + windowsInfos->actionMenu.height && mouseInfos->clicFocus == None && mouse_b & 1)
        {
            mouseInfos->clicFocus = ActionMenu; // update the click focus
            clicOnActionMenu(sequence, *characters, *debugInfos, mouseInfos, windowsInfos); // update mouse infos depending on where exactly the clic is
        }
        // else if the mouse is in the sequence window
        else if(mouse_x > windowsInfos->sequenceMenu.offsetX && mouse_x < windowsInfos->sequenceMenu.offsetX + windowsInfos->sequenceMenu.width && mouse_y > windowsInfos->sequenceMenu.offsetY && mouse_y < windowsInfos->sequenceMenu.offsetY + windowsInfos->sequenceMenu.height && mouseInfos->clicFocus == None)
        {
            mouseInfos->clicFocus = SequenceMenu; // update click focus
            clicOnSequenceMenu(sequence, debugInfos, mouseInfos, windowsInfos); // update mouse infos depending on the where the clic is
        }
        else
        {
            newCharacter = isClickOnCharacter(*characters, map);

            if(newCharacter != -1) characters->current = newCharacter;
        }
    }
    else // same thing, but the order of the condition change
    {
        if(mouse_x > windowsInfos->sequenceMenu.offsetX && mouse_x < windowsInfos->sequenceMenu.offsetX + windowsInfos->sequenceMenu.width && mouse_y > windowsInfos->sequenceMenu.offsetY && mouse_y < windowsInfos->sequenceMenu.offsetY + windowsInfos->sequenceMenu.height && mouseInfos->clicFocus == None)
        {
            mouseInfos->clicFocus = SequenceMenu;
            clicOnSequenceMenu(sequence, debugInfos, mouseInfos, windowsInfos);
        }
        else if(mouse_x > windowsInfos->actionMenu.offsetX && mouse_x < windowsInfos->actionMenu.offsetX + windowsInfos->actionMenu.width && mouse_y > windowsInfos->actionMenu.offsetY && mouse_y < windowsInfos->actionMenu.offsetY + windowsInfos->actionMenu.height && mouseInfos->clicFocus == None && mouse_b & 1)
        {
            mouseInfos->clicFocus = ActionMenu;
            clicOnActionMenu(sequence, *characters, *debugInfos, mouseInfos, windowsInfos);
        }
        else
        {
            newCharacter = isClickOnCharacter(*characters, map);

            if(newCharacter != -1) characters->current = newCharacter;
        }
    }

    mouseInfos->startingX = mouse_x;
    mouseInfos->startingY = mouse_y;
}

Bool isMouseOnButton(ClicFocus window, int buttonNumber, WindowsInfos windowsInfos)
{
    /** Returns true if the mouse is on the specified button */

    int x = mouse_x;
    int y = mouse_y;

    switch (window) // depending on the window
    {
        case ActionMenu:
            // return true if the mouse is on the button specified by the buttonNumber
            return (x > windowsInfos.actionMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) && x < windowsInfos.actionMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) + windowsInfos.buttonSize && y > windowsInfos.actionMenu.offsetY + windowsInfos.offsetFromTop + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) && y < windowsInfos.actionMenu.offsetY + windowsInfos.offsetFromTop + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) + windowsInfos.buttonSize);
        break;
        case SequenceMenu:
            return (x > windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) && x < windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) + windowsInfos.buttonSize && y > windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTop + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) && y < windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTop + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) + windowsInfos.buttonSize);
        break;
        case DebugMenu:
            return (x > windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) && x < windowsInfos.sequenceMenu.offsetX + windowsInfos.offsetFromLeft + windowsInfos.distanceBetweenButtons * (buttonNumber%4) + windowsInfos.buttonSize && y > windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTopDebug + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) && y < windowsInfos.sequenceMenu.offsetY + windowsInfos.offsetFromTopDebug + windowsInfos.distanceBetweenButtons * floor(buttonNumber/4) + windowsInfos.buttonSize);
        break;
        default:break;
    }

    return False;
}

void releaseMouse(Sequence* sequences, Characters* characters, Map* map, DebugInfos* debugInfos, MouseInfos* mouseInfos, DragInfos* dragInfos, WindowsInfos windowsInfos)
{
    /** Execute actions when the user release the mouse clic */

    if(mouseInfos->clicFocus == ActionMenu) // if the clic began on the action menu
    {
        if(isMouseOnButton(ActionMenu, mouseInfos->element, windowsInfos)) // if the mouse is on the same button when released as it was when clicked
            addActionToSequence(&sequences[characters->current], mouseInfos->element, sequences[characters->current].count); // add the action to the end of the sequence depending on the button clicked
    }
    else if(mouseInfos->clicFocus == SequenceMenu && debugInfos->status == Stop) // if the clic began on the section menu and the debug is stopped (we can't modify the sequence while debugging !)
    {
        if(isMouseOnButton(SequenceMenu, mouseInfos->element, windowsInfos)) // if the mouse is on the same button when released as it was when clicked
        {
            if(mouseInfos->currentClic == LeftClic) // if it was a left clic
            {
                sequences[characters->current].action[mouseInfos->element].count -= 1; // we decrease the count of the selected action

                if(sequences[characters->current].action[mouseInfos->element].count == 0) // if the count is null
                    deleteActionFromSequence(&sequences[characters->current], mouseInfos->element); // we remove the action
            }
            else // if it was a right clic
                deleteActionFromSequence(&sequences[characters->current], mouseInfos->element); // we directly remove the action
        }
    }
    else if(mouseInfos->clicFocus == DebugMenu && !areAllSequencesEmpty(sequences, *characters)) // if the clic began on the debug menu
    {
        if(isMouseOnButton(DebugMenu, mouseInfos->element, windowsInfos)) // if the mouse is on the same button when released as it was when clicked
        {
            switch(mouseInfos->element) // depending on the clicked button
            {
                case Run: // if it's the run button
                    if(debugInfos->status == Stop)
                    {
                        initDebugInfos(debugInfos, sequences, *characters); // we set the debugInfo status to Run
                        resetCharacters(characters);
                        loadMapFromFile(map, -1);
                    }
                    else
                        debugInfos->status = Run;
                break;
                case Pause: // pause button
                    if(debugInfos->status == Run) // if the previous status was Run
                        debugInfos->status = Pause; // we update it to Pause
                break;
                case Step: // step button
                    if(debugInfos->status == Stop && !areAllSequencesEmpty(sequences, *characters))
                        initDebugInfos(debugInfos, sequences, *characters);

                    debugInfos->status = Step;
                break;
                case Stop: // stop button
                    //initDebugInfos(debugInfos); // we reinitialize the level
                    debugInfos->status = Stop;
                    resetCharacters(characters);
                    resetMap(map);
                break;
            }
        }
    }
    else if(mouseInfos->clicFocus == Dragging)
    {
        int offset = None;
        int id;

        getDraggingID(&sequences[characters->current], dragInfos, windowsInfos);

        id = dragInfos->id;

        if(sequences[characters->current].action[id].type == mouseInfos->element)
            offset = 0;
        else if(id-1 >= 0)
            if (sequences[characters->current].action[id-1].type == mouseInfos->element)
                offset = 1;

        if(offset == 1)
            id--;

        if(id != None)
            addActionToSequence(&sequences[characters->current], mouseInfos->element, id);
    }

    initMouseInfos(mouseInfos); // we reinitialize the mouse informations
}
