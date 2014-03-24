#ifndef __CocoMagic__Setting__
#define __CocoMagic__Setting__

#include "Common.h"

class Setting : public CCLayer
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
    
    CREATE_FUNC(Setting);
    
    void InitSprites();
    
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    // on-off 버튼을 선택할 시 필요한 변수들
    CCSprite* selectedSprite;
    int selectedBtn;
    CCPoint selectedPos;
    CCPoint selectedTouchPos;
    CCPoint standardBtnPos;
    
private:
    SpriteClass* spriteClass;

};

#endif /* defined(__CocoMagic__Setting__) */
