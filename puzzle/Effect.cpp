#include "Effect.h"

enum
{
    z1 = 200,
};

void Effect::Init(Effect* effect, Puzzle* layer)
{
    pThis = effect;
    gameLayer = layer;
}

void Effect::InitCheck()
{
    for (int i = 0 ; i < NUMOFSKILL ; i++)
        isDone[i] = false;
}


void Effect::PlayEffect(int skillNum, std::vector<CCPoint> pos, int queue_pos)
{
    switch (skillNum)
    {
        case 100: PlayEffect_Spirit(); break;
        case -1: PlayEffect_CycleOnly(skillNum, pos); break;
        case -9: PlayEffect_CycleOnly(skillNum, pos); break;
        //case -17: PlayEffect_CycleOnly(skillNum, pos); break;
        case 0:  PlayEffect_0(pos); break;
        case 8:  PlayEffect_8(pos); break;
        case 16: PlayEffect_16(pos); break;

        case 1:  PlayEffect_1(pos); break;
        case 9:  PlayEffect_9(pos, queue_pos); break;
        case 17: PlayEffect_17(pos); break;
            
        case 5: PlayEffect_5(pos); break;
        case 13: PlayEffect_13(pos); break;
        case 21: PlayEffect_21(pos); break;
            
        case 2: PlayEffect_2(pos); break;
            
        case 7: PlayEffect_7(pos); break;
            
        default: PlayEffect_Default(pos); break;
    }
}

void Effect::PlayEffect_Default(std::vector<CCPoint> pos)
{
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;

        CCSprite* sp = CCSprite::create("images/fire.png");
        CCParticleSystem* par = CCParticleFlower::create();
        par->retain();
        par->setTexture(sp->getTexture());
        
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetTouch8Position(x, y));
        par->setLife(0.5);
        
        par->setSpeed(300);
        par->setSpeedVar(150);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.95f;
        startColor.g = 0.95f;
        startColor.b = 0.95f;
        par->setStartColor(startColor);
        
        par->setDuration(0.2f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
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
    sprite->setPosition(gameLayer->SetTouch8Position((int)pos[0].x, (int)pos[0].y));
    sprite->setScale(0.5f);
    sprite->setOpacity(0);
    gameLayer->addChild(sprite, z1);
    
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create(
                    CCScaleTo::create(0.35f, 1.0f), CCSequence::create(
                        CCFadeIn::create(0.30f), CCFadeOut::create(0.05f), NULL), NULL),
                CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL),
                    NULL);
    sprite->runAction(action);
}

void Effect::SetSpirit(int type)
{
    if (type == 0) // 불의 정령
    {
        fire = CCParticleSystemQuad::create("particles/fire2.plist");
        fire->setPosition(gameLayer->SetTouch8Position(0, ROW_COUNT-1));
        fire->retain();
        fire->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(fire, 2000);
    }
    else if (type == 1) // 물의 정령
    {
        water = CCParticleSystemQuad::create("particles/water.plist");
        water->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, ROW_COUNT-1));
        water->retain();
        water->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(water, 2000);
    }
    else // 땅의 정령
    {
        land = CCParticleSystemQuad::create("particles/land.plist");
        land->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, 0));
        land->retain();
        land->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(land, 2000);
    }
    //CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
    //batch->addChild(m_emitter);
    //puzzleLayer->addChild(batch, 2000);
}
CCParticleSystemQuad* Effect::GetSpirit(int type)
{
    if (type == 0)
        return fire;
    else if (type == 1)
        return water;
    return land;
}
void Effect::ReleaseSpirit(int type)
{
    if (type == 0)
        fire->removeFromParentAndCleanup(true);
    else if (type == 1)
        water->removeFromParentAndCleanup(true);
    else
        land->removeFromParentAndCleanup(true);
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
        par->setPosition(gameLayer->SetTouch8Position(x, y));
        
        par->setDuration(0.15f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
    }
}

