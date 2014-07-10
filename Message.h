#ifndef __CocoMagic__Message__
#define __CocoMagic__Message__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
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
    void SceneCallback();
    
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
    void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    
    void XmlParseMsg(xml_document *xmlDoc);
    void XmlParseMsgReceiveOne(xml_document *xmlDoc);
    void ParseProfileImage(char* data, int size, int idx);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
    int newProfileCnt; 
    int httpStatus;
    int httpMsgIdx;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCLayer* tLayer;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    SpriteClass* spriteClassScroll;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
};

#endif /* defined(__CocoMagic__Message__) */
