#ifndef __CocoMagic__FairyOneInfo__
#define __CocoMagic__FairyOneInfo__

#include "../Common.h"
#include "../Fairy.h"

class FairyOneInfo : public CCLayer
{
public:
    static CCScene* scene(int cfi);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(FairyOneInfo);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isByTopaz;
    bool isAlreadyBought;
    
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
};

#endif /* defined(__CocoMagic__FairyInfo__) */
