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
    
    void RenewAllTime();
    void Notification(CCObject* obj);
    
    void ShowPopup();
    
    void InitSprites();
    void InitProperties();
    void MakeScroll();
    
    void PotionTimer(float f);
    void ProfileTimer(float f);
    
    void ShowHintOfMP();
    
    CCScrollView* GetScrollView();
    
    void onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data);
    
    void EndScene();
    
    CREATE_FUNC(Ranking);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    bool isOnceScrollViewTouched;
    bool isHintOfMPShown;
    
    int popupStatus;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCSprite* pBackground;
    CCSprite* pBlack;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassProperty;
    
    CCScrollView* scrollView;
    
    CCScale9Sprite* balloon;
    CCLabelTTF* ball;
};

#endif /* defined(__CocoMagic__Ranking__) */



