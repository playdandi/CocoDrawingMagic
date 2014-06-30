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
    static CCLayer* MakeCloud(bool isUI = false);
    static CCLayer* MakeFlower();
    static CCLayer* MakeFlowerNew();
    static CCLayer* GetFairy(int id);
    
    static void Anim_Flower_Hide(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    static void Anim_Flower_Padac(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    static void Anim_Flower_MoveFace(CCLayer* layer);
    static void Anim_Flower_MoveEyebrow(CCLayer* layer);
    
    static void Anim_Cloud_Curl(CCLayer* gameLayer, SEL_CallFuncND cf, CCPoint pos);
    
    static void Anim_Sun_Sigh(CCLayer* layer, CCLayer* gameLayer, SEL_CallFuncND cf);
    static void Anim_Sun_MoveEye(CCLayer* layer);
    static void Anim_Sun_RotateTails(CCLayer* layer);
};

#endif /* defined(__CocoMagic__Fairy__) */
