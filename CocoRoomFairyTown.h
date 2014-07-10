#ifndef __CocoMagic__CocoRoomFairyTown__
#define __CocoMagic__CocoRoomFairyTown__

#include "Common.h"
#include "Fairy.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoomFairyTown : public CCLayer,
                        public CCScrollViewDelegate
{
public:
    ~CocoRoomFairyTown();
    static CCScene* scene();
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    void SceneCallback();
    
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    void Notification(CCObject* obj);
    
    CREATE_FUNC(CocoRoomFairyTown);
    
    void InitSprites();
    void MakeScroll();
    void RenewScroll();
    
    void EndScene();
    void EndSceneCallback();
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseFairyList(xml_document *xmlDoc);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool scrollViewTouch;
    
private:
    CCLayer* tLayer;
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    SpriteClass* spriteClassScroll;
    
    CCScrollView* scrollView;
    CCLayer* scrollContainer;
    
    std::vector<class FairyEach*> fairyData;
};

class FairyEach
{
public:
    FairyEach(int cfi, int costType, int costValue);
    int GetCommonId();
    int GetCostType();
    int GetCostValue();
private:
    int commonFairyId;
    int costType;
    int costValue;
};

#endif /* defined(__CocoMagic__CocoRoomFairyTown__) */
