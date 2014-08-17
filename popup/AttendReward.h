#ifndef __CocoMagic__AttendReward__
#define __CocoMagic__AttendReward__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class AttendReward : public CCLayer
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
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();
    
    CREATE_FUNC(AttendReward);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
    
    CCRect rect;
    int kind;
    int idx;
    
    CCScale9Sprite* sp;
    CCSprite* mask;
    CCClippingNode* clip;
    CCNode* node;
};


#endif /* defined(__CocoMagic__AttendReward__) */
