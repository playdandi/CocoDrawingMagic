#ifndef __CocoMagic__PuzzleAnim__
#define __CocoMagic__PuzzleAnim__

#include "Common.h"
#include "Puzzle.h"


class PuzzleAnim
{
public:
    void Init(PuzzleAnim* pAnim, Puzzle* layer);
    CCLayer* InitFairy(Puzzle* puzzleLayer);
    void StartAnim(CCLayer* fairyLayer, int fairyTimer);
    
private:
    PuzzleAnim* thisLayer;
    Puzzle* puzzleLayer;
    float width;
    float timerH;
    float boardH;
    float voy;
    float mcW;
};

#endif /* defined(__CocoMagic__PuzzleAnim__) */
