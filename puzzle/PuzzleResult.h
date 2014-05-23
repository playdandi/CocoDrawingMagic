#ifndef __magician__PuzzleResult__
#define __magician__PuzzleResult__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class PuzzleResult : public CCLayer
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
    
    CREATE_FUNC(PuzzleResult);
    
    void InitSprites();
    void InitSkills();
    void SkillTimer(float f);
    void Callback(CCNode* sender, void* p);
    
    void EndScene();
    void EndSceneCallback(CCNode* sender, void* pointer);
    
protected:
    CCSize m_winSize;
    
private:
    CCSprite* pBlack;
    CCSprite* pBlackClose;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassSkill;
    CCLayer* l;
    
    CCDrawNode* timerStencil;
    CCClippingNode* timerClip;
};

#endif /* defined(__magician__PuzzleResult__) */
