#include "Effect.h"

enum
{
    z1 = 200,
};

void Effect::Init(Effect* effect, Puzzle* layer)
{
    pThis = effect;
    gameLayer = layer;
    
    circle = NULL;
    circle_fire = NULL;
    circle_water = NULL;
    circle_land = NULL;
    
    sp_fire = NULL;
    A8_icon = NULL;
    spirit_fire = NULL;
    spirit_water = NULL;
    spirit_land = NULL;
    m_orb = NULL;
    m_W8_bg = NULL;
    F8_bg = NULL;
    m_F8_fountain = NULL;
    W7_bg = NULL;
    iced_bar = NULL;
    timerLayer = NULL;
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
        //case 100: PlayEffect_Spirit(); break;
        case -1: PlayEffect_CycleOnly(skillNum, pos); break;
        case -9: PlayEffect_CycleOnly(skillNum, pos); break;
        case -17: PlayEffect_CycleOnly(skillNum, pos); break;
        case 0:  PlayEffect_0(pos); break;
        case 8:  PlayEffect_8(pos); break;
        case 16: PlayEffect_16(pos); break;
            
        case 1:  PlayEffect_1(pos); break;
        case 9:  PlayEffect_9(pos, queue_pos); break;
        case 17: PlayEffect_17(pos); break;
            
        //case 5: PlayEffect_5(pos); break;
        case 13: PlayEffect_13(pos); break;
        case 21: PlayEffect_21(pos); break;
            
        case 2: PlayEffect_2(pos, queue_pos); break; // 10개이상 추가점수
        case 19: PlayEffect_19(pos, queue_pos); break; // 10개이상 추가별사탕
            
        case 10: PlayEffect_10(); break;
        case 11: PlayEffect_11(); break;
            
        case 14: PlayEffect_14(); break;
            
        //case 7: PlayEffect_7(skillNum, pos, queue_pos); break;
        case 15: PlayEffect_15(skillNum, pos, queue_pos); break;
        case 23: PlayEffect_23(skillNum, pos, queue_pos); break;
            
        case 6: PlayEffect_6(skillNum); break;
            
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
        
        sp_fire = CCSprite::create("particles/fire.png");
        CCParticleSystem* par = CCParticleFlower::create();
        //par->retain();
        par->setTexture(sp_fire->getTexture());
        
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

void Effect::PlayEffect_MagicCircle(int skillNum)
{
    if (circle == NULL)
    {
        circle = CCParticleSystemQuad::create("particles/magic_circle.plist");
        circle->setAnchorPoint(ccp(0.5, 0.5));
        circle->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120));
        circle->setScaleX(3.6f);
        circle->setScaleY(1.2f);
        circle->setDuration(0.5f); // 1초 고정
        gameLayer->addChild(circle, z1);
        
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_MagicCircle_Callback), this), NULL);
        circle->setTag(0);
        circle->runAction(action);
    }
    
    if (skillNum < 8 && circle_fire == NULL)
    {
        circle_fire = CCParticleSystemQuad::create("particles/circle_fire.plist");
        circle_fire->setAnchorPoint(ccp(0.5, 0.5));
        circle_fire->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120));
        gameLayer->addChild(circle_fire, 5);
        
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(3.0f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_MagicCircle_Callback), this), NULL);
        circle_fire->setTag(1);
        circle_fire->runAction(action);
        
    }
    if (skillNum >= 8 && skillNum < 16 && circle_water == NULL)
    {
        circle_water = CCParticleSystemQuad::create("particles/circle_water.plist");
        circle_water->setAnchorPoint(ccp(0.5, 0.5));
        circle_water->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120));
        circle_water->setScale(2.0f);
        gameLayer->addChild(circle_water, 5);
        
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.15f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_MagicCircle_Callback), this), NULL);
        circle_water->setTag(2);
        circle_water->runAction(action);
    }
    if (skillNum >= 16 && skillNum < 24 && circle_land == NULL)
    {
        // 아직 없음
    }
}
void Effect::PlayEffect_MagicCircle_Callback(CCNode* sender, void* pointer)
{
    Effect* ef = (Effect*)pointer;
    int tag = sender->getTag();
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    
    if (tag == 0) ef->circle = NULL;
    else if (tag == 1) ef->circle_fire = NULL;
    else if (tag == 2) ef->circle_water = NULL;
}

void Effect::PlayEffect_SkillIcon(int skillNum)
{
    int num;
    if (skillNum < 8) num = skillNum+21;
    else if (skillNum < 16) num = skillNum+3;
    else if (skillNum < 24) num = skillNum+15;
    char name[40];
    sprintf(name, "skill_%d.png", num);
    CCSprite* skill = CCSprite::createWithSpriteFrameName(name);
    skill->setScale(1.0f);
    skill->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120+30));
    skill->setOpacity(0);
    gameLayer->addChild(skill, z1);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCSpawn::create(CCFadeOut::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_SkillIcon_Callback), this), NULL);
    skill->runAction(action);
}
void Effect::PlayEffect_SkillIcon_Callback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
}

