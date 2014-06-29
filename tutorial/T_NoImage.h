#ifndef __CocoMagic__T_NoImage__
#define __CocoMagic__T_NoImage__

#include "Common.h"

class T_NoImage : public CCLayer
{
public:
    static CCScene* scene(int popupType, int btnType, std::vector<int> data, int etc);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(T_NoImage);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    
    void EndScene();

protected:
    CCSize winSize;
    bool isTouched;
    bool isEnded;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCSprite* ttrPos;
    CCSprite* ttrArrow;
};

#endif /* defined(__CocoMagic__T_NoImage__) */
