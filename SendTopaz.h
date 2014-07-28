#ifndef __CocoMagic__SendTopaz__
#define __CocoMagic__SendTopaz__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class SendTopaz : public CCLayer,
public CCScrollViewDelegate
{
public:
    static CCScene* scene(int topaz_id);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    CREATE_FUNC(SendTopaz);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();
    
    void EndScene();
    
    void ProfileTimer(float f);
    void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    CCLayer* tLayer;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
};

#endif /* defined(__CocoMagic__SendTopaz__) */
