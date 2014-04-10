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
    
    void InitSprites();
    void MakeScroll();
    
    void EndScene();
    void EndSceneCallback();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseMsg(char* data, int size);
    
    void ReleaseAll();
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    std::vector<CCLayer*> layer;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
    
    std::vector<Msg*> msgData;
};

#endif /* defined(__CocoMagic__Message__) */
