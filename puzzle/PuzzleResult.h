#ifndef __magician__PuzzleResult__
#define __magician__PuzzleResult__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class PuzzleResult : public CCLayer
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
    
    CREATE_FUNC(PuzzleResult);
    
    void InitSprites();
    
    void EndScene();
    void EndSceneCallback(CCNode* sender, void* pointer);
    
    CCSize m_winSize;
    
protected:
    CCSprite* pBlack;
    CCSprite* pBlackClose;
    
    SpriteClass* spriteClass;
};

#endif /* defined(__magician__PuzzleResult__) */
