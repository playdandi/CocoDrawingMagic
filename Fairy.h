#ifndef __CocoMagic__Fairy__
#define __CocoMagic__Fairy__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;

class Fairy
{
public:
    static CCLayer* MakeEmpty();
    static CCLayer* MakeSun();
    static CCLayer* MakeFlower();
    static CCLayer* GetFairy(int id);
};

#endif /* defined(__CocoMagic__Fairy__) */
