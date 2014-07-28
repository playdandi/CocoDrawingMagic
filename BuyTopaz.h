#ifndef __CocoMagic__BuyTopaz__
#define __CocoMagic__BuyTopaz__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
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
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(BuyTopaz);
    
    void InitSprites();
    void MakeScroll();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseDeveloperPayload(xml_document *xmlDoc, int priceTopazIdx, std::string friendKakaoId);

    void SetErrorFlag(bool flag);
    
    void EndScene();
    
    void onSendLinkMessageComplete();
    void onSendLinkMessageErrorComplete(char const *status, char const *error);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    int httpStatus;
    
    bool isTryingPurchase;
    
private:
    CCLayer* tLayer;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCLayer* itemContainer;
};

#endif /* defined(__CocoMagic__BuyTopaz__) */
