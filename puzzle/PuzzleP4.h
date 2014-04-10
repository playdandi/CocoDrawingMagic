#ifndef __poops8__PuzzleP4__
#define __poops8__PuzzleP4__

#include "Common.h"

#define BLOCKED 1
#define CONNECTED 2
#define ITEM 3


class PuzzleP4
{
public:
    static PuzzleP4* CreateP4(void* parent, int zOrder, int type = -100);
    void CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    void InitChild();
    CCSprite* GetLeftUp();
    CCSprite* GetRightUp();
    CCSprite* GetLeftDown();
    CCSprite* GetRightDown();
    //CCSprite* GetPiece();
    //void SetPosition(CCPoint pos);
    
private:
    CCSprite* leftup;
    CCSprite* leftdown;
    CCSprite* rightup;
    CCSprite* rightdown;
    int type;
    void* parent;
    int zOrder;
};

#endif