void Effect::PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos)
{
    // 태양열, 파도, 클로버
    CCSprite* sp_cycle;
    if (skillNum == -1)
        sp_cycle = CCSprite::createWithSpriteFrameName("icon/cycle_sun.png");
    else if (skillNum == -9)
        sp_cycle = CCSprite::createWithSpriteFrameName("icon/cycle_sea.png");
    else
        sp_cycle = CCSprite::createWithSpriteFrameName("icon/cycle_clover.png");
    
    sp_cycle->setAnchorPoint(ccp(0.5, 0.5));
    sp_cycle->setPosition(gameLayer->SetTouch8Position((int)pos[0].x, (int)pos[0].y));
    sp_cycle->setScale(0.5f);
    sp_cycle->setOpacity(0);
    gameLayer->addChild(sp_cycle, z1);
    
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.35f, 1.0f), CCSequence::create( CCFadeIn::create(0.30f), CCFadeOut::create(0.05f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
    sp_cycle->runAction(action);
}

void Effect::SetSpirit(int type)
{
    CCActionInterval* action = CCSequence::create(CCFadeIn::create(0.7f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::SetSpirit_Callback), this), NULL );
    
    if (type == 0)
    {
        spirit_fire = CCSprite::createWithSpriteFrameName("icon/spirit_fire.png");
        spirit_fire->setPosition(gameLayer->SetTouch8Position(0, ROW_COUNT-1));
        spirit_fire->setOpacity(0);
        gameLayer->addChild(spirit_fire, 100);
        spirit_fire->runAction(action);
    }
    else if (type == 1)
    {
        spirit_water = CCSprite::createWithSpriteFrameName("icon/spirit_water.png");
        spirit_water->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, ROW_COUNT-1));
        spirit_water->setOpacity(0);
        gameLayer->addChild(spirit_water, 100);
        spirit_water->runAction(action);
    }
    else if (type == 2)
    {
        spirit_land = CCSprite::createWithSpriteFrameName("icon/spirit_land.png");
        spirit_land->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, 0));
        spirit_land->setOpacity(0);
        gameLayer->addChild(spirit_land, 100);
        spirit_land->runAction(action);
    }
    
    /*
    if (type == 0) // 불의 정령
    {
        fire = CCParticleSystemQuad::create("particles/fire2.plist");
        //fire = CCParticleSystemQuad::create("particles/spirit_fire.plist");
        fire->setPosition(gameLayer->SetTouch8Position(0, ROW_COUNT-1));
        fire->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(fire, 100);
    }
    else if (type == 1) // 물의 정령
    {
        water = CCParticleSystemQuad::create("particles/water.plist");
        water->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, ROW_COUNT-1));
        //water->retain();
        water->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(water, 100);
    }
    else // 땅의 정령
    {
        land = CCParticleSystemQuad::create("particles/land.plist");
        CCPoint p =gameLayer->SetTouch8Position(COLUMN_COUNT-1, 0);
        land->setPosition(ccp(p.x, p.y-20));
        //land->retain();
        land->setAnchorPoint(ccp(0.5, 0.5));
        gameLayer->addChild(land, 100);
    }
     */
}
void Effect::SetSpirit_Callback(CCNode* sender, void* pointer)
{
    sender->runAction( CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.5f, 0.95f), CCScaleTo::create(0.5f, 1.05f), NULL)) );
}

CCSprite* Effect::GetSpirit(int type)
{
    if (type == 0) return spirit_fire;
    else if (type == 1) return spirit_water;
    else return spirit_land;
}

void Effect::RemoveSpirit(int type)
{
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::RemoveSpirit_Callback), this), NULL);
    GetSpirit(type)->setTag(type);
    GetSpirit(type)->runAction(action);
}
void Effect::RemoveSpirit_Callback(CCNode* sender, void* pointer)
{
    int type = sender->getTag();
    sender->removeFromParentAndCleanup(true);
    sender = NULL;
    /*
    if (type == 0) spirit_fire = NULL;
    else if (type == 1) spirit_water = NULL;
    else if (type == 2) spirit_land = NULL;
    */
}

/*
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
    switch (type)
    {
        case 0:
            fire->removeFromParentAndCleanup(true);
            fire = NULL;
            break;
        case 1:
            water->removeFromParentAndCleanup(true);
            water = NULL;
            break;
        case 2:
            land->removeFromParentAndCleanup(true);
            land = NULL;
            break;
    }
}
*/

void Effect::PlayEffect_0(std::vector<CCPoint> pos)
{
    // 불1 : red 기본 추가점수 이펙트
    isDone[0] = true;
    
    int x, y;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        sp_fire = CCSprite::create("particles/fire.png");
        CCParticleSystem* par = CCParticleFire::create();
        //par->retain();
        par->setTexture(sp_fire->getTexture());
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
        sp_fire = CCSprite::create("particles/fire.png");
        CCParticleSystem* par = CCParticleGalaxy::create();
        par->retain();
        par->setTexture(sp_fire->getTexture());
        
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetTouch8Position(x, y));
        par->setLife(1);

        par->setStartSize(120);
        par->setSpeed(200);
        par->setSpeedVar(150);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.95f;
        startColor.g = 0.05f;
        startColor.b = 0.05f;
        par->setStartColor(startColor);
        
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
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water1.plist");
        //m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(1.0f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
        
        /*
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
         */
    }
}


