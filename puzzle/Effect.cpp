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


void Effect::PlayEffect(int skillNum, int queue_pos, std::vector<CCPoint> pos)
{
    switch (skillNum)
    {
        case 100: PlayEffect_Spirit(); break;
        case -1: PlayEffect_CycleOnly(skillNum, pos); break;
        case -9: PlayEffect_CycleOnly(skillNum, pos); break;
        case -17: PlayEffect_CycleOnly(skillNum, pos); break;
        case 0:  PlayEffect_0(pos); break;
        case 8:  PlayEffect_8(pos); break;
        case 16: PlayEffect_16(pos); break;

        case 1:  PlayEffect_1(pos); break;
        case 9:  PlayEffect_9(pos, queue_pos); break;
        case 17: PlayEffect_17(pos); break;
            
        case 5: PlayEffect_5(pos); break;
        case 13: PlayEffect_13(pos); break;
        case 21: PlayEffect_21(pos); break;
            
        case 2: PlayEffect_2(pos, queue_pos); break; // 10개이상 추가점수
        case 19: PlayEffect_19(pos, queue_pos); break; // 10개이상 추가별사탕
            
        case 10: PlayEffect_10(); break;
        case 11: PlayEffect_11(); break;
            
        case 14: PlayEffect_14(); break;
        
        case 7: PlayEffect_7_15_23(skillNum, pos, queue_pos); break;
        case 15: PlayEffect_7_15_23(skillNum, pos, queue_pos); break;
        case 23: PlayEffect_7_15_23(skillNum, pos, queue_pos); break;
        
        case 6: PlayEffect_6(skillNum); break;
//        case -6: PlayEffect_6(skillNum, queue_pos); break;
            
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

        CCSprite* sp = CCSprite::create("particles/fire.png");
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
    // 태양열, 파도, 클로버
    CCLog("skillNUM : 사이클!");
    CCSprite* sprite;
    if (skillNum == -1)
        sprite = CCSprite::createWithSpriteFrameName("icon/cycle_sun.png");
    else if (skillNum == -9)
        sprite = CCSprite::createWithSpriteFrameName("icon/cycle_sea.png");
    else
        sprite = CCSprite::createWithSpriteFrameName("icon/cycle_clover.png");
    
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
        CCSprite* sp = CCSprite::create("particles/fire.png");
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
    // sound
    gameLayer->GetSound()->PlaySkillSound(1);
    
    // 불2A : red 사이클 이펙트
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        CCSprite* sp = CCSprite::create("particles/fire.png");
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
    //queue_pos_now = queue_pos;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    
    // 터뜨릴 개수가 없으면 바로 Falling을 시작하자.
    if ((int)pos.size() <= 1)
    {
        //pThis->gameLayer->Falling(pThis->queue_pos_now);
        pThis->gameLayer->Falling(queue_pos);
        return;
    }

    CCSprite* sp = CCSprite::create("particles/fire.png");
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
    // sound
    pThis->gameLayer->GetSound()->PlaySkillSound(9);
    
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
        
        // sound
        if (pThis->callbackCnt % 2 == 1)
            pThis->gameLayer->GetSound()->PlaySkillSound(9);
            
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
        
        //pThis->gameLayer->Falling(pThis->queue_pos_now);
        pThis->gameLayer->Falling(pThis->queuePos);
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
        CCSprite* sp = CCSprite::create("particles/fire.png");
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
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/LavaFlow.plist");
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
    // W6 : 6개 이상 한번 더
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        CCPoint p = gameLayer->SetTouch8Position(x, y);
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water6.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ccp((int)p.x, (int)p.y+20));
        m_emitter->setScale(1.5f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
    /* 폭포 버전 */
    /*
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water6_2.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->vs.height));
    m_emitter->setScale(1.0f);
    gameLayer->addChild(m_emitter, 10);
    m_emitter->setAutoRemoveOnFinish(true);
    */
}
void Effect::PlayEffect_21(std::vector<CCPoint> pos)
{
    CCLog("이펙트 땅6 : 6개*2 ;; %d", (int)pos.size());
    // E6 : 그린 피스 6개 이상 한 번 더
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        CCLog("%d %d", x, y);
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land6.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
}


void Effect::PlayEffectCallback(CCNode* sender, void* data)
{
    sender->removeAllChildrenWithCleanup(true);
    sender->removeFromParentAndCleanup(true);
}


void Effect::PlayEffect_7_15_23(int num, std::vector<CCPoint> pos, int queue_pos)
{
    skillPos.clear();
    skillPos = pos;
    callbackCnt = 0;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    skillNum = num;
    
    A8_icon = NULL;
    if (num == 7)
    {
        A8_icon = CCSprite::createWithSpriteFrameName("icon/dragon.png");
        A8_icon->setScale(1.2f);
    }
    else if (num == 15)
    {
        A8_icon = CCSprite::createWithSpriteFrameName("icon/goddess.png");
        A8_icon->setScale(1.5f);
    }
    else
    {
        A8_icon = CCSprite::createWithSpriteFrameName("icon/tree.png");
        A8_icon->setScale(0.9f);
    }
    
    //A8_icon->setPosition(gameLayer->GetPuzzleLayer()->getPosition());
    A8_icon->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120+130));
    A8_icon->setOpacity(0);
    gameLayer->addChild(A8_icon, z1);
    
    
    // 기존 한붓그리기 된 위치까지 모두 합친다.
    std::vector<CCPoint> originPos = pThis->gameLayer->GetPiece8xy(true);
    for (int i = 0 ; i < originPos.size() ; i++)
        skillPos.push_back(originPos[i]);
    originPos.clear();
 
    int x, y;
    for (int i = 0 ; i < skillPos.size() ; i++)
    {
        x = (int)skillPos[i].x;
        y = (int)skillPos[i].y;
        
        CCActionInterval* action;
        if (i < (int)pos.size())
        {
            // 적용 대상의 모든 피스마다 '음영처리 및 점점 줄어들기' 액션
            action = CCSequence::create( CCSpawn::create(CCFadeTo::create(0.8f, 100), CCScaleTo::create(0.8f, gameLayer->GetPuzzleP8Set()->GetSprite(x, y)->getScale()*0.9f), NULL),
                CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect7_15_23_Callback), this), NULL);
        }
        else
        {
            // 원래 기존 한붓그리기 된 위치이므로, 그냥 딜레이만 주자.
            action = CCSequence::create( CCDelayTime::create(0.8f),
                        CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect7_15_23_Callback), this), NULL);
        }
        gameLayer->GetPuzzleP8Set()->GetSprite(x, y)->runAction(action);
        
        if (num == 7)
        {
            // 불 모으는 이펙트 실행
            CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_1.plist");
            m_emitter->retain();
            m_emitter->setAnchorPoint(ccp(0.5, 0.5));
            m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
            m_emitter->setScale(2.0f);
            gameLayer->addChild(m_emitter, 2000);
            m_emitter->setAutoRemoveOnFinish(true);
        }
    }
    
    // 용, 여신, 나무에 대한 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCFadeIn::create(0.5f), NULL), CCDelayTime::create(0.3f), CCFadeOut::create(0.5f),
        CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect7_15_23_Icon_Callback), this), NULL);
    A8_icon->runAction(action);
}

