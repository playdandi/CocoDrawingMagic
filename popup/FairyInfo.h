#ifndef __CocoMagic__FairyInfo__
#define __CocoMagic__FairyInfo__

#include "Common.h"

class FairyInfo : public CCLayer
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
    
    CREATE_FUNC(FairyInfo);
    
    void InitSprites();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    
    CCSprite* pBlack;
    
    bool isTouched;
    
    
private:
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__FairyInfo__) */