void Effect::PlayEffect_1(std::vector<CCPoint> pos)
{
    // 불2A : red 사이클 이펙트
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
        par->setPosition(gameLayer->SetTouch8Position(x, y));
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
        moon->setPosition(gameLayer->SetTouch8Position(x, y));
        moon->setScale(0.3f);
        
        CCLayer* layer = CCLayer::create();
        layer->setPosition(ccp(0, 0));
        layer->setContentSize(CCSize(1080, 1920));
        layer->addChild(moon, z1);
        
        CCDrawNode* stencil = CCDrawNode::create();
        stencil->drawDot(gameLayer->SetTouch8Position(x-1, y), 154/2, ccc4f(0, 0, 0, 255));
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


void Effect::PlayEffect_9(std::vector<CCPoint> pos, int queue_pos)
{
    // 물2A : blue 사이클 주변부 이펙트
    skillPos.clear();
    skillPos = pos;
    
    queue_pos_now = queue_pos;
    
    // 터뜨릴 개수가 없으면 바로 Falling을 시작하자.
    if ((int)pos.size() <= 1)
    {
        pThis->gameLayer->Falling(pThis->queue_pos_now);
        return;
    }

    CCSprite* sp = CCSprite::create("images/fire.png");
    CCParticleSystem* par = CCParticleGalaxy::create();
    par->retain();
    par->setTexture(sp->getTexture());
    
    //par->setTotalParticles(800);
    par->setAnchorPoint(ccp(0.5, 0.5));
    //par->setPosition(gameLayer->SetTouch8Position((int)pos[1].x-(int)pos[0].x, (int)pos[1].y-(int)pos[0].y));
    CCPoint first = gameLayer->SetTouch8Position((int)pos[1].x, (int)pos[1].y);
    par->setPosition(ccp((int)first.x-(int)pos[0].x, (int)first.y-(int)pos[0].y));
    par->setLife(1);
    //par->setLifeVar(1);
    
    //par->setGravity(ccp(0, -10));
    par->setStartSize(200);
    
    //par->setEmitterMode(kCCParticleModeRadius);
    //par->setStartRadius(50);
    //par->setStartRadiusVar(10);
    
    par->setSpeed(200);
    par->setSpeedVar(100);
    
    ccColor4F startColor = par->getStartColor();
    startColor.r = 0.08f;
    startColor.g = 0.82f;
    startColor.b = 0.99f;
    par->setStartColor(startColor);
    
    par->setDuration(-1);
    par->setAutoRemoveOnFinish(false);
    
    gameLayer->addChild(par, z1);
    
    // delta
    deltaPos = pos[0];
    //CCLog("delta  : %d %d", (int)deltaPos.x, (int)deltaPos.y);
    
    callbackCnt = 1;
    
    CCFiniteTimeAction* action = CCSequence::create(CCBezierBy::create(0.2f, GetBezierConfig(pThis, 1)),
            CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect9Callback), (void*)pThis),
            NULL);
    par->runAction(action);
}

ccBezierConfig Effect::GetBezierConfig(Effect* pThis, int mul)
{
    CCPoint deltaPos = pThis->deltaPos;
    
    ccBezierConfig bezier;
    int x, y;
    if ((int)deltaPos.y == 0) // 가로 이동
    {
        if ((int)deltaPos.x < 0) x = -(int)deltaPos.x/2;
        else x = (int)deltaPos.x/2;
        y = mul * (int)deltaPos.x;
    }
    else if ((int)deltaPos.x == 0) // 세로 이동
    {
        if ((int)deltaPos.y < 0) y = -(int)deltaPos.y/2;
        else y = (int)deltaPos.y/2;
        x = mul * (int)deltaPos.y;
    }
    else // 대각선 이동
    {
        if (mul < 0) {
            x = (int)deltaPos.x + (mul * 50);
            y = 0;
        }
        else {
            x = 0;
            y = (int)deltaPos.y + (mul * 50);
        }
    }
    //CCLog("bezier %d %d", x, y);
    bezier.controlPoint_1 = ccp(x, y);
    bezier.controlPoint_2 = ccp(x, y);
    bezier.endPosition = deltaPos;
    return bezier;
}

