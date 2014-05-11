#ifndef __CocoMagic__InviteFriend__
#define __CocoMagic__InviteFriend__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

#define ADDED 0
#define NOTADDED 1

class InviteFriend : public CCLayer,
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
    
    CREATE_FUNC(InviteFriend);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    
    CCScrollView* scrollView;
    
    SpriteClass* spriteClass;
    
    std::vector<std::string> name;
    std::vector<int> state;
};

#endif /* defined(__CocoMagic__InviteFriend__) */