void Effect::PlayEffect_9(std::vector<CCPoint> pos, int queue_pos)
{
    // 물2A : blue 사이클 주변부 이펙트
    
    skillPos.clear();
    skillPos = pos;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    
    // 터뜨릴 개수가 없으면 바로 Falling을 시작하자.
    if ((int)pos.size() <= 1)
    {
        gameLayer->Falling(queue_pos);
        return;
    }
    
    sp_fire = CCSprite::create("particles/fire.png");
    CCParticleSystem* par = CCParticleGalaxy::create();
    par->setTexture(sp_fire->getTexture());
    par->setAnchorPoint(ccp(0.5, 0.5));
    
    CCPoint first = gameLayer->SetTouch8Position((int)pos[1].x, (int)pos[1].y);
    par->setPosition(ccp((int)first.x-(int)pos[0].x, (int)first.y-(int)pos[0].y));
    
    par->setLife(1);
    par->setStartSize(200);
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
    
    callbackCnt = 1;
    
    // sound
    gameLayer->GetSound()->PlaySkillSound(9);
    
    CCFiniteTimeAction* action = CCSequence::create(CCBezierBy::create(0.2f, GetBezierConfig(this, 1)), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect9Callback), this), NULL);
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
    bezier.controlPoint_1 = ccp(x, y);
    bezier.controlPoint_2 = ccp(x, y);
    bezier.endPosition = deltaPos;
    return bezier;
}

void Effect::Effect9Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    
    // bomb piece
    CCSprite* piece = pThis->gameLayer->GetSpriteP8((int)pThis->skillPos[pThis->callbackCnt].x, (int)pThis->skillPos[pThis->callbackCnt].y);
    if (piece != NULL)
    {
        CCFiniteTimeAction* bomb = CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
        piece->runAction(bomb);
        
        CCLog("%d %d", (int)pThis->skillPos[pThis->callbackCnt].x, (int)pThis->skillPos[pThis->callbackCnt].y);
        
        // 물 사이클 스킬은 Puzzle의 Bomb함수를 쓰지 않기 때문에, 여기서 개수를 cnt해야 한다.
        pThis->gameLayer->UpdatePieceBombCnt( pThis->gameLayer->GetPuzzleP8Set()->GetType((int)pThis->skillPos[pThis->callbackCnt].x, (int)pThis->skillPos[pThis->callbackCnt].y), 1 );
    }
    
    if (pThis->callbackCnt < (int)pThis->skillPos.size()-1)
    {
        pThis->callbackCnt++;
        
        // sound
        if (pThis->callbackCnt % 2 == 1)
            pThis->gameLayer->GetSound()->PlaySkillSound(9);
        
        int mul = (pThis->callbackCnt % 2 == 0) ? -1 : 1;
        CCFiniteTimeAction* action = CCSequence::create(CCBezierBy::create(0.2f, GetBezierConfig(pThis, mul)), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect9Callback), (void*)pThis), NULL);
        sender->runAction(action);
    }
    else
    {
        int x, y;
        for (int i = 1 ; i < pThis->skillPos.size() ; i++)
        {
            x = (int)pThis->skillPos[i].x;
            y = (int)pThis->skillPos[i].y;
            CCLog("[[%d %d]]", x, y);
            //if (pThis->gameLayer->GetPuzzleP8Set()->GetObject(x, y)->GetPiece() != NULL)
            if (pThis->gameLayer->GetSpriteP8(x, y) != NULL)
            {
                CCLog("(%d %d)", x, y);
                pThis->gameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
                pThis->gameLayer->SetSpriteP8Null(x, y);
            }
        }
        pThis->skillPos.clear();
        
        ((CCParticleSystem*)sender)->setDuration(0.1);
        ((CCParticleSystem*)sender)->setAutoRemoveOnFinish(true);
        
        pThis->gameLayer->Falling(pThis->queuePos);
    }
}

void Effect::PlayEffect_17(std::vector<CCPoint> pos)
{
    gameLayer->GetSound()->PlaySkillSound(17);
    
    // 땅2A : green 사이클 주변부 이펙트
    int x, y;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land2.plist");
        //m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
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
        CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
        first->runAction(action);
        
        //// particle ////
        sp_fire = CCSprite::create("particles/fire.png");
        CCParticleSystem* par = CCParticleFlower::create();
        //par->retain();
        par->setTexture(sp_fire->getTexture());
        
        par->setAnchorPoint(ccp(0.5, 0.5));
        par->setPosition(gameLayer->SetTouch8Position(x, y));
        par->setLife(0.5);
        
        par->setSpeed(300);
        par->setSpeedVar(150);
        
        ccColor4F startColor = par->getStartColor();
        startColor.r = 0.1f;
        startColor.g = 0.9f;
        startColor.b = 0.1f;
        par->setStartColor(startColor);
        
        par->setDuration(0.15f);
        par->setAutoRemoveOnFinish(true);
        
        gameLayer->addChild(par, z1);
    }
}