void Effect::Effect9Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    
    //CCLog("callback : %d , %d", pThis->callbackCnt, (int)pThis->skill9Pos.size()-1);
    
    // bomb piece
    CCSprite* piece = pThis->gameLayer->GetSpriteP8((int)pThis->skillPos[pThis->callbackCnt].x, (int)pThis->skillPos[pThis->callbackCnt].y);
    if (piece != NULL)
    {
        //CCLog("let's bomb");
        CCFiniteTimeAction* bomb = CCSpawn::create(
                CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
        piece->runAction(bomb);
    }
    
    if (pThis->callbackCnt < (int)pThis->skillPos.size()-1)
    {
        pThis->callbackCnt++;
        int mul = (pThis->callbackCnt % 2 == 0) ? -1 : 1;
        CCFiniteTimeAction* action = CCSequence::create(
            CCBezierBy::create(0.2f, GetBezierConfig(pThis, mul)),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect9Callback), (void*)pThis),
            NULL);
        sender->runAction(action);
    }
    else
    {
        //CCLog("effect 9 callback done");
        int x, y;
        for (int i = 1 ; i < pThis->skillPos.size() ; i++)
        {
            x = (int)pThis->skillPos[i].x;
            y = (int)pThis->skillPos[i].y;
            if (pThis->gameLayer->GetPuzzleP8Set()->GetObject(x, y)->GetPiece() != NULL)
            {
                pThis->gameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
                pThis->gameLayer->SetSpriteP8Null(x, y);
            }
        }
        pThis->skillPos.clear();
        
        ((CCParticleSystem*)sender)->setDuration(0.1);
        ((CCParticleSystem*)sender)->setAutoRemoveOnFinish(true);
        
        //sender->removeAllChildrenWithCleanup(true);
        //sender->removeFromParentAndCleanup(true);
        
        pThis->gameLayer->Falling(pThis->queue_pos_now);
    }
}

void Effect::PlayEffect_17(std::vector<CCPoint> pos)
{
    //return;
    // 땅2A : green 사이클 주변부 이펙트
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
//
        
        //CCActionInterval* action = CCRipple3D::create(3.0f, CCSizeMake(PIECE8_WIDTH, PIECE8_HEIGHT), gameLayer->SetTouch8Position(x, y), PIECE8_WIDTH, 4, 160);
        //CCActionInterval* ripple = CCRipple3D::create(1.0f, CCSizeMake(30, 30), gameLayer->SetTouch8Position(x, y), PIECE8_WIDTH/2, 4, 160);
        //CCActionInterval* wave = CCWaves::create(0.5f, CCSizeMake(5, 5), 2, 10, true, true);
        //CCFiniteTimeAction* action = CCSequence::create(ripple,
        //        CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL),
        //        NULL);
        
        //CCLog("RIPPLE!");
        //gameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
        
        //gameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(wave);
        
    }
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
        first->setPosition(gameLayer->SetTouch8Position(x, y));
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
        par->setPosition(gameLayer->SetTouch8Position(x, y));
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
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
        batch->addChild(m_emitter);

        gameLayer->addChild(batch, 2000);
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
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
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
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
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


void Effect::PlayEffect_7(std::vector<CCPoint> pos)
{
    skillPos.clear();
    skillPos = pos;
    callbackCnt = 0;
    
    dragon = CCSprite::createWithSpriteFrameName("icon/dragon.png");
    dragon->setPosition(ccp(500, 1450));
    dragon->setScale(0.0f);
    dragon->setOpacity(0);
    gameLayer->addChild(dragon, z1);
    
    CCFiniteTimeAction* action = CCSequence::create(
                CCSpawn::create( CCFadeIn::create(1.0f), CCScaleTo::create(1.0f, 0.5f), CCMoveBy::create(1.0f, ccp(0, 150)), CCTintTo::create(1.0f, 225, 25, 0), NULL),
                CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect7Callback), (void*)pThis),
                NULL);
    dragon->runAction(action);
}

