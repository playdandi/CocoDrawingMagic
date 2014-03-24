#ifndef __CocoMagic__Sketchbook__
#define __CocoMagic__Sketchbook__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class Sketchbook : public CCLayer,
                   public CCScrollViewDelegate
{
public:
    static CCScene* scene(int tab);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    CREATE_FUNC(Sketchbook);
    
    void InitSprites();
    void MakeScroll(int state);
    void MakeScrollFire();
    void MakeScrollWater();
    void MakeScrollLand();
    void MakeScrollMaster();
    void MakeScrollSlot();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollViewFire;
    CCScrollView* scrollViewWater;
    CCScrollView* scrollViewLand;
    CCScrollView* scrollViewMaster;
    CCScrollView* scrollViewSlot;
    bool isScrolling;
    bool isScrollingSlot;
    CCLayer* containerFire;
    CCLayer* containerWater;
    CCLayer* containerLand;
    CCLayer* containerMaster;
    CCLayer* containerSlot;
    bool isScrollViewTouched;
    
private:
    SpriteClass* spriteClass;
    SpriteClass* spriteClassFire;
    SpriteClass* spriteClassWater;
    SpriteClass* spriteClassLand;
    SpriteClass* spriteClassMaster;
    
    int curState;
    CCLayer* fire;
    CCLayer* water;
    CCLayer* land;
    CCLayer* master;
    
    std::vector<int> type;
};

#endif /* defined(__CocoMagic__Sketchbook__) */
