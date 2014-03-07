#ifndef __CocoMagic__Profile__
#define __CocoMagic__Profile__

#include "Common.h"

class Profile : public CCLayer
{
public:
    static CCScene* scene();
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    void EndScene();
    
    CREATE_FUNC(Profile);
    
    void InitSprites();
    
protected:
    CCSize winSize;
    bool isTouched;
    
private:
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__Profile__) */
