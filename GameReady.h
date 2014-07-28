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
    void SceneCallback();
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(GameReady);
    
    void InitSprites();
    void InitProperties();
    void InitFairy();
    void InitSkill();
    void MakeScrollSlot();

    std::string ItemDesc(int idx);
    
    void ShowHintOfBuyingSlot();
    
    void RenewInfo();
    void ShowRewardPopup();
    
    void PotionTimer(float f);
    
    void StartGame();
    
    void EndScene();
    void EndSceneCallback(CCNode* sender, void* data);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    bool isStarting;
    
    bool itemSelected[5];
    int callbackType;
    
    CCRect rect;
    int kind;
    int idx;
    
    int popupStatus;
    bool doNotShowExpPopup;
    
private:
    CCLayer* tLayer;
    
    CCSprite* pBlack;
    CCSprite* pBlackClose;
    
    CCLayer* fairyLayer;
    CCLayer* skillLayer;
    CCLayer* containerSlot;
    CCScrollView* scrollViewSlot;
    
    CCLayer* itemNumberLayer[5];
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassProperty;
    SpriteClass* spriteClassFairy;
    SpriteClass* spriteClassSkill;
    SpriteClass* spriteClassSlot;
    
    CCScale9Sprite* sp9;
    CCDrawNode* stencil;
    CCClippingNode* clip;
    
    CCScale9Sprite* balloon;
    CCLabelTTF* ball;
};

#endif /* defined(__CocoMagic__GameReady__) */