void Effect::Effect7_15_23_Icon_Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}

void Effect::Effect7_15_23_Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    
    pThis->callbackCnt++;
    pThis->A8_icon->setOpacity(255 * (pThis->callbackCnt / pThis->skillPos.size()));
    if (pThis->callbackCnt == pThis->skillPos.size())
    {
        std::vector<CCPoint> pos = pThis->gameLayer->GetPiece8xy(true);
        
        bool flag;
        // 터뜨리는 이펙트 실행
        if (pThis->skillNum == 7)
        {
            int x, y;
            for (int i = 0 ; i < pThis->skillPos.size() ; i++)
            {
                x = (int)pThis->skillPos[i].x;
                y = (int)pThis->skillPos[i].y;
                
                flag = true;
                for (int j = 0 ; j < pos.size() ; j++)
                {
                    if ((int)pos[j].x == x && (int)pos[j].y == y)
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_3.plist");
                    m_emitter->retain();
                    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
                    m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(x, y));
                    m_emitter->setScale(1.0f);
                    pThis->gameLayer->addChild(m_emitter, 2000);
                    m_emitter->setAutoRemoveOnFinish(true);
                }
                else
                {
                    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_2.plist");
                    m_emitter->retain();
                    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
                    m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(x, y));
                    m_emitter->setScale(1.0f);
                    pThis->gameLayer->addChild(m_emitter, 2000);
                    m_emitter->setAutoRemoveOnFinish(true);
                }
            }
        }
        
        // 터뜨리기
        pThis->gameLayer->Bomb(pThis->queuePos, pThis->skillPos);
        pos.clear();
    }
    
    /*
    if (pThis->callbackCnt == 0)
    {
        //int x, y;
        //for (int i = 0 ; i < pThis->skillPos.size() ; i++)
        //{
            int x = (int)pThis->skillPos[pThis->callbackCnt].x;
            int y = (int)pThis->skillPos[pThis->callbackCnt].y;
            
            CCSprite* sp = CCSprite::create("particles/fire.png");
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
    */
}