//void Effect::PlayEffect_5(std::vector<CCPoint> pos)
void Effect::PlayEffect_5(int x, int y)
{
    //CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire6_3.plist");
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/LavaFlow.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
    m_emitter->setScale(1.5f);
    gameLayer->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
    
    /*
    // 6개 이상 한번 더 (F6 : 불꽃놀이)
    int x, y;
    CCPoint p;
    
    multi_callbackCnt.clear();
    for (int i = 0; i < pos.size(); i++)
        multi_callbackCnt.push_back(0);
    
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        //CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/LavaFlow.plist");
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire6_3.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(4.0f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
     */
}
void Effect::PlayEffect_5_Callback(CCNode* sender, void* pointer)
{
    Effect* ef = (Effect*)pointer;
    
    int idx = ((CCParticleSystemQuad*)sender)->getTag();
    ef->multi_callbackCnt[idx]++;
    if (ef->multi_callbackCnt[idx] < 3)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire6.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        CCPoint p = ((CCParticleSystemQuad*)sender)->getPosition();
        if (ef->multi_callbackCnt[idx] == 1)
            m_emitter->setPosition(ccp(p.x-110, p.y-50));
        else if (ef->multi_callbackCnt[idx] == 2)
            m_emitter->setPosition(ccp(p.x+60, p.y-50));
        //else
        //    m_emitter->setPosition(ccp(p.x-60, p.y-40));
        m_emitter->setScale(5.0f);
        m_emitter->setTag(idx);
        ef->gameLayer->addChild(m_emitter, 2000);
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.2f), CCCallFuncND::create(ef->gameLayer, callfuncND_selector(Effect::PlayEffect_5_Callback), ef), NULL);
        m_emitter->runAction(action);
        m_emitter->setAutoRemoveOnFinish(true);
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
        //m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ccp((int)p.x, (int)p.y+20));
        m_emitter->setScale(1.5f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
}
void Effect::PlayEffect_21(std::vector<CCPoint> pos)
{
    // E6 : 그린 피스 6개 이상 한 번 더
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land6.plist");
        //m_emitter->retain();
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


void Effect::PlayEffect_7(std::vector< std::vector<CCPoint> > pos_d, std::vector<CCPoint> pos, int queue_pos)
{
    //callbackCnt = 0;
    cbCnt.clear();
    skillPos = pos;
    skillDoublePos = pos_d;
    queuePos = queue_pos;
    F8_bomb_cbCnt.clear();
    for (int i = 0 ; i < pos.size() ; i++)
        F8_bomb_cbCnt.push_back(0);
    
    // 어두운 배경
    F8_bg = NULL;
    F8_bg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, gameLayer->m_winSize.width, gameLayer->m_winSize.height));
    F8_bg->setPosition(ccp(0, 0));
    F8_bg->setAnchorPoint(ccp(0, 0));
    F8_bg->setColor(ccc3(0, 0, 0));
    F8_bg->setOpacity(0);
    gameLayer->addChild(F8_bg, z1);
    CCActionInterval* action = CCFadeTo::create(0.5f, 127);
    F8_bg->runAction(action);
    
    // 마법진 위에 용 출현
    A8_icon = CCSprite::createWithSpriteFrameName("icon/dragon.png");
    A8_icon->setAnchorPoint(ccp(0.5, 0));
    A8_icon->setOpacity(0);
    int A8_icon_height = (gameLayer->vo.y+gameLayer->vs.height-50-50-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    A8_icon->setScale( (float)A8_icon_height / (float)A8_icon->getContentSize().height );
    A8_icon->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120- 100));
    gameLayer->addChild(A8_icon, z1);
    
    CCActionInterval* action2 = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.8f, ccp(0, 100)), CCSequence::create(CCFadeIn::create(0.4f), CCFadeOut::create(0.4f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect7_Callback_1), this), NULL);
    A8_icon->runAction(action2);
}


Effect* eff;

void Effect::Effect7_Callback_1(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true); // 용 삭제
    Effect* ef = (Effect*)pointer;
    
    eff = ef;
    
    // fountain effect 출현
    ef->m_F8_fountain = CCParticleSystemQuad::create("particles/fire8_fountain.plist");
    //ef->m_F8_fountain->retain();
    ef->m_F8_fountain->setAnchorPoint(ccp(0.5, 0.5));
    ef->m_F8_fountain->setPosition(ccp(ef->gameLayer->m_winSize.width/2, ef->gameLayer->vo.y+ef->gameLayer->tbSize.height+ef->gameLayer->boardSize.height+120));
    ef->m_F8_fountain->setScale(2.0f);
    ef->gameLayer->addChild(ef->m_F8_fountain, 1500);
    
    // 혜성 시작
    ef->callbackCnt = 0;
    ef->F8_finishCnt = 0;
    
    //ef->gameLayer->schedule(schedule_selector(Effect::Effect7_Comet), 0.2f, (int)ef->skillPos.size()-1, 0);
    
    ef->gameLayer->GetSkill()->F8_Timer();
}

