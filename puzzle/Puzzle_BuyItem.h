#ifndef __CocoMagic__Puzzle_BuyItem__
#define __CocoMagic__Puzzle_BuyItem__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class Puzzle_BuyItem : public CCLayer
{
public:
    static CCScene* scene(int id);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    void MakeOpenedSkillSprites();
    void MakeClosedSkillSprites();
    
    void SortMySkillByUserId();
    
    std::string SkillDescription();
    
    void EndScene(bool use);
    
    void XmlParsePurchaseInGameItem(xml_document *xmlDoc);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    CREATE_FUNC(Puzzle_BuyItem);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
    int btnStatus;
    bool isOpened;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCSprite* pBlack;
    CCSprite* bar;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
    
    std::vector<MySkill*> sList;
    
    CCSprite* sp;
    CCScale9Sprite* mask;
    CCClippingNode* clip;
    CCNode* node;
};

#endif /* defined(__CocoMagic__Puzzle_BuyItem__) */
