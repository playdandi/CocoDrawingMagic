#include "Effect.h"
#include "PuzzleResult.h"

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
        case 17: PlayEffect_17(pos, queue_pos); break;
            
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
    ////CCLog("PlayEffect_SkillIcon : %d", skillNum);
    // 스킬 실제 고유번호 계산
    int num = SkillInfo::ConvertedToOriginal(skillNum);
    
    char name[40];
    sprintf(name, "skill_%d.png", num);
    CCSprite* skill = CCSprite::createWithSpriteFrameName(name);
    skill->setScale(1.0f);
    skill->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120+30));
    skill->setOpacity(0);
    gameLayer->addChild(skill, z1);
    
    skillIcon_callbackCnt = skillNum; // 스킬 번호 임시저장 (떡갈나무지팡이, 끈질긴생명력 스킬에 이용)
    ////CCLog("PlayEffect_SkillIcon : (callbackCnt = %d)", callbackCnt);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCSpawn::create(CCFadeOut::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_SkillIcon_Callback), this), NULL);
    skill->runAction(action);
}
void Effect::PlayEffect_SkillIcon_Callback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
    
    Effect* pThis = (Effect*)p;
    //CCPoint pos = ccp(pThis->gameLayer->m_winSize.width/2, pThis->gameLayer->vo.y+pThis->gameLayer->tbSize.height+pThis->gameLayer->boardSize.height+120);
    ////CCLog("pos x,y  = %d , %d", (int)pos.x, (int)pos.y);
    ////CCLog("PlayEffect_SkillIcon_Callback : %d", pThis->skillIcon_callbackCnt);
    if (pThis->skillIcon_callbackCnt == 18)
        pThis->PlayEffect_18(ccp(pThis->gameLayer->m_winSize.width/2, pThis->gameLayer->vo.y+pThis->gameLayer->tbSize.height+pThis->gameLayer->boardSize.height+120));
    else if (pThis->skillIcon_callbackCnt == 22)
        pThis->PlayEffect_22(ccp(pThis->gameLayer->m_winSize.width/2, pThis->gameLayer->vo.y+pThis->gameLayer->tbSize.height+pThis->gameLayer->boardSize.height+120));
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
    sp_cycle->setScale(2.0f);
    sp_cycle->setOpacity(0);
    gameLayer->addChild(sp_cycle, z1);
    
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(1.8f, 1.0f), CCSequence::create( CCFadeIn::create(0.30f), CCFadeOut::create(0.05f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
    sp_cycle->runAction(action);
}

void Effect::SetSpirit(int type)
{
    CCActionInterval* action = CCSequence::create(CCFadeIn::create(0.7f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::SetSpirit_Callback), this), NULL );
    
    if (type == 0)
    {
        spirit_fire = CCSprite::createWithSpriteFrameName("icon/spirit_fire.png");
        spirit_fire->setPosition(gameLayer->SetTouch8Position(COLUMN_COUNT-1, 0));
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
        spirit_land->setPosition(gameLayer->SetTouch8Position(0, ROW_COUNT-1));
        spirit_land->setOpacity(0);
        gameLayer->addChild(spirit_land, 100);
        spirit_land->runAction(action);
    }
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
    GetSpirit(type)->resumeSchedulerAndActions();
    GetSpirit(type)->runAction(action);
}
void Effect::RemoveSpirit_Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
    sender = NULL;
}
void Effect::SpiritEffect(int type, int centerX, int centerY)
{
    // 정령을 터치할 때 나타나는 이펙트
    
    if (type == 0)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/spirit_fire.plist");
        m_emitter->setPosition(gameLayer->SetTouch8Position(centerX, centerY));
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setAutoRemoveOnFinish(true);
        gameLayer->addChild(m_emitter, 100);
    }
    else if (type == 1)
    {
        int x, y;
        for (int i = 0 ; i < gameLayer->GetSkill()->GetResult().size() ; i++)
        {
            x = gameLayer->GetSkill()->GetResult()[i].x;
            y = gameLayer->GetSkill()->GetResult()[i].y;
            
            CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/spirit_water.plist");
            m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
            m_emitter->setAnchorPoint(ccp(0.5, 0.5));
            m_emitter->setAutoRemoveOnFinish(true);
            gameLayer->addChild(m_emitter, 100);
        }
    }
    else
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/spirit_land.plist");
        m_emitter->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height/2));
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setAutoRemoveOnFinish(true);
        gameLayer->addChild(m_emitter, 100);
    }
}
void Effect::FadeSpirit(bool fadeIn)
{
    if (gameLayer->GetSkill()->IsSpiritAlive(0))
    {
        if (fadeIn) spirit_fire->runAction(CCFadeIn::create(0.5f));
        else spirit_fire->runAction(CCFadeOut::create(0.5f));
    }
    if (gameLayer->GetSkill()->IsSpiritAlive(1))
    {
        if (fadeIn) spirit_water->runAction(CCFadeIn::create(0.5f));
        else spirit_water->runAction(CCFadeOut::create(0.5f));
    }
    if (gameLayer->GetSkill()->IsSpiritAlive(2))
    {
        if (fadeIn) spirit_land->runAction(CCFadeIn::create(0.5f));
        else spirit_land->runAction(CCFadeOut::create(0.5f));
    }
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
    int x = (int)pThis->skillPos[pThis->callbackCnt].x;
    int y = (int)pThis->skillPos[pThis->callbackCnt].y;
    
    CCSprite* piece = pThis->gameLayer->GetSpriteP8(x, y);
    if (piece != NULL)
    {
        CCFiniteTimeAction* bomb = CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
        piece->runAction(bomb);
        
        // 물 사이클 스킬은 Puzzle의 Bomb함수를 쓰지 않기 때문에, 여기서 개수를 cnt해야 한다.
        pThis->gameLayer->UpdatePieceBombCnt( pThis->gameLayer->GetPuzzleP8Set()->GetType(x, y), 1 );
        
        // 주변 연결피스 제거
        pThis->gameLayer->RemoveConnectPiecesXY(x, y);
    }
    
    // 점수 업데이트
    // 발동점수 + [ SIGMA[k=1~파도폭파수] { [(k)*(10+k*발동점수의 4%)]) } ]
    int basicScore = pThis->gameLayer->GetSkill()->GetBasicSkillScore(9); // 발동점수
    int A2_addedScore = (int)((float)(pThis->callbackCnt)*(10.0f+(float)(pThis->callbackCnt)*((float)(basicScore*4)/100.0f)));
    A2_addedScore += (int)((float)basicScore / (float)(pThis->skillPos.size()-1));
    
    pThis->gameLayer->UpdateScore(1, basicScore+A2_addedScore);
    pThis->gameLayer->ShowSkillScore(basicScore+A2_addedScore, (float)((pThis->callbackCnt-1)*20+100)/100.0f, pThis->queuePos, x, y);
    // scale : 1.0f, 1.2f, 1.4f, ......
    
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
            if (pThis->gameLayer->GetSpriteP8(x, y) != NULL)
            {
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

void Effect::PlayEffect_17(std::vector<CCPoint> pos, int queue_pos)
{
    callbackCnt = 0;
    callbackDone = 0;
    skillPos = pos;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    
    PlayEffect_17_Callback(NULL, this);
}
void Effect::PlayEffect_17_Callback(CCNode* sender, void* p)
{
    Effect* pThis = (Effect*)p;
    int x[3], y[3];
    
    int size;
    if (pThis->skillPos.size() < 5) size = 1;
    else if (pThis->skillPos.size() < 9) size = 2;
    else size = 3;
    pThis->callbackCnt += size;
    
    int numOfBomb = (int)pThis->skillPos.size() / size; // 총 터지는 횟수 (네잎클로버 출현 횟수)
    if ((int)pThis->skillPos.size() % size > 0)
        numOfBomb++;
    
    int cnt = 0;
    if (pThis->callbackDone < pThis->skillPos.size())
    {
        if (size >= 3 && pThis->callbackCnt-3 < pThis->skillPos.size())
        {
            x[cnt] = pThis->skillPos[ pThis->callbackCnt-3 ].x;
            y[cnt] = pThis->skillPos[ pThis->callbackCnt-3 ].y;
            cnt++;
        }
        if (size >= 2 && pThis->callbackCnt-2 < pThis->skillPos.size())
        {
            x[cnt] = pThis->skillPos[ pThis->callbackCnt-2 ].x;
            y[cnt] = pThis->skillPos[ pThis->callbackCnt-2 ].y;
            cnt++;
        }
        if (size >= 1 && pThis->callbackCnt-1 < pThis->skillPos.size())
        {
            x[cnt] = pThis->skillPos[ pThis->callbackCnt-1 ].x;
            y[cnt] = pThis->skillPos[ pThis->callbackCnt-1 ].y;
            cnt++;
        }
        
        // sound
        pThis->gameLayer->GetSound()->PlaySkillSound(17);
        
        // 점수 업데이트
        // 최종점수 = 발동점수 + [(drag수-1)*(10+drag수*발동점수의 3%)])
        int dragNum = pThis->gameLayer->GetPiece8xy(true).size(); // 드래그수 구하기
        if (pThis->gameLayer->IsRoundInFeverTime(true))
            dragNum = pThis->gameLayer->GetPosForFeverTime(true).size();
        int basicScore = pThis->gameLayer->GetSkill()->GetBasicSkillScore(17); // 발동점수
        int addedScore = (int)((float)(dragNum-1)*(10.0f+(float)dragNum*((float)(basicScore*3)/100.0f))); // 가중치점수
        pThis->gameLayer->UpdateScore(1, (basicScore+addedScore)/numOfBomb);
        float scale = pThis->gameLayer->GetScoreBasicScale(dragNum);
        pThis->gameLayer->ShowSkillScore((basicScore+addedScore)/numOfBomb, scale, pThis->queuePos, x[0], y[0]);
        
  
        pThis->callbackDone += cnt;
        
        for (int i = 0 ; i < cnt ; i++)
        {
            pThis->gameLayer->UpdatePieceBombCnt( pThis->gameLayer->GetPuzzleP8Set()->GetType(x[i], y[i]), 1 );
            pThis->gameLayer->RemoveConnectPiecesXY(x[i], y[i]);
            
            // effect
            CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land2.plist");
            m_emitter->setAnchorPoint(ccp(0.5, 0.5));
            m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(x[i], y[i]));
            pThis->gameLayer->addChild(m_emitter, 2000);
            m_emitter->setAutoRemoveOnFinish(true);
            
            // 피스 폭발
            CCSprite* piece = pThis->gameLayer->GetSpriteP8(x[i], y[i]);
            if (piece != NULL)
            {
                CCFiniteTimeAction* bomb;
                if (i == 0)
                    bomb = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::PlayEffect_17_Callback), pThis), NULL );
                else
                    bomb = CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
                piece->runAction(bomb);
            }
        }
        
        
        //else // 아마 이 경우는 없을 거야...
        //    PlayEffect_17_Callback(NULL, pThis);
    }
    else
    {
        int x, y;
        for (int i = 0 ; i < pThis->skillPos.size() ; i++)
        {
            x = (int)pThis->skillPos[i].x;
            y = (int)pThis->skillPos[i].y;
            if (pThis->gameLayer->GetSpriteP8(x, y) != NULL)
            {
                pThis->gameLayer->GetPuzzleP8Set()->RemoveChild(x, y);
                pThis->gameLayer->SetSpriteP8Null(x, y);
            }
        }
        pThis->skillPos.clear();

        pThis->gameLayer->Falling(pThis->queuePos);
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
        //first->setScale(0.45f);
        gameLayer->addChild(first, z1);
        
        CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.03f);
        CCAnimate* animate = CCAnimate::create(animation);
        CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffectCallback), NULL), NULL);
        first->runAction(action);
        
        //// particle ////
        sp_fire = CCSprite::create("particles/fire.png");
        CCParticleSystem* par = CCParticleFlower::create();
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
    
    animFrames->removeAllObjects();
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
    //CCParticleSystemQuad* m_emit = CCParticleSystemQuad::create("particles/land6.plist");
    
    int x, y;
    for (int i = 0; i < pos.size(); i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/land6.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        gameLayer->addChild(m_emitter, 2000);
        //m_emitter->setAutoRemoveOnFinish(true);
        
        CCActionInterval* action = CCSequence::create( CCDelayTime::create(0.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL );
        m_emitter->runAction(action);
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

    int total_H = (gameLayer->vs.height-7-120) - (gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    float scaled_H = (float)total_H * 7.0f / 11.0f;
    if (scaled_H < 350.0f)
        scaled_H = 350.0f;
    A8_icon->setScale( scaled_H / (float)A8_icon->getContentSize().height );
    
    //int A8_icon_height = (gameLayer->vo.y+gameLayer->vs.height-50-50-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    //A8_icon->setScale( (float)A8_icon_height / (float)A8_icon->getContentSize().height );
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
    ef->m_F8_fountain->setAnchorPoint(ccp(0.5, 0.5));
    ef->m_F8_fountain->setPosition(ccp(ef->gameLayer->m_winSize.width/2, ef->gameLayer->vo.y+ef->gameLayer->tbSize.height+ef->gameLayer->boardSize.height+120));
    ef->m_F8_fountain->setScale(2.0f);
    ef->gameLayer->addChild(ef->m_F8_fountain, 1500);
    
    // 혜성 시작
    ef->callbackCnt = 0;
    ef->F8_finishCnt = 0;
    
    ef->gameLayer->GetSkill()->F8_Timer();
}

void Effect::Effect7_Comet(float f)
{
    /*
    //CCLog("timer callback cnt : %d", eff->callbackCnt);
    if (eff->gameLayer->IsPaused())
        return;
    Effect7_Callback_2(NULL, eff);
    */
}

void Effect::Effect7_Callback_2(int idx, void* pointer) // 혜성 떨구기
{
    Effect* ef = (Effect*)pointer;
    
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
    
    if (ef->gameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
    {
        //CCLog("big fire : %d %d", x, y);
        // big sound
        ef->gameLayer->GetSound()->PlayDesignatedSound(72);
        
        // 점수 업데이트 [ 발동점수의30% + (터지는피스개수 * 발동점수의 3%) ]
        float basicScore = (float)ef->gameLayer->GetSkill()->GetBasicSkillScore(7);
        int addedScore = (int)((float)(basicScore * 0.3f) + (basicScore * 0.03f * (float)ef->skillDoublePos[idx].size()));
        ef->gameLayer->UpdateScore(1, addedScore);
        ef->gameLayer->ShowSkillScore(addedScore, 1.5f, ef->queuePos, x, y);
        //ef->gameLayer->ShowSkillAddedScore(addedScore, ef->queuePos, 1, x, y);
        
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
        
        // 점수 업데이트 (발동점수의 10%)
        int addedScore = (int)((float)ef->gameLayer->GetSkill()->GetBasicSkillScore(7) * 0.1f);
        ef->gameLayer->UpdateScore(1, addedScore);
        ef->gameLayer->ShowSkillScore(addedScore, 1.2f, ef->queuePos, x, y);
        //ef->gameLayer->ShowSkillAddedScore(addedScore, ef->queuePos, 1, x, y);

        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire8_smallfire.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(ef->gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(2.0f);
        m_emitter->setAutoRemoveOnFinish(true);
        ef->gameLayer->addChild(m_emitter, 1500);
    }

    // 실제로 폭파!
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
        //CCLog("끝? %d %d", ef->F8_finishCnt, (int)ef->skillPos.size());
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
    m_W8_bg->setAnchorPoint(ccp(0.5, 0));
    m_W8_bg->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+190+100));
    
    float totalH = (gameLayer->vs.height-7-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+190+100);
    m_W8_bg->setContentSize(CCSize(gameLayer->m_winSize.width, totalH));
    float h = m_W8_bg->getContentSize().height;
    m_W8_bg->setScaleY(totalH/h);
    gameLayer->addChild(m_W8_bg, 6);
    
    // 마법진 위에 여신 출현
    // (소리)
    gameLayer->GetSound()->PlayDesignatedSound(150);
    A8_icon = CCSprite::createWithSpriteFrameName("icon/goddess.png");
    A8_icon->setAnchorPoint(ccp(0.5, 0));
    A8_icon->setOpacity(0);
    
    int total_H = (gameLayer->vs.height-7-120) - (gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    float scaled_H = (float)total_H * 7.0f / 11.0f;
    if (scaled_H < 300.0f)
        scaled_H = 300.0f;
    A8_icon->setScale( scaled_H / (float)A8_icon->getContentSize().height );
    
    //int A8_icon_height = (gameLayer->vo.y+gameLayer->vs.height-50-50-120) - (gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    //A8_icon->setScale( (float)A8_icon_height / (float)A8_icon->getContentSize().height );
    A8_icon->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120- 100));
    gameLayer->addChild(A8_icon, z1);

    CCActionInterval* action2 = CCSpawn::create(CCMoveBy::create(0.7f, ccp(0, 100)), CCFadeIn::create(0.7f), NULL);
    A8_icon->runAction(action2);
    
    // orb 출현
    m_orb = CCParticleSystemQuad::create("particles/water8_orb.plist");
    m_orb->setAnchorPoint(ccp(0.5, 0));
    orb_pos = ccp(gameLayer->m_winSize.width/2, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+120+scaled_H);
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
    ////CCLog("size = %d", (int)m_orb->getStartSize());
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
    // 나무 나타나는 소리
    gameLayer->GetSound()->PlayDesignatedSound(230);
    
    // 나무 출현
    A8_icon = CCSprite::createWithSpriteFrameName("icon/tree.png");
    A8_icon->setAnchorPoint(ccp(0.5, 0));
    A8_icon->setOpacity(0);
    
    int total_H = (gameLayer->vs.height-7-120) - (gameLayer->tbSize.height+gameLayer->boardSize.height+120);
    float scaled_H = (float)total_H * 7.0f / 11.0f;
    if (scaled_H < 350.0f)
        scaled_H = 350.0f;
    A8_icon->setScale( scaled_H / (float)A8_icon->getContentSize().height );
    
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


void Effect::PlayEffect_6_Fever()
{
    CCSprite* F7_icon = CCSprite::createWithSpriteFrameName("icon/icon_gauge_big.png");
    F7_icon->setPosition(gameLayer->SetTouch8Position(3, 3));
    F7_icon->setScale(3.0f);
    F7_icon->setOpacity(0);
    gameLayer->addChild(F7_icon, 5010);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.7f, 4.0f), CCSequence::create( CCFadeIn::create(0.4f), CCFadeOut::create(0.3f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_6_Fever_Callback), this), NULL );
    F7_icon->runAction(action);
}
void Effect::PlayEffect_6_Fever_Callback(CCNode *sender, void* pointer) // 코코타임으로 인한 피버타임 시작
{
    sender->removeFromParentAndCleanup(true);
    Effect* pThis = (Effect*)pointer;
    pThis->gameLayer->StartFeverTime();
}