void Effect::Effect7_Comet(float f)
{
    /*
    CCLog("timer callback cnt : %d", eff->callbackCnt);
    if (eff->gameLayer->IsPaused())
        return;
    Effect7_Callback_2(NULL, eff);
    */
}

void Effect::Effect7_Callback_2(int idx, void* pointer) // 혜성 떨구기
{
    Effect* ef = (Effect*)pointer;
    
    //CCLog("혜성 callback cnt : %d", idx);
    int x = (int)ef->skillPos[idx].x;
    int y = (int)ef->skillPos[idx].y;
    CCPoint pos = ef->gameLayer->SetTouch8Position(x, y);
    
    // 혜성이 떨어진다~!
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire8_comet.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition((int)pos.x+400, (int)pos.y+1920);
    m_emitter->setScale(2.0f);
    ef->gameLayer->addChild(m_emitter, 1500);
    m_emitter->setTag(idx);
    
    CCActionInterval* action = CCSequence::create( CCMoveTo::create(0.5f, pos), CCCallFuncND::create(ef->gameLayer, callfuncND_selector(Effect::Effect7_Callback_3), ef), NULL);
    m_emitter->runAction(action);
    
    ef->gameLayer->GetSound()->PlayDesignatedSound(70);
}

void Effect::Effect7_Callback_3(CCNode* sender, void* pointer) // 혜성 떨어지고 폭파
{
    int idx = sender->getTag();
    
    // 혜성 삭제
    ((CCParticleSystemQuad*)sender)->setDuration(0.1f);
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    
    Effect* ef = (Effect*)pointer;

    int x = (int)ef->skillPos[idx].x;
    int y = (int)ef->skillPos[idx].y;
    //CCLog("Effect : bomb start = %d (%d, %d)", (int)idx, x, y);
    
    /*if (ef->gameLayer->GetSpriteP8(x, y) == NULL)
    {
        CCLog("빠자! (%d, %d)", x, y);
        return;
    }*/
    
    if (ef->gameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
    {
        // big sound
        ef->gameLayer->GetSound()->PlayDesignatedSound(72);
        
        CCLog("big fire : %d %d", x, y);
        for (int i = 0 ; i < ef->skillDoublePos[idx].size() ; i++)
        {
            x = (int)ef->skillDoublePos[idx][i].x;
            y = (int)ef->skillDoublePos[idx][i].y;
            if (ef->gameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
            {
                CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire8_bigfire.plist");
                m_emitter->setAnchorPoint(ccp(0.5, 0.5));
                m_emitter->setPosition(ef->gameLayer->SetTouch8Position(x, y));
                m_emitter->setScale(2.0f);
                m_emitter->setAutoRemoveOnFinish(true);
                ef->gameLayer->addChild(m_emitter, 1500);
            }
        }
    }
    else
    {
        // small sound
        ef->gameLayer->GetSound()->PlayDesignatedSound(71);
        
        CCLog("small fire : %d %d", x, y);
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire8_smallfire.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ef->gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(2.0f);
        m_emitter->setAutoRemoveOnFinish(true);
        ef->gameLayer->addChild(m_emitter, 1500);
    }
    
    // 폭파 개수 갱신
    //ef->gameLayer->UpdatePieceBombCnt(ef->gameLayer->GetPuzzleP8Set()->GetType(x, y), (int)ef->skillDoublePos[idx].size());
    
    // 폭파!
    //CCLog("bomb (%d) : size = %d", idx, (int)ef->skillDoublePos[idx].size());
    //CCLog("폭파! (%d)", idx);
    //ef->gameLayer ->Bomb(ef->queuePos, ef->skillDoublePos[idx], idx);
    ef->gameLayer->GetSkill()->F8_Bomb(ef->queuePos, ef->skillDoublePos[idx], idx);
}
std::vector<CCPoint> Effect::GetDoublePos(int idx)
{
    return skillDoublePos[idx];
}

void Effect::Effect7_Clear()
{
    m_F8_fountain->setDuration(0.01f);
    m_F8_fountain->setAutoRemoveOnFinish(true);
    F8_bg->removeFromParentAndCleanup(true);
}
void Effect::Effect7_Callback_4(cocos2d::CCNode *sender, void *pointer)
{
    Effect* ef = (Effect*)pointer;
    int idx = ((CCSprite*)sender)->getTag();
    
    ef->F8_bomb_cbCnt[idx]++;
    if (ef->F8_bomb_cbCnt[idx] == (int)ef->skillDoublePos[idx].size())
    {
        for (int i = 0 ; i < ef->skillDoublePos[idx].size() ; i++)
        {
            ef->gameLayer->GetPuzzleP8Set()->RemoveChild((int)ef->skillDoublePos[idx-1][i].x, (int)ef->skillDoublePos[idx-1][i].y);
            ef->gameLayer->SetSpriteP8Null((int)ef->skillDoublePos[idx-1][i].x, (int)ef->skillDoublePos[idx-1][i].y);
        }
        
        // 완전히 끝
        ef->F8_finishCnt++;
        CCLog("끝? %d %d", ef->F8_finishCnt, (int)ef->skillPos.size());
        if (ef->F8_finishCnt == (int)ef->skillPos.size())
        {
            ef->gameLayer->Falling((int)ef->queuePos);
            
            ef->m_F8_fountain->setDuration(0.01f);
            ef->m_F8_fountain->setAutoRemoveOnFinish(true);
            ef->F8_bg->removeFromParentAndCleanup(true);
        }
    }
}


void Effect::PlayEffect_15(int num, std::vector<CCPoint> pos, int queue_pos) // '여신의 은총'
{
    // 어두운 배경
    F8_bg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, gameLayer->m_winSize.width, 500));
    F8_bg->setPosition(ccp(0, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+15+gameLayer->floorSize.height));
    F8_bg->setAnchorPoint(ccp(0, 0));
    F8_bg->setColor(ccc3(0,0,0));
    gameLayer->addChild(F8_bg, 5);
    F8_bg->runAction(CCFadeTo::create(0.5f, 255));
    
    // 전체 배경 효과
    m_W8_bg = CCParticleSystemQuad::create("particles/water8_bg.plist");
    //m_emitter->retain();
    m_W8_bg->setAnchorPoint(ccp(0.5, 0));
    m_W8_bg->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+190+100));
    m_W8_bg->setScale(1.0f);
    gameLayer->addChild(m_W8_bg, 6);
    
    // 마법진 위에 여신 출현
    // (소리)
    gameLayer->GetSound()->PlayDesignatedSound(150);
    A8_icon = CCSprite::createWithSpriteFrameName("icon/goddess.png");
    A8_icon->setAnchorPoint(ccp(0.5, 0));
    A8_icon->setOpacity(0);
    int A8_icon_height = (gameLayer->vo.y+gameLayer->vs.height-50-50-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    A8_icon->setScale( (float)A8_icon_height / (float)A8_icon->getContentSize().height );
    A8_icon->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120- 100));
    gameLayer->addChild(A8_icon, z1);

    CCActionInterval* action2 = CCSpawn::create(CCMoveBy::create(0.7f, ccp(0, 100)), CCFadeIn::create(0.7f), NULL);
    A8_icon->runAction(action2);
    
    // orb 출현
    m_orb = CCParticleSystemQuad::create("particles/water8_orb.plist");
    m_orb->setAnchorPoint(ccp(0.5, 0));
    orb_pos = ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120+A8_icon_height);
    m_orb->setPosition(orb_pos);
    m_orb->setScale(1.0f);
    m_orb->setStartSize(100);
    gameLayer->addChild(m_orb, z1);
}
void Effect::Effect15_Last(std::vector<CCPoint> pos, void* pointer)
{
    Effect* ef = (Effect*)pointer;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water8_piece.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ef->gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        m_emitter->setScale(2.5f);
        m_emitter->setDuration(0.35f);
        ef->gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
}
void Effect::Effect15_Bomb(std::vector<CCPoint> pos, void* pointer)
{
    Effect* ef = (Effect*)pointer;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water8_piece.plist");
        //m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ef->gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        m_emitter->setScale(2.5f);
        ef->gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
        
        CCParticleSystemQuad* m_move = CCParticleSystemQuad::create("particles/water8_move.plist");
        //m_emitter->retain();
        m_move->setAnchorPoint(ccp(0.5, 0.5));
        m_move->setPosition(ef->gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        m_move->setScale(2.0f);
        ef->gameLayer->addChild(m_move, 2001);
        
        CCActionInterval* action = CCSequence::create( CCEaseIn::create(CCMoveTo::create(0.5f, orb_pos), 0.5f), CCCallFuncND::create(ef->gameLayer, callfuncND_selector(Effect::Effect15_Callback), ef), NULL );
        m_move->runAction(action);
    }
}
void Effect::Effect15_Callback(CCNode* sender, void* pointer)
{
    ((CCParticleSystemQuad*)sender)->setDuration(0.05f);
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
}
void Effect::AddOrbMaxParticle(int v)
{
    int res = std::min((int)m_orb->getStartSize()+v, 100);
    m_orb->setStartSize(res);
    //CCLog("size = %d", (int)m_orb->getStartSize());
}
void Effect::Effect15_Clear()
{
    // 여신 나오는 소리
    gameLayer->GetSound()->PlayDesignatedSound(151);
    
    F8_bg->removeFromParentAndCleanup(true);
    m_W8_bg->removeFromParentAndCleanup(true);
    A8_icon->removeFromParentAndCleanup(true);
    m_orb->removeFromParentAndCleanup(true);
}


