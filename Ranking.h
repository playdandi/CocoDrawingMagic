#ifndef __CocoMagic__Ranking__
#define __CocoMagic__Ranking__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class Ranking : public CCLayer,
                public CCScrollViewDelegate
{
public:
	bool init();
    static CCScene* scene();
	
    virtual void onEnter();
    virtual void onPause();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    //virtual void registerWithTouchDispatcher();
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void MakeScroll();

    //void touchDownAction(CCObject* sender, CCControlEvent controlEvent);
    
    CREATE_FUNC(Ranking);
    
protected:
    CCSize winSize;
    
    CCScrollView* scrollView;
    CCPoint scrollViewLastPoint;
    std::vector<CCSprite*> profiles;
    
    //std::vector<std::string> usernames;
    //std::vector<int> userPotionStates;
    
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    bool isOnceScrollViewTouched;
    
    bool isKeyBackClicked;
    
private:
    
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__Ranking__) */