void Effect::PlayEffect_6(int num)
{
    // 피스 새로고침
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fire7_coco.plist");
    m_emitter->setPosition(ccp(200, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+60+150));
    m_emitter->setScale(1.2f);
    gameLayer->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
}
void Effect::PlayEffect_6_Fire(std::vector< std::vector<CCPoint> > pos, int queue_pos, int cnt)
{
    // 피스 새로고침 (덩어리에 생기는 이펙트들)
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
    
    ////CCLog("리뉴 콜백 %d", pThis->callbackCnt);
    
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

            m_emitter->setAnchorPoint(ccp(0.5, 0.5));
            m_emitter->setPosition(pThis->gameLayer->SetTouch8Position(x, y));
            m_emitter->setScale(1.5f);
            m_emitter->setAutoRemoveOnFinish(true);
            pThis->gameLayer->addChild(m_emitter, 2000);
        }
        
        // 폭발 액션
        //pThis->gameLayer->Bomb(pThis->queuePos, pThis->gameLayer->GetSkill()->GetResultDouble()[pThis->callbackCnt]);
        pThis->gameLayer->GetSkill()->Renew_Bomb(pThis->callbackCnt);
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
        ////CCLog("리뉴 콜백 끝");
        // 콜백 끝
        pThis->gameLayer->GetSkill()->RenewPuzzle_End(pThis->gameLayer->GetSkill(), pThis->queuePos);
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
    
    //if (queue_pos == -1) // 떡갈나무지팡이 실행한 경우
    //    deltaPos = ccp(x, y);
    //else
    deltaPos = gameLayer->SetTouch8Position(x, y);
    plus_callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), this), CCFadeOut::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2Callback), this), NULL), NULL), NULL);
    plus->runAction(action);
    
    // 작은 불 이펙트
    CCParticleSystemQuad* m_emitter2 = CCParticleSystemQuad::create("particles/fire8_smallfire.plist");
    m_emitter2->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter2->setPosition(gameLayer->SetTouch8Position(x, y));
    m_emitter2->setScale(1.8f);
    m_emitter2->setAutoRemoveOnFinish(true);
    gameLayer->addChild(m_emitter2, 1500);
}
void Effect::Effect2Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}
void Effect::Effect2CallbackNewSprite(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;
    pThis->plus_callbackCnt++;
    if (pThis->plus_callbackCnt < 5)
    {
        CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
        if (pThis->plus_callbackCnt == 1)
            plus->setPosition(ccp(pThis->deltaPos.x-20, pThis->deltaPos.y+20));
        else if (pThis->plus_callbackCnt == 2)
            plus->setPosition(ccp(pThis->deltaPos.x+20, pThis->deltaPos.y+5));
        else if (pThis->plus_callbackCnt == 3)
            plus->setPosition(ccp(pThis->deltaPos.x-10, pThis->deltaPos.y+10));
        else if (pThis->plus_callbackCnt == 4)
            plus->setPosition(ccp(pThis->deltaPos.x+10, pThis->deltaPos.y+15));
        plus->setScale(0.5);
        plus->setOpacity(0);
        pThis->gameLayer->addChild(plus, z1+1);
        CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), pThis), CCFadeOut::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect2Callback), pThis), NULL), NULL), NULL);
        plus->runAction(action);
    }
}