void Effect::PlayEffect_6(int num)
{
    // F7 : 코코타임
    
    //if (num > 0) // (코코 바꾸기 + 코코 주위 링)
   // {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_coco.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ccp(100+100, gameLayer->vs.height+gameLayer->vo.y-500+150));
        m_emitter->setScale(1.2f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    //}
}
void Effect::PlayEffect_6_Fire(std::vector<CCPoint> pos, int queue_pos)
{
    skillPos = pos;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(100+100, gameLayer->vs.height+gameLayer->vo.y-500+150));
    m_emitter->setScale(1.0f);
    gameLayer->addChild(m_emitter, 2000);
    
    CCActionInterval* action = CCSequence::create( CCEaseIn::create(CCMoveTo::create(0.1f, gameLayer->SetTouch8Position((int)pos[0].x, (int)pos[0].y)), 0.1f),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect6Callback), this), NULL);
    m_emitter->runAction(action);
}
void Effect::Effect6Callback(CCNode* sender, void* pointer)
{
    ((CCParticleSystemQuad*)sender)->setDuration(0.1f);
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
   
    Effect* pThis = (Effect*)pointer;
    pThis->gameLayer->Bomb(pThis->queuePos, pThis->skillPos);
    // sound
    pThis->gameLayer->GetSound()->PlayBomb();
}

void Effect::PlayEffect_2(std::vector<CCPoint> pos, int queue_pos)
{
    // F3 : 10개이상 제거 시 추가점수
    int x = (int)pos[pos.size()-1].x;
    int y = (int)pos[pos.size()-1].y;
    if (gameLayer->IsCycle(queue_pos))
    {
        x = (int)pos[0].x;
        y = (int)pos[0].y;
    }
    /*
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire3.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    CCPoint emit_pos = gameLayer->SetTouch8Position(x, y);
    //m_emitter->setPosition((int)emit_pos.x, (int)emit_pos.y-30);
    CCLog("effect F3 === %d %d", x, y);
    //CCParticleBatchNode *batch = CCParticleBatchNode::createWithTexture(m_emitter->getTexture());
    //batch->addChild(m_emitter);
    //m_emitter->setStartColor(ccc4f(0.97, 0.03, 0.03, 1));
    //m_emitter->setEndColor(ccc4f(0.97, 0.03, 0.03, 1));
    //m_emitter->setStartColor(ccc4f(0.1, 0.1, 0.95, 1));
    //m_emitter->setEndColor(ccc4f(0.1, 0.1, 0.95, 0));
    //batch->setScale(3.0f);
    m_emitter->setScale(4.0f);
    gameLayer->addChild(m_emitter, 2000);
    
    
    m_emitter->setDuration(0.5f);
    m_emitter->setAutoRemoveOnFinish(true);
     */
    
    deltaPos = gameLayer->SetTouch8Position(x, y);
    callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create(
                            CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)),
        CCSequence::create( CCFadeIn::create(0.2f),
        CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), this),
                           CCFadeOut::create(0.2f),
        CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2Callback), this), NULL), NULL), NULL);
    plus->runAction(action);
}
void Effect::Effect2Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}
void Effect::Effect2CallbackNewSprite(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    pThis->callbackCnt++;
    if (pThis->callbackCnt < 5)
    {
        CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
        if (pThis->callbackCnt == 1)
            plus->setPosition(ccp(pThis->deltaPos.x-20, pThis->deltaPos.y+20));
        else if (pThis->callbackCnt == 2)
            plus->setPosition(ccp(pThis->deltaPos.x+20, pThis->deltaPos.y+5));
        else if (pThis->callbackCnt == 3)
            plus->setPosition(ccp(pThis->deltaPos.x-10, pThis->deltaPos.y+10));
        else if (pThis->callbackCnt == 4)
            plus->setPosition(ccp(pThis->deltaPos.x+10, pThis->deltaPos.y+15));
        plus->setScale(0.5);
        plus->setOpacity(0);
        pThis->gameLayer->addChild(plus, z1+1);
        CCFiniteTimeAction* action = CCSequence::create(
                    CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)),
                    CCSequence::create( CCFadeIn::create(0.2f),
                CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), pThis),
                                        CCFadeOut::create(0.2f),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2Callback), pThis), NULL), NULL), NULL);
        plus->runAction(action);
    }
}
    

