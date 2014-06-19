#ifndef __CocoMagic__NoImage__
#define __CocoMagic__NoImage__

#include "Common.h"

class NoImage : public CCLayer
{
public:
    static CCScene* scene(int popupType, int btnType, std::vector<int> data, int etc);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(NoImage);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void ReplaceScene(std::string to, int type, int btnType);
    void EndScene();
    void EndSceneCallback();
    void Exit();
    
    void HttpRequest(std::string url, std::string param);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    void XmlParseBuyTopaz(char* data, int size);
    void XmlParseBuyStarCandy(char* data, int size);
    void XmlParseBuyPotion(char* data, int size);
    void XmlParseSendPotion(char* data, int size);
    void XmlParseMsg(char* data, int size);
    void XmlParseSendTopaz(char* data, int size);
    void XmlParseUpgradeStaff(char* data, int size);
    void XmlParseUpgradeFairy(char* data, int size);
    void XmlParseBuyFairy(char* data, int size);
    void XmlParseUsingFairy(char* data, int size);
    void XmlParseBuySkillSlot(char* data, int size);
    void XmlParseBuySkillProperty(char* data, int size);
    void XmlParseGetFirstSkill(char* data, int size);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isTouchDone;
    bool isEnded;
    bool isKeybackTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__NoImage__) */
