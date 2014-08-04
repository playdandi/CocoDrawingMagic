#include "T_Effect.h"

enum
{
    z1 = 200,
};

void T_Effect::Init(T_Effect* effect, T_Puzzle* layer)
{
    pThis = effect;
    gameLayer = layer;
    
    circle = NULL;
    circle_fire = NULL;
    circle_water = NULL;
    circle_land = NULL;
    
    sp_fire = NULL;
    A8_icon = NULL;
    F8_bg = NULL;
    m_F8_fountain = NULL;
    timerLayer = NULL;
}

void T_Effect::InitCheck()
{
    for (int i = 0 ; i < NUMOFSKILL ; i++)
        isDone[i] = false;
}


void T_Effect::PlayEffect_Default(std::vector<CCPoint> pos)
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

void T_Effect::PlayEffect_MagicCircle(int skillNum)
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
        
        CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.5f), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::PlayEffect_MagicCircle_Callback), this), NULL);
        circle->setTag(0);
        circle->runAction(action);
    }
}
void T_Effect::PlayEffect_MagicCircle_Callback(CCNode* sender, void* pointer)
{
    T_Effect* ef = (T_Effect*)pointer;
    int tag = sender->getTag();
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    
    if (tag == 0) ef->circle = NULL;
    else if (tag == 1) ef->circle_fire = NULL;
    else if (tag == 2) ef->circle_water = NULL;
}

void T_Effect::PlayEffect_SkillIcon(int skillNum)
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
    
    callbackCnt = skillNum; // 스킬 번호 임시저장 (떡갈나무지팡이, 끈질긴생명력 스킬에 이용)
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCFadeIn::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCSpawn::create(CCFadeOut::create(0.5f), CCMoveBy::create(0.5f, ccp(0, 50)), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::PlayEffect_SkillIcon_Callback), this), NULL);
    skill->runAction(action);
}
void T_Effect::PlayEffect_SkillIcon_Callback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
}

void T_Effect::PlayEffect_CycleOnly(int skillNum, std::vector<CCPoint> pos)
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
    
    CCFiniteTimeAction* action = CCSequence::create( CCSpawn::create( CCScaleTo::create(1.8f, 1.0f), CCSequence::create( CCFadeIn::create(0.30f), CCFadeOut::create(0.05f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::PlayEffectCallback), NULL), NULL);
    sp_cycle->runAction(action);
}
void T_Effect::PlayEffectCallback(CCNode* sender, void* data)
{
    sender->removeAllChildrenWithCleanup(true);
    sender->removeFromParentAndCleanup(true);
}


void T_Effect::PlayEffect_0(std::vector<CCPoint> pos)
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
        par->setTexture(sp_fire->getTexture());
    
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

void T_Effect::PlayEffect_1(std::vector<CCPoint> pos)
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


void T_Effect::PlayEffect_9(std::vector<CCPoint> pos, int queue_pos)
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
    
    CCFiniteTimeAction* action = CCSequence::create(CCBezierBy::create(0.2f, GetBezierConfig(this, 1)), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::Effect9Callback), this), NULL);
    par->runAction(action);
}

ccBezierConfig T_Effect::GetBezierConfig(T_Effect* pThis, int mul)
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