void Effect::PlayEffect_18(CCPoint p)
{
    // E3 : 떡갈나무지팡이

    // sound
    gameLayer->GetSound()->PlaySkillSound(18);
    
    // 추가별사탕 개수를 실제로 증가시킴
    //gameLayer->UpdateStarCandy(1, gameLayer->GetSkill()->E3_Get_AddedCandy());
    
    //deltaPos = p;
    callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    //plus->setPosition(ccp(deltaPos.x, deltaPos.y));
    plus->setPosition(ccp(p.x, p.y));
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect18CallbackNewSprite), this), CCFadeOut::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect18Callback), this), NULL), NULL), NULL);
    plus->runAction(action);
}
void Effect::Effect18Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}
void Effect::Effect18CallbackNewSprite(CCNode* sender, void* pointer)
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
        CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect18CallbackNewSprite), pThis), CCFadeOut::create(0.2f), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(Effect::Effect18Callback), pThis), NULL), NULL), NULL);
        plus->runAction(action);
    }
    else
    {
        // 끝났음. 다음 보너스 스킬로 넘어가자.
        //pThis->gameLayer->GetSkill()->E3_Done();
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


void Effect::PlayEffect_22(CCPoint p)
{
    //CCLog("PlayEffect_22");
    // E7 : 끈질긴 생명력
    CCSprite* potion = CCSprite::createWithSpriteFrameName("icon/icon_potion.png");
    potion->setPosition(p);
    potion->setScale(1.5f);
    potion->setOpacity(0);
    gameLayer->addChild(potion, z1+1);
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCFadeIn::create(0.4f), CCMoveBy::create(0.4f, ccp(0, 50)), NULL), CCSpawn::create( CCFadeOut::create(0.4f), CCMoveBy::create(0.4f, ccp(0, 50)), NULL), CCDelayTime::create(0.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect22Callback), this), NULL );
    potion->runAction(action);
}
void Effect::Effect22Callback(CCNode* sender, void* pointer)
{
    //CCLog("Effect22 callback");
    sender->removeFromParentAndCleanup(true);
    
    ((Effect*)pointer)->gameLayer->GetSkill()->E7_Done();
}



