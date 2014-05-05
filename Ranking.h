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
    static CCScene* scene(int from = -1);
	
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
    
    void EndScene();
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void InitProperties();
    void MakeScroll();
    
    void PotionTimer(float f);
    
    CREATE_FUNC(Ranking);
    
protected:
    CCSize winSize;
    
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    bool isOnceScrollViewTouched;
    
    bool isKeyBackClicked;
    
private:
    CCSprite* pBackground;
    CCSprite* pBlack;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassProperty;
    
    std::vector<CCLayer*> profileLayers;
    CCScrollView* scrollView;
    //CCPoint scrollViewLastPoint;
    //std::vector<CCSprite*> profiles;
};

#endif /* defined(__CocoMagic__Ranking__) */



