#ifndef __CocoMagic__RankUp__
#define __CocoMagic__RankUp__

#include "Common.h"

class RankUp : public CCLayer
{
public:
    static CCScene* scene();
	bool init();
	
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    void InitSprites();

    void Callback(CCNode* sender, void* p);
    
    void EndScene();
    void EndSceneCallback(CCNode* sender, void* pointer);
    
    CREATE_FUNC(RankUp);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCSprite* pBackground;
    CCSprite* pBlackClose;
    CCSprite* pBlackOpen;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__RankUp__) */
