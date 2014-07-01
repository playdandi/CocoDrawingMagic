#ifndef __magician__PuzzleResult__
#define __magician__PuzzleResult__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class PuzzleResult : public CCLayer,
                    public CCScrollViewDelegate
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
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    CREATE_FUNC(PuzzleResult);
    
    void InitSprites();
    void InitSkills();
    void SkillTimer(float f);
    void ScoreTimer(float f);
    void TopazTimer(float f);
    void StarCandyTimer(float f);
    void MPTimer(float f);
    void Callback(CCNode* sender, void* p);
    void Callback_ProgressBar(CCNode* sender, void* p);
    
    void Notification(CCObject* obj);
    
    std::string GetMissionContent();
    
    void EndScene();
    void EndSceneCallback(CCNode* sender, void* pointer);
    
protected:
    CCSize m_winSize;
    bool isTouched;
    
    int varScore;
    int score;
    
    float topaz;
    float starcandy;
    float mp;
    
    bool isScrolling;
    int skillIdx;

private:
    CCSprite* pBlack;
    CCSprite* pBlackClose;
    CCLayer* pScoreLayer;
    
    SpriteClass* spriteClass;
    SpriteClass* spriteClassSkill;
    
    CCLayer* l;
    CCLayer* barLayer;
    
    CCDrawNode* timerStencil;
    CCClippingNode* timerClip;
    CCDrawNode* timerStencil2;
    CCClippingNode* timerClip2;
    
    CCScrollView* scrollView;
};

#endif /* defined(__magician__PuzzleResult__) */
