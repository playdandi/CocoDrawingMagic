#ifndef __CocoMagic__Setting__
#define __CocoMagic__Setting__

#include "Common.h"

class Setting : public CCLayer
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
    
    CREATE_FUNC(Setting);
    
    void InitSprites();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    bool btn[5];
    
private:
    SpriteClass* spriteClass;

};

#endif /* defined(__CocoMagic__Setting__) */
