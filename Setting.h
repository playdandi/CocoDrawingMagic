#ifndef __CocoMagic__Setting__
#define __CocoMagic__Setting__

#include "Common.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

class Setting : public CCLayer
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
    
    CREATE_FUNC(Setting);
    
    void InitSprites();
    
    void Notification(CCObject* obj);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseResult(xml_document *xmlDoc);
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
    int selectedBtn;
    CCPoint selectedPos;
    CCPoint selectedTouchPos;
    CCPoint standardBtnPos;
    
    bool kakaoMsgReserved;
    bool pushNotiReserved;
    bool potionMsgReserved;
    
    CCSprite* m_pLogoutBtn;
    
    CCSprite* cafe;
    
    int code;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCLayer* tLayer;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCSprite* selectedSprite;
};

#endif /* defined(__CocoMagic__Setting__) */