void Effect::PlayEffect_10()
{
    deltaPos = ccp(130, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+60+220);
    plus_callbackCnt = 0;
    CCSprite* plus = CCSprite::createWithSpriteFrameName("icon/icon_plus.png");
    plus->setPosition(deltaPos);
    plus->setScale(0.5);
    plus->setOpacity(0);
    gameLayer->addChild(plus, z1+1);
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(0.4f, 1.5f), CCMoveBy::create(0.4f, ccp(0, 50)), CCSequence::create( CCFadeIn::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2CallbackNewSprite), this), CCFadeOut::create(0.2f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect2Callback), this), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_10_Callback), this), NULL), NULL), NULL );
    plus->runAction(action);
}
void Effect::PlayEffect_10_Callback(CCNode* sender, void* pointer)
{
    Effect* pThis = (Effect*)pointer;

    // 점수 보여주기
    int W3_addedScore = pThis->gameLayer->GetSkill()->GetBasicSkillScore(10);
    W3_addedScore *= (pThis->gameLayer->GetCombo()/10);
    //CCLog("W3 현재 콤보 : %d", pThis->gameLayer->GetCombo());
    //CCLog("score = %d", W3_addedScore);
    
    pThis->gameLayer->UpdateScore(1, W3_addedScore);
    pThis->gameLayer->ShowSkillScore(W3_addedScore, 1.5f, -1);
}

