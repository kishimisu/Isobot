#include "headers/main.h"

void initDebugInfos(DebugInfos* debugInfos, Sequence* sequences, Characters characters)
{
    /** Initializes informations concerning the debugging mode */

    int i;

    for(i = 0; i < characters.count; i++)
    {
        debugInfos->count[i] = 0;
        debugInfos->currentAction[i] = 0;
    }

    debugInfos->firstStep = True;
    debugInfos->newStep = True;
    debugInfos->status = Run;

    debugInfos->longestSequence = getLongestSequence(sequences, characters);
}

int getLongestSequence(Sequence* sequences, Characters characters)
{
    /** Returns the id of the longest active sequence */

    int i, j;
    int maxSteps = 0, index = 0;
    int* steps;

    steps = malloc(characters.count * sizeof(int)); // the total number of steps for each sequence

    if(steps == NULL) error("steps = malloc(characters.count * sizeof(int))");

    for(i = 0; i < characters.count; i++) // for each sequence
    {
        steps[i] = 0; // initialize the total number to 0

        for(j = 0; j < SequenceLimit; j++) // for each action
        {
            if(sequences[i].action[j].count == 0) // if the action is empty, switch to the next sequence
                break;

            steps[i] += sequences[i].action[j].count; // else add the stack number to the total number of step in the sequence
        }
    }

    for(i = 0; i < characters.count; i++) // then for each sequence
    {
        if(steps[i] > maxSteps) // if the current sequence's number of step is greater than the previous maximum
        {
            maxSteps = steps[i]; // update the maximum
            index = i; // update the id of the longest sequence
        }
    }

    free(steps);

    return index; // return the id of the longest sequence
}

Bool areAllSequencesEmpty(Sequence* sequences, Characters characters)
{
    /** Returns true if all the sequences are empty, false otherwise */

    int i;

    for(i = 0; i < characters.count; i++) // for each sequence
        if(sequences[i].count > 0) // if it's not empty return false
            return False;

    return True; // else return true
}

void initSequences(Sequence* sequences, Characters characters)
{
    /** Initializes all the sequence arrays */

    int i;

    for(i = 0; i < characters.count; i++) // for every sequence in the array
        initSequence(&sequences[i]); // initialize it

}

void initSequence(Sequence* sequence)
{
    /** Initializes a sequence array to empty */

    int i;

    for(i = 0; i < SequenceLimit; i++) // for every element in the sequence array
    {
        sequence->action[i].type = None; // empty it
        sequence->action[i].count = 0;
    }

    sequence->count = 0; // set the number of actions to 0
}

void addActionToSequence(Sequence* sequence, ActionType type, int index)
{
    /** Add an action at an index in the sequence, or increase the stack at the index */

    int i;

    if(sequence->action[index].type == type)
        sequence->action[index].count += 1;
    else
    {
        if(index > 0)
        {
            if(sequence->action[index-1].type == type)
            {
                sequence->action[index-1].count += 1;
                return;
            }
        }

        if(sequence->count < SequenceLimit)
        {
            for(i = SequenceLimit-1; i > index; i--)
            {
                sequence->action[i] = sequence->action[i-1];
            }

            sequence->action[index].type = type;
            sequence->action[index].count = 1;
            sequence->count += 1;
        }
    }
}

void deleteActionFromSequence(Sequence* sequence, int index)
{
    /** Delete an action from the sequence at a specific index */

    int i;

    for(i = index; i < sequence->count; i++) // for every action after the one to remove
        sequence->action[i] = sequence->action[i+1]; // move it to the left

    sequence->action[sequence->count-1].type = -1; // set the last action to empty
    sequence->action[sequence->count-1].count = 0;

    sequence->count -= 1; // decrease the number of actions
}

void executeAllSequences(Sequence* sequences, DebugInfos* debugInfos, Map* map, Characters* characters)
{
    /** Run the sequence and update the debug infos depending on the debug mode */

    int i;
    static clock_t previousTime = 0;
    clock_t timeElapsedSincePreviousAction = clock() - previousTime;

    if(debugInfos->newStep || debugInfos->firstStep) // if it's the first action from the sequence, no need to update the debug infos beforehand
    {
        debugInfos->newStep = False;
        debugInfos->firstStep = False;

        for(i = 0; i < characters->count; i++)
            executeActionFromSequence(sequences[i].action[debugInfos->currentAction[i]].type, map, characters, i); // execute the next action

        previousTime = clock(); // update the time when the action was executed
    }
    else if(timeElapsedSincePreviousAction > 1000 || debugInfos->status == Step) // if it has been more than 1 second since the previous action or the debug mode is set to Step
    {
        if(prepareDebugInfosForNextStep(debugInfos, sequences, *characters)) // update the debug infos
        {
            debugInfos->status = Stop;
            resetCharacters(characters);
            resetMap(map);
            return; // if the sequence is completed, the debugging has been reset and we exit the function
        }

        for(i = 0; i < characters->count; i++)
            executeActionFromSequence(sequences[i].action[debugInfos->currentAction[i]].type, map, characters, i); // else execute the next action

        previousTime = clock(); // update the time when the action was executed
    }

    if(debugInfos->status == Step) // if the debug mode was set to Step
        debugInfos->status = Pause; // pause the debugging
}

