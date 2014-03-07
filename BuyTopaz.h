#ifndef __CocoMagic__BuyTopaz__
#define __CocoMagic__BuyTopaz__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class BuyTopaz : public CCLayer,
                 public CCScrollViewDelegate
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
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    void EndScene();
    
    CREATE_FUNC(BuyTopaz);
    
    void InitSprites();
    void MakeScroll();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollView;
    bool isScrolling;
    
private:
    SpriteClass* spriteClass;
    
    std::vector<std::string> spriteName;
    std::vector<std::string> cost;
};

#endif /* defined(__CocoMagic__BuyTopaz__) */