void T_Effect::Effect9Callback(CCNode* sender, void* pointer)
{
    T_Effect* pThis = (T_Effect*)pointer;
    
    // bomb piece
    int x = (int)pThis->skillPos[pThis->callbackCnt].x;
    int y = (int)pThis->skillPos[pThis->callbackCnt].y;
    
    CCSprite* piece = pThis->gameLayer->GetSpriteP8(x, y);
    if (piece != NULL)
    {
        CCFiniteTimeAction* bomb = CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
        piece->runAction(bomb);
        
        // 물 사이클 스킬은 Puzzle의 Bomb함수를 쓰지 않기 때문에, 여기서 개수를 cnt해야 한다.
        //pThis->gameLayer->UpdatePieceBombCnt( pThis->gameLayer->GetPuzzleP8Set()->GetType(x, y), 1 );
        
        // 주변 연결피스 제거
        //pThis->gameLayer->RemoveConnectPiecesXY(x, y);
    }
    
    // 점수 업데이트
    // 발동점수 + [ SIGMA[k=1~파도폭파수] { [(k)*(10+k*발동점수의 2%)]) } ]
    int basicScore = pThis->gameLayer->GetSkill()->GetBasicSkillScore(9); // 발동점수
    int A2_addedScore = (int)((float)(pThis->callbackCnt)*(10.0f+(float)(pThis->callbackCnt)*((float)(basicScore*2)/100.0f)));
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
        CCFiniteTimeAction* action = CCSequence::create(CCBezierBy::create(0.2f, GetBezierConfig(pThis, mul)), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(T_Effect::Effect9Callback), (void*)pThis), NULL);
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

void T_Effect::PlayEffect_17(std::vector<CCPoint> pos, int queue_pos)
{
    callbackCnt = 0;
    callbackDone = 0;
    skillPos = pos;
    queuePos = queue_pos; // 이 스킬이 터지는 동안은 lock에 의해 queue_pos가 증가하지 않을 것이기 떄문에, 이렇게 한 변수에 둬도 괜찮을 것이다.
    
    PlayEffect_17_Callback(NULL, this);
}
void T_Effect::PlayEffect_17_Callback(CCNode* sender, void* p)
{
    T_Effect* pThis = (T_Effect*)p;
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
        // 최종점수 = 발동점수 + [(drag수-1)*(10+drag수*발동점수의 5%)])
        int dragNum = pThis->gameLayer->GetPiece8xy(true).size(); // 드래그수 구하기
        int basicScore = pThis->gameLayer->GetSkill()->GetBasicSkillScore(17); // 발동점수
        int addedScore = (int)((float)(dragNum-1)*(10.0f+(float)dragNum*((float)(basicScore*5)/100.0f))); // 가중치점수
        pThis->gameLayer->UpdateScore(1, (basicScore+addedScore)/numOfBomb);
        float scale = pThis->gameLayer->GetScoreBasicScale(dragNum);
        pThis->gameLayer->ShowSkillScore((basicScore+addedScore)/numOfBomb, scale, pThis->queuePos, x[0], y[0]);
        
        
        pThis->callbackDone += cnt;
        
        for (int i = 0 ; i < cnt ; i++)
        {
            //pThis->gameLayer->UpdatePieceBombCnt( pThis->gameLayer->GetPuzzleP8Set()->GetType(x[i], y[i]), 1 );
            //pThis->gameLayer->RemoveConnectPiecesXY(x[i], y[i]);
            
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
                    bomb = CCSequence::create( CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL), CCCallFuncND::create(pThis->gameLayer, callfuncND_selector(T_Effect::PlayEffect_17_Callback), pThis), NULL );
                else
                    bomb = CCSpawn::create(CCScaleTo::create(0.2f, 1.5f), CCFadeOut::create(0.3f), NULL);
                piece->runAction(bomb);
            }
        }
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





void T_Effect::PlayEffect_7(std::vector< std::vector<CCPoint> > pos_d, std::vector<CCPoint> pos, int queue_pos)
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
    
    CCActionInterval* action2 = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.8f, ccp(0, 100)), CCSequence::create(CCFadeIn::create(0.4f), CCFadeOut::create(0.4f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::Effect7_Callback_1), this), NULL);
    A8_icon->runAction(action2);
}


static T_Effect* eff;

void T_Effect::Effect7_Callback_1(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true); // 용 삭제
    T_Effect* ef = (T_Effect*)pointer;
    
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

void T_Effect::Effect7_Comet(float f)
{
    /*
     //CCLog("timer callback cnt : %d", eff->callbackCnt);
     if (eff->gameLayer->IsPaused())
     return;
     Effect7_Callback_2(NULL, eff);
     */
}

