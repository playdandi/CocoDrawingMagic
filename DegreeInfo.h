#ifndef __CocoMagic__DegreeInfo__
#define __CocoMagic__DegreeInfo__

#include "Common.h"

class DegreeInfo : public CCLayer
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
    
    CREATE_FUNC(DegreeInfo);
    
    void InitSprites();
    
protected:
    CCSize winSize;

    CCSprite* pBlack;
    
    bool isTouched;
    
    CCLayer* l;
    
private:
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__DegreeInfo__) */
