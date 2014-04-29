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
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(Profile);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
private:
    CCSprite* pBlack;
    CCLayer* scoreLayer;
    CCLayer* profileLayer;
    CCLayer* skillLayer;
    CCLayer* fairyLayer;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__Profile__) */
