#ifndef __CocoMagic__MagicList__
#define __CocoMagic__MagicList__

#include "Common.h"

class MagicList : public CCLayer
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
    
    CREATE_FUNC(MagicList);
    
    void InitSprites();
    
    void EndLayer();
    void EndLayerCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollView;
    bool isScrolling;
    
private:
    SpriteClass* spriteClass;
    CCLayer* layer;
};


#endif /* defined(__CocoMagic__MagicList__) */
