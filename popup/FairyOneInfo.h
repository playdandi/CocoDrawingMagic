#ifndef __CocoMagic__FairyOneInfo__
#define __CocoMagic__FairyOneInfo__

#include "../Common.h"
#include "../Fairy.h"

class FairyOneInfo : public CCLayer
{
public:
    static CCScene* scene(int idx, int prio);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(FairyOneInfo);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isByTopaz;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__FairyInfo__) */