void Effect::PlayEffect_19(std::vector<CCPoint> pos, int queue_pos)
{
    // E4 : 10개이상 제거 시 추가별사탕
    int x = (int)pos[pos.size()-1].x;
    int y = (int)pos[pos.size()-1].y;
    if (gameLayer->IsCycle(queue_pos))
    {
        x = (int)pos[0].x;
        y = (int)pos[0].y;
    }
    /*
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water4.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    CCPoint emit_pos = gameLayer->SetTouch8Position(x, y);
    m_emitter->setPosition((int)emit_pos.x, (int)emit_pos.y-30);
    
    //m_emitter->setStartColor(ccc4f(0.97, 0.03, 0.03, 1));
    //m_emitter->setEndColor(ccc4f(0.97, 0.03, 0.03, 1));
    m_emitter->setScale(4.0f);
    gameLayer->addChild(m_emitter, 4000);
    
    m_emitter->setDuration(0.5f);
    m_emitter->setAutoRemoveOnFinish(true);
     */
    deltaPos = gameLayer->SetTouch8Position(x, y);
    callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_starcandy_1.png");
    plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create(
                CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)),
            CCSequence::create( CCFadeIn::create(0.2f),
            CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect19CallbackNewSprite), this),
                                CCFadeOut::create(0.2f),
            CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect19Callback), this), NULL), NULL), NULL);
    plus->runAction(action);
}
void Effect::Effect19Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}
void Effect::Effect19CallbackNewSprite(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    pThis->callbackCnt++;
    if (pThis->callbackCnt < 5)
    {
        if (pThis->callbackCnt == 4) pThis->callbackCnt = 6;
        char s[27];
        sprintf(s, "icon/icon_starcandy_%d.png", pThis->callbackCnt+1);
        CCSprite* plus = CCSprite::createWithSpriteFrameName(s);
        if (pThis->callbackCnt == 1)
            plus->setPosition(ccp(pThis->deltaPos.x-20, pThis->deltaPos.y+20));
        else if (pThis->callbackCnt == 2)
            plus->setPosition(ccp(pThis->deltaPos.x+20, pThis->deltaPos.y+5));
        else if (pThis->callbackCnt == 3)
            plus->setPosition(ccp(pThis->deltaPos.x-10, pThis->deltaPos.y+10));
        else if (pThis->callbackCnt == 4)
            plus->setPosition(ccp(pThis->deltaPos.x+10, pThis->deltaPos.y+15));
        plus->setScale(0.5);
        plus->setOpacity(0);
        pThis->gameLayer->addChild(plus, z1+1);
        CCFiniteTimeAction* action = CCSequence::create(
                            CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)),
            CCSequence::create( CCFadeIn::create(0.2f),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect19CallbackNewSprite), pThis),
                                CCFadeOut::create(0.2f),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect19Callback), pThis), NULL), NULL), NULL);
        plus->runAction(action);
    }
}


void Effect::PlayEffect_10()
{
    // W3 : 콤보 비례 추가 점수
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire3.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(150, 1600));

    //m_emitter->setStartColor(ccc4f(0.97, 0.03, 0.03, 1));
    //m_emitter->setEndColor(ccc4f(0.97, 0.03, 0.03, 1));
    m_emitter->setScale(3.0f);
    gameLayer->addChild(m_emitter, 2000);
    
    m_emitter->setDuration(0.5f);
    m_emitter->setAutoRemoveOnFinish(true);
}