int prepareDebugInfosForNextStep(DebugInfos* debugInfos, Sequence* sequences, Characters characters)
{
    /** Update debug infos for the next action */

    int i;

    for(i = 0; i < characters.count; i++)
    {
        if(sequences[i].count > 0 && debugInfos->currentAction[i] <= sequences[i].count)
        {
            debugInfos->count[i] += 1; // increase the stack tracker

            if(debugInfos->count[i] >= sequences[i].action[debugInfos->currentAction[i]].count) // if it's greater than the stack of the current action
            {
                debugInfos->currentAction[i] += 1; // set the current action to the next one
                debugInfos->count[i] = 0; // reset the stack
            }
            if(debugInfos->currentAction[i] >= sequences[i].count && i == debugInfos->longestSequence) // if the current action is greater than the number of action and it's the longest action
                return 1; // stop the debug mode
        }
    }

    return 0;
}

void executeActionFromSequence(ActionType type, Map* map, Characters* characters, int index)
{
    /** Execute a specific action from the sequence */

    Character* character = &characters->entity[index]; // breathe variable

    switch(type) // switch the type of action
    {
        case MoveForward:
            moveCharacter(character, *map, character->direction);
        break;
        case RotateCharacterLeft:
            character->direction -= 1;

            if(character->direction == -1)
                character->direction = 3;
        break;
        case RotateCharacterRight:
            character->direction += 1;

            if(character->direction > 3)
                character->direction = 0;
        break;
        case Finish:
            if(map->cases[character->x][character->y][character->z-1].type == End)
            {
                map->cases[character->x][character->y][character->z-1].type = Validated;
                character->didWin = True;
            }
        break;
        case RotateCameraRight:
            rotateMap(map, Right);
            rotateCharacters(characters, map, Right);
        break;
        case RotateCameraLeft:
            rotateMap(map, Left);
            rotateCharacters(characters, map, Left);
        break;
        default: break;
    }
}

void clicOnSequenceMenu(Sequence* sequence, DebugInfos* debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos)
{
    /** Update all the informations structures depending on the exact position of the clic in the window */

    int i;

    windowsInfos->actionMenu.drawPriority = 0; // update the drawing priority of the windows
    windowsInfos->sequenceMenu.drawPriority = 1;

    for(i = 0; i < sequence->count; i++) // for every action in the sequence
    {
        if(isMouseOnButton(SequenceMenu, i, *windowsInfos)) // if the mouse is on an action button
        {
            mouseInfos->clicFocus = SequenceMenu; // update the clic focus
            mouseInfos->element = i; // update the selected element

            return;
        }
    }

    for(i = 0; i < DebugButtonCount; i++) // for every debug button
    {
        if(isMouseOnButton(DebugMenu, i, *windowsInfos) && mouse_b & 1) // if the mouse is on the button
        {
            mouseInfos->clicFocus = DebugMenu; // update clic focus and selected element
            mouseInfos->element = i;
        }
    }

    // if the mouse is in the drag zone
    if(mouse_x > windowsInfos->sequenceMenu.offsetX && mouse_x < windowsInfos->sequenceMenu.offsetX + windowsInfos->sequenceMenu.width && mouse_y > windowsInfos->sequenceMenu.offsetY && mouse_y < windowsInfos->sequenceMenu.offsetY + windowsInfos->dragOffset)
        windowsInfos->sequenceMenu.drag = True; // indicate that the user is currently dragging the window
}