void Effect::PlayEffect_23(int num, std::vector<CCPoint> pos, int queue_pos) // '고대나무'
{
    // 나무
    // 나무 나타나는 소리
    gameLayer->GetSound()->PlayDesignatedSound(230);
    A8_icon = CCSprite::createWithSpriteFrameName("icon/tree.png");
    A8_icon->setAnchorPoint(ccp(0.5, 0));
    A8_icon->setOpacity(0);
    int A8_icon_height = (gameLayer->vo.y+gameLayer->vs.height-50-50-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    A8_icon->setScale( (float)A8_icon_height / (float)A8_icon->getContentSize().height );
    A8_icon->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120- 100));
    gameLayer->addChild(A8_icon, z1);
    
    // 나무 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.7f, ccp(0, 100)), CCFadeIn::create(0.7f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect23_Callback), this), NULL);
    A8_icon->runAction(action);
    
    // 배경 파티클
    m_W8_bg = CCParticleSystemQuad::create("particles/land8_bg.plist");
    m_W8_bg->setAnchorPoint(ccp(0.5, 0));
    m_W8_bg->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+190+100));
    m_W8_bg->setScale(1.0f);
    gameLayer->addChild(m_W8_bg, 6);
}
void Effect::Effect23_Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    pThis->gameLayer->GetSkill()->E8_Start();
}
void Effect::Effect23_Bomb(int x)
{
    // 폭발할 때 이펙트
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land8.plist");
    //m_emitter->retain();
    m_emitter->setPosition(gameLayer->SetTouch8Position(x, 3));
    m_emitter->setScaleX(1.4f);
    m_emitter->setScaleY(1.4f);
    gameLayer->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
}

