#ifndef __CocoMagic__T_MagicList__
#define __CocoMagic__T_MagicList__

#include "Common.h"

class T_MagicList : public CCLayer,
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
    
    void InitSprites();
    //void InitBtn();
    void MakeScrollSlot(bool isAutoMove);
    
    void Notification(CCObject* obj);

    void EndScene();
    void EndSceneCallback();
    
    void RenewSlot(bool isAutoMove);
    
    CREATE_FUNC(T_MagicList);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
    int code;
    
private:
    SpriteClass* spriteClass;
    //SpriteClass* spriteClassBtn;
    SpriteClass* spriteClassSlot;
    CCLayer* layer;
    
    CCLayer* containerSlot;
    CCScrollView* scrollViewSlot;
    
    CCSprite* ttrPos;
    CCSprite* ttrArrow;
};


#endif /* defined(__CocoMagic__T_MagicList__) */
