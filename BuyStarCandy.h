#ifndef __CocoMagic__BuyStarCandy__
#define __CocoMagic__BuyStarCandy__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class BuyStarCandy : public CCLayer
{
public:
    static CCScene* scene(int parent);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    //virtual void scrollViewDidScroll(CCScrollView* view);
    //virtual void scrollViewDidZoom(CCScrollView* view);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(BuyStarCandy);
    
    void InitSprites();
    void MakeScroll();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    //CCScrollView* scrollView;
    //bool isScrolling;
    
private:
    SpriteClass* spriteClass;
    
    std::vector<std::string> spriteName;
    std::vector<std::string> cost;
};

#endif /* defined(__CocoMagic__BuyStarCandy__) */
