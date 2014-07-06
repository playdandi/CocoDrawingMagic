#ifndef __CocoMagic__CocoRoomTodayCandy__
#define __CocoMagic__CocoRoomTodayCandy__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoomTodayCandy : public CCLayer,
                            public CCScrollViewDelegate
{
public:
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

    CREATE_FUNC(CocoRoomTodayCandy);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();
    
    void SetSelectMode(int idx);
    void RefreshTodayCandyList(int idx);
    void RefreshProfileList();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    SpriteClass* spriteClassList;
    CCLayer* scrollContainer;
    CCScrollView* scrollView;
    
    std::vector<bool> selected;
};

#endif /* defined(__CocoMagic__CocoRoomTodayCandy__) */
