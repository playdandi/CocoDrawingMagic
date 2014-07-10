#ifndef __CocoMagic__GetDegree__
#define __CocoMagic__GetDegree__

#include "Common.h"

class GetDegree : public CCLayer
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
    
    CREATE_FUNC(GetDegree);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void SoundCallback(CCNode* sender, void *p);
    void SpriteCallback(CCNode* sender, void* p);
    void DegreeCallback(CCNode* sender, void* p);
    void InitDegree();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isLoadingDone;
    int callbackCnt;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCSprite* pBackground;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__GetDegree__) */