void Effect::Effect23_Clear()
{
    m_W8_bg->setDuration(0.1f);
    m_W8_bg->setAutoRemoveOnFinish(true);
    //A8_icon->removeFromParentAndCleanup(true);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.7f, ccp(0, -100)), CCFadeOut::create(0.7f), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect23_Clear_Callback), this), NULL);
    A8_icon->runAction(action);
}
void Effect::Effect23_Clear_Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}



void Effect::PlayEffect_6(int num)
{
    // F7 : 코코타임 (코코 주위의 링)
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_coco.plist");
    //m_emitter->retain();
    m_emitter->setPosition(ccp(200, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+60+150));
    m_emitter->setScale(1.2f);
    gameLayer->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
}
void Effect::PlayEffect_6_Fire(std::vector< std::vector<CCPoint> > pos, int queue_pos, int cnt)
{
    // F7 : 코코타임 (덩어리에 생기는 이펙트들)
    // 1회 : fire7_1, 2회 : fire7_2, 3회부터 : fire7_3 + (덩어리 기준 이펙트 1개 공통)
    
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    callbackCnt = 0;
    
    Effect6Callback(NULL, this);
}
void Effect::Effect6Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    
    // 이전 덩어리 이펙트 삭제
    if (pThis->callbackCnt > 0)
    {
        ((CCParticleSystemQuad*)sender)->setDuration(0.01f);
        ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    }
    
    if (pThis->callbackCnt < pThis->gameLayer->GetSkill()->GetResultDouble().size() &&
        pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt].size() > 0)
    {
        int x, y;
        int minx = 999, miny = 999, maxx = -1, maxy = -1;
        for (int i = 0 ; i < pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt].size() ; i++)
        {
            x = pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt][i].x;
            y = pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt][i].y;
            minx = std::min(minx, x);
            miny = std::min(miny, y);
            maxx = std::max(maxx, x);
            maxy = std::max(maxy, y);
            
            CCParticleSystemQuad* m_emitter;
            if (pThis->callbackCnt == 0)
                m_emitter = CCParticleSystemQuad::create("particles/fire7_1.plist");
            else if (pThis->callbackCnt == 1)
                m_emitter = CCParticleSystemQuad::create("particles/fire7_2.plist");
            else
                m_emitter = CCParticleSystemQuad::create("particles/fire7_3.plist");
            //m_emitter->retain();
            m_emitter->setAnchorPoint(ccp(0.5, 0.5));
            m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(x, y));
            m_emitter->setScale(1.5f);
            m_emitter->setAutoRemoveOnFinish(true);
            pThis->gameLayer->addChild(m_emitter, 2000);
        }
        
        // 폭발 액션
        pThis->gameLayer->Bomb(pThis->queuePos, pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt]);
        pThis->gameLayer->GetSound()->PlayBomb();
        
        pThis->callbackCnt++;
        
        // 덩어리 기준 이펙트 1개
        float xx = (float)(maxx+minx)/2.0;
        float yy = (float)(maxy+miny)/2.0;
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_final.plist");
        //m_emitter->retain();
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(xx, yy));
        m_emitter->setScale(1.0f);
        m_emitter->setAutoRemoveOnFinish(true);
        pThis->gameLayer->addChild(m_emitter, 2000);
        
        CCActionInterval* action = CCSequence::create( CCDelayTime::create(0.25f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect6Callback), pThis), NULL);
        m_emitter->runAction(action);
    }
    else
    {
        // 콜백 끝
        pThis->gameLayer->GetSkill()->F7_Continue(pThis->gameLayer->GetSkill(), pThis->queuePos);
    }
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
    
    deltaPos = gameLayer->SetTouch8Position(x, y);
    callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), this), CCFadeOut::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2Callback), this), NULL), NULL), NULL);
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
        CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), pThis), CCFadeOut::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2Callback), pThis), NULL), NULL), NULL);
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

    deltaPos = gameLayer->SetTouch8Position(x, y);
    callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_starcandy_1.png");
    plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create(CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect19CallbackNewSprite), this), CCFadeOut::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect19Callback), this), NULL), NULL), NULL);
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
        CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect19CallbackNewSprite), pThis), CCFadeOut::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect19Callback), pThis), NULL), NULL), NULL);
        plus->runAction(action);
    }
}


