#ifndef __CocoMagic__NoImage__
#define __CocoMagic__NoImage__

#include "Common.h"

class NoImage : public CCLayer
{
public:
    static CCScene* scene(int popupType, int btnType, std::vector<int> data);
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    virtual void keyBackClicked();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent* event);
    
    CREATE_FUNC(NoImage);
    
    void InitSprites();
    
    void ReplaceScene(std::string to, int type, int btnType);
    void EndScene();
    void EndSceneCallback();
    
protected:
    CCSize winSize;
    
    CCSprite* pBlack;
    
    bool isTouched;
    
private:
    SpriteClass* spriteClass;
    
    
};

#endif /* defined(__CocoMagic__NoImage__) */
