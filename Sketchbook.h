#ifndef __CocoMagic__Sketchbook__
#define __CocoMagic__Sketchbook__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class Sketchbook : public CCLayer,
                   public CCScrollViewDelegate
{
public:
    static CCScene* scene(int tab, int fromWhere, int prio);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    virtual void registerWithTouchDispatcher(void);
    
    void Notification(CCObject* obj);
    
    void InitSprites();
    void CheckProperties();
    void MakeScroll(int state, bool isFromPopup = false);
    void MakeScrollBook(int idx);
    void MakeScrollSlot();
    
    void SetMenuChange(int state);
    void SetTouchLock(bool val);
    
    SkillInfo* GetNextSkillInfo(int state);
    
    void EndScene();
    void EndSceneCallback();
    
    void XmlParsePracticeSkill(xml_document *xmlDoc);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    int FromWhere();
    
    CREATE_FUNC(Sketchbook);
    
protected:
    CCSize winSize;
    bool isTouched;
     
    int curState;
    
    bool isScrolling;
    bool isSlotTouched;
    bool isScrollViewTouched;
    
private:
    CCSprite* pBlack;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassBook;
    SpriteClass* spriteClassSlot;
    
    CCScrollView* scrollView;
    CCScrollView* scrollViewSlot;
    
    CCLayer* containerBook;
    CCLayer* containerSlot;
};

#endif /* defined(__CocoMagic__Sketchbook__) */