void Effect::PlayEffect_10()
{
    // W3 : 콤보 비례 추가 점수
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire3.plist");
    //m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(150, 1600));

    m_emitter->setScale(3.0f);
    gameLayer->addChild(m_emitter, 2000);
    
    m_emitter->setDuration(0.5f);
    m_emitter->setAutoRemoveOnFinish(true);
}

void Effect::PlayEffect_11()
{
    // W4 : 콤보 비례 추가 별사탕
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water4.plist");
    //m_emitter->retain();
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(150, 1600));
    
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
        
        CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.10f), CCFadeOut::create(0.10f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
        W7Start->runAction(startAction);
        
        if (gameLayer->GetSkill()->IsSpiritAlive(0))
            spirit_fire->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(1))
            spirit_water->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(2))
            spirit_land->pauseSchedulerAndActions();
        
        gameLayer->GetFairyLayer()->pauseSchedulerAndActions();
        
        
        
        // iced-bar
        iced_bar = CCSprite::createWithSpriteFrameName("background/bar_ice.png");
        iced_bar->setAnchorPoint(ccp(0.5, 0));
        iced_bar->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y-35));
        gameLayer->addChild(iced_bar, 5000);
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
        
        CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.10f), CCFadeOut::create(0.10f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
        W7Start->runAction(startAction);
        
        W7_bg->removeFromParentAndCleanup(true);
        iced_bar->removeFromParentAndCleanup(true);
        W7_bg = NULL;
        iced_bar = NULL;
        
        if (gameLayer->GetSkill()->IsSpiritAlive(0))
            spirit_fire->resumeSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(1))
            spirit_water->resumeSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(2))
            spirit_land->resumeSchedulerAndActions();
        
        gameLayer->GetFairyLayer()->resumeSchedulerAndActions();
    }
}
void Effect::Effect14Callback(CCNode* sender, void* data)
{
    sender->removeFromParentAndCleanup(true);
}



void Effect::ShowStarCandy(std::vector<CCPoint> pos)
{
    starCandyCallbackCnt = 3;
    starCandyPos.clear();
    starCandyPos = pos;
    
    for (int i = 3; i < pos.size() ; i++)
    {
        CCSprite* candy = CCSprite::createWithSpriteFrameName("icon/icon_starcandy_4.png");
        candy->setPosition(gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        candy->setOpacity(0);
        candy->setScale(1.0f);
        gameLayer->addChild(candy, 2000);
        
        //CCRepeatForever::create(CCRotateBy::create(0.1f, 90))
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create( CCFadeIn::create(0.1f), CCEaseOut::create(CCMoveTo::create(0.5f, ccp(gameLayer->m_winSize.width/2, gameLayer->vs.height+gameLayer->vo.y-370)), 0.5f), CCFadeOut::create(0.2f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
        candy->runAction(action);
        
        /*
         CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.16f, ccp(0, 50)), CCRotateBy::create(0.16f, 360),
         CCSequence::create(CCFadeIn::create(0.08f),
         CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback), this),
         CCFadeOut::create(0.08f), NULL), NULL),
         CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
         candy->runAction(action);
         */
        
        /*
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
         */
    }
}
void Effect::ShowStarCandy_Callback_Done(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}
/*
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
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.16f, ccp(0, 50)), CCRotateBy::create(0.16f, 360), CCSequence::create(CCFadeIn::create(0.08f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback), pThis), CCFadeOut::create(0.08f), NULL), NULL), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), pThis), NULL);
        candy->runAction(action);
    }
}
*/



void Effect::RemoveAllObjects()
{
    if (spirit_fire != NULL) spirit_fire->removeFromParentAndCleanup(true);
    if (spirit_water != NULL) spirit_water->removeFromParentAndCleanup(true);
    if (spirit_land != NULL) spirit_land->removeFromParentAndCleanup(true);
}