void Effect::PlayEffect_11()
{
    // W4 : 콤보 비례 추가 별사탕
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water4.plist");
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
    
    if (gameLayer->GetSkill()->W7_IsFreezed())
    {
        // 번쩍
        CCSprite* W7Start = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
        W7Start->setAnchorPoint(ccp(0,0));
        W7Start->setPosition(ccp(0, 0));
        W7Start->setColor(ccc3(255,255,255));
        W7Start->setOpacity(0);
        gameLayer->addChild(W7Start, 5000);
        CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.1f), CCFadeOut::create(0.1f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
        W7Start->runAction(startAction);
        
        if (gameLayer->GetSkill()->IsSpiritAlive(0))
            spirit_fire->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(1))
            spirit_water->pauseSchedulerAndActions();
        if (gameLayer->GetSkill()->IsSpiritAlive(2))
            spirit_land->pauseSchedulerAndActions();
        
        if (myInfo->GetActiveFairyId() > 0)
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
        
        if (myInfo->GetActiveFairyId() > 0)
            gameLayer->GetFairyLayer()->resumeSchedulerAndActions();
    }
}
void Effect::Effect14Callback(CCNode* sender, void* data)
{
    sender->removeFromParentAndCleanup(true);
}


// 연결피스 생성 시의 요정에 나타나는 이펙트
void Effect::FairySkill()
{
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/fairy_skill.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(gameLayer->m_winSize.width-280+100, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+60+160));
    m_emitter->setScale(1.0f);
    gameLayer->addChild(m_emitter, 99);
    m_emitter->setAutoRemoveOnFinish(true);
}