void Effect::PlayEffect_11()
{
    // W4 : 콤보 비례 추가 별사탕
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water4.plist");
    m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(150, 1600));
    
    //m_emitter->setStartColor(ccc4f(0.97, 0.03, 0.03, 1));
    //m_emitter->setEndColor(ccc4f(0.97, 0.03, 0.03, 1));
    m_emitter->setScale(3.0f);
    gameLayer->addChild(m_emitter, 4000);
    
    m_emitter->setDuration(0.5f);
    m_emitter->setAutoRemoveOnFinish(true);
}

void Effect::PlayEffect_14()
{
    // W7 : 시간을 얼리다
    
    if (gameLayer->GetSkill()->W7GetVar())
    {
        // 번쩍
        CCSprite* W7Start = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
        W7Start->setAnchorPoint(ccp(0,0));
        W7Start->setPosition(ccp(0, 0));
        W7Start->setColor(ccc3(255,255,255));
        W7Start->setOpacity(0);
        gameLayer->addChild(W7Start, 5000);
        
        CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.10f), CCFadeOut::create(0.10f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
        W7Start->runAction(startAction);
        
        if (gameLayer->GetSkill()->IsSpiritAlive(0))
            fire->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(1))
            water->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(2))
            land->pauseSchedulerAndActions();
        
        gameLayer->GetFairyLayer()->pauseSchedulerAndActions();
        
        // iced-bar sound
        gameLayer->GetSound()->PlaySkillSound(14);
        // iced-bar
        iced_bar = CCSprite::createWithSpriteFrameName("background/bar_ice.png");
        iced_bar->setAnchorPoint(ccp(0.5, 0));
        iced_bar->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y));
        gameLayer->addChild(iced_bar, 500);
        /*
        // stencil (타이머의 얼음을 가려주는 것)
        timerLayer = CCLayer::create();
        timerLayer->setPosition(ccp(0, 0));
        timerLayer->addChild(iced_bar, 900);
        
        // 직사각형 clip 만들기
        stencil = CCDrawNode::create();
        CCPoint pos = ccp(iced_bar->getPosition().x-gameLayer->tbSize.width/2, iced_bar->getPosition().y);
        CCSize tb = gameLayer->tbSize;
        CCPoint ver[] = {ccp(pos.x-tb.width, pos.y), ccp(pos.x, pos.y), ccp(pos.x, pos.y+tb.height), ccp(pos.x-tb.width, pos.y+tb.height) };
        
        stencil->drawPolygon(ver, 4, ccc4f(0,0,0,2555), 0, ccc4f(0,0,0,255));
        //stencil->drawDot(pos, gameLayer->tbSize.height/2, ccc4f(0, 0, 0, 255));
        clip = CCClippingNode::create(stencil);
        clip->addChild(timerLayer);
        gameLayer->addChild(clip, 1000);

        // 클립을 오른쪽으로 움직인다. 동시에 layer는 왼쪽으로 움직여야 layer가 정지해 있고, 그림이 점점 나타나듯이 보이게 된다.
        CCActionInterval* moveLayerAction = CCSequence::create(CCMoveBy::create(0.4f, ccp(-gameLayer->tbSize.width, 0)), NULL);
        CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.4f, ccp(gameLayer->tbSize.width, 0)), NULL);
        
        //    CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL)
        clip->runAction(CCEaseOut::create(action, 0.4f));
        timerLayer->runAction(CCEaseOut::create(moveLayerAction, 0.4f));
        */
        
        W7_bg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
        W7_bg->setAnchorPoint(ccp(0,0));
        W7_bg->setPosition(ccp(0, 0));
        W7_bg->setColor(ccc3(0,0,0));
        W7_bg->setOpacity(50);
        gameLayer->addChild(W7_bg, 15);
    }
    else
    {
        // 번쩍
        CCSprite* W7Start = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
        W7Start->setAnchorPoint(ccp(0,0));
        W7Start->setPosition(ccp(0, 0));
        W7Start->setColor(ccc3(255,255,255));
        W7Start->setOpacity(0);
        gameLayer->addChild(W7Start, 5000);
        
        CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.10f), CCFadeOut::create(0.10f),
                    CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
        W7Start->runAction(startAction);
        
        W7_bg->removeFromParentAndCleanup(true);
        iced_bar->removeFromParentAndCleanup(true);
        W7_bg = NULL;
        iced_bar = NULL;
        
        if (gameLayer->GetSkill()->IsSpiritAlive(0))
            fire->resumeSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(1))
            water->resumeSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(2))
            land->resumeSchedulerAndActions();
        
        gameLayer->GetFairyLayer()->resumeSchedulerAndActions();
    }
}
void Effect::Effect14Callback(CCNode* sender, void* data)
{
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




void Effect::ShowStarCandy(std::vector<CCPoint> pos)
{
    starCandyCallbackCnt = 3;
    starCandyPos.clear();
    starCandyPos = pos;
    
    //if (pos.size() >= 4)
    for (int i = 3; i < pos.size() ; i++)
    {
        CCSprite* candy = CCSprite::createWithSpriteFrameName("icon/icon_starcandy_4.png");
        candy->setPosition(gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        candy->setOpacity(0);
        candy->setScale(1.0f);
        gameLayer->addChild(candy, 2000);
        
        //CCRepeatForever::create(CCRotateBy::create(0.1f, 90))
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create( CCFadeIn::create(0.1f), CCEaseOut::create(CCMoveTo::create(1.0f, ccp(gameLayer->m_winSize.width/2, gameLayer->vs.height+gameLayer->vo.y-370)), 1.0f), CCFadeOut::create(0.3f), NULL), NULL),
            CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
        candy->runAction(action);
        
        /*
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.16f, ccp(0, 50)), CCRotateBy::create(0.16f, 360),
            CCSequence::create(CCFadeIn::create(0.08f),
                               CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback), this),
                               CCFadeOut::create(0.08f), NULL), NULL),
            CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
        candy->runAction(action);
         */
        
        // 별 파티클
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/basic_starcandy.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        //m_emitter->setStartColor(ccc4f(0.1, 0.1, 0.95, 1));
        //m_emitter->setEndColor(ccc4f(0.1, 0.1, 0.95, 0));
        m_emitter->setScale(1.5f);
        gameLayer->addChild(m_emitter, 1999);
        
        CCActionInterval* action2 = CCSequence::create( CCFadeIn::create(0.1f), CCEaseOut::create(CCMoveTo::create(1.0f, ccp(gameLayer->m_winSize.width/2, gameLayer->vs.height+gameLayer->vo.y-370)), 1.0f), CCFadeOut::create(0.3f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
        m_emitter->runAction(action2);
        //m_emitter->setDuration(0.2f);
        //m_emitter->setAutoRemoveOnFinish(true);
    }
}
void Effect::ShowStarCandy_Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    pThis->starCandyCallbackCnt++;
    if (pThis->starCandyCallbackCnt < (int)pThis->starCandyPos.size())
    {
        CCSprite* candy = CCSprite::createWithSpriteFrameName("icon/icon_starcandy_4.png");
        candy->setPosition(pThis->gameLayer->SetTouch8Position((int)pThis->starCandyPos[pThis->starCandyCallbackCnt].x, (int)pThis->starCandyPos[pThis->starCandyCallbackCnt].y));
        candy->setOpacity(0);
        candy->setScale(1.0f);
        pThis->gameLayer->addChild(candy, 2000);
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.16f, ccp(0, 50)), CCRotateBy::create(0.16f, 360),
            CCSequence::create(CCFadeIn::create(0.08f),
                       CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback), pThis),
                               CCFadeOut::create(0.08f), NULL), NULL),
            CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), pThis), NULL);
        candy->runAction(action);
        /*
        // 별 파티클
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/basic_starcandy.plist");
        m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(pThis->gameLayer->SetTouch8Position((int)pThis->starCandyPos[pThis->starCandyCallbackCnt].x, (int)pThis->starCandyPos[pThis->starCandyCallbackCnt].y));
        //m_emitter->setStartColor(ccc4f(0.1, 0.1, 0.95, 1));
        //m_emitter->setEndColor(ccc4f(0.1, 0.1, 0.95, 0));
        m_emitter->setScale(2.0f);
        pThis->gameLayer->addChild(m_emitter, 1000);
        
        m_emitter->setDuration(0.2f);
        m_emitter->setAutoRemoveOnFinish(true);
         */
    }
}
void Effect::ShowStarCandy_Callback_Done(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}





