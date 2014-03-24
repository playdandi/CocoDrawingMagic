#ifndef __CocoMagic__CocoRoom__
#define __CocoMagic__CocoRoom__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoom : public CCLayer,
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
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(CocoRoom);
    
    void EndScene();
    void EndSceneCallback();
    
    void InitSprites();
    void MakeSprites(int state);
    void MakeSpritesCoco();
    void MakeScrollCoco();
    void MakeSpritesFairy();
    void MakeScrollFairy();
    void MakeSpritesCandy();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollViewCoco;
    CCScrollView* scrollViewFairy;
    bool isScrolling;
    CCLayer* containerCoco;
    CCLayer* containerFairy;
    
private:
    SpriteClass* spriteClass;
    SpriteClass* spriteClassCoco;
    SpriteClass* spriteClassFairy;
    SpriteClass* spriteClassCandy;
    
    int curState;
    CCLayer* coco;
    CCLayer* fairy;
    CCLayer* candy;
};

#endif /* defined(__CocoMagic__CocoRoom__) */
