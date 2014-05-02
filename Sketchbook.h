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
    static CCScene* scene(int tab, int fromWhere);
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
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll(int state, bool isFromPopup = false);
    void MakeScrollFire();
    void MakeScrollWater();
    void MakeScrollLand();
    void MakeScrollMaster();
    void MakeScrollSlot();
    
    void SetMenuChange(int state);
    
    SkillInfo* GetNextSkillInfo(int state);
    
    void EndScene();
    void EndSceneCallback();
    
    void XmlParsePracticeSkill(char* data, int size);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    int FromWhere();
    
protected:
    CCSize winSize;
    bool isTouched;
     
    int curState;
    
    bool isScrolling;
    bool isSlotTouched;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassFire;
    SpriteClass* spriteClassWater;
    SpriteClass* spriteClassLand;
    SpriteClass* spriteClassMaster;
    SpriteClass* spriteClassSlot;
    
    CCScrollView* scrollViewFire;
    CCScrollView* scrollViewWater;
    CCScrollView* scrollViewLand;
    CCScrollView* scrollViewMaster;
    CCScrollView* scrollViewSlot;
    
    CCLayer* containerFire;
    CCLayer* containerWater;
    CCLayer* containerLand;
    CCLayer* containerMaster;
    CCLayer* containerSlot;
    
    CCLayer* fire;
    CCLayer* water;
    CCLayer* land;
    CCLayer* master;
};

#endif /* defined(__CocoMagic__Sketchbook__) */
