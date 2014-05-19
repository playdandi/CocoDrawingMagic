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
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseResult(char* data, int size);
    
    void EndScene();
    
protected:
    CCSize winSize;
    bool isTouched;
    
    int selectedBtn;
    CCPoint selectedPos;
    CCPoint selectedTouchPos;
    CCPoint standardBtnPos;
    
    bool kakaoMsgReserved;
    bool pushNotiReserved;
    bool potionMsgReserved;
    
private:
    CCSprite* pBlack;
    SpriteClass* spriteClass;
    
    CCSprite* selectedSprite;
};

#endif /* defined(__CocoMagic__Setting__) */
