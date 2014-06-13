#ifndef __CocoMagic__BuyTopaz__
#define __CocoMagic__BuyTopaz__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class BuyTopaz : public CCLayer
{
public:
    static CCScene* scene(int parent);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(BuyTopaz);
    
    void InitSprites();
    void MakeScroll();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseDeveloperPayload(char* data, int size, int priceTopazIdx);
    
    void verifyPayloadAndProvideItem(const char* data, const char* signature, int topaz_id);
    void XmlParseVerifyPurchaseResult(char* data, int size);
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isTouched;
    int httpStatus;
    
    BuyTopaz* haha;
    
private: // 해제해야 하는 변수들은 여기에 넣자.
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCLayer* itemContainer;
};

#endif /* defined(__CocoMagic__BuyTopaz__) */