// 새로 만들어진 연결피스에 나타날 이펙트
void Effect::NewlyMadeConnPiece(int x, int y)
{
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/gamestart.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(gameLayer->SetTouch4Position(x, y));
    m_emitter->setScale(1.5f);
    gameLayer->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
}

/*
void Effect::ShowStarCandy(CCPoint pos, int size)
{
    CCParticleSystemQuad* m_emitter;
    if (size <= 5)
        m_emitter = CCParticleSystemQuad::create("particles/starcandy.plist");
    else if (size <= 9)
        m_emitter = CCParticleSystemQuad::create("particles/starcandy_big.plist");
    else
        m_emitter = CCParticleSystemQuad::create("particles/starcandy_bigbig.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(gameLayer->SetTouch8Position((int)pos.x, (int)pos.y));
    m_emitter->setScale(1.3f);
    gameLayer->addChild(m_emitter, 1001);
    
    m_emitter->setDuration(1.0f);
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.05f), CCMoveTo::create(0.65f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL);
    m_emitter->runAction(action);
}
*/
void Effect::ShowStarCandy(bool isCycle, std::vector<CCPoint> pos)
{
    starcandy_0_isCycle = isCycle;
    
    int size = pos.size();
    
    int z; // 시작 위치
    if (isCycle) z = 0;
    else z = size-1;
    
    int total; // 별사탕 나올 개수
    if (size <= 5) total = 1;
    else if (size <= 9) total = 2;
    else if (size <= 14) total = 3;
    else if (size <= 19) total = 4;
    else total = 5;
    
    for (int i = 0 ; i < total ; i++)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/starcandy.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position((int)pos[z].x, (int)pos[z].y));
        gameLayer->addChild(m_emitter, 100);
        m_emitter->setScale(1.5f);
        m_emitter->setDuration(0.6f + (i+1)*0.15f);
        
        CCActionInterval* action;
        if (i < total-1)
        {
            //action = CCSequence::create( CCDelayTime::create((i+1)*0.15f), CCSpawn::create( CCScaleTo::create(0.6f, 0.5f), CCMoveTo::create(0.6f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), NULL ), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::ShowStarCandy_Callback_Done), this), NULL );
            action = CCSequence::create( CCDelayTime::create((i+1)*0.15f), CCSpawn::create( CCScaleTo::create(0.6f, 0.5f), CCMoveTo::create(0.6f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), NULL ), NULL );
            m_emitter->setAutoRemoveOnFinish(true);
        }
        else
        {
            action = CCSequence::create( CCDelayTime::create((i+1)*0.15f), CCSpawn::create( CCScaleTo::create(0.6f, 0.5f), CCMoveTo::create(0.6f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), NULL ), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::UpdateStarcandy_0), this), NULL );
            m_emitter->setAutoRemoveOnFinish(false);
            m_emitter->setTag(size);
        }
        m_emitter->runAction(action);
        
        z--;
        if (z < 0)
            z = size-1;
    }
}

void Effect::ShowStarCandyAll(bool isCycle, std::vector<CCPoint> pos)
{
    starcandy_1_isCycle = isCycle;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/starcandy.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        gameLayer->addChild(m_emitter, 100);
        m_emitter->setScale(1.5f);
        m_emitter->setDuration(0.7f);
        
        CCActionInterval* action;
        if (i+1 < (int)pos.size())
        {
            action = CCSequence::create( CCDelayTime::create(0.1f), CCSpawn::create( CCScaleTo::create(0.6f, 0.5f), CCMoveTo::create(0.6f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), NULL), NULL );
            m_emitter->setAutoRemoveOnFinish(true);
        }
        else
        {
            action = CCSequence::create( CCDelayTime::create(0.1f), CCSpawn::create( CCScaleTo::create(0.6f, 0.5f), CCMoveTo::create(0.6f, ccp(100+140-20, gameLayer->vo.y+gameLayer->vs.height-47-20/2+3-7)), NULL ), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::UpdateStarcandy_1), this), NULL );
            m_emitter->setAutoRemoveOnFinish(false);
            m_emitter->setTag((int)pos.size());
        }
        m_emitter->runAction(action);
    }
}

void Effect::ShowStarCandy_Callback_Done(CCNode* sender, void* pointer)
{
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
}