void T_Effect::Effect7_Callback_2(int idx, void* pointer) // 혜성 떨구기
{
    T_Effect* ef = (T_Effect*)pointer;
    
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
    
    CCActionInterval* action = CCSequence::create( CCMoveTo::create(0.5f, pos), CCCallFuncND::create(ef->gameLayer, callfuncND_selector(T_Effect::Effect7_Callback_3), ef), NULL);
    m_emitter->runAction(action);
    
    ef->gameLayer->GetSound()->PlayDesignatedSound(70);
}

void T_Effect::Effect7_Callback_3(CCNode* sender, void* pointer) // 혜성 떨어지고 폭파
{
    int idx = sender->getTag();
    
    // 혜성 삭제
    ((CCParticleSystemQuad*)sender)->setDuration(0.1f);
    ((CCParticleSystemQuad*)sender)->setAutoRemoveOnFinish(true);
    
    T_Effect* ef = (T_Effect*)pointer;
    
    int x = (int)ef->skillPos[idx].x;
    int y = (int)ef->skillPos[idx].y;
    
    if (ef->gameLayer->GetPuzzleP8Set()->GetType(x, y) == PIECE_RED)
    {
        // big sound
        ef->gameLayer->GetSound()->PlayDesignatedSound(72);
        
        //CCLog("big fire : %d %d", x, y);
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
        
        //CCLog("small fire : %d %d", x, y);
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
std::vector<CCPoint> T_Effect::GetDoublePos(int idx)
{
    return skillDoublePos[idx];
}

void T_Effect::Effect7_Clear()
{
    m_F8_fountain->setDuration(0.01f);
    m_F8_fountain->setAutoRemoveOnFinish(true);
    F8_bg->removeFromParentAndCleanup(true);
}
void T_Effect::Effect7_Callback_4(cocos2d::CCNode *sender, void *pointer)
{
    T_Effect* ef = (T_Effect*)pointer;
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


void T_Effect::PlayEffect_8(std::vector<CCPoint> pos)
{
    // 물1 : blue 기본 추가점수
    int x, y;
    
    for (int i = 0 ; i < pos.size() ; i++)
    {
        x = (int)pos[i].x;
        y = (int)pos[i].y;
        
        CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/water1.plist");
        m_emitter->setAnchorPoint(ccp(0.5, 0.5));
        m_emitter->setPosition(gameLayer->SetTouch8Position(x, y));
        m_emitter->setScale(1.0f);
        gameLayer->addChild(m_emitter, 2000);
        m_emitter->setAutoRemoveOnFinish(true);
    }
}


void T_Effect::PlayEffect_16(std::vector<CCPoint> pos)
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
        gameLayer->addChild(first, z1);
        
        CCAnimation* animation = CCAnimation::createWithSpriteFrames(animFrames, 0.03f);
        CCAnimate* animate = CCAnimate::create(animation);
        CCFiniteTimeAction* action = CCSequence::create(animate, CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::PlayEffectCallback), NULL), NULL);
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




void T_Effect::ShowStarCandy(std::vector<CCPoint> pos)
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
        
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create( CCFadeIn::create(0.1f), CCEaseOut::create(CCMoveTo::create(0.5f, ccp(gameLayer->m_winSize.width/2, gameLayer->vs.height+gameLayer->vo.y-370)), 0.5f), CCFadeOut::create(0.2f), NULL), NULL), CCCallFuncND::create(gameLayer, callfuncND_selector(T_Effect::ShowStarCandy_Callback_Done), this), NULL);
        candy->runAction(action);
    }
}
void T_Effect::ShowStarCandy_Callback_Done(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
}


void T_Effect::RemoveAllObjects()
{
    /*
    if (gameLayer->GetSkill()->IsSpiritAlive(0)) spirit_fire->removeFromParentAndCleanup(true);
    if (gameLayer->GetSkill()->IsSpiritAlive(1)) spirit_water->removeFromParentAndCleanup(true);
    if (gameLayer->GetSkill()->IsSpiritAlive(2)) spirit_land->removeFromParentAndCleanup(true);
     */
}

