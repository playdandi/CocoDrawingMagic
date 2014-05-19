#ifndef __CocoMagic__CocoRoomFairyTown__
#define __CocoMagic__CocoRoomFairyTown__

#include "Common.h"
#include "Fairy.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoomFairyTown : public CCLayer,
                        public CCScrollViewDelegate
{
public:
    ~CocoRoomFairyTown();
    static CCScene* scene();
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
    
    CREATE_FUNC(CocoRoomFairyTown);
    
    void InitSprites();
    void MakeScroll();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool scrollViewTouch;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
};

#endif /* defined(__CocoMagic__CocoRoomFairyTown__) */
