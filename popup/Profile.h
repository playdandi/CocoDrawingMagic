#ifndef __CocoMagic__Profile__
#define __CocoMagic__Profile__

#include "Common.h"
#include "../Fairy.h"

class Profile : public CCLayer
{
public:
    ~Profile(void);
    static CCScene* scene(int idx);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    void EndScene();
    
    CREATE_FUNC(Profile);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    CCLayer* tLayer;
};

#endif /* defined(__CocoMagic__Profile__) */
