#ifndef __CocoMagic__FairyOneInfo__
#define __CocoMagic__FairyOneInfo__

#include "Common.h"

class FairyOneInfo : public CCLayer
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
    
    CREATE_FUNC(FairyOneInfo);
    
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
