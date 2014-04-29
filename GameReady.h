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
    ~GameReady(void);
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
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(GameReady);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    void MakeScroll();
    
    void EndScene();
    //void EndSceneCallback();
    void EndSceneCallback(CCNode* sender, void* data);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    bool isStarting;
    
    bool itemSelected[5];
    int callbackType;
    
private:
    CCSprite* pBlack;
    CCSprite* pBlackClose;
    std::vector<CCLayer*> itemNumLayer;
    
    CCLayer* fairyLayer;
    CCLayer* skillLayer;
    
    CCScrollView* scrollView;
    
    SpriteClass* spriteClass;
};

#endif /* defined(__CocoMagic__GameReady__) */
