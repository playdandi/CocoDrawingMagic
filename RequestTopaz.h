#ifndef __CocoMagic__RequestTopaz__
#define __CocoMagic__RequestTopaz__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class RequestTopaz : public CCLayer,
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
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();
    void RenewScroll();
    
    void EndScene();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseResult(char* data, int size, int friendKakaoId);
    
    CREATE_FUNC(RequestTopaz);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    SpriteClass* spriteClassScroll;
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
};

#endif /* defined(__CocoMagic__RequestTopaz__) */