void Effect::UpdateStarcandy_0(CCNode* sender, void* p) // showstarcandy general
{
    Effect* pThis = (Effect*)p;
    int size = ((CCParticleSystemQuad*)sender)->getTag();
    sender->removeFromParentAndCleanup(true);

    pThis->gameLayer->UpdateStarCandy(0, size, pThis->starcandy_0_isCycle);
}
void Effect::UpdateStarcandy_1(CCNode* sender, void* p) // showstarcandy all
{
    Effect* pThis = (Effect*)p;
    int size = ((CCParticleSystemQuad*)sender)->getTag();
    sender->removeFromParentAndCleanup(true);
    
    pThis->gameLayer->UpdateStarCandy(1, 5*size, pThis->starcandy_1_isCycle);
}


void Effect::PlayEffect_FeverBg()
{
    // 어두운 배경
    fever_black_bg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, gameLayer->m_winSize.width, gameLayer->m_winSize.height));
    fever_black_bg->setPosition(ccp(0, 0));
    fever_black_bg->setAnchorPoint(ccp(0, 0));
    fever_black_bg->setColor(ccc3(0,0,0));
    gameLayer->addChild(fever_black_bg, 5);
    fever_black_bg->runAction(CCFadeTo::create(0.5f, 255));
    
    // 번쩍
    CCSprite* W7Start = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
    W7Start->setAnchorPoint(ccp(0,0));
    W7Start->setPosition(ccp(0, 0));
    W7Start->setColor(ccc3(255,255,255));
    W7Start->setOpacity(0);
    gameLayer->addChild(W7Start, 5000);
    CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.1f), CCFadeOut::create(0.1f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
    W7Start->runAction(startAction);
    
    // 피버타임 배경
    feverBg = CCParticleSystemQuad::create("particles/feverTime_bg.plist");
    feverBg->setAnchorPoint(ccp(0.5, 0.5));
    feverBg->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->m_winSize.height/2));
    gameLayer->addChild(feverBg, 99);
    
    // 코코 뒤에 이펙트
    feverCoco = CCParticleSystemQuad::create("particles/feverTime_coco.plist");
    feverCoco->setAnchorPoint(ccp(0.5, 0.5));
    feverCoco->setScale(1.7f);
    feverCoco->setPosition(ccp(100+120, gameLayer->vo.y+gameLayer->tbSize.height+gameLayer->boardSize.height+60+100));
    gameLayer->addChild(feverCoco, 99);
}
void Effect::PlayEffect_FeverBg_Off()
{
    fever_black_bg->removeFromParentAndCleanup(true);
    feverBg->setDuration(0.1f);
    feverBg->setAutoRemoveOnFinish(true);
    feverCoco->setDuration(0.1f);
    feverCoco->setAutoRemoveOnFinish(true);
    
    // 번쩍
    CCSprite* W7Start = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, 1080, 1920));
    W7Start->setAnchorPoint(ccp(0,0));
    W7Start->setPosition(ccp(0, 0));
    W7Start->setColor(ccc3(255,255,255));
    W7Start->setOpacity(0);
    gameLayer->addChild(W7Start, 5000);
    CCActionInterval* startAction = CCSequence::create(CCFadeIn::create(0.1f), CCFadeOut::create(0.1f), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::Effect14Callback), this), NULL);
    W7Start->runAction(startAction);
}

void Effect::PlayEffect_FeverCircle(CCPoint p, CCPoint bp, int size)
{
    CCSprite* fc = CCSprite::createWithSpriteFrameName("fever_circle.png");
    fc->setPosition(gameLayer->SetTouch8Position(p.x, p.y));
    fc->setOpacity(0);
    fc->setScale((float)(size*2+1)/(float)3);
    gameLayer->addChild(fc, 2000);
    
    std::vector<CCPoint> pp = gameLayer->GetPosForFeverTime(true);
    int dx = 10/2 * ((int)(p.x - bp.x));
    int dy = 10/2 * ((int)(p.y - bp.y));
    
    float angle = 10.0f;
    if ((dx <= 0 && dy < 0) || (dx < 0 && dy == 0) || (dx < 0 && dy > 0))
        angle = -10.0f;
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.2f), CCRotateBy::create(0.2f, angle), CCMoveBy::create(0.2f, ccp(dx, dy)), NULL), CCSpawn::create(CCFadeOut::create(0.2f), CCRotateBy::create(0.2f, angle), NULL), CCMoveBy::create(0.2f, ccp(dx, dy)), CCCallFuncND::create(gameLayer, callfuncND_selector(Effect::PlayEffect_FeverCircle_Callback), this), NULL );
    fc->runAction(action);
}
void Effect::PlayEffect_FeverCircle_Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}

