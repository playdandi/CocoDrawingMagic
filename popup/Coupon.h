#ifndef __CocoMagic__Coupon__
#define __CocoMagic__Coupon__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class Coupon : public CCLayer,
                public CCIMEDelegate,
                public CCTextFieldDelegate
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
    
    //virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo &info);
    //virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info);

    virtual bool onTextFieldAttachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldDetachWithIME(CCTextFieldTTF * pSender);
    virtual bool onTextFieldInsertText(CCTextFieldTTF * pSender, const char * text, int nLen);
    virtual bool onTextFieldDeleteBackward(CCTextFieldTTF * pSender, const char * delText, int nLen);
    virtual bool onDraw(CCTextFieldTTF* pSender);

    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseCouponResult(xml_document *xmlDoc);
    
    CREATE_FUNC(Coupon);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    int label_pos;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
    
    CCTextFieldTTF* m_pCouponField;
};

#endif /* defined(__CocoMagic__Coupon__) */
