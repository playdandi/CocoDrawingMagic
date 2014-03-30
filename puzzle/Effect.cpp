#include "Effect.h"

enum
{
    z1 = 200,
};

void Effect::Init(Puzzle* layer)
{
    gameLayer = layer;
}

void Effect::InitCheck()
{
    for (int i = 0 ; i < NUMOFSKILL ; i++)
        isDone[i] = false;
}


void Effect::PlayEffect(int skillNum, std::vector<CCPoint> pos)
{
    switch (skillNum)
    {
        case -1: PlayEffect_CycleOnly(skillNum, pos); break;
        case -9: PlayEffect_CycleOnly(skillNum, pos); break;
        //case -17: PlayEffect_CycleOnly(skillNum, pos); break;
        case 0:  PlayEffect_0(pos); break;
        case 1:  PlayEffect_1(pos); break;
        case 8:  PlayEffect_8(pos); break;
        case 9:  PlayEffect_9(pos); break;
        case 16: PlayEffect_16(pos); break;
            
        case 5: PlayEffect_5(pos); break;
        case 13: PlayEffect_13(pos); break;
        case 21: PlayEffect_21(pos); break;
    }
}


void Effect::PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos)
{
    // 태양열, 파도, ?
    CCSprite* sprite;
    if (skillNum == -1)
        sprite = CCSprite::createWithSpriteFrameName("pieces/sun_1.png");
    else if (skillNum == -9)
        sprite = CCSprite::createWithSpriteFrameName("pieces/sea_1.png");
    //else
    //    sprite = CCSprite::createWithSpriteFrameName("pieces/.png");
    
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(gameLayer->SetPiece8Position((int)pos[0].x, (int)pos[0].y));
    sprite->setScale(0.5f);
    sprite->setOpacity(0);
    gameLayer->addChild(sprite, z1);
    
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create(
                    CCScaleTo::create(0.40f, 1.0f), CCSequence::create(
                        CCFadeIn::create(0.35f), CCFadeOut::create(0.05f), NULL), NULL),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL),
                    NULL);
    sprite->runAction(action);
}

void Effect::PlayEffect_0(std::vector<CCPoint> pos)
{
    // 불1 : red 기본 추가점수 이펙트
    isDone[0] = true;
    
    int x, y;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        CCSprite* sp = CCSprite::create("images/fire.png");
        CCParticleSystem* par = CCParticleFire::create();
        par->retain();
        par->setTexture(sp->getTexture());
        /*
         if (blend == "blend") {
         ccBlendFunc bf = {GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA}; // blend
         par->setBlendFunc(bf);
         }
         else if (blend == "dodge") {
         ccBlendFunc bf = {GL_ONE, GL_ONE}; // linear dodge
         par->setBlendFunc(bf);
         }
         else if (blend == "screen") {
         ccBlendFunc bf = {GL_ONE_MINUS_DST_COLOR, GL_ONE}; // screen
         par->setBlendFunc(bf);
         }
         */
        
        ccBlendFunc bf = {GL_ONE_MINUS_DST_COLOR, GL_ONE}; // screen
        par->setBlendFunc(bf);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.9f;
        startColor.g = 0.2f;
        startColor.b = 0.2f;
        par->setStartColor(startColor);
        
        //par->setTotalParticles(700);
        par->setLife(0.5);
        
        par->setScale(1.2f);
        par->setSpeed(300);
        par->setSpeedVar(200);
        
        par->setGravity(ccp(0, -200));
        
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetPiece8Position(x, y));
        
        par->setDuration(0.15f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
    }
}

void Effect::PlayEffect_1(std::vector<CCPoint> pos)
{
    // 불2A : red 사이클 이펙트
    //if (isDone[1])
    //    return;
    //isDone[1] = true;
    
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        CCSprite* sp = CCSprite::create("images/fire.png");
        CCParticleSystem* par = CCParticleGalaxy::create();
        par->retain();
        par->setTexture(sp->getTexture());
        
        //par->setTotalParticles(800);
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetPiece8Position(x, y));
        par->setLife(1);
        //par->setLifeVar(1);
        
        //par->setGravity(ccp(0, -10));
        //par->set
        par->setStartSize(120);
        //CCLog("size = %f", par->getStartSize());
        
        par->setSpeed(200);
        par->setSpeedVar(150);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.95f;
        startColor.g = 0.05f;
        startColor.b = 0.05f;
        par->setStartColor(startColor);
        
        //par->setEmissionRate(par->getTotalParticles() / par->getLife());
        
        par->setDuration(0.3f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
    }
    
    
}

