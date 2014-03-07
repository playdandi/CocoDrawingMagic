#ifndef __CocoMagic__Message__
#define __CocoMagic__Message__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class Message : public CCLayer,
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
    
    CREATE_FUNC(Message);
    
    void InitSprites();
    void MakeScroll();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollView;
    bool isScrolling;
    
private:
    SpriteClass* spriteClass;
    
    std::vector<std::string> msg_content;
    std::vector<int> msg_type;
};

#endif /* defined(__CocoMagic__Message__) */
