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
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(DegreeInfo);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    
private:
    SpriteClass* spriteClass;
    CCLayer* tLayer;
};

#endif /* defined(__CocoMagic__DegreeInfo__) */