void Effect::ShowItemPaintArrow(int xx, int yy)
{
    itemPaintArrow.clear();
    for (int x = xx-1 ; x <= xx+1 ; x++)
    {
        for (int y = yy-1 ; y <= yy+1 ; y++)
        {
            if (x == xx && y == yy)
                continue;
            if (x < 0 || x > COLUMN_COUNT-1 || y < 0 || y > ROW_COUNT-1)
                continue;
            if ( (x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) )
                continue;
            
            CCSprite* sp;
            int rt, t;
            if (x == xx && y == yy+1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_s.png"); rt = 0; t = 1; }
            else if (x == xx+1 && y == yy) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_s.png"); rt = 90; t = 1; }
            else if (x == xx && y == yy-1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_s.png"); rt = 180; t = 1; }
            else if (x == xx-1 && y == yy) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_s.png"); rt = -90; t = 1; }
            
            else if (x == xx+1 && y == yy-1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_d.png"); rt = 0; t = 2; }
            else if (x == xx-1 && y == yy-1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_d.png"); rt = 90; t = 2; }
            else if (x == xx-1 && y == yy+1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_d.png"); rt = 180; t = 2; }
            else if (x == xx+1 && y == yy+1) { sp = CCSprite::createWithSpriteFrameName("icon/paint_c_d.png"); rt = -90; t = 2; }
            
            if (t == 1) sp->setAnchorPoint(ccp(0.5, 0));
            else if (t == 2) sp->setAnchorPoint(ccp(0, 1));
            sp->setRotation(rt);
            sp->setPosition(gameLayer->SetTouch8Position(xx, yy));
            sp->setTag(rt-t); // rt - t (고유값)
            sp->setOpacity(180);
            sp->setColor(ccc3(150,150,150));
            gameLayer->addChild(sp, 200);
            itemPaintArrow.push_back(sp);
        }
    }
}
void Effect::ChangeItemPaintArrow(int dx, int dy)
{
    for (int i = 0 ; i < itemPaintArrow.size() ; i++)
    {
        itemPaintArrow[i]->setColor(ccc3(150,150,150));
        if ( (dx == 0 && dy == 1 && itemPaintArrow[i]->getTag() == 0-1) ||
             (dx == 1 && dy == 0 && itemPaintArrow[i]->getTag() == 90-1) ||
             (dx == 0 && dy == -1 && itemPaintArrow[i]->getTag() == 180-1) ||
             (dx == -1 && dy == 0 && itemPaintArrow[i]->getTag() == -90-1) ||
             (dx == 1 && dy == -1 && itemPaintArrow[i]->getTag() == 0-2) ||
             (dx == -1 && dy == -1 && itemPaintArrow[i]->getTag() == 90-2) ||
             (dx == -1 && dy == 1 && itemPaintArrow[i]->getTag() == 180-2) ||
             (dx == 1 && dy == 1 && itemPaintArrow[i]->getTag() == -90-2) )
        {
            itemPaintArrow[i]->setColor(ccc3(255,255,255));
        }
    }
}
void Effect::RemoveItemPaintArrow()
{
    for (int i = 0 ; i < itemPaintArrow.size() ; i++)
        itemPaintArrow[i]->removeFromParentAndCleanup(true);
    itemPaintArrow.clear();
}

//void Effect::PlayEffect_ItemStaff(int x, int y, int type)
void Effect::PlayEffect_ItemStaff(std::vector<CCPoint> pos, int type)
{
    for (int i = 0 ; i < pos.size() ; i++)
    {
        CCParticleSystemQuad* m_emitter;
        if (type == PIECE_YELLOW)
            m_emitter = CCParticleSystemQuad::create("particles/item_staff_yellow.plist");
        else if (type == PIECE_WHITE)
            m_emitter = CCParticleSystemQuad::create("particles/item_staff_white.plist");
        m_emitter->setPosition(gameLayer->SetTouch8Position((int)pos[i].x, (int)pos[i].y));
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setScale(2.5f);
        m_emitter->setAutoRemoveOnFinish(true);
        gameLayer->addChild(m_emitter, 100);
    }
}

void Effect::BonusBomb(std::vector<CCPoint> pos)
{
    int x, y, c;
    int minx = 999, miny = 999, maxx = -1, maxy = -1;
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = pos[i].x;
        y = pos[i].y;
        
        minx = std::min(minx, x);
        miny = std::min(miny, y);
        maxx = std::max(maxx, x);
        maxy = std::max(maxy, y);
        
        // 기본 이펙트
        c = rand()%3;
        CCParticleSystemQuad* m_emitter;
        if (c == 0) m_emitter = CCParticleSystemQuad::create("particles/fire7_1.plist");
        else if (c == 1) m_emitter = CCParticleSystemQuad::create("particles/fire7_2.plist");
        else m_emitter = CCParticleSystemQuad::create("particles/fire7_3.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(1.5f);
        m_emitter->setAutoRemoveOnFinish(true);
        gameLayer->addChild(m_emitter, 2000);
    }
    /*
    // 덩어리 기준 이펙트 1개
    float xx = (float)(maxx+minx)/2.0;
    float yy = (float)(maxy+miny)/2.0;
    CCParticleSystemQuad* m_emitter2 = CCParticleSystemQuad::create("particles/fire7_final.plist");
    m_emitter2->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter2->setPosition(gameLayer->SetTouch8Position(xx, yy));
    m_emitter2->setScale(1.0f);
    m_emitter2->setAutoRemoveOnFinish(true);
    gameLayer->addChild(m_emitter2, 2000);
    */
}

void Effect::ShowNewRecordEffect(void* p)
{
    PuzzleResult* pr = (PuzzleResult*)p;
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/gameresult_newrecord.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(ccp(gameLayer->m_winSize.width/2, gameLayer->m_winSize.height/2));
    m_emitter->setScale(1.0f);
    m_emitter->setAutoRemoveOnFinish(true);
    pr->addChild(m_emitter, 5000);
}


void Effect::RemoveAllObjects()
{
    if (gameLayer->GetSkill()->IsSpiritAlive(0)) spirit_fire->removeFromParentAndCleanup(true);
    if (gameLayer->GetSkill()->IsSpiritAlive(1)) spirit_water->removeFromParentAndCleanup(true);
    if (gameLayer->GetSkill()->IsSpiritAlive(2)) spirit_land->removeFromParentAndCleanup(true);
}


