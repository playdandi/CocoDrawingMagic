#ifndef __CocoMagic__SelectProperty__
#define __CocoMagic__SelectProperty__

#include "Common.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

class SelectProperty : public CCLayer
{
public:
    static CCScene* scene(int isGameStart);
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
    
    void EndScene(bool isInGameTtrStart);
    
    CREATE_FUNC(SelectProperty);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;

private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
};


#endif /* defined(__CocoMagic__SelectProperty__) */
