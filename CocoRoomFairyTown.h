#ifndef __CocoMagic__CocoRoomFairyTown__
#define __CocoMagic__CocoRoomFairyTown__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoomFairyTown : public CCLayer,
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
    
    CREATE_FUNC(CocoRoomFairyTown);
    
    void InitSprites();
    void MakeScroll();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    CCScrollView* scrollView;
    bool isScrolling;
    
private:
    SpriteClass* spriteClass;
    
    std::vector<int> grade;
    std::vector<std::string> name;
};

#endif /* defined(__CocoMagic__CocoRoomFairyTown__) */