void Effect::Effect7Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    
    if (pThis->callbackCnt == 0)
    {
        //int x, y;
        //for (int i = 0 ; i < pThis->skillPos.size() ; i++)
        //{
            int x = (int)pThis->skillPos[pThis->callbackCnt].x;
            int y = (int)pThis->skillPos[pThis->callbackCnt].y;
            
            CCSprite* sp = CCSprite::create("images/fire.png");
            CCParticleSystem* par = CCParticleSun::create();
            par->retain();
            par->setTexture(sp->getTexture());
            
            par->setAnchorPoint(ccp(0.5, 0.5));
            par->setPosition(ccp(500, 1450+150));
            par->setLife(0.5);
            
            par->setStartSize(100);
            par->setSpeed(250);
            par->setSpeedVar(150);
            
            ccColor4F startColor = par->getStartColor();
            startColor.r = 0.95f;
            startColor.g = 0.05f;
            startColor.b = 0.05f;
            par->setStartColor(startColor);
            
            par->setDuration(-1);
            par->setAutoRemoveOnFinish(false);
            par->setTag(pThis->callbackCnt);
            
            pThis->gameLayer->addChild(par, z1);
        
            pThis->callbackCnt++;
        
            CCActionInterval* action = CCSequence::create(
                CCMoveTo::create(1.0f, pThis->gameLayer->SetTouch8Position(x, y)),
                CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect7Callback), (void*)pThis),
                NULL);
            par->runAction(action);
        //}
    }
    else
    {
        int tag = sender->getTag();
        ((CCParticleSystem*)sender)->setDuration(0.1f);
        ((CCParticleSystem*)sender)->setAutoRemoveOnFinish(true);
        if (pThis->skillPos.size()-1 == tag)
        {
            pThis->gameLayer->Bomb(pThis->skillPos);
        }
        pThis->gameLayer->removeChild(pThis->dragon);
        //pThis->dragon->removeChild(
        //pThis->dragon = NULL;
    }
}

void Effect::PlayEffect_2(std::vector<CCPoint> pos)
{
    // F3 : 10개이상 제거 시 추가점수
    
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    int x = (int)pos[pos.size()-1].x;
    int y = (int)pos[pos.size()-1].y;
    if (gameLayer->IsCycle())
    {
        x = (int)pos[0].x;
        y = (int)pos[0].y;
    }
    plus->setPosition(gameLayer->SetTouch8Position(x, y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    
    CCFiniteTimeAction* action = CCSequence::create(
                    CCSpawn::create(CCScaleTo::create(0.5f, 1.5f),
                                    CCSequence::create(CCFadeIn::create(0.25f), CCFadeOut::create(0.25f), NULL), NULL),
                    CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2Callback), (void*)pThis),
                    NULL );
    plus->runAction(action);
}
void Effect::Effect2Callback(CCNode* sender, void* pointer)
{
    //Effect* pThis = (Effect*)pointer;
    sender->removeFromParentAndCleanup(true);
}




void Effect::PlayEffect_Spirit()
{
    return;
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("images/BoilingFoam.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(70, 300));
    m_emitter->setStartColor(ccc4f(0.1, 0.1, 0.95, 1));
    m_emitter->setEndColor(ccc4f(0.1, 0.1, 0.95, 0));
    m_emitter->setScale(1.2f);
    gameLayer->addChild(m_emitter, 1000);
    
    CCParticleSystemQuad* eye1 = CCParticleSystemQuad::create("images/BoilingFoam.plist");
    eye1->retain();
    eye1->setAnchorPoint(ccp(0.5, 0.5));
    eye1->setPosition(ccp(50, 350));
    eye1->setStartColor(ccc4f(0.99, 0.01, 0.01, 1));
    eye1->setEndColor(ccc4f(0.99, 0.01, 0.01, 1));
    eye1->setScale(0.4f);
    gameLayer->addChild(eye1, 1050);
    //CCParticleBatchNode *batch1 = CCParticleBatchNode::createWithTexture(eye1->getTexture());
    //batch1->addChild(eye1);
    
    CCParticleSystemQuad* eye2 = CCParticleSystemQuad::create("images/BoilingFoam.plist");
    eye2->retain();
    eye2->setAnchorPoint(ccp(0.5, 0.5));
    eye2->setPosition(ccp(95, 350));
    eye2->setStartColor(ccc4f(0.99, 0.01, 0.01, 1));
    eye2->setEndColor(ccc4f(0.99, 0.01, 0.01, 1));
    eye2->setScale(0.4f);
    gameLayer->addChild(eye2, 1050);
    
    
    //CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
    //batch->addChild(m_emitter);
    //
    
    //CCParticleBatchNode *batch2 = CCParticleBatchNode::createWithTexture(eye2->getTexture());
    //batch2->addChild(eye2);
    
    //gameLayer->addChild(batch, 2000);
    //gameLayer->addChild(batch1, 2001);
    //gameLayer->addChild(batch2, 2001);
    //gameLayer->addChild(m_emitter, z1+1);
}