#ifndef __CocoMagic__SketchDetail__
#define __CocoMagic__SketchDetail__

#include "Common.h"

class SketchDetail : public CCLayer
{
public:
    ~SketchDetail(void);
    static CCScene* scene(int id);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(SketchDetail);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    int btnStatus;
    
private:
    CCSprite* pBlack;
    CCLayer* scoreLayer;
    CCLayer* profileLayer;
    CCLayer* skillLayer;
    CCLayer* fairyLayer;
    SpriteClass* spriteClass;
};


#endif /* defined(__CocoMagic__SketchDetail__) */
