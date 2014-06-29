#ifndef __poops8__T_PuzzleP4__
#define __poops8__T_PuzzleP4__

#include "Common.h"

#define BLOCKED 1
#define CONNECTED 2
#define ITEM 3

class T_PuzzleP4
{
public:
    static T_PuzzleP4* CreateP4(void* parent, int zOrder, int type = -100, int designatedType = -1);
    void CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos, float half_width);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    void InitChild();
    CCSprite* GetLeftUp();
    CCSprite* GetRightUp();
    CCSprite* GetLeftDown();
    CCSprite* GetRightDown();
    
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
