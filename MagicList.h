#ifndef __CocoMagic__MagicList__
#define __CocoMagic__MagicList__

#include "Common.h"

class MagicList : public CCLayer,
                    public CCScrollViewDelegate
{
public:
    static CCScene* scene(int fromWhere);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void Scene_Callback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    void InitSprites();
    void InitBtn();
    void MakeScrollSlot(bool isAutoMove);
    
    void RenewCheck();
    
    void Notification(CCObject* obj);
    
    void TryEnd();
    
    void EndScene();
    void EndSceneCallback();
    
    void RenewSlot(bool isAutoMove);
    void SendToParent();
    
    void XmlParseSkillSlot(xml_document *xmlDoc);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    CREATE_FUNC(MagicList);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isScrollViewTouched;
    
    int code;
    int last_scid;
    
private:
    SpriteClass* spriteClass;
    SpriteClass* spriteClassBtn;
    SpriteClass* spriteClassSlot;
    CCLayer* layer;
    
    CCLayer* containerSlot;
    CCScrollView* scrollViewSlot;
};


#endif /* defined(__CocoMagic__MagicList__) */
