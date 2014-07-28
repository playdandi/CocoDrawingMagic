#ifndef __poops8__PuzzleP4__
#define __poops8__PuzzleP4__

#include "Common.h"

#define BLOCKED 1
#define CONNECTED 2
#define ITEM 3


class PuzzleP4
{
public:
    static PuzzleP4* CreateP4(void* parent, int zOrder, int type = -100, int designatedType = -1);
    //void CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos, float half_width);
    void CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint pos, float half_width, int prob, int decidedType = -1);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    void InitChild();
    CCSprite* GetLeftUp();
    CCSprite* GetRightUp();
    CCSprite* GetLeftDown();
    CCSprite* GetRightDown();
    void SetRemoved(bool flag);
    bool IsRemoved();
    
private:
    CCSprite* leftup;
    CCSprite* leftdown;
    CCSprite* rightup;
    CCSprite* rightdown;
    int type;
    void* parent;
    int zOrder;
    bool isRemoved;
};

#endif