void Effect::PlayEffect_8(std::vector<CCPoint> pos)
{
    // 물1 : blue 기본 추가점수
    int x, y;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCSprite* moon = CCSprite::createWithSpriteFrameName("pieces/moon_1.png");
        moon->setAnchorPoint(ccp(0.5, 0.5));
        moon->setPosition(gameLayer->SetPiece8Position(x, y));
        moon->setScale(0.3f);
        
        CCLayer* layer = CCLayer::create();
        layer->setPosition(ccp(0, 0));
        layer->setContentSize(CCSize(1080, 1920));
        layer->addChild(moon, z1);
        
        CCDrawNode* stencil = CCDrawNode::create();
        stencil->drawDot(gameLayer->SetPiece8Position(x-1, y), 154/2, ccc4f(0, 0, 0, 255));
        CCClippingNode* clip = CCClippingNode::create(stencil);
        clip->addChild(layer);
        gameLayer->addChild(clip, z1);
        
        CCActionInterval* moveLayerAction = CCSequence::create(CCMoveBy::create(0.2f, ccp(-154*2, 0)),
              CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
        CCFiniteTimeAction* action = CCSequence::create( CCMoveBy::create(0.2f, ccp(154*2, 0)),
              CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
        
        clip->runAction(action);
        layer->runAction(moveLayerAction);
    }
}


void Effect::PlayEffect_9(std::vector<CCPoint> pos)
{
    // 물2A : blue 사이클 이펙트
    //if (isDone[9])
    //    return;
    //isDone[9] = true;
    
    
}

void Effect::PlayEffect_16(std::vector<CCPoint> pos)
{
    // 땅1 : green 기본 추가점수 이펙트
    isDone[16] = true;
    
    int x, y;
    char name[30];
    
    CCArray* animFrames = CCArray::createWithCapacity(6);
    for (int j = 0 ; j < 6 ; j++)
    {
        sprintf(name, "anim/green_%d.png", j+1);
        CCSpriteFrame* frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
        animFrames->addObject(frame);
    }
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCSprite* first = CCSprite::createWithSpriteFrameName("anim/green_1.png");
        first->setAnchorPoint(ccp(0.5, 0.5));
        first->setPosition(gameLayer->SetPiece8Position(x, y));
        first->setScale(0.45f);
        gameLayer->addChild(first, z1);
        
        CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.03f);
        CCAnimate* animate = CCAnimate::create(animation);
        CCFiniteTimeAction* action = CCSequence::create(animate,
                CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL),
                NULL);
        first->runAction(action);
        
        
        //// particle ////
        CCSprite* sp = CCSprite::create("images/fire.png");
        CCParticleSystem* par = CCParticleFlower::create();
        par->retain();
        par->setTexture(sp->getTexture());
        
        //par->setTotalParticles(800);
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetPiece8Position(x, y));
        par->setLife(0.5);
        //par->setLifeVar(1);
        
        //par->setGravity(ccp(0, -10));
        
        par->setSpeed(300);
        par->setSpeedVar(150);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.1f;
        startColor.g = 0.9f;
        startColor.b = 0.1f;
        par->setStartColor(startColor);
        
        //par->setEmissionRate(par->getTotalParticles() / par->getLife());
        
        par->setDuration(0.15f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
    }
}


void Effect::PlayEffect_5(std::vector<CCPoint> pos)
{
    // 6개이상한번더
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("images/LavaFlow.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetPiece8Position(x, y));
        CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
        batch->addChild(m_emitter);
        
        //batch->setAnchorPoint(ccp(0.5, 0.5));
        //batch->setPosition(ccp(x, y));
        gameLayer->addChild(batch, 2000);
        //gameLayer->addChild(m_emitter, z1+1);
    }
}
void Effect::PlayEffect_13(std::vector<CCPoint> pos)
{
    return;
    // 6개이상한번더
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("images/LavaFlow.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetPiece8Position(x, y));
        CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
        batch->addChild(m_emitter);
        
        //batch->setAnchorPoint(ccp(0.5, 0.5));
        //batch->setPosition(ccp(x, y));
        gameLayer->addChild(batch, 2000);
        //gameLayer->addChild(m_emitter, z1+1);
    }
}
void Effect::PlayEffect_21(std::vector<CCPoint> pos)
{
    return;
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("images/LavaFlow.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetPiece8Position(x, y));
        CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
        batch->addChild(m_emitter);
        
        //batch->setAnchorPoint(ccp(0.5, 0.5));
        //batch->setPosition(ccp(x, y));
        gameLayer->addChild(batch, 2000);
        //gameLayer->addChild(m_emitter, z1+1);
    }
}


void Effect::PlayEffectCallback(CCNode* sender, void* data)
{
    sender->removeAllChildrenWithCleanup(true);
    sender->removeFromParentAndCleanup(true);
}

