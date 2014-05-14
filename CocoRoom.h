#ifndef __CocoMagic__CocoRoom__
#define __CocoMagic__CocoRoom__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Fairy.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoom : public CCLayer,
                 public CCScrollViewDelegate
{
public:
    static CCScene* scene(int tab, int prio);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(CocoRoom);
    
    void EndScene();
    void EndSceneCallback();
    
    void InitSprites();
    void MakeSprites(int state);
    
    void MakeSpritesCoco();
    void SetStaffBuildUp();
    void MakeScrollCoco();
    
    void MakeSpritesFairy();
    void SetFairyBuildUp();
    void MakeScrollFairy();
    
    void MakeSpritesCandy();
    void SetTodayCandyList();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseFairyList(char* data, int size);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isSlotTouched;
    
private:
    CCParticleSystem* par;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassCoco;
    SpriteClass* spriteClassFairy;
    SpriteClass* spriteClassCandy;
    
    SpriteClass* spriteClassFairySlot;
    SpriteClass* spriteClassCocoSlot;
    SpriteClass* spriteClassCandyList;
    
    CCScrollView* scrollView;
    CCLayer* containerCoco;
    CCLayer* containerFairy;
    
    int curState;
    CCLayer* coco;
    CCLayer* fairy;
    CCLayer* candy;
};

#endif /* defined(__CocoMagic__CocoRoom__) */
