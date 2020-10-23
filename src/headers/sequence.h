#ifndef SEQUENCE_H
#define SEQUENCE_H

void initDebugInfos(DebugInfos* debugInfos, Sequence* sequences, Characters characters);
void initSequences(Sequence* sequences, Characters characters);
void initSequence(Sequence* sequence);

int getLongestSequence(Sequence* sequences, Characters characters);
Bool areAllSequencesEmpty(Sequence* sequences, Characters characters);

void addActionToSequence(Sequence* sequence, ActionType type, int index);
void deleteActionFromSequence(Sequence* sequence, int index);

void executeAllSequences(Sequence* sequences, DebugInfos* debugInfos, Map* map, Characters* characters);
void executeActionFromSequence(ActionType type, Map* map, Characters* characters, int index);
int prepareDebugInfosForNextStep(DebugInfos* debugInfos, Sequence* sequences, Characters characters);

void clicOnSequenceMenu(Sequence* sequence, DebugInfos* debugInfos, MouseInfos* mouseInfos, WindowsInfos* windowsInfos);
void drawSequenceMenu(Sequence sequence, Characters characters, DebugInfos debugInfos, MouseInfos mouseInfos, WindowsInfos windowsInfos, SpriteCollection sprites, BITMAP* buffer);
void dragSequenceMenu(WindowsInfos* windowsInfos);

void debugSequence(Sequence sequence);

#endif // SEQUENCE_H
