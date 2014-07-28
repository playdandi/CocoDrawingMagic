#ifndef __CocoMagic__T_SketchDetail__
#define __CocoMagic__T_SketchDetail__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class T_SketchDetail : public CCLayer
{
public:
    static CCScene* scene(int id);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    //void MakeOpenedSkillSprites();
    void MakeClosedSkillSprites();
    
    //std::string SkillDescription(int scid);
    
    void EndScene(bool isNoti);
    
    void XmlParseUpgradeOrPurchaseSkill(xml_document *xmlDoc, int tag);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    CREATE_FUNC(T_SketchDetail);
    
protected:
    CCSize winSize;
    bool isTouched;
    
    int btnStatus;
    
private:
    CCSprite* pBlack;
    CCSprite* bar;
    SpriteClass* spriteClass;
    
    CCScale9Sprite* ttrPos;
    CCSprite* ttrArrow;
};

#endif /* defined(__CocoMagic__T_SketchDetail__) */
