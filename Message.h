#ifndef __CocoMagic__Message__
#define __CocoMagic__Message__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class Message : public CCLayer,
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
    
    CREATE_FUNC(Message);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();
    void RenewScroll();
    
    void EndScene();
    void EndSceneCallback();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseMsg(char* data, int size);
    void XmlParseMsgReceiveOne(char* data, int size);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
    int httpStatus;
    int httpMsgIdx;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    SpriteClass* spriteClassScroll;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
};

#endif /* defined(__CocoMagic__Message__) */
