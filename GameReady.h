#ifndef __CocoMagic__GameReady__
#define __CocoMagic__GameReady__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class GameReady : public CCLayer,
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
    
    CREATE_FUNC(GameReady);
    
    void InitSprites();
    void MakeScroll();
    
    void ShowMagicSlot();
    
    void EndLayer();
    void EndLayerCallback();
    
protected:
    CCSize winSize;
    
    std::vector<CCSprite*> sprites;
    std::vector<CCPoint> pos;
    std::vector<std::string> names;
    
    std::vector<CCScale9Sprite*> sprites9;
    std::vector<std::string> names9;
    std::vector<CCPoint> pos9;
    std::vector<CCPoint> size9;
    
    CCScrollView* scrollView;
    
private:
    bool itemSelected[5];
    
    bool isTouched;
    bool isScrolling;
};

#endif /* defined(__CocoMagic__GameReady__) */
