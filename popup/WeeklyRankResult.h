#ifndef __CocoMagic__WeeklyRankResult__
#define __CocoMagic__WeeklyRankResult__

#include "Common.h"

class WeeklyRankResult : public CCLayer
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
    
    CREATE_FUNC(WeeklyRankResult);
    
    void InitSprites();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__WeeklyRankResult__) */
