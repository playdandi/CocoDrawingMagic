#ifndef __CocoMagic__PuzzlePause__
#define __CocoMagic__PuzzlePause__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class PuzzlePause : public CCLayer
{
public:
    static CCScene* scene(int h);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(PuzzlePause);
    
    void InitSprites();
    void InitSprites2();
    
    void ResumeGame();
    void EndGame();
    void EndScene();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseSessionCheck(xml_document *xmlDoc);
    
protected:
    CCSize m_winSize;
    bool isKeybackTouched;
    
private:
    CCSprite* pBlack;    
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__PuzzlePause__) */
