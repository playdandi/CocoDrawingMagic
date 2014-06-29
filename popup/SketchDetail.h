#ifndef __CocoMagic__SketchDetail__
#define __CocoMagic__SketchDetail__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class SketchDetail : public CCLayer
{
public:
    ~SketchDetail(void);
    static CCScene* scene(int id, int prio);
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
    
    void MakeOpenedSkillSprites();
    void MakeClosedSkillSprites();
    
    std::string SkillDescription(int scid);
    
    void EndScene(bool isNoti);
    
    void XmlParseUpgradeOrPurchaseSkill(xml_document *xmlDoc, int tag);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    CREATE_FUNC(SketchDetail);
    
protected:
    CCSize winSize;
    bool isTouched;
    
    int btnStatus;
    
private:
    CCSprite* pBlack;
    CCSprite* bar;
    SpriteClass* spriteClass;
    
    CCSprite* ttrPos;
    CCSprite* ttrArrow;
};


#endif /* defined(__CocoMagic__SketchDetail__) */
