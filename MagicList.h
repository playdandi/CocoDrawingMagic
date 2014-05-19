#ifndef __CocoMagic__MagicList__
#define __CocoMagic__MagicList__

#include "Common.h"

class MagicList : public CCLayer,
                    public CCScrollViewDelegate
{
public:
    static CCScene* scene(int fromWhere, int prio);
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
    void MakeScrollSlot();
    
    void Notification(CCObject* obj);
    
    void TryEnd();
    
    void EndScene();
    void EndSceneCallback();
    
    void RenewSlot();
    void SendToParent();
    
    void XmlParseSkillSlot(char* data, int size);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    CREATE_FUNC(MagicList);
    
protected:
    CCSize winSize;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
private:
    SpriteClass* spriteClass;
    SpriteClass* spriteClassSlot;
    CCLayer* layer;
    
    CCLayer* containerSlot;
    CCScrollView* scrollViewSlot;
};


#endif /* defined(__CocoMagic__MagicList__) */
