#ifndef __CocoMagic__CocoRoom__
#define __CocoMagic__CocoRoom__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Fairy.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class CocoRoom : public CCLayer,
                 public CCScrollViewDelegate
{
public:
    static CCScene* scene(int tab);
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
    
    CREATE_FUNC(CocoRoom);
    
    void EndScene();
    void EndSceneCallback();
    
    void RenewAllData();
    
    void ShowHintOfMP();
    
    void InitSprites();
    void MakeSprites(int state);
    
    void SetMenuChange(int state);
    
    void MakeSpritesCoco();
    void SetStaffBuildUp();
    void MakeScrollCoco();
    
    void MakeSpritesFairy();
    void SetFairyBuildUp();
    void ShowHintOfFairy();
    void MakeScrollFairy();
    
    void MakeSpritesCandy();
    void SetTodayCandyList();

    void DecideUser();
    void TodayCandy_Result(char* selectedKakaoId);
    void Callback(CCNode* sender, void* data);
    void Callback2(CCNode* sender, void* kakaoId);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseFairyList(xml_document *xmlDoc);
    void XmlParseTodayCandy(xml_document *xmlDoc);
    
protected:
    CCSize winSize;
    bool isKeybackTouched;
    bool isTouched;
    bool isScrolling;
    bool isSlotTouched;
    int curState;
    
    bool isHintOfMPShown;
    
    bool isTodayCandyWorking;
    CCSprite* todaycandy_bg;
    
    CCRect rect;
    int kind;
    int idx;
    
private:
    CCLayer* tLayer;
    CCParticleSystem* par;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassCoco;
    SpriteClass* spriteClassFairy;
    SpriteClass* spriteClassCandy;
    
    SpriteClass* spriteClassFairySlot;
    SpriteClass* spriteClassCocoSlot;
    SpriteClass* spriteClassCandyList;
    
    CCScrollView* scrollView;
    CCLayer* containerCoco;
    CCLayer* containerFairy;
    
    CCScale9Sprite* balloon;
    CCLabelTTF* ball;
    CCScale9Sprite* balloon2;
    CCLabelTTF* ball2;
    
    CCLayer* coco;
    CCLayer* fairy;
    CCLayer* candy;
    
    CCSprite* floor;
    CCDrawNode* stencil;
    CCClippingNode* clip;
};

#endif /* defined(__CocoMagic__CocoRoom__) */
