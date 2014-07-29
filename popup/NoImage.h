#ifndef __CocoMagic__NoImage__
#define __CocoMagic__NoImage__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class NoImage : public CCLayer
{
public:
    static CCScene* scene(int popupType, int btnType, std::vector<int> data, int etc, std::vector<std::string> sdata);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(NoImage);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void onLogoutComplete();
    void onLogoutErrorComplete(char const* status, char const* error);
    void onUnregisterComplete();
    void onUnregisterErrorComplete(char const* status, char const* error);
    void onAuthComplete(bool result);
    void onAuthErrorComplete(char const* status, char const* error);
    void onSendLinkMessageComplete();
    void onSendLinkMessageErrorComplete(char const *status, char const *error);

    
    void ReplaceScene(std::string to, int type, int btnType);
    void EndScene();
    void EndSceneCallback();
    void Exit();
    
    void HttpRequest(std::string url, std::string param);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    void XmlParseBuyTopaz(xml_document *xmlDoc);
    void XmlParseBuyStarCandy(xml_document *xmlDoc);
    void XmlParseBuyPotion(xml_document *xmlDoc);
    void XmlParseSendPotion(xml_document *xmlDoc);
    void XmlParseMsg(xml_document *xmlDoc);
    void XmlParseSendTopaz(xml_document *xmlDoc);
    void XmlParseUpgradeStaff(xml_document *xmlDoc);
    void XmlParseUpgradeFairy(xml_document *xmlDoc);
    void XmlParseBuyFairy(xml_document *xmlDoc);
    void XmlParseUsingFairy(xml_document *xmlDoc);
    void XmlParseBuySkillSlot(xml_document *xmlDoc);
    void XmlParseBuySkillProperty(xml_document *xmlDoc);
    void XmlParseGetFirstSkill(xml_document *xmlDoc);
    void XmlParseKakaoUnregister(xml_document *xmlDoc);
    void XmlParseGetPotionStatus(xml_document *xmlDoc);
    void XmlParsePurchaseSkill(xml_document *xmlDoc);
    
    void ShowHintOfUpgrade();
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isTouchDone;
    bool isEnded;
    
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCLayer* tLayer;
    
    CCRect rect;
    int kind;
    int idx;
    
    CCScale9Sprite* balloon;
    CCLabelTTF* ball;
};

#endif /* defined(__CocoMagic__NoImage__) */
