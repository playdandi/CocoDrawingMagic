#ifndef __CocoMagic__ParticleTest__
#define __CocoMagic__ParticleTest__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"

using namespace cocos2d;
using namespace cocos2d::extension;

class ParticleTest : public CCLayer
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
    
    CREATE_FUNC(ParticleTest);
    
    void InitSprites();
    
    void EndScene();
    void EndSceneCallback();
    
    void MakeParticle(std::string type, CCSprite* sp, std::string blend, float scale, int speed, CCPoint ap, CCPoint pos);
    void MakeParticles(int type, CCSprite* s);
    void MakeColorPaper(float f);
    void RemoveColorPaper(CCNode* sender);
    
protected:
    CCSize winSize;
    bool isTouched;
    
private:
    int type;
    int stype;
    CCSprite* sp_x;
    CCSprite* btn;
    CCSprite* btn2;
    CCLabelTTF* label;
    CCLabelTTF* label2;
    std::vector<CCParticleSystem*> particles;
    
    CCSprite* s1;
    CCSprite* s2;
};

#endif /* defined(__CocoMagic__ParticleTest__) */
