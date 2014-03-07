#ifndef __CocoMagic__Ranking__
#define __CocoMagic__Ranking__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

#define POTION_REMAIN 0
#define POTION_SEND 1
#define POTION_X 2
#define POTION_NOTHING 3

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
    
    void AddSprites();
    void MakeScroll();

    // popup related func.
    void ShowBuyTopaz();
    void ShowGameReady();
    
    void touchDownAction(CCObject* sender, CCControlEvent controlEvent);
    
    CREATE_FUNC(Ranking);
    
protected:
    CCSize winSize;
    
    CCScrollView* scrollView;
    std::vector<CCSprite*> profiles;
    
    std::vector<std::string> usernames;
    std::vector<int> userPotionStates;
    
    //Sound* sound;
    
    bool isScrolling;
    
private:
    std::vector<CCSprite*> sprites;
    std::vector<CCPoint> pos;
    std::vector<std::string> names;
    
    std::vector<CCScale9Sprite*> sprites9;
    std::vector<std::string> names9;
    std::vector<CCPoint> pos9;
    std::vector<CCPoint> size9;
};

#endif /* defined(__CocoMagic__Ranking__) */



