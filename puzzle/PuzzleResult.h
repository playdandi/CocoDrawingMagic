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
    
    void EndLayer();
    void EndLayerCallback();
    
    CCSize winSize;
    
protected:
    CCTexture2D* tBackground;
    CCTexture2D* tColorBG;
    
    CCSprite* pBackground;
    CCSprite* pBlack;
    
    //static int res_cycleCnt;
    //static int res_allCnt[12+1];
};

#endif /* defined(__magician__PuzzleResult__) */
