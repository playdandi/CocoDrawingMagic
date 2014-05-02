#ifndef __CocoMagic__SketchDetail__
#define __CocoMagic__SketchDetail__

#include "Common.h"

class SketchDetail : public CCLayer
{
public:
    ~SketchDetail(void);
    static CCScene* scene(int id);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(SketchDetail);
    
    void InitSprites();
    void InitFairy();
    void InitSkill();
    
    void EndScene();
    void EndSceneCallback();
    
    void XmlParseUpgradeOrPurchaseSkill(char* data, int size, int tag);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
protected:
    CCSize winSize;
    bool isTouched;
    
    int btnStatus;
    
private:
    CCSprite* pBlack;
    CCLayer* descLayer;
    SpriteClass* spriteClass;
};


#endif /* defined(__CocoMagic__SketchDetail__) */