void drawSequenceMenu(Sequence sequence,  Characters characters, DebugInfos debugInfos, MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer)
{
    /** Draw the sequence menu: sequence buttons and debug buttons */

    int i, x, y;

    int sequenceMenuOffsetX = windowsInfos.sequenceMenu.offsetX; // set some variables so the code isn't too heavy
    int sequenceMenuOffsetY = windowsInfos.sequenceMenu.offsetY;

    int xOffset = windowsInfos.offsetFromLeft;
    int yOffset = windowsInfos.offsetFromTop;

    int buttonSize = windowsInfos.buttonSize;
    int distanceBetweenButtons = windowsInfos.distanceBetweenButtons;

    // draw the sequence background
    stretch_sprite(buffer, sprites.sequenceBackground, sequenceMenuOffsetX, sequenceMenuOffsetY, windowsInfos.sequenceMenu.width, windowsInfos.sequenceMenu.height);

    for(i = 0; i < sequence.count; i++) // for every action in the sequence
    {
        if(sequence.action[i].type == None) // if the action is empty, exit the loop
            break;

        // if the debug is running and the current action is being executed, draw a green background
        if(debugInfos.status != Stop && debugInfos.currentAction[characters.current] == i)
            stretch_sprite(buffer, sprites.buttonBackground[Debugging], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
        // else if the mouse is on the button and the clic focus correspond to the same button, draw a darker background
        else if(mouseInfos.clicFocus == SequenceMenu && mouseInfos.element == i && debugInfos.status == Stop && mouse_x > sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4) && mouse_x < sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4) + buttonSize && mouse_y > sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4) && mouse_y < sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4) + buttonSize)
            stretch_sprite(buffer, sprites.buttonBackground[Clicked], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
        else // else draw the normal background
            stretch_sprite(buffer, sprites.buttonBackground[Idle], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);

        // draw the action button
        stretch_sprite(buffer, sprites.actionButton[sequence.action[i].type], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);

        if(sequence.action[i].count > 1) // if the stack is greater than 1
        {
            x = sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4) + windowsInfos.buttonSize / 2; // update x and y coordinates to draw the stack
            y = sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4) + windowsInfos.buttonSize * 3/4;

            printCount(buffer, sprites.customFont, sequence.action[i].count, x, y); // print the stack
        }
    }

    yOffset = windowsInfos.offsetFromTopDebug; // get to the bottom of the window

    for(i = 0; i < DebugButtonCount; i++) // for every debug button
    {
        // if the mouse is on the button and the clic focus correspond to the same button, draw a darker background
        if(isMouseOnButton(DebugMenu, i, windowsInfos) && mouseInfos.clicFocus == DebugMenu && mouseInfos.element == i)
            stretch_sprite(buffer, sprites.buttonBackground[Clicked], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
        else // else draw a normal background
            stretch_sprite(buffer, sprites.buttonBackground[Idle], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);

        // under specific conditions, some buttons can't be clicked and have a darker background (example you can't click on the pause button when the debug is already paused)
        if(((debugInfos.status == Pause || debugInfos.status == Step) && i == Pause) || (debugInfos.status == Run && i == Run))
            stretch_sprite(buffer, sprites.buttonBackground[Clicked], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);

        // draw the debug button
        stretch_sprite(buffer, sprites.debugButton[i], sequenceMenuOffsetX + xOffset + distanceBetweenButtons * (i%4), sequenceMenuOffsetY + yOffset + distanceBetweenButtons * floor(i/4), buttonSize, buttonSize);
    }
}

void dragSequenceMenu(WindowsInfos* windowsInfos)
{
    /** Manage the position of the sequence window when the user is dragging it */

    static int startingX, startingY, startingXOffset, startingYOffset;
    static Bool firstClic = True;

    if(mouse_b & 1) // if the user is still dragging it
    {
        if(firstClic) // if it's the first click on the window
        {
            // initialize the starting coordinates
            startingX = mouse_x;
            startingY = mouse_y;
            startingXOffset = windowsInfos->sequenceMenu.offsetX;
            startingYOffset = windowsInfos->sequenceMenu.offsetY;

           firstClic = False; // not the first click anymore
        }

        // draw the window at the new location depending on the current mouse location and the starting coordinates
        windowsInfos->sequenceMenu.offsetX = startingXOffset - (startingX - mouse_x);
        windowsInfos->sequenceMenu.offsetY = startingYOffset - (startingY - mouse_y);
    }
    else // else reset the firstClic property and indicates that the user isn't dragging the window anymore
    {
        firstClic = True;
        windowsInfos->sequenceMenu.drag = False;
    }
}

void debugSequence(Sequence sequence)
{
    int i;

    for(i = 0; i < sequence.count; i++)
    {
        printf("[%d](%d,%d)\n",i,sequence.action[i].type, sequence.action[i].count);
    }
    printf("\n");
}
