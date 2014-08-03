#ifndef __CocoMagic__T_Skip__
#define __CocoMagic__T_Skip__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class T_Skip : public CCLayer
{
public:
    static CCScene* scene(int h, int from);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(T_Skip);
    
    void InitSprites();
    
    void ResumeGame();
    void EndGame();
    void EndScene();
    
protected:
    CCSize m_winSize;
    bool isKeybackTouched;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__T_Skip__) */
