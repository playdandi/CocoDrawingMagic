#include "Puzzle.h"

enum
{
	zBackground = 0,
	zGameObject = 100,
    zGameObjectSP = 101,
    zPieceConn = 3,
};

static int addedPotion; // 1 : '끈질긴 생명력' 발동됨, otherwise, 0
static int numOfFreezeTime; // '시간을 얼리다' 발동 횟수 (max)
static int numOfCocoTime; // '코코타임' 발동 횟수 (max)
static Puzzle* pPuzzlePtr;

CCScene* Puzzle::scene(int potion, int freezeTime, int cocoTime)
{
    addedPotion = potion;
    numOfFreezeTime = freezeTime;
    numOfCocoTime = cocoTime;
    CCLog("코코타임 = %d", numOfCocoTime);
    CCLog("시간을 얼리다 = %d", numOfFreezeTime);
    CCLog("끈질긴 생명력 = %d", addedPotion);
    
	CCScene* pScene = CCScene::create();
    
	Puzzle* pLayer = Puzzle::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Puzzle::onEnter()
{
    CCLog("Puzzle : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
    
    
    // Fade Out (서서히 밝아진다) + 3/2/1/시작 액션으로 이동
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyAndStart), this), NULL);
    pBlackOpen->runAction(action);
    
    // 미션 상세화면 : 시작하자마자 보이기
    if (missionType > 0)
    {
        pMissionSpriteDetail->runAction( CCFadeIn::create(0.3f) );
        pMissionSpriteDetailIcon->runAction( CCFadeIn::create(0.3f) );
        pMissionSpriteDetailContent->runAction( CCFadeIn::create(0.3f) );
    }
}
void Puzzle::onExit()
{
    CCLog("Puzzle : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Puzzle::keyBackClicked()
{
    if (isKeybackTouched || m_bTouchStarted || isGameOver)
        return;
    isKeybackTouched = true;
    
    if (!isInGamePause)
        PauseGame();
}

bool Puzzle::init()
{
	if (!CCLayerColor::initWithColor(ccc4(43, 93, 151, 255))) // 파란 배경
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("Puzzle", this);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    this->setTouchPriority(0);
    
    isFalling = false;
    isGameStarted = false;
    isGameOver = false;
    isInGamePause = false;
    
    board_wh = 1076.0f;
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Puzzle::Notification), "Puzzle", NULL);
    
    // action set
    //runningActions = CCSet::create();
    
    sound = new Sound();
    sound->PreLoadInGameSound();
    effect = new Effect();
    effect->Init(effect, this);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/popup.plist");
    // game3.plist는 loading화면에서 preload했음.
    
    
    // item 사용 여부
    item_clear = CCUserDefault::sharedUserDefault()->getBoolForKey("item_00", false);
    item_time = CCUserDefault::sharedUserDefault()->getBoolForKey("item_01", false);
    item_paint = CCUserDefault::sharedUserDefault()->getBoolForKey("item_02", false);
    item_staff = CCUserDefault::sharedUserDefault()->getBoolForKey("item_03", false);
    
    totalCnt_paint = (item_paint) ? 2+rand()%5 : 0; // 2~6개
    totalCnt_staff = (item_staff) ? 2+rand()%5 : 0; // 2~6개
    if (item_paint && item_staff && totalCnt_staff == totalCnt_paint)
    {
        totalCnt_staff--;
        if (totalCnt_staff > 2)
            totalCnt_staff = 3;
    }
    CCLog("paint(%d), staff(%d)", totalCnt_paint, totalCnt_staff);
    iNumOfPaintItemRemained = totalCnt_paint; // 지금까지 나타난 paint item 수
    iNumOfStaffItemRemained = totalCnt_staff; // 지금까지 나타난 staff item 수
    iNumOfPaintItemNeeded = 0; // paint item 발동 횟수
    iNumOfStaffItemNeeded = 0; // staff item 발동 횟수

    //iNumOfFairySkillStart = 0; // 요정 스킬 발동해야 하는 횟수
    
    // 장착한 요정의 능력치 실행
    CONN_PIECE_PROB = 60;               // 연결피스 확률
    FEVER_TIME_MAX = FEVER_TIME*1000;   // feverTime 시간
    ADD_STARCANDY_PERCENT = 0;          // 추가별사탕 %
    if (myInfo->GetActiveFairyId() > 0)
    {
        switch (FairyInfo::GetObj(myInfo->GetActiveFairyId())->GetType())
        {
            case 1:  CONN_PIECE_PROB += 10; break;
            case 9:  FEVER_TIME_MAX += (1*1000); break;
            case 10: ADD_STARCANDY_PERCENT += 10; break;
        }
    }
    CCLog("요정능력 : %d %d %d", CONN_PIECE_PROB, FEVER_TIME_MAX, ADD_STARCANDY_PERCENT);
    
    feverStartCnt = FEVER_START_COMBO;
    
    spriteClassInfo = new SpriteClass();
    spriteClass = new SpriteClass();
    
    puzzleLayer = new CCLayer();
    this->addChild(puzzleLayer, 20);
    puzzleP8set = new PuzzleP8Set();
    puzzleP8set->SetGameLayer(this);
    puzzleP8set->SetPuzzleLayer(puzzleLayer);
    puzzleP4set = new PuzzleP4Set();
    puzzleP4set->SetGameLayer(this);
    puzzleP4set->SetPuzzleLayer(puzzleLayer);
    
    // skill algorithm
    skill = new PuzzleSkill();
    skill->SetGameLayer(this);
    InitSkills();

	m_winSize = CCDirector::sharedDirector()->getWinSize();
    srand(time(NULL));
    InitSprites();
    InitCoco();
    InitFairy();
    InitBoard();
    SetScoreAndStarCandy();
    SetCombo();
    SetTimer();
    
    PlayEffect(100, NULL);
    
    isStartGameEnd = false;
    isMagicTime = false;
    isFeverTime = false;
    
    isCancelling = false;
    m_iSpiritSP = 0;
    
    W3_total = 0;
    W4_total = 0;
    
    pBlackOpen = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlackOpen->setAnchorPoint(ccp(0, 0));
    pBlackOpen->setPosition(ccp(0, 0));
    pBlackOpen->setColor(ccc3(0,0,0));
    this->addChild(pBlackOpen, 199);
    
    iTouchRound = 0;
    
    // 각 피스 색깔마다 터뜨린 총 개수
    for (int i = 0 ; i < 5; i++)
        iPieceBombCnt[i] = 0;
    
    XMLStatus = 0;
    
    readyCnt = 0;
    isRenewing = false;
    isHintShown = false;
    
    bonusTimeState = -999;
    
    //magicCnt = 0; // 마법발동 횟수 (피버타임 끝나면 0으로 초기화)
    
    pPuzzlePtr = this;
    
    //m_bIsItemPossible = true;
    
    readySprite = NULL;
    
    dragScore = 0;
    
	return true;
}

void Puzzle::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        //((Puzzle*)Depth::GetCurPointer())->setTouchEnabled(true);
        //((Puzzle*)Depth::GetCurPointer())->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        this->setKeypadEnabled(true);
        this->setTouchPriority(Depth::GetCurPriority());
        CCLog("Puzzle : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        m_bTouchStarted = false;
        isKeybackTouched = false;
        if (isGameStarted) // 만약 게임 시작 안 한 경우 ('고' 글자 나오기 전) 이면, 어차피 '고' 지난 후 음악이 재생되니까, 여기서는 하지 않음.
        {
            sound->ResumeBackgroundInGameSound();
            sound->ResumeAllEffects();
        }
        
        // 정령 다시 움직이기
        if (skill->IsSpiritAlive(0))
            effect->GetSpirit(0)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(1))
            effect->GetSpirit(1)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(2))
            effect->GetSpirit(2)->resumeSchedulerAndActions();
        
        // '레디' sprite의 action resume
        //if (readySprite != NULL)
        //    CCDirector::sharedDirector()->getActionManager()->resumeTarget(readySprite);
        CCDirector::sharedDirector()->getActionManager()->resumeTargets(runningActions);
        runningActions->release();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("Puzzle 터치 비활성");
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        ((Puzzle*)depth[depth.size()-2]->GetCurPointer())->setTouchEnabled(false);
        ((Puzzle*)depth[depth.size()-2]->GetCurPointer())->setKeypadEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        sound->StopBackgroundInGameSound();
        sound->ResumeAllEffects();
        
        // 종료하고 Ranking으로 돌아가자.
        CCLog("Puzzle 종료. Ranking으로 돌아감.");
        this->EndScene();
    }
    else if (param->intValue() == 3)
    {
        isGameOver = true; // 미리 게임이 끝났다는 flag를 만들어둔다.
    }
}

void Puzzle::InitSkills()
{
    std::vector<int> skillNum, skillProb, skillLv;

    spirit_scid = -1;
    int id;
    for (int i = 0 ; i < inGameSkill.size() ; i++)
    {
        id = inGameSkill[i];
        MySkill* ms = MySkill::GetObj(id);
        if (ms != NULL)
        {
            skillNum.push_back( SkillInfo::Converted(id) );
            skillProb.push_back(SkillBuildUpInfo::GetObj(id, ms->GetLevel())->GetProb());
            CCLog("%d(%d) : %d, %d", id, skillNum[skillNum.size()-1], ms->GetLevel(), skillProb[skillProb.size()-1]);
            skillLv.push_back(ms->GetLevel());
            
            if (id % 10 == 3) // 정령 스킬이 있다면 저장해 둔다. (튜토리얼 위해)
                spirit_scid = id;
        }
    }
    
    skill->Init(skillNum, skillProb, skillLv);
}

void Puzzle::InitInfoBar()
{
    int offset = 20;
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png1", ccp(0.5, 1), ccp(m_winSize.width/2, vo.y+vs.height-7), CCSize(1075-6,75+25+offset), "", "Puzzle", this, 6) );
    
    // 별사탕 숫자배경
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png1", ccp(0, 0.5), ccp(100-20, vo.y+vs.height-47-offset/2-7), CCSize(160, 50+10), "", "Puzzle", this, 6) );
    
    // 별사탕
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(1, 0.5), ccp(125-20, vs.height+vo.y-60-offset/2-7 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_3.png", ccp(1, 0.5), ccp(95-20, vs.height+vo.y-47-offset/2-7 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_4.png", ccp(1, 0.5), ccp(110-20, vs.height+vo.y-28-offset/2-7 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    // 게이지바 배경
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png2", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+vs.height-45-offset/2-7 ), CCSize(164, 35+15+offset/2), "", "Puzzle", this, 6) );
    
    // 게이지바 (노란색)
    gaugeLayer = CCLayer::create();
    gaugeLayer->setAnchorPoint(ccp(0,0));
    gaugeLayer->setPosition(ccp(0,0));
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_gauge.png", ccp(0.5, 0.5), ccp(m_winSize.width-170 - 149, vo.y+vs.height-45+2-offset/2-7 ), CCSize(149, 35+offset/2), "", "Layer", gaugeLayer, 7, 0, 255, 98765) );
    // 게이지바 clipping하기
    gaugeStencil = CCDrawNode::create();
    CCPoint pos = ccp(m_winSize.width-170 - 149.0f/2.0f, vo.y+vs.height-45+2-offset/2-7 - (35+offset/2)/2.0f );
    CCPoint ver[] = { ccp(pos.x, pos.y), ccp(pos.x+149, pos.y), ccp(pos.x+149, pos.y+35+offset/2), ccp(pos.x, pos.y+35+offset/2) };
    gaugeStencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    gaugeClip = CCClippingNode::create(gaugeStencil);
    gaugeClip->addChild(gaugeLayer);
    this->addChild(gaugeClip, 7);
    
    
    // 게이지바 아이콘(모자)
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_gauge_big.png", ccp(1, 0.5), ccp(m_winSize.width-170-82+20 +5-7, vo.y+vs.height-47-offset/2-7), CCSize(0, 0), "", "Puzzle", this, 8) );
    
    // pause button
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_pause.png", ccp(1, 0.5), ccp(m_winSize.width-25, vs.height+vo.y-50+5-offset/2-7), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    // 값들 (별사탕 개수)
    spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 40, ccp(1, 0.5), ccp(100+140-20, vo.y+vs.height-47-offset/2+3-7), ccc3(255,255,255), "", "Puzzle", this, 20, 0, 255, 1) ); // font 원래 40
    
    pMissionSpriteDetail = NULL;
    pMissionSpriteDetailIcon = NULL;
    pMissionSpriteDetailContent = NULL;
    
    // 미션창
    if (missionType > 0)
    {
        // 배경
        spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "background/bg_mission.png", ccp(1, 1), ccp(m_winSize.width+35, vo.y+vs.height-50-50+7 -30-offset/2-7), CCSize(0, 0), "", "Puzzle", this, 6) );
        ((CCSprite*)spriteClassInfo->FindSpriteByName("background/bg_mission.png"))->setScale(1.2f);
     
        // 미션 타입에 따른 그림
        char name[30];
        float scale;
        int fontSize, padding;
        CCPoint pos;

        switch (missionType)
        {
            case 1:
                if (missionRefVal == 1) sprintf(name, "pieces/%d.png", PIECE_BLUE);
                else if (missionRefVal == 2) sprintf(name, "pieces/%d.png", PIECE_RED);
                else if (missionRefVal == 3) sprintf(name, "pieces/%d.png", PIECE_GREEN);
                scale = 0.5f;
                pos = ccp(55, 106);
                fontSize = 72;
                padding = 15;
                break;
            case 2:
                sprintf(name, "skill_%d.png", missionRefVal);
                scale = 0.6f;
                pos = ccp(55, 106);
                fontSize = 68;
                padding = 10;
                break;
            case 3:
                sprintf(name, "icon/icon_menu_coco.png");
                scale = 1.5f;
                pos = ccp(35, 111);
                fontSize = 88;
                padding = 20;
                break;
            case 4:
                sprintf(name, "icon/all_piece.png");
                scale = 0.85f;
                pos = ccp(45, 103);
                fontSize = 88;
                padding = 20;
                break;
        }
        spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), pos, CCSize(0, 0), "background/bg_mission.png", "0", NULL, 6, 1) );
        pMissionSprite = ((CCSprite*)spriteClassInfo->FindSpriteByName(name));
        pMissionSprite->setScale(scale);
        
        // 아래에 상세화면
        spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "background/bg_mission_detail.png", ccp(0.5, 0), ccp(m_winSize.width/2, vo.y+63 +5), CCSize(0, 0), "", "Puzzle", this, 100, 0, 0) );
        ((CCSprite*)spriteClassInfo->FindSpriteByName("background/bg_mission_detail.png"))->setScale((float)1044/(float)966);
        CCSize bs = ((CCSprite*)spriteClassInfo->FindSpriteByName("background/bg_mission_detail.png"))->getContentSize();
        pMissionSpriteDetail = ((CCSprite*)spriteClassInfo->FindSpriteByName("background/bg_mission_detail.png"));
        
        // 상세화면 내용
        sprintf(name, "%s2", name);
        spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0.5), ccp(0,0), CCSize(0, 0), "background/bg_mission_detail.png", "0", NULL, 100, 1, 0) );
        pMissionSpriteDetailIcon = ((CCSprite*)spriteClassInfo->FindSpriteByName(name));
        
        if (missionType == 1) scale = 0.6f;
        else if (missionType == 2) scale = 0.7f;
        else if (missionType == 3) scale = 1.3f;
        else if (missionType == 4) scale = 1.0f;
        ((CCSprite*)spriteClassInfo->FindSpriteByName(name))->setScale(scale);
        CCSize ps = ((CCSprite*)spriteClassInfo->FindSpriteByName(name))->getContentSize();
        ps.width *= scale;
        ps.height *= scale;
        
        // 상세화면의 미션 내용
        std::string missionContent = Common::GetMissionContent(missionType, missionVal, missionRefVal);
        spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel(missionContent, fontList[4], fontSize, ccp(1, 0.5), ccp(0,0), ccc3(255,255,255), "background/bg_mission_detail.png", "0", NULL, 100, 1, 0, 88888) );
        CCSize fs = ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(88888))->getContentSize();
        pMissionSpriteDetailContent = ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(88888));
        
        // (위치 지정)
        float offset = (ps.width + (float)padding + fs.width) / 2.0f;
        ((CCSprite*)spriteClassInfo->FindSpriteByName(name))->setPosition(ccp(bs.width/2-offset, bs.height/2));
        ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(88888))->setPosition(ccp(bs.width/2+offset, bs.height/2));
        
        // 달성해야 할 회수
        sprintf(name, "%d", missionVal);
        spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 32, ccp(0, 1), ccp(171+9, 137), ccc3(255,255,255), "background/bg_mission.png", "0", NULL, 6, 1, 255, 99997) );
        
        spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel("/", fontList[0], 32, ccp(0.5, 1), ccp(171, 137), ccc3(255,255,255), "background/bg_mission.png", "0", NULL, 6, 1, 255, 99998) );
        
        // 현재 달성한 회수
        iMissionCnt = 0;
        isMissionDone = false;
        sprintf(name, "%d", iMissionCnt);
        spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 32, ccp(1, 1), ccp(171-9, 137), ccc3(255,255,255), "background/bg_mission.png", "0", NULL, 6, 2, 255, 99999) );
        pMissionLabel = ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(99999));
    }
    
    for (int i = 0 ; i < spriteClassInfo->spriteObj.size(); i++)
        spriteClassInfo->AddChild(i);
}

void Puzzle::InitSprites()
{
    vs = CCDirector::sharedDirector()->getVisibleSize();
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    
    InitInfoBar();
    
    // timer bar 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png2", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f), CCSize(1044, 63), "", "Puzzle", this, 7) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png3", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1010, 35), "", "Puzzle", this, 7) );
    
    // 실제 timer bar
    timerLayer = CCLayer::create();
    timerLayer->setAnchorPoint(ccp(0,0));
    timerLayer->setPosition(ccp(0,0));
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_timer.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1000, 22), "", "Layer", timerLayer, 7) );
    
    tbSize = CCSize(1044, 63+20);
    //tbSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    
    // timer clock icon
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/clock.png", ccp(0.5, 0.5), ccp(m_winSize.width/2 + 1000/2 - 20-17-57/(float)2, vo.y+31.5f+5), CCSize(0,0), "", "Puzzle", this, 196) );
    pClock = ((CCSprite*)spriteClass->FindSpriteByName("icon/clock.png"));
    
    // timer bar에 clipping node 만들기
    timerStencil = CCDrawNode::create();
    CCPoint pos = ccp(m_winSize.width/2 - 1000/2, vo.y+31.5f+5 - 22/2);
    CCPoint ver[] = { ccp(pos.x, pos.y), ccp(pos.x+1000, pos.y), ccp(pos.x+1000, pos.y+22), ccp(pos.x, pos.y+22) };
    timerStencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    timerClip = CCClippingNode::create(timerStencil);
    timerClip->addChild(timerLayer);
    this->addChild(timerClip, 195);
    
    // 화면 비율에 맞춰 board size 만들기
    boardSize = CCSize(vs.height*1.920/2.920 - tbSize.height, vs.height*1.920/2.920 - tbSize.height);
    if (boardSize.height >= board_wh)
        boardSize = CCSize(board_wh, board_wh);
    puzzleLayer->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height/2));
    //CCLog("BOARD SIZE = %f", boardSize.height);
    
    // puzzle board
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0.5, 0.5), ccp(0, 0), CCSize(0, 0), "", "Layer", puzzleLayer, 20) );
    float oh = ((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->getContentSize().height;
    ((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->setScale((float)boardSize.height/(float)oh);
    
    // 피스를 보드판에 mask 적용하기 위해, 보드판을 하나 더 만들어둔다 (mask용)
    boardSP = CCSprite::createWithSpriteFrameName("background/board.png");
    boardSP->setPosition(ccp(0, 0));
    boardSP->setScale((float)boardSize.height/(float)oh);
    boardClip = CCClippingNode::create();
    boardClip->setInverted(false);
    boardClip->setAlphaThreshold(0);
    boardClip->setStencil(boardSP);
    puzzleLayer->addChild(boardClip, 21);
    
    // 화면 비율에 맞춰 piece 1개의 size 지정하기
    PIECE8_WIDTH = (float)152 * (float)boardSize.height/(float)board_wh;
    PIECE8_HEIGHT = (float)152 * (float)boardSize.height/(float)board_wh;
    PIECE8_FRAME_WIDTH = (float)154 * (float)boardSize.height/(float)board_wh;
    PIECE8_FRAME_HEIGHT = (float)154 * (float)boardSize.height/(float)board_wh;
    //CCLog("%f %f", PIECE8_WIDTH, PIECE8_HEIGHT);
    
    // 구름
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_cloud_near.png", ccp(1, 0), ccp(m_winSize.width, vo.y+vs.height*1.920f/2.920f+350), CCSize(0, 0), "", "Puzzle", this, 1) );
    // 배경 그림
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_upper.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height-30), CCSize(0, 0), "", "Puzzle", this, 0) );
    
    // 퍼즐판 뒤 잔디
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_grass.png", ccp(0, 1), ccp(0, vo.y+tbSize.height+boardSize.height+80), CCSize(0, 0), "", "Puzzle", this, 5) );
    // 길바닥
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_floor.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height+15), CCSize(0, 0), "", "Puzzle", this, 4) );
    floorSize = ((CCSprite*)spriteClass->FindSpriteByName("bg_floor.png"))->getContentSize();
    // 길바닥 위 작은 잔디
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_grass_mini.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height-5), CCSize(0, 0), "", "Puzzle", this, 3) );
    // 산
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_mountain.png", ccp(1, 0), ccp(m_winSize.width, vo.y+tbSize.height+boardSize.height+floorSize.height), CCSize(0, 0), "", "Puzzle", this, 2) );
    // 성
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_castle.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height+floorSize.height-35), CCSize(0, 0), "", "Puzzle", this, 2) );
    
    // 코코 그림자
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/coco_shadow.png", ccp(0.5, 0.5), ccp(215, vo.y+tbSize.height+boardSize.height+95), CCSize(0, 0), "", "Puzzle", this, 20, 0, 255, -2) ); // tag = -2 (피버타임 때 없애기)
    
    // 마법진
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/magic_circle.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120), CCSize(0, 0), "", "Puzzle", this, 6) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/magic_circle.png"))->setScale(0.9f);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    // 잔디 액션
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +3)), CCMoveBy::create(1.0f, ccp(0, -3)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    ((CCSprite*)spriteClass->FindSpriteByName("bg_grass.png"))->runAction(rep);
}

void Puzzle::InitCoco()
{
    cocoLayer = CCLayer::create();
    
    CCSprite* cap = CCSprite::createWithSpriteFrameName("anim/coco/cap.png");
    cap->setPosition(ccp(-10, 52));
    cocoLayer->addChild(cap, 102);
    
    CCSprite* capshadow = CCSprite::createWithSpriteFrameName("anim/coco/cap_shadow.png");
    capshadow->setPosition(ccp(2, 40));
    cocoLayer->addChild(capshadow, 99);
    
    CCSprite* head = CCSprite::createWithSpriteFrameName("anim/coco/head.png");
    head->setPosition(ccp(0, 0));
    cocoLayer->addChild(head, 100);
    
    CCSprite* body = CCSprite::createWithSpriteFrameName("anim/coco/body.png");
    body->setPosition(ccp(17, -97));
    cocoLayer->addChild(body, 98);
    
    CCSprite* leg1 = CCSprite::createWithSpriteFrameName("anim/coco/leg.png");
    leg1->setPosition(ccp(-10, -162));
    cocoLayer->addChild(leg1, 97);
    CCSprite* leg2 = CCSprite::createWithSpriteFrameName("anim/coco/leg.png");
    leg2->setPosition(ccp(30, -157));
    cocoLayer->addChild(leg2, 97);
    
    CCSprite* arm1 = CCSprite::createWithSpriteFrameName("anim/coco/arm_1.png");
    arm1->setPosition(ccp(10, -80));
    cocoLayer->addChild(arm1, 105);
    
    CCSprite* arm2 = CCSprite::createWithSpriteFrameName("anim/coco/arm_2.png");
    arm2->setPosition(ccp(15, -72));
    arm2->setOpacity(0);
    cocoLayer->addChild(arm2, 105);
    
    CCSprite* arm3 = CCSprite::createWithSpriteFrameName("anim/coco/arm_3.png");
    arm3->setPosition(ccp(20, -50)); // 3
    arm3->setOpacity(0);
    cocoLayer->addChild(arm3, 105);
    
    CCSprite* staff = CCSprite::createWithSpriteFrameName("anim/coco/staff.png");
    staff->setPosition(ccp(65, -27));  // 1
    cocoLayer->addChild(staff, 104);
    
    cocoLayer->setScale(0.8f);
    cocoLayer->setPosition(ccp(100, vo.y+tbSize.height+boardSize.height+60));
    this->addChild(cocoLayer, 100);
    
    cocoFrameNumber = 0;
    
    coco_sp.clear();
    coco_sp.push_back(cap);
    coco_sp.push_back(capshadow);
    coco_sp.push_back(head);
    coco_sp.push_back(arm1);
    coco_sp.push_back(arm2);
    coco_sp.push_back(arm3);
    coco_sp.push_back(staff);
    coco_sp.push_back(body);
    
    // action
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, +5)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    cocoLayer->runAction(rep);
}

void Puzzle::CocoAnim(float f)
{
    //CCLog("CocoAnim : %d", cocoFrameNumber);
    if (cocoFrameNumber == 0) // 2번
    {
        coco_sp[3]->setOpacity(0);
        coco_sp[4]->setOpacity(255);
        coco_sp[6]->setRotation(-18);
        coco_sp[6]->setPosition(ccp(60, 5));
    }
    else if (cocoFrameNumber == 1)//2) // 3번
    {
        coco_sp[0]->setPosition(ccp(coco_sp[0]->getPosition().x, coco_sp[0]->getPosition().y+2));
        coco_sp[1]->setPosition(ccp(coco_sp[1]->getPosition().x, coco_sp[1]->getPosition().y+2));
        coco_sp[2]->setPosition(ccp(coco_sp[2]->getPosition().x, coco_sp[2]->getPosition().y+2));
        coco_sp[7]->setPosition(ccp(coco_sp[7]->getPosition().x, coco_sp[7]->getPosition().y+2));
        
        coco_sp[4]->setOpacity(0);
        coco_sp[5]->setOpacity(255);
        
        coco_sp[5]->setRotation(0); // 3
        coco_sp[5]->setPosition(ccp(20, -50+2)); // 3
        coco_sp[6]->setRotation(-35); // 3
        coco_sp[6]->setPosition(ccp(47, 42+2)); // 3
    }
    else if (cocoFrameNumber == 2)//6) // 4번
    {
        coco_sp[0]->setPosition(ccp(coco_sp[0]->getPosition().x, coco_sp[0]->getPosition().y+4));
        coco_sp[1]->setPosition(ccp(coco_sp[1]->getPosition().x, coco_sp[1]->getPosition().y+4));
        coco_sp[2]->setPosition(ccp(coco_sp[2]->getPosition().x, coco_sp[2]->getPosition().y+4));
        coco_sp[7]->setPosition(ccp(coco_sp[7]->getPosition().x, coco_sp[7]->getPosition().y+4));
        
        coco_sp[5]->setRotation(-20); // 4
        coco_sp[5]->setPosition(ccp(10, -30+4)); // 4
        coco_sp[6]->setRotation(-50); // 3
        coco_sp[6]->setPosition(ccp(22, 50+4)); // 3
    }
    else if (cocoFrameNumber == 3)//12) // 5번
    {
        coco_sp[0]->setRotation(-10);
        coco_sp[0]->setPosition(ccp(-20, 52));
        coco_sp[1]->setRotation(-10);
        coco_sp[1]->setPosition(ccp(-8, 40));
        coco_sp[2]->setRotation(-10);
        coco_sp[2]->setPosition(ccp(-10, 0));
        coco_sp[7]->setRotation(-10);
        coco_sp[7]->setPosition(ccp(7, -97));
        
        coco_sp[5]->setRotation(-30); // 5
        coco_sp[5]->setPosition(ccp(5, -22)); // 5
        coco_sp[6]->setRotation(-65); // 5
        coco_sp[6]->setPosition(ccp(-15, 70)); // 5
    }
    else if (cocoFrameNumber == 4)//18) // 6번
    {
        coco_sp[0]->setRotation(0);
        coco_sp[0]->setPosition(ccp(-10, 52));
        coco_sp[1]->setRotation(0);
        coco_sp[1]->setPosition(ccp(2, 40));
        coco_sp[2]->setRotation(0);
        coco_sp[2]->setPosition(ccp(0, 0));
        coco_sp[7]->setRotation(0);
        coco_sp[7]->setPosition(ccp(17, -97));
        
        coco_sp[5]->setRotation(70); // 6
        coco_sp[5]->setPosition(ccp(10, -100)); // 6
        coco_sp[6]->setRotation(35); // 6
        coco_sp[6]->setPosition(ccp(100, -95)); // 6
    }
    else if (cocoFrameNumber == 5)//19) // 7번
    {
        coco_sp[0]->setRotation(10);
        coco_sp[0]->setPosition(ccp(10, 57));
        coco_sp[1]->setRotation(10);
        coco_sp[1]->setPosition(ccp(22, 45));
        coco_sp[2]->setRotation(10);
        coco_sp[2]->setPosition(ccp(20, 5));
        
        coco_sp[5]->setRotation(90);
        coco_sp[5]->setPosition(ccp(10, -100));
        coco_sp[6]->setRotation(90);
        coco_sp[6]->setPosition(ccp(85, -170));
    }
    else if (cocoFrameNumber == 6)//34) // 8번
    {
        coco_sp[0]->setRotation(5);
        coco_sp[0]->setPosition(ccp(0, 52));
        coco_sp[1]->setRotation(5);
        coco_sp[1]->setPosition(ccp(12, 40));
        coco_sp[2]->setRotation(5);
        coco_sp[2]->setPosition(ccp(10, 0));
        
        coco_sp[5]->setRotation(80);
        coco_sp[5]->setPosition(ccp(10, -90));
        coco_sp[6]->setRotation(80);
        coco_sp[6]->setPosition(ccp(85, -160));
    }
    else if (cocoFrameNumber == 7)//38) // 1번 (원래대로)
    {
        coco_sp[0]->setRotation(0);
        coco_sp[0]->setPosition(ccp(-10, 52));
        coco_sp[1]->setRotation(0);
        coco_sp[1]->setPosition(ccp(2, 40));
        coco_sp[2]->setRotation(0);
        coco_sp[2]->setPosition(ccp(0, 0));
        
        coco_sp[3]->setOpacity(255);
        coco_sp[4]->setOpacity(0);
        coco_sp[5]->setOpacity(0);
        
        coco_sp[6]->setRotation(0);
        coco_sp[6]->setPosition(ccp(65, -27));
        
        cocoFrameNumber = 0;
        
        this->unschedule(schedule_selector(Puzzle::CocoAnim));
    }
    
    cocoFrameNumber++;
}

void Puzzle::InitFairy()
{
    if (myInfo->GetActiveFairyId() > 0)
    {
        fairyTimer = 0; // 요정 액션 타이머
        switch (myInfo->GetActiveFairyId())
        {
            case 1:
                CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/fairy_flower.plist");
                fairyLayer = Fairy::MakeFlowerNew();
                fairyLayer->setPosition(ccp(m_winSize.width-280, vo.y+tbSize.height+boardSize.height+60+30));
                break;
            case 2:
                fairyLayer = Fairy::MakeSun();
                fairyLayer->setPosition(ccp(m_winSize.width-280+100, vo.y+tbSize.height+boardSize.height+60+30+135));
                break;
            case 3:
                CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/fairy_cloud.plist");
                fairyLayer = Fairy::MakeCloud();
                fairyLayer->setPosition(ccp(m_winSize.width-280-50, vo.y+tbSize.height+boardSize.height+60+30+30));
                break;
            default:
                fairyLayer = CCLayer::create();
                break;
        }
        this->addChild(fairyLayer, 100);
        
        // 요정 그림자
        fairyShadow = CCSprite::createWithSpriteFrameName("sun/sun_shadow.png");
        fairyShadow->setPosition(ccp(m_winSize.width-170, vo.y+tbSize.height+boardSize.height+95));
        fairyShadow->setScale(1.25f);
        fairyShadow->setTag(-1); // tag = -1 (그림자 피버타임 때 지우기 위해서)
        this->addChild(fairyShadow, 20);
        
        // action
        CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, +5)), NULL);
        CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
        fairyLayer->runAction(rep);
    }
}

void Puzzle::AnimFairy_Callback(CCNode* sender, void* pointer)
{
    sender->removeAllChildrenWithCleanup(true);
    sender->removeFromParentAndCleanup(true);
    fairyLayer->setVisible(true);
}

void Puzzle::ChangeAnimFairy(float f)
{
    // Pause 상태, 액티브 스킬 발동 중, 게임이 끝났을 때 -> 시간을 정지시킨다.
    if (isInGamePause || m_iSkillSP > 0 || isGameOver)
        return;
    
    // 요정 액션 관련 (2초마다 액션 실행)
    fairyTimer += 100;
    if (fairyTimer % 2000 == 0)
    {
        int n;
        CCPoint pos;
        switch (myInfo->GetActiveFairyId())
        {
            case 1: // 꽃등신
                n = rand() % 4;
                if (n == 0 || n == 1)
                {
                    // 기존 요정 잠시 안 보이게 한다.
                    fairyLayer->setVisible(false);
                    
                    // anim action
                    CCSize s = fairyLayer->getContentSize();
                    float w = s.width * 0.5f;
                    float h = s.height * 0.5f;
                    pos = ccp(m_winSize.width-280 + w-18, vo.y+tbSize.height+boardSize.height+60+30 + h-5);
                    
                    if (n == 0) // 꽃 접었다 펴기
                        Fairy::Anim_Flower_Hide(this, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                    else // 초록잎 파닥파닥
                        Fairy::Anim_Flower_Padac(this, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                }
                else if (n == 2) // 얼굴 왔다갔다하기
                    Fairy::Anim_Flower_MoveFace(fairyLayer);
                else // 눈썹 씰룩씰룩
                    Fairy::Anim_Flower_MoveEyebrow(fairyLayer);
                break;
            
            case 2: // 은근해
                n = rand() % 3;
                if (n == 0) // 한숨
                    Fairy::Anim_Sun_Sigh(fairyLayer, this, callfuncND_selector(Puzzle::AnimFairy_Callback));
                else if (n == 1) // 눈 움직이기
                    Fairy::Anim_Sun_MoveEye(fairyLayer);
                else // 꼬리 회전
                    Fairy::Anim_Sun_RotateTails(fairyLayer);
                break;
                
            case 3: // 구르미
                if (fairyTimer % 10000 == 0) // 이 요정은 10초에 한번 액션 발동.
                {
                    fairyLayer->setVisible(false);
                    pos = ccp(m_winSize.width-280-50, vo.y+tbSize.height+boardSize.height+60+30+30);
                    Fairy::Anim_Cloud_Curl(this, callfuncND_selector(Puzzle::AnimFairy_Callback), pos);
                }
                break;

            default:
                break;
        }
    }
}

void Puzzle::InitBoard()
{
    // 8각형
	for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;

            puzzleP8set->CreatePiece(x, y);
            spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
            puzzleP8set->AddChild(x, y);
		}
	}
    
    // 4각형
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            puzzleP4set->CreateObject(x, y);
            if (puzzleP4set->CreatePiece(x, y))
                puzzleP4set->AddChild(x, y);
        }
    }
    
    
    // 4각형, 8각형의 lock(semaphore)을 위한 것들
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            m_bLockP8[x][y] = 0;
            m_bLockP4[x][y] = 0;
        }
    }
    // 여러 동시 drop을 고려하여 drop ready queue를 5칸 할당해 놓는다.
    for (int i = 0 ; i < QUEUE_CNT ; i++)
    {
        std::vector<CCPoint> temp1;
        std::vector<CCPoint> temp2;
        std::vector<CCPoint> temp3;
        std::vector<CCPoint> temp4;
        std::vector<CCSprite*> temp5;
        std::vector<CCPoint> temp6;
        lock4xy.push_back(temp1);
        lock8xy.push_back(temp2);
        piece8xy.push_back(temp3);
        piece4xy.push_back(temp4);
        strap.push_back(temp5);
        posForFeverTime.push_back(temp6);
        
        lastPosition.push_back(CCPointZero);

        feverBombOrderCnt[i] = -1;
        
        // skill state 초기화
        m_iState[i] = -1;
        m_iNextState[i] = -1;
        // cycle
        m_bIsCycle[i] = false;
        // skill lock
        m_bSkillLock[i] = false;
        
        wasFallen[i] = true;
    }
    
    touch_cnt = 0;
    
    m_bTouchStarted = true;
    m_iSkillSP = 0;
    m_bIsSpiritExecuted = false;
    m_iSpiritSP = 0;
    m_iItemSP = 0;
}

void Puzzle::SetScoreAndStarCandy()
{
    //iScore = 0;
    iScore = FakeScore(0);
    //char n[10];
    //sprintf(n, "%d", RealScore());
    pScoreLayer = Common::MakeScoreLayer(RealScore());
    CCSize s = pScoreLayer->getContentSize();
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93-7));
    //pScoreLayer->setVisible(false);
    this->addChild(pScoreLayer, 6);
    
    iStarCandy = FakeStarCandy(0);
    pStarCandyLabel = (CCLabelTTF*)spriteClassInfo->FindLabelByTag(1);
}

int Puzzle::FakeScore(int score)
{
    return score*2 + 3982;
}
int Puzzle::RealScore()
{
    return (iScore-3982)/2;
}
int Puzzle::FakeStarCandy(int candy)
{
    return candy*3 + 271064;
}
int Puzzle::RealStarCandy()
{
    return (iStarCandy-271064)/3;
}

void Puzzle::UpdateScore(int type, int data)
{
    int score;
    if (type == 0)
    {
        // 기본점수 : (50 + 15(n-3)^1.2) * n
        // 기본점수 : (200 + MP/2 + 15(n-3)^1.2) * n
        // 기본드래그 개수, 피버로 터진 추가개수 따로 구하기
        int dragNum_basic = data;
        int dragNum_fever = 0;
        if (IsRoundInFeverTime(true))
        {
            dragNum_fever = dragNum_basic - GetPosForFeverTime(true).size();
            dragNum_basic = GetPosForFeverTime(true).size();
        }

        // 한붓그리기 기본점수 계산
        //score = ((50 + 15*pow(dragNum_basic-3, 1.2f)) * dragNum_basic); // 순수 드래그 개수로 따지는 기본점수
        score = (200 + myInfo->GetMPTotal()/2 + 15*pow(dragNum_basic-3, 1.2f)) * dragNum_basic;
        
        if (m_bIsCycle[(touch_cnt-1)%QUEUE_CNT]) // (1000 + 드래그점수)*2 = 사이클 점수
            score += (1000+score)*2;
        // 피버타임 때 추가 개수로 따지는 기본점수
        if (dragNum_fever > 0)
        {
            if (dragNum_fever <= 3)
                score += ((200 + myInfo->GetMPTotal()/2) * dragNum_fever);
            else
                score += ((200 + myInfo->GetMPTotal()/2 + 15*pow(dragNum_fever-3, 1.2f)) * dragNum_fever);
        }
    }
    else if (type == 1)
    {
        // 기본 스킬에 의한 추가점수 (A1)
        score = data;
    }
    
    iScore = FakeScore( RealScore() + score );

    pScoreLayer->removeAllChildren();
    pScoreLayer->removeFromParentAndCleanup(true);
    
    pScoreLayer = Common::MakeScoreLayer(RealScore());
    CCSize s = pScoreLayer->getContentSize();
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93-7));
    this->addChild(pScoreLayer, 6);
    
    // show score (type이 0이란 말은 기본 한붓그리기 점수를 의미함)
    // 각 1번스킬 때 보여주지 마라!
    int queue_pos = (touch_cnt-1)%QUEUE_CNT;
    if (!isGameOver && type == 0 &&
         (!skill->IsApplied(0, queue_pos) && !(skill->IsApplied(8, queue_pos)) && !(skill->IsApplied(16, queue_pos))) )
    {
        std::vector<CCPoint> pos;
        if (IsRoundInFeverTime(true))
            pos = GetPosForFeverTime(true);
        else
            pos = piece8xy[(touch_cnt-1)%QUEUE_CNT];
        
        int idx;
        if (m_bIsCycle[(touch_cnt-1)%QUEUE_CNT])
            idx = 0;
        else
            idx = pos.size()-1;
        
        ShowBasicScore(score, pos[idx], (int)pos.size());
    }
}

void Puzzle::ShowBasicScore(int score, CCPoint pos, int size) // 한붓그리기 기본점수
{
    float scale = GetScoreBasicScale(size);
    CCLayer* layer = Common::MakeItemNumberLayer(Common::MakeComma(score), scale);
    this->addChild(layer, 100);
    
    pos = SetTouch8Position(pos.x, pos.y);
    layer->setPosition(ccp(pos.x+layer->getContentSize().width/2, pos.y));
    
    int cnt = layer->getChildrenCount();
    for (int i = 0 ; i < cnt ; i++)
    {
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create(CCFadeIn::create(0.3f), CCFadeOut::create(0.3f), NULL), CCMoveBy::create(0.6f, ccp(0, 30)), NULL ), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ShowSkillScore_Callback), (void*)layer->getChildrenCount()), NULL );
        layer->getChildByTag(i)->runAction(action);
    }
}
void Puzzle::ShowSkillScore(int score, float scale, int queue_pos, int etc, int etc2, int height) // 스킬로 인한 발동점수 (+가중치)
{
    //CCLog("%d %.2f %d %d %d %d", score, scale, queue_pos, etc, etc2, height);
    CCLayer* layer = Common::MakeItemNumberLayer(Common::MakeComma(score), scale);
    this->addChild(layer, 100);
    
    CCPoint pos;
    if (queue_pos == -2) // 마법진 위 표시 (ex : 떡갈나무지팡이, 궁극스킬들)
        pos = ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120+30);
    else if (queue_pos == -1) // 코코 머리 위 (ex : 한방울한방울)
        pos = ccp(130, vo.y+tbSize.height+boardSize.height+60+220); //ccp(150, 1600);
    else if (etc == -1) // 기본 경우 (마지막 피스)
        pos = SetTouch8Position(lastPosition[queue_pos].x, lastPosition[queue_pos].y);
    else // (x, y)를 직접 받은 경우
        pos = SetTouch8Position(etc, etc2);
    
    layer->setPosition(ccp(pos.x+layer->getContentSize().width/2, pos.y+height));
    
    bool flag = false;
    float fadeInTime = 0.3f;
    float fadeOutTime = 0.3f;
    
    if (height > 0)
    {
        fadeInTime = 0.7f;
        fadeOutTime = 0.3f;
        flag = true;
    }
    else if (queue_pos >= 0 && m_iState[queue_pos] == SKILL_DOUBLESIX && globalType[queue_pos] != PIECE_RED)
    {
        // 얼음비, 상쾌한 바람 => fade_in을 길게 끌자.
        fadeInTime = 0.6f;
        flag = true;
    }
    
    if (!flag) // scale마다 fade in/out time 조절하기 (클수록 오래 보임)
    {
        if (scale < 1.5f) { fadeInTime = 0.3f; fadeOutTime = 0.3f; }
        else if (scale < 2.0f) { fadeInTime = 0.6f; fadeOutTime = 0.2f; }
        else if (scale < 2.5f) { fadeInTime = 0.7f; fadeOutTime = 0.3f; }
        else { fadeInTime = 0.9f; fadeOutTime = 0.3f; }
    }
    
    int cnt = layer->getChildrenCount();
    for (int i = 0 ; i < cnt ; i++)
    {
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create(CCFadeIn::create(fadeInTime), CCFadeOut::create(fadeOutTime), NULL), CCMoveBy::create(fadeInTime+fadeOutTime, ccp(0, 30)), NULL ), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ShowSkillScore_Callback), (void*)layer->getChildrenCount()), NULL );
        layer->getChildByTag(i)->runAction(action);
    }
}
void Puzzle::ShowSkillScore_Callback(CCNode* sender, void* data)
{
    //sender->removeAllChildren();
    if ((int)data == sender->getTag())
    {
        sender->removeFromParentAndCleanup(true);
        sender->getParent()->removeFromParentAndCleanup(true);
    }
    else
        sender->removeFromParentAndCleanup(true);
    //sender->removeFromParentAndCleanup(true);
}
/*
void Puzzle::ShowSkillAddedScore(int score, int queue_pos, int height, int x, int y)
{
    CCLayer* layer = Common::MakeItemNumberLayer(Common::MakeComma(score), 2);
    this->addChild(layer, 6000);

    CCPoint pos;
    if (x != -1 && y != -1) // '파도타기'
        pos = SetTouch8Position(x, y);
    else
        pos = SetTouch8Position(lastPosition[queue_pos].x, lastPosition[queue_pos].y);
    //label->setPosition(ccp(pos.x, pos.y+height*50));
    layer->setPosition(ccp(pos.x+layer->getContentSize().width/2, pos.y+height*110));
    
    int cnt = layer->getChildrenCount();
    for (int i = 0 ; i < cnt ; i++)
    {
        CCActionInterval* action = CCSequence::create( CCSpawn::create( CCSequence::create(CCFadeIn::create(0.3f), CCFadeOut::create(0.3f), NULL), CCMoveBy::create(0.6f, ccp(0, 30)), NULL ), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ShowSkillScore_Callback), (void*)layer->getChildrenCount()), NULL );
        layer->getChildByTag(i)->runAction(action);
    }
}
*/
float Puzzle::GetScoreBasicScale(int size) // 한붓그리기한 개수에 따라 점수 크기 조정
{
    if (isFeverTime)
        size = GetPosForFeverTime(true).size();
    float scale = 1.0f;
    if (size <= 5) scale = 1.0f;
    else if (size <= 9) scale = 1.3f;
    else if (size <= 14) scale = 1.5f;
    else if (size <= 19) scale = 2.0f;
    else if (size >= 20) scale = 3.0f;
    //CCLog("Size = %d , scale = %f", size, scale);
    return scale;
}


void Puzzle::UpdateStarCandy(int type, int data, bool isCycle)
{
    int added = 1; // 기본값 1을 주고,
    if (isCycle) // 사이클이면 5를 더 추가한다.
        added += 5;
    
    if (type == 0)
    {
        int basic;
        if (data <= 5) basic = 0;
        else if (data <= 9) basic = 3;
        else if (data <= 14) basic = 6;
        else if (data <= 19) basic = 9;
        else basic = 12;
        
        iStarCandy = FakeStarCandy( RealStarCandy() + (basic + data + added) );
    }
    else if (type == 1)
    {
        iStarCandy = FakeStarCandy( RealStarCandy() + (data + added) );
    }
    char s[8];
    sprintf(s, "%d", RealStarCandy());
    pStarCandyLabel->setString(s);
}

void Puzzle::SetCombo()
{
    iCombo = 0;
    maxCombo = 0;
    iFeverCombo = 0;
    forFeverTimeCnt = 0;
}

int Puzzle::GetCombo()
{
    return iCombo;
}

void Puzzle::UpdateCombo()
{
    iComboTimer = 0;
    iCombo++;
    maxCombo = std::max(maxCombo, iCombo);
    
    char temp[11];
    sprintf(temp, "%d Combo!", iCombo);
    
    if (!isFeverTime && !isGameOver && !isInGamePause)
    {
        iFeverCombo++;
        CCLog("FEVER COMBO = %d", iFeverCombo);
        
        // 피버 게이지 증가 action
        CCActionInterval* action = CCMoveBy::create(0.1f, ccp(149.0f / (float)feverStartCnt, 0));
        ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->runAction(action);
    
        if (iFeverCombo >= feverStartCnt && !isGameOver) // 15콤보가 되면 피버타임!
        {
            StartFeverTime();
        }
    }
        
    if (iCombo == 1)
    {
        // 처음 콤보가 적용될 때만 타이머 적용을 하면 된다.
        this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    }
    
    if (iCombo > 0 && iCombo % 10 == 0)
    {
        // 10의 배수마다 W3(콤보비례추가점수) 스킬 발동
        skill->Invoke(10, NULL);
    }
    
    if (iCombo > 1) // 2콤보부터 화면에 노출
    {
        CCLayer* pComboLayer = Common::MakeCombo(iCombo);
        pComboLayer->setAnchorPoint(ccp(0, 0));
        CCPoint p = cocoLayer->getPosition();
        pComboLayer->setPosition(ccp(p.x-30, p.y+10));
        this->addChild(pComboLayer, 3000);
        
        CCFiniteTimeAction* action = CCSequence::create(CCFadeIn::create(0.3f), CCDelayTime::create(0.15f), CCFadeOut::create(0.3f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ComboCallback), NULL), NULL);
        
        int cnt = pComboLayer->getTag();
        for (int i = 0 ; i <= cnt ; i++)
            pComboLayer->getChildByTag(i)->runAction((CCActionInterval*)action->copy());
        pComboLayer->runAction((CCActionInterval*)action->copy());
    }
}
void Puzzle::ComboCallback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
}

void Puzzle::ComboTimer(float f)
{
    // 피스 renew 중, Pause 상태, 액티브 스킬 발동 중 -> 시간을 정지시킨다.
    if (isRenewing || isInGamePause || m_iSkillSP > 0)
        return;
    
    iComboTimer += 100;
    if (iComboTimer >= MAX_COMBO_TIME)
    {
        this->unschedule(schedule_selector(Puzzle::ComboTimer));
        
        maxCombo = std::max(maxCombo, iCombo);
        iCombo = 0;
    }
}

void Puzzle::HintTimer(float f)
{
    // 피스 renew 중, Pause 상태, 액티브 스킬 발동 중, 터치 중, 게임이 끝났을 때 -> 시간을 정지시킨다.
    if (isRenewing || isInGamePause || m_iSkillSP > 0 || m_bTouchStarted || isGameOver)
    {
        iHintTime = 0;
        return;
    }
    
    iHintTime += 100;
    if (!isHintShown && iHintTime >= 2500) // 2.5초 동안
    {
        isHintShown = true;
        skill->GiveHint();
    }
}
bool Puzzle::IsHintShown()
{
    return isHintShown;
}

void Puzzle::FeverTimer(float f)
{
    // 스킬발동중 + Pause 상태 + 게임오버 + 리뉴잉 때는 중지
    if (m_iSkillSP > 0 || isGameOver || isInGamePause || isRenewing)
        return;
    
    iFeverTime -= 100;
    //CCLog("fever time = %d", iFeverTime);
    
    if (iFeverTime < 1000) // 1초 남은 시점에서부터 소리를 서서히 줄인다.
    {
        sound->ReduceBackgroundMusicVolume(iFeverTime/100);
    }
    
    // 피버 게이지 줄이기
    if (iFeverTime > 0)
    {
        CCPoint p = ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->getPosition();
        float changed_x = p.x - (149.0f * (100.0f / (float)(FEVER_TIME*1000)));
        ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->setPosition(ccp(changed_x, p.y));
    }
    // 피버타임 끝
    else //if (iFeverTime <= 0)
    {
        if (isGameOver || isInGamePause || isRenewing)
            return;
        this->unschedule(schedule_selector(Puzzle::FeverTimer));
        effect->PlayEffect_FeverBg_Off();
        iFeverCombo = 0;
        isFeverTime = false;
        CCLog("피버타임 끝.................");
        
        // 그림자 없애기
        ((CCSprite*)spriteClass->FindSpriteByTag(-2))->setOpacity(255);
        if (myInfo->GetActiveFairyId() > 0)
            fairyShadow->setOpacity(255);
        
        if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
        {
            volumeCnt = 0;
            if (!isInGamePause)
                sound->PlayBackgroundInGameSound(2);
        }
    }
}

void Puzzle::ReadyAndStart(CCNode* sender, void* pointer)
{
    if (sender != NULL)
        sender->removeFromParentAndCleanup(true); // pBlackOpen 제거
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    // 정령 튜토리얼을 실행해야 한다면, 그것부터!
    //CCLog("spirit_scid = %d", spirit_scid);
    //CCLog("client data = %d", CCUserDefault::sharedUserDefault()->getBoolForKey("spirit_tutorial", false));
    if (spirit_scid != -1 && !CCUserDefault::sharedUserDefault()->getBoolForKey("spirit_tutorial", false))
    {
        // 미션 상세화면 안 보여준다.
        if (missionType > 0)
        {
            pMissionSpriteDetail->stopAllActions();
            pMissionSpriteDetailIcon->stopAllActions();
            pMissionSpriteDetailContent->stopAllActions();
            pMissionSpriteDetail->setOpacity(0);
            pMissionSpriteDetailIcon->setOpacity(0);
            pMissionSpriteDetailContent->setOpacity(0);
        }
        
        CCPoint p;
        if (spirit_scid == 23)
        {
            spirit_sp = CCSprite::createWithSpriteFrameName("icon/spirit_fire.png");
            p = SetTouch8Position(COLUMN_COUNT-1, 0);
            balloon = CCScale9Sprite::create("images/tutorial_balloon2.png");
            balloon->setAnchorPoint(ccp(1, 0));
            balloon->setPosition(ccp(m_winSize.width-10, p.y));
            ball = CCLabelTTF::create("안녕 코코야! 게임중에 나를 터치하면\n사랑의 불꽃으로 붉은 피스를 퍼뜨릴 수 있어!\n(게임을 시작하려면 나를 터치해)", fontList[0].c_str(), 36);
        }
        else if (spirit_scid == 13)
        {
            spirit_sp = CCSprite::createWithSpriteFrameName("icon/spirit_water.png");
            p = SetTouch8Position(COLUMN_COUNT-1, ROW_COUNT-1);
            balloon = CCScale9Sprite::create("images/tutorial_balloon2.png");
            balloon->setAnchorPoint(ccp(1, 0));
            balloon->setPosition(ccp(m_winSize.width-10, p.y));
            ball = CCLabelTTF::create("안녕 코코야! 게임중에 나를 터치하면\n흰색/노란색 피스를 파란 피스로 바꿀 수 있어!\n(게임을 시작하려면 나를 터치해)", fontList[0].c_str(), 36);
        }
        else if (spirit_scid == 33)
        {
            spirit_sp = CCSprite::createWithSpriteFrameName("icon/spirit_land.png");
            p = SetTouch8Position(0, ROW_COUNT-1);
            balloon = CCScale9Sprite::create("images/tutorial_balloon.png");
            balloon->setAnchorPoint(ccp(0, 0));
            balloon->setPosition(ccp(10, p.y));
            ball = CCLabelTTF::create("안녕 코코야! 게임중에 나를 터치하면\n초록색 피스를 모을 수 있어!\n(게임을 시작하려면 나를 터치해)", fontList[0].c_str(), 36);
        }
        spirit_sp->setPosition(p);
        this->addChild(spirit_sp, 100);
        
        balloon->setContentSize(CCSize(720, 240));
        this->addChild(balloon, 100);
        ball->setPosition(ccp(720/2, 240/2+30));
        ball->setColor(ccc3(255,255,255));
        balloon->addChild(ball, 101);
        
        CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
        balloon->runAction( CCRepeatForever::create(action) );
    }
    
    else // 게임 준비
    {
        sound->PlayVoice(VOICE_READY);
        
        readySprite = CCSprite::createWithSpriteFrameName("letter_ready.png");
        readySprite->setAnchorPoint(ccp(0.5, 0.5));
        readySprite->setPosition(ccp(m_winSize.width/2+20, vo.y+tbSize.height+boardSize.height+120+180));
        readySprite->setScale(1.2f);
        readySprite->setOpacity(0);
        this->addChild(readySprite, 5000);
        
        CCActionInterval* action = CCSequence::create( CCFadeIn::create(0.5f), CCDelayTime::create(0.8f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::Ready_C), NULL), NULL);
        readySprite->runAction(action);
    }
}
void Puzzle::Ready_C(CCNode* sender, void* p)
{
    sound->PlayVoice(VOICE_GO);
    
    sender->removeFromParentAndCleanup(true);
    readySprite = NULL;
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName("letter_go.png");
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120+180));
    sprite->setScale(1.2f);
    this->addChild(sprite, 5000);
    
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyCallback), this), NULL);
    sprite->runAction(action);

    // 미션 상세화면 fade out
    if (missionType > 0)
    {
        pMissionSpriteDetail->runAction( CCFadeOut::create(0.3f) );
        pMissionSpriteDetailIcon->runAction( CCFadeOut::create(0.3f) );
        pMissionSpriteDetailContent->runAction( CCFadeOut::create(0.3f) );
    }
}

void Puzzle::ReadyCallback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
    
    isGameStarted = true;
    m_bTouchStarted = false;
    
    // 배경음 재생
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
    {
        volumeCnt = 10;
        if (!isInGamePause)
            sound->PlayBackgroundInGameSound(0);
    }
    
    ((Puzzle*)pointer)->schedule(schedule_selector(Puzzle::UpdateTimer), 0.1f);
    ((Puzzle*)pointer)->schedule(schedule_selector(Puzzle::HintTimer), 0.1f);
    if (myInfo->GetActiveFairyId() > 0)
        ((Puzzle*)pointer)->schedule(schedule_selector(Puzzle::ChangeAnimFairy), 0.1f);
}

void Puzzle::SetTimer()
{
    CCScale9Sprite* timerbar = ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_timer.png"));
    CCPoint p = timerbar->getPosition();
    CCSize s = timerbar->getContentSize();
    
    //feverRemainTime = 0;
    
    iTimer = 1000 * PUZZLE_TIME;
    if (item_time)
        iTimer += (1000 * 5);

    char n[5];
    sprintf(n, "%d", PUZZLE_TIME + (item_time * 5));
    pTimerLabel = CCLabelTTF::create(n, fontList[0].c_str(), 24);
    pTimerLabel->setAnchorPoint(ccp(0.5, 0.5));
    pTimerLabel->setColor(ccc3(0,0,0));
    pTimerLabel->setPosition(ccp(m_winSize.width/2 + 1000/2 - 20, vo.y+31.5f+5+2));
    this->addChild(pTimerLabel, 1000);
}

void Puzzle::UpdateTimer(float f)
{
    //////////////////////////////////////// 시간을 정지해야 하는 상황들 ////////////////////////////////////
    // 피스 renew 중, Pause 상태, 액티브 스킬 발동 중, 게임이 끝났을 때
    if (isRenewing || isInGamePause || m_iSkillSP > 0 || isGameOver)
        return;
    
    // "W7 : 시간을 얼리다" 스킬이 적용 중이면 5초 동안 정지한다.
    if (skill->W7_IsFreezed())
    {
        skill->W7SetTime(skill->W7GetTime()-100);
        if (skill->W7GetTime() == 0)
        {
            skill->W7_SetNonFreeze(); // false로 바꿔 다시 시간을 흐르게 한다.
            //bFreezeFlag = false;
            PlayEffect(14, NULL);
        }
        return;
    }
    
    // "W8 : 여신의 은총" 스킬이 적용 중이면 정지한다.
    if (skill->W8_IsActive())
        return;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    iTimer -= 100;
    
    // timer 이동
    float delta = (float)1000/((PUZZLE_TIME + (item_time*5))*10);
    CCPoint tl = timerLayer->getPosition();
    CCPoint tp = timerClip->getPosition();
    timerClip->setPosition(ccp(tp.x-delta, tp.y));
    timerLayer->setPosition(ccp(tl.x+delta, tl.y)); // timer bar
    

    if (volumeCnt < 10) // 1초동안 소리를 서서히 높인다.
        sound->GainBackgroundMusicVolume(++volumeCnt);
    
    if (iTimer > 5000)
    {
        CCPoint tc = pClock->getPosition();
        pTimerLabel->setPosition(ccp(pTimerLabel->getPosition().x-delta, pTimerLabel->getPosition().y)); // timer number
        pClock->setPosition(ccp(tc.x-delta, tc.y)); // clock
    }
    
    // 5초 남았을 때 이펙트+목소리 표현
    if (iTimer == 5000)
    {
        // 성우 소리 + 시계 소리
        sound->PlayVoice(VOICE_TIMELIMIT);
        sound->PlayClock();
        
        timelimit = CCSprite::createWithSpriteFrameName("letter_timelimit.png");
        timelimit->setAnchorPoint(ccp(0.5, 0.5));
        CCPoint p = SetTouch8Position(0, 0);
        timelimit->setPosition(ccp(p.x+30, p.y+30));
        timelimit->setScale(1.2f);
        this->addChild(timelimit, 5000);
        
        CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.05f, -10), CCRotateBy::create(0.1f, 20), CCRotateBy::create(0.1f, -20), CCRotateBy::create(0.05f, 10), NULL);
        timelimit->runAction(action);
        
        // 시계 살짝 키우고 색깔블링크+흔들기 시작
        pTimerLabel->setColor(ccc3(255,255,255));
        pTimerLabel->setFontSize(30);
        pTimerLabel->setPosition(ccp(pTimerLabel->getPosition().x+5, pTimerLabel->getPosition().y));
        pClock->setScale(1.5f);
        pClock->setPosition(ccp(pClock->getPosition().x-5, pClock->getPosition().y)); // 위치 살짝 왼쪽으로
        ccColor3B color = pClock->getColor();
        CCActionInterval* action2 = CCSpawn::create( CCSequence::create(CCTintTo::create(0.2f, 255, 0, 0), CCTintTo::create(0.2f, color.r, color.g, color.b), NULL ), CCSequence::create(CCRotateBy::create(0.15f, 20), CCRotateBy::create(0.3f, -40), CCRotateBy::create(0.15f, 20), NULL), NULL );
        pClock->runAction( CCRepeatForever::create(action2) );
    }
    else if (iTimer < 5000 && iTimer % 1000 == 0) // 4,3,2,1초에 '시간이없어' 문구 흔들거리기
    {
        CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.05f, -10), CCRotateBy::create(0.1f, 20), CCRotateBy::create(0.1f, -20), CCRotateBy::create(0.05f, 10), NULL);
        timelimit->runAction(action);
    }
    
    if (iTimer % 1000 == 0)
    {
        char temp[3];
        sprintf(temp, "%d", iTimer/1000);
        pTimerLabel->setString(temp);
        
        if (iTimer % 3000 == 0 && iTimer > 5000)
        {
            // 시계 아이콘 흔들거리기 (매 3초마다) (5초 이하로는 하지 않는다)
            CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.15f, 30), CCRotateBy::create(0.3f, -60), CCRotateBy::create(0.15f, 30), NULL);
            pClock->runAction(action);
        }
        
        if (iTimer == 0) // game over
        {
            CCLog("Game Over (%d)", iTimer);
            isGameOver = true;
            isKeybackTouched = true;
            if (isFeverTime)
            {
                this->unschedule(schedule_selector(Puzzle::FeverTimer));
                effect->PlayEffect_FeverBg_Off();
                iFeverCombo = 0;
                isFeverTime = false;
            }
            this->unschedule(schedule_selector(Puzzle::UpdateTimer));
            this->unschedule(schedule_selector(Puzzle::HintTimer));
            this->unschedule(schedule_selector(Puzzle::ComboTimer));;
            if (myInfo->GetActiveFairyId() > 0)
                this->unschedule(schedule_selector(Puzzle::ChangeAnimFairy));
            this->setTouchEnabled(false);
            this->setKeypadEnabled(false);
            sound->StopBackgroundSound();
            
            // hint 취소
            if (isHintShown)
                StopAllActionsAtPieces();
            
            // 그리고 있던 것들 모두 취소
            CancelDrawing();
            timelimit->removeFromParentAndCleanup(true);
            pClock->setOpacity(0);
            pTimerLabel->setOpacity(0);
            ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_outer.png2"))->setOpacity(0);
            ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_inner.png3"))->setOpacity(0);
            ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_timer.png"))->setOpacity(0);
            
            overBg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
            overBg->setAnchorPoint(ccp(0, 0));
            overBg->setPosition(ccp(0, 0));
            overBg->setColor(ccc3(0,0,0));
            overBg->setOpacity(220);
            this->addChild(overBg, 9990);
            
            CCSprite* sprite = CCSprite::createWithSpriteFrameName("icon_gameover.png");
            sprite->setAnchorPoint(ccp(0.5, 0.5));
            sprite->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
            this->addChild(sprite, 9991);
            
            CCActionInterval* action = CCSequence::create(CCDelayTime::create(2.0f), CCCallFuncND::create((CCObject*)Depth::GetCurPointer(), callfuncND_selector(Puzzle::GameOver_Callback), this), NULL);
            sprite->runAction(action);
            
            // sound
            sound->StopClock(); // 시계소리 멈춤
            sound->PlayVoice(VOICE_GAMEOVER);
        }
        
        // 정령 준비 발동 ('시간을 얼리다' 발동 중에는 NO!)
        if (iTimer % 2000 == 0 && !skill->W7_IsFreezed())
        {
            skill->SpiritTry(0, 0);
            skill->SpiritTry(1, 0);
            skill->SpiritTry(2, 0);
        }
    }
    
    /*
    if (iTimer % 10000 == 0)
    {
        CCLog("요정이 연결피스 하나 생성하라고 함");
        iNumOfFairySkillStart++;
    }
    */
    if (IsItemStaff() && iNumOfStaffItemRemained > 0 && (PUZZLE_TIME*1000 - iTimer) % Time100(totalCnt_staff) == 0)
    {
        CCLog("staff 아이템 +1 , (%d)", (PUZZLE_TIME*1000-iTimer));
        iNumOfStaffItemRemained--;
        iNumOfStaffItemNeeded++;
    }
    if (IsItemPaint() && iNumOfPaintItemRemained > 0 && (PUZZLE_TIME*1000 - iTimer) % Time100(totalCnt_paint) == 0)
    {
        CCLog("paint 아이템 +1 , (%d)", (PUZZLE_TIME*1000-iTimer));
        iNumOfPaintItemRemained--;
        iNumOfPaintItemNeeded++;
    }
}
             
int Puzzle::Time100(int denom) // 아이템 나타나는 시간 계산용
{
    int interval = (int)(((float)PUZZLE_TIME / (float)(denom+1)) * 1000.0f);
    if (interval % 100 >= 50) // 100자리까지 반올림
        interval = interval - interval%100 + 100;
    else // 100자리 내림
        interval = interval - interval%100;
    
    //CCLog("TIME100  :  %d", interval);

    return interval;
}

void Puzzle::GameOver_Callback(CCNode* sender, void* pointer)
{
    CCLog("GameOver_Callback");
    sender->removeFromParentAndCleanup(true);
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName("icon_bonustime.png");
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    this->addChild(sprite, 9991);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.5f), CCCallFuncND::create((Puzzle*)pointer, callfuncND_selector(Puzzle::BonusTime), pointer), NULL);
    sprite->runAction(action);
    
    // sound
    sound->PlayVoice(VOICE_BONUS);
    
    bonusTimeState = -2;
}

void Puzzle::BonusTime(CCNode* sender, void* pointer)
{
    Puzzle* pThis = (Puzzle*)pointer;
    
    if (pThis->bonusTimeState == -2)
    {
        pThis->overBg->removeFromParentAndCleanup(true);
        sender->removeFromParentAndCleanup(true);
    }
    
    pThis->bonusTimeState++;
    CCLog("BonusTime : %d", pThis->bonusTimeState);
    
    // delay 주기 (boardSP는 그냥 fake sprite)
    float delayTime = 0.7f;
    if (pThis->bonusTimeState == -1)
        delayTime = 0.01f;
    else if (pThis->bonusTimeState <= 1)
        delayTime = 0.3f;
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(delayTime), CCCallFuncND::create(pThis, callfuncND_selector(Puzzle::BonusTime_Callback), pThis), NULL );
    boardSP->runAction(action);
}
void Puzzle::BonusTime_Callback(CCNode* sender, void* pointer)
{
    Puzzle* pThis = (Puzzle*)pointer;
    
    // 기본보너스폭발(+떡갈나무지팡이) + 색깔붓 + 지팡이 + 끈질긴생명력 순서대로 진행함.
    // 다 끝나면 game_end.php 호출.
    switch (pThis->bonusTimeState)
    {
        case -1:
            bonusBombCnt = rand()%5 + 3;
            if (skill->IsSkillNumberExists(18)) // 떡갈나무지팡이 스킬 발동
                skill->Invoke(18, NULL);
            pThis->skill->BonusBomb(0); // 보너스 폭발 (3~7회 랜덤 덩어리) => 떡갈나무지팡이 발동한 경우 별사탕 이펙트도 같이!
            break;
        case 0: // item (paint)
            pThis->skill->LastItems(pThis->bonusTimeState, 0);
            break;
        case 1: // item (staff)
            pThis->skill->LastItems(pThis->bonusTimeState, 0);
            break;
        case 2: // 끈질긴 생명력
            if (addedPotion > 0)
                pThis->skill->Invoke(22, NULL);
            else
                pThis->GameEnd(NULL, pThis);
            break;
        default:
            pThis->GameEnd(NULL, pThis);
            break;
    }
}

void Puzzle::SetGlobalType(int queue_pos, int pieceType)
{
    globalType[queue_pos] = pieceType;
}

void Puzzle::StartFeverTime() // feverTime 시작 세팅
{
    if (!isFeverTime && !isGameOver && !isInGamePause)
    {
        if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
        {
            volumeCnt = 10;
            if (!isInGamePause)
                sound->PlayBackgroundInGameSound(1);
        }
        
        CCLog("피버타임 시작!!!");
        ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->stopAllActions();
        int offset = 20;
        ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->setPosition(ccp(m_winSize.width-170, vo.y+vs.height-45+2-offset/2-7 ));
        
        // 그림자 없애기
        ((CCSprite*)spriteClass->FindSpriteByTag(-2))->setOpacity(0);
        if (myInfo->GetActiveFairyId() > 0)
            fairyShadow->setOpacity(0);
        
        effect->PlayEffect_FeverBg();
        
        iFeverCombo = 0;
        iFeverTime = FEVER_TIME_MAX;
        isFeverTime = true;
        this->schedule(schedule_selector(Puzzle::FeverTimer), 0.1f);
    }
}
void Puzzle::EndFeverTime()
{
    /*
    if (!skill->IsFTBombing())
    {
    isFeverTime = false;
    CancelDrawing();
    skill->FT_StartEnd(touch_cnt%QUEUE_CNT);
    feverRemainTime = 0;
    forFeverTimeCnt = 0;
    }
     */
}
bool Puzzle::IsFeverTime()
{
    return isFeverTime;
}
bool Puzzle::IsRoundInFeverTime(bool afterCast)
{
    if (afterCast)
        return isFeverRound[(touch_cnt-1)%QUEUE_CNT];
    return isFeverRound[touch_cnt%QUEUE_CNT];
}
int Puzzle::GetFeverRemainTime()
{
    return feverRemainTime;
}

// 연결피스 하나 생성하기 by 요정
void Puzzle::FairySkillAction()
{
    // 요정이 없으면 행하지 않는다.
    if (myInfo->GetActiveFairyId() <= 0)
        return;
    
    std::vector<CCPoint> pos;
    pos.clear();
    
    for (int x = 1 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            if (!IsConnected(x, y))
                pos.push_back(ccp(x, y));
        }
    }
    if ((int)pos.size() == 0)
        return;
    
    int idx = rand() % (int)pos.size();
    int x = pos[idx].x;
    int y = pos[idx].y;
    pos.clear();
    
    effect->FairySkill(); // 요정 이펙트
    effect->NewlyMadeConnPiece(x, y); // 새로 만들어진 연결피스에 나타날 이펙트
    
    // 연결피스 실제 생성
    //puzzleP4set->CreatePiece(x, y, -100, CONNECTED);
    //puzzleP4set->AddChild(x, y);
    
    CCLog("Fairy SKILL ACTION = (%d, %d)", x, y);
}


// 터치에 대한 좌표 계산 (보드판 [x][y]의 중심 좌표(실제 좌표시스템의 좌표)를 리턴해줌.
CCPoint Puzzle::SetTouch8Position(int x, int y)
{
    CCPoint relative = SetPiece8Position(x, y);
    CCPoint layer = puzzleLayer->getPosition();
    return ccp(relative.x + layer.x, relative.y + layer.y);
}
// 터치에 대한 좌표 계산 (보드판 [x][y]의 중심 좌표(실제 좌표시스템의 좌표)를 리턴해줌.
CCPoint Puzzle::SetTouch4Position(int x, int y)
{
    CCPoint relative = SetPiece4Position(x, y);
    CCPoint layer = puzzleLayer->getPosition();
    return ccp(relative.x + layer.x, relative.y + layer.y);
}

CCPoint Puzzle::SetPiece8Position(int x, int y)
{
    int posX = (x-3)*PIECE8_WIDTH;
    int posY = (y-3)*PIECE8_HEIGHT;
    return ccp(posX, posY);
}
CCPoint Puzzle::SetPiece4Position(int x, int y)
{
    float posX = (float)(x-3)*PIECE8_WIDTH - PIECE8_WIDTH/2.0f;
    float posY = (float)(y-3)*PIECE8_HEIGHT - PIECE8_HEIGHT/2.0f;
    return ccp(posX, posY);
}

/*
bool IsValidInCircle(CCPoint center, CCPoint point)
{
    float radius = 53.0;
    float sq = (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y);
    
    return sq <= radius*radius;
}

bool IsValidInOcta(CCPoint center, CCPoint point)
{
    int rect = 54, tri = 32;
    CCPoint lu = ccp(center.x-rect, center.y+rect);
    CCPoint ld = ccp(center.x-rect, center.y-rect);
    CCPoint ru = ccp(center.x+rect, center.y+rect);
    CCPoint rd = ccp(center.x+rect, center.y-rect);
    if (abs(point.x-lu.x)+abs(point.y-lu.y) <= tri || abs(point.x-ld.x)+abs(point.y-ld.y) <= tri ||
        abs(point.x-ru.x)+abs(point.y-ru.y) <= tri || abs(point.x-rd.x)+abs(point.y-rd.y) <= tri) {
        return false;
    }
    if ((center.x-rect <= point.x && point.x <= center.x+rect) &&
        (center.y-rect <= point.y && point.y <= center.y+rect)) {
        return true;
    }
    return false;
}
*/

bool Puzzle::IsValidInSquare(CCPoint center, CCPoint point)
{
    // 처음 터치할 때만 보는 것이므로, 완전한 정4각형 범위까지 허용한다.
    //return abs(center.x-point.x)+abs(center.y-point.y) < PIECE8_WIDTH-1;
    if ((center.x-PIECE8_WIDTH/2 < point.x && point.x < center.x+PIECE8_WIDTH/2) &&
        (center.y-PIECE8_WIDTH/2 < point.y && point.y < center.y+PIECE8_WIDTH/2)) {
        return true;
    }
    return false;
}

CCPoint Puzzle::BoardStartPosition(CCPoint point)
{
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            //if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
            //    (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
            if (x == 0 && y == 0)
                continue;
            
            //if (IsValidInOcta(SetPiece8Position(x, y), point))
            if (IsValidInSquare(SetTouch8Position(x, y), point))
                return ccp(x, y);
		}
	}
    return ccp(-1, -1);
}


int Puzzle::GetDist(CCPoint center, CCPoint point)
{
    int dist = (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y);
    if (dist > PIECE8_WIDTH*PIECE8_WIDTH + (PIECE8_HEIGHT/2)*(PIECE8_HEIGHT/2))
        return 999999999;
    return dist;
}

CCPoint Puzzle::BoardMovePosition(CCPoint point)
{
    CCPoint resultXY = ccp(-1, -1);
    int dist = 999999999;
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            if (puzzleP8set->GetObject(x, y) == NULL)
                continue;

            int d = GetDist(SetTouch8Position(x, y), point);
            if ( d < dist && (globalType[touch_cnt%QUEUE_CNT] == puzzleP8set->GetType(x, y) || m_bIsItemTouched) )
            {
                dist = d;
                resultXY = ccp(x, y);
            }
		}
	}
    return resultXY;
}


void Puzzle::StartMagicTime(float f)
{
    if (m_iStartMagicTimeStatus < 30)
    {
        float vol = 0.7f * (1.0f - (float)(m_iStartMagicTimeStatus+1)/(float)30);
        sound->SetBackgroundMusicVolume(vol);
        magicTimeBg->setOpacity(magicTimeBg->getOpacity()+(float)160/(float)30);
        //CCLog("opacity : %d", (int)magicTimeBg->getOpacity());
        m_iStartMagicTimeStatus++;
    }
    else
    {
        //CCLog("magic time done");
        this->unschedule(schedule_selector(Puzzle::StartMagicTime));
        sound->StopBackgroundSound();
        sound->SetBackgroundMusicVolume(0.7f);
        
        m_bTouchStarted = false;
        isMagicTime = false;
    }
}

void Puzzle::PauseGame()
{
    if (isInGamePause)
        return;
    isInGamePause = true;
    
    // '레디'->'고' 사이가 문제가 되기 때문에, 그 액션을 pause 시키자.
    //CCDirector::sharedDirector()->getActionManager()->pauseTarget(readySprite);
    
    runningActions = CCDirector::sharedDirector()->getActionManager()->pauseAllRunningActions();
    runningActions->retain();
    CCLog("러닝액션 개수 : %d", runningActions->count());
    
    sound->PauseBackgroundInGameSound();
    sound->PauseAllEffects();
    
    CancelDrawing();
    
    if (isHintShown)
        StopAllActionsAtPieces();
    // 정령 멈추기
    if (skill->IsSpiritAlive(0))
        effect->GetSpirit(0)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(1))
        effect->GetSpirit(1)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(2))
        effect->GetSpirit(2)->pauseSchedulerAndActions();
    
    Common::ShowNextScene(this, "Puzzle", "PuzzlePause", false, vo.y+tbSize.height+boardSize.height+60);
}

void Puzzle::StopAllActionsAtPieces() // 힌트액션 초기화를 위해~!
{
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 0 ; y < ROW_COUNT ; y++)
        {
            // 네 모서리에 위치한 존재하지 않는 부분
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            spriteP8[x][y]->stopAllActions();
            spriteP8[x][y]->setColor(ccc3(255,255,255));
        }
    }
    for (int x = 1 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            puzzleP4set->StopAllActions(x, y);
            puzzleP4set->SetOpacity(x, y, 255);
        }
    }
    
    iHintTime = 0;
    isHintShown = false;
}

bool Puzzle::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isGameOver)
        return false;
    CCPoint point = pTouch->getLocation();
    
    if (spirit_sp != NULL) // 정령 튜토리얼 중이었으면 튜토리얼을 모두 지우고, '레디~고'로 넘어간다.
    {
        if (!spirit_sp->boundingBox().containsPoint(point))
            return false;
        
        spirit_sp->removeFromParentAndCleanup(true);
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
        spirit_sp = NULL;
        ball = NULL;
        balloon = NULL;
        
        spirit_scid = -1;
        CCUserDefault::sharedUserDefault()->setBoolForKey("spirit_tutorial", true);
        
        // 미션 상세화면 있으면 다시 보여준다.
        if (missionType > 0)
        {
            pMissionSpriteDetail->setOpacity(255);
            pMissionSpriteDetailIcon->setOpacity(255);
            pMissionSpriteDetailContent->setOpacity(255);
        }
        
        ReadyAndStart(NULL, this);
    }

    // 보드판과 상관없는 것들 (lock에 제한받지 않는 것들)
    // 1) pause button
    CCRect rect = ((CCSprite*)spriteClassInfo->FindSpriteByName("icon/icon_pause.png"))->boundingBox();
    rect.setRect(rect.getMinX()-50, rect.getMinY()-50, rect.getMaxX()-rect.getMinX()+100, rect.getMaxY()-rect.getMinY()+100);
    if (rect.containsPoint(point))
    {
        PauseGame();
        return true;
    }
    
    CCLog("touchBegan (%d) : %d %d %d %d", touch_cnt%QUEUE_CNT, m_iSkillSP, m_bTouchStarted, m_iSpiritSP, m_bIsSpiritExecuted);
    
    if (!isGameStarted)
        return true;
    // 스킬 발동 중 or touchEnd가 되기 전까지 or 정령스킬 실행 중이면 터치 금지
    if (m_iSkillSP > 0 || m_bTouchStarted || m_bIsSpiritExecuted || isRenewing)
        return true;
        //return false;
    
    m_bTouchStarted = true; // 터치 시작 lock 걸기
    

    // 어느 퍼즐인지 찾기
    CCPoint boardPos = BoardStartPosition(point);
    int x = (int)boardPos.x;
    int y = (int)boardPos.y;
    
    if (x == -1 && y == -1) // 찾지 못했다면 퍼즐 피스를 클릭한 것이 아니므로 그냥 종료.
        return (m_bTouchStarted = false);
    
    if (m_bLockP8[x][y] > 0) // 터치한 locked면 중지.
        return (m_bTouchStarted = false);
    
    // hint action 중지 + hint time 초기화
    if (isHintShown)
        StopAllActionsAtPieces();
    
    
    // 3가지 정령 중 하나를 터치할 때 동작한다. (좌표 기준인데, 정령이 없으면 아예 시도하지 않는다)
    if ((x == 0 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == 0))
    {
        if (m_iSpiritSP == 0 && !skill->W8_IsActive()) // '여신의 은총' 실행 중이면 중지.
        {
            // 정령이 살아있지 않다면 터치 종료.
            if (x == 0 && y == ROW_COUNT-1 && !skill->IsSpiritAlive(2)) // 땅의 정령 (왼쪽 위)
                return (m_bTouchStarted = false);
            else if (x == COLUMN_COUNT-1 && y == ROW_COUNT-1 && !skill->IsSpiritAlive(1)) // 물의 정령 (오른쪽 위)
                return (m_bTouchStarted = false);
            else if (x == COLUMN_COUNT-1 && y == 0 && !skill->IsSpiritAlive(0)) // 불의 정령 (오른쪽 아래)
                return (m_bTouchStarted = false);
            
            // semaphore 1 증가 + 실행상태 lock 걸기
            m_iSpiritSP++;
            m_bIsSpiritExecuted = true;
            
            m_bTouchStarted = false;
            
            // 각 정령에 관련된 스킬 발동!
            if (x == 0 && y == ROW_COUNT-1) // 땅
                skill->Invoke(20, NULL);
            else if (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) // 물
                skill->Invoke(12, NULL);
            else if (x == COLUMN_COUNT-1 && y == 0) // 불
                skill->Invoke(4, NULL);
        }
        
        return (m_bTouchStarted = false);
    }
    
    
    if (puzzleP8set->GetObject(x, y) == NULL) // 혹시 존재하지 않는 피스(삭제 등의 이유)라면 역시 중지.
        return (m_bTouchStarted = false);
    
    // '여신의 은총' 발동 중인데 아이템을 클릭한 경우 -> 무시해야 한다.
    if ( skill->W8_IsActive() &&
        ( (puzzleP8set->GetType(x, y) >= ITEM_PAINT_RED && puzzleP8set->GetType(x, y) <= ITEM_PAINT_WHITE) ||
         puzzleP8set->GetType(x, y) == ITEM_STAFF ) )
        return (m_bTouchStarted = false);
    
    
    m_bIsCycle[touch_cnt%QUEUE_CNT] = false;
    m_bLockP8[x][y]++; // lock 걸기
    
    // 벡터 초기화
    piece8xy[touch_cnt%QUEUE_CNT].clear();
    piece4xy[touch_cnt%QUEUE_CNT].clear();
    strap[touch_cnt%QUEUE_CNT].clear();

    // 8각형 piece를 터뜨릴 목록에 추가
    piece8xy[touch_cnt%QUEUE_CNT].push_back(ccp(x, y));
    
    
    // 아이템(드래그용)을 클릭한 경우
    if (puzzleP8set->GetType(x, y) >= ITEM_PAINT_RED && puzzleP8set->GetType(x, y) <= ITEM_PAINT_WHITE)
    {
        CCLog("아이템 페인트 : %d %d (%d)", x, y, m_iItemSP);
        //if (!m_bIsItemPossible)
        if (m_iItemSP > 0)
        {
            if (m_bLockP8[x][y] > 0)
                m_bLockP8[x][y]--; // lock 다시 풀기
            return (m_bTouchStarted = false);
        }
        
        effect->ShowItemPaintArrow(x, y); // 화살표 나타내기
        
        item_dx = -999;
        item_dy = -999;
        m_bIsItemTouched = true;
    }
    // 아이템(터치용)을 클릭한 경우
    else if (puzzleP8set->GetType(x, y) == ITEM_STAFF)
    {
        CCLog("아이템 스태프 : %d %d (%d)", x, y, m_iItemSP);
        //if (!m_bIsItemPossible)
        if (m_iItemSP > 0)
        {
            if (m_bLockP8[x][y] > 0)
                m_bLockP8[x][y]--; // lock 다시 풀기
            return (m_bTouchStarted = false);
        }
        //m_bIsItemPossible = false;
        m_iItemSP++;
        
        globalType[touch_cnt%QUEUE_CNT] = ITEM_STAFF;
        
        skill->ApplyItemStaff(x, y, item_dx, item_dy, touch_cnt%QUEUE_CNT);
        m_bLockP8[x][y]--; // lock 다시 풀기
        touch_cnt++;
        
        return (m_bTouchStarted = false);
    }
    // 일반 피스를 클릭한 경우
    else
    {
        // 마법띠 동그라미 처음에 띄우기
        CCSprite* sp = CCSprite::createWithSpriteFrameName("pieces/strap_connector.png");
        sp->setPosition(SetTouch8Position(x, y));
        this->addChild(sp, 1000);
        strap[touch_cnt%QUEUE_CNT].push_back(sp);
    }

    // global piece type 지정
    globalType[touch_cnt%QUEUE_CNT] = puzzleP8set->GetType(x, y);

    // 0.9배 축소 action
    spriteP8[x][y]->setScale(spriteP8[x][y]->getScale() * 0.9f);
    spriteP8[x][y]->setColor(ccc3(140,140,140));
    
    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size());
    
    return true;
}

void Puzzle::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isGameStarted || isGameOver || m_iSkillSP > 0 || m_bIsSpiritExecuted || isRenewing)
        return;

	if (m_bTouchStarted && !m_bIsCycle[touch_cnt%QUEUE_CNT])
	{
        CCPoint point = pTouch->getLocation();
        
        // 어느 퍼즐인지 찾기
        CCPoint boardPos = BoardMovePosition(point);
        int x = (int)boardPos.x;
        int y = (int)boardPos.y;
    
        if (x == -1 && y == -1)
            return;
        
        // 바로 직전에 한붓그리기한 piece 위치
        int size = piece8xy[touch_cnt%QUEUE_CNT].size();
        int firstX = piece8xy[touch_cnt%QUEUE_CNT][0].x;
        int firstY = piece8xy[touch_cnt%QUEUE_CNT][0].y;
        int beforeX = piece8xy[touch_cnt%QUEUE_CNT][size-1].x;
        int beforeY = piece8xy[touch_cnt%QUEUE_CNT][size-1].y;
        
        // locked면 중지 (cycle을 생성하는 상황은 예외로 둔다)
        if (m_bLockP8[x][y] > 0)
        {
            if (piece8xy[touch_cnt%QUEUE_CNT].size() < 3) // 일단 3개 미만이면 무조건 끝.
                return;
            else if (!(x == firstX && y == firstY))
                return;
        }
        
        // 이웃하는 8방향 피스가 아니면 stop
        if ((x == beforeX && abs(y-beforeY) > 1) || (y == beforeY && abs(x-beforeX) > 1) ||
            abs(x-beforeX)+abs(y-beforeY) > 2)
            return;

        
        // 아이템인지 검사
        if (m_bIsItemTouched)
        {
            int dx = x - beforeX;
            int dy = y - beforeY;
            if (-1 <= dx && dx <= 1 && -1 <= dy && dy <= 1)
            {
                item_dx = dx;
                item_dy = dy;
                effect->ChangeItemPaintArrow(item_dx, item_dy);
            }
            return;
        }
        
        // 선택된 피스와 바로 직전 피스의 type이 같고, 맨하탄distance가 2 이하인 경우
        if ((puzzleP8set->GetType(x, y) == puzzleP8set->GetType(beforeX, beforeY) ))
        {
            bool flag = false;
 
            // 대각선으로 연결을 시도한 경우 (4각형 체크를 위해서)
            if (abs(x-beforeX) == 1 && abs(y-beforeY) == 1)
            {
                // BLOCKED면 막힌 것이므로 한붓그리기를 못하게 한다.
                int p4X = (beforeX > x) ? beforeX : x;
                int p4Y = (beforeY > y) ? beforeY : y;
                
                if (puzzleP4set->GetType(p4X, p4Y) == BLOCKED)
                    return;
                
                // 터뜨릴 목록에 지금 대각선으로 지나치는 diamond 좌표 추가
                piece4xy[touch_cnt%QUEUE_CNT].push_back(ccp(p4X, p4Y));
                puzzleP4set->SetOpacity(p4X, p4Y, 100);
                
                // strap + strap connector추가
                CCSprite* sp;
                if ((beforeX > x && beforeY > y) || (beforeX < x && beforeY < y))
                    sp = CCSprite::createWithSpriteFrameName("icon/strap_vertical_2.png");
                else
                    sp = CCSprite::createWithSpriteFrameName("icon/strap_vertical.png");
                sp->setPosition(SetTouch4Position(p4X, p4Y));
                this->addChild(sp, 1000);
                strap[touch_cnt%QUEUE_CNT].push_back(sp);
                
                flag = true;
                if (x == firstX && y == firstY && size >= 3)
                    m_bIsCycle[touch_cnt%QUEUE_CNT] = true;
            }
            
            // 가로나 세로로 연결을 시도한 경우
            else if ((x == beforeX && abs(y-beforeY) == 1) || (y == beforeY && abs(x-beforeX) == 1))
            {
                // strap추가
                CCPoint pos = SetTouch8Position(x, y);
                CCSprite* sp = CCSprite::createWithSpriteFrameName("icon/strap_horizontal.png");
                if (beforeX == x-1)
                    sp->setPosition(ccp((int)pos.x-PIECE8_FRAME_WIDTH/2, (int)pos.y));
                else if (beforeX == x+1)
                    sp->setPosition(ccp((int)pos.x+PIECE8_FRAME_WIDTH/2, (int)pos.y));
                else if (beforeY == y-1)
                {
                    sp->setRotation(90);
                    sp->setPosition(ccp((int)pos.x, (int)pos.y-PIECE8_FRAME_HEIGHT/2));
                }
                else
                {
                    sp->setRotation(90);
                    sp->setPosition(ccp((int)pos.x, (int)pos.y+PIECE8_FRAME_HEIGHT/2));
                }
                this->addChild(sp, 1000);
                strap[touch_cnt%QUEUE_CNT].push_back(sp);
                
                flag = true;
                if (x == firstX && y == firstY && size >= 3)
                    m_bIsCycle[touch_cnt%QUEUE_CNT] = true;
            }
            
            if (flag)
            {
                // 콤보 끊김시간 갱신
                if (iCombo > 0 && iComboTimer < MAX_COMBO_TIME)
                    iComboTimer = 0;
                
                if (m_bIsCycle[touch_cnt%QUEUE_CNT]) // cycle을 표시해주는 액션이 들어간다.
                {
                    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size()+1);
                    sound->PlayCycle();
                    
                    CCSprite* sp = CCSprite::create("images/cycle.png");
                    CCPoint p = SetTouch8Position(x, y);
                    sp->setPosition(ccp(p.x, p.y-100));
                    sp->setOpacity(0);
                    this->addChild(sp, 30);
                    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCMoveBy::create(0.6f, ccp(0, 50)), CCSequence::create(CCFadeIn::create(0.3f), CCFadeOut::create(0.3f), NULL), NULL), CCCallFuncND::create(this, callfuncND_selector(Puzzle::Cycle_Callback), this), NULL );
                    sp->runAction(action);
                }
                else
                {
                    // lock
                    m_bLockP8[x][y]++;
                
                    // 8각형 piece를 터뜨릴 목록에 추가
                    piece8xy[touch_cnt%QUEUE_CNT].push_back(ccp(x, y));
            
                    // 0.9배 축소
                    spriteP8[x][y]->setScale(spriteP8[x][y]->getScale() * 0.9f);
                    //spriteP8[x][y]->setOpacity(140);
                    spriteP8[x][y]->setColor(ccc3(140,140,140));
                    
                    // piece move action
                    int dx = 15 * (x - beforeX);
                    int dy = 15 * (y - beforeY);
                    CCFiniteTimeAction* action = CCSequence::create(
                        CCMoveBy::create(0.05f, ccp(dx, dy)), CCMoveBy::create(0.05f, ccp(-dx, -dy)), NULL);
                    spriteP8[x][y]->runAction(action);
                    
                    CCSprite* sp = CCSprite::createWithSpriteFrameName("pieces/strap_connector.png");
                    sp->setPosition(SetTouch8Position(x, y));
                    this->addChild(sp, 1000);
                    strap[touch_cnt%QUEUE_CNT].push_back(sp);
                    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size());
                    
                    // 쓸모없어진 연결피스 음영처리
                    CheckUselessDiaPieces();
                }
            }
        }
	}
}

void Puzzle::Cycle_Callback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
}

void Puzzle::CheckUselessDiaPieces()
{
    int xi, yi, xj, yj, xk, yk;
    int mx, my;
    bool flag;
    
    for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
    {
        xi = piece8xy[touch_cnt%QUEUE_CNT][i].x;
        yi = piece8xy[touch_cnt%QUEUE_CNT][i].y;
        for (int j = i+1 ; j < piece8xy[touch_cnt%QUEUE_CNT].size() ; j++)
        {
            xj = piece8xy[touch_cnt%QUEUE_CNT][j].x;
            yj = piece8xy[touch_cnt%QUEUE_CNT][j].y;
            
            if (abs(xi-xj) == 1 && abs(yi-yj) == 1)
            {
                mx = std::max(xi, xj); // 연결피스 x
                my = std::max(yi, yj); // 연결피스 y
                
                if (!IsConnected(mx, my))
                    continue;
                if (i == 0 && j == piece8xy[touch_cnt%QUEUE_CNT].size()-1)
                    continue;
                
                
                //CCLog("(%d,%d), %d %d  %d %d  , %d %d", i, j, xi, yi, xj, yj, mx, my);
                
                flag = true;
                for (int k = 0 ; k < piece4xy[touch_cnt%QUEUE_CNT].size() ; k++)
                {
                    xk = piece4xy[touch_cnt%QUEUE_CNT][k].x;
                    yk = piece4xy[touch_cnt%QUEUE_CNT][k].y;
                    if (mx == xk && my == yk)
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    //CCLog("연결피스추가!");
                    piece4xy[touch_cnt%QUEUE_CNT].push_back( ccp(mx, my) );
                    puzzleP4set->SetOpacity(mx, my, 100);
                }
            }
        }
    }
}

void Puzzle::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isGameStarted || isGameOver || m_iSkillSP > 0 || m_bIsSpiritExecuted || isRenewing)
        return;

    if (m_bTouchStarted)
    {
        effect->RemoveItemPaintArrow();  // 화살표 없앤다.
        
        // 아이템을 선택했었다면, 아이템을 행한다. (페인트, 스태프 아이템)
        if ((item_dx != -999 && item_dy != -999) && !(item_dx == 0 && item_dy == 0))
        {
            m_bTouchStarted = false;
            wasFallen[touch_cnt%QUEUE_CNT] = true;
            int x = piece8xy[touch_cnt%QUEUE_CNT][0].x;
            int y = piece8xy[touch_cnt%QUEUE_CNT][0].y;
            m_bLockP8[x][y]--; // 아이템 피스 위치를 unlock (어차피 다시 lock 걸기 때문에)
            
            if (puzzleP8set->GetType(x, y) >= ITEM_PAINT_RED && puzzleP8set->GetType(x, y) <= ITEM_PAINT_WHITE)
            {
                //m_bIsItemPossible = false;
                m_iItemSP++;
                skill->ApplyItemPaint(x, y, item_dx, item_dy, puzzleP8set->GetType(x, y), touch_cnt%QUEUE_CNT);
            }
            touch_cnt++;
        }
        
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        else if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
            // 일단 정령 스킬부터 semaphore 증가.
            if (!skill->W8_IsActive())
                m_iSpiritSP++;
            
            int size = piece8xy[touch_cnt%QUEUE_CNT].size();
            lastPosition[touch_cnt%QUEUE_CNT] = piece8xy[touch_cnt%QUEUE_CNT][size-1];
            if (m_bIsCycle[touch_cnt%QUEUE_CNT])
                lastPosition[touch_cnt%QUEUE_CNT] = piece8xy[touch_cnt%QUEUE_CNT][0];
            
            // 다시 한붓그린 부분 lock을 푼다
            int x, y;
            for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                x = piece8xy[touch_cnt%QUEUE_CNT][i].x;
                y = piece8xy[touch_cnt%QUEUE_CNT][i].y;
                m_bLockP8[x][y]--;
            }
            
            // 4각형 음영처리 원상복구
            for (int i = 0 ; i < piece4xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                x = (int)piece4xy[touch_cnt%QUEUE_CNT][i].x;
                y = (int)piece4xy[touch_cnt%QUEUE_CNT][i].y;
                puzzleP4set->SetOpacity(x, y, 255);
            }
            piece4xy[touch_cnt%QUEUE_CNT].clear();
            
            // 스트랩 그림들 제거한다.
            for (int i = 0 ; i < strap[touch_cnt%QUEUE_CNT].size() ; i++)
                strap[touch_cnt%QUEUE_CNT][i]->removeFromParentAndCleanup(true);
            strap[touch_cnt%QUEUE_CNT].clear();
            
            ////////////////////////////////////////////////////////////////////////////////
            // '여신의 은총' 발동 중이면 다음을 행한다.
            if (skill->W8_IsActive())
            {
                wasFallen[touch_cnt%QUEUE_CNT] = true; // 여신 때는 동시에 한붓그리기 못하니 true로 둬도 괜찮다.
                
                int x = piece8xy[touch_cnt%QUEUE_CNT][0].x;
                int y = piece8xy[touch_cnt%QUEUE_CNT][0].y;
                
                // 피버타임이라도 푸른 피스를 터뜨릴 때만 적용한다.
                if (isFeverTime && puzzleP8set->GetType(x, y) == PIECE_BLUE)
                    AddPiecesByFeverTime(piece8xy[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
                
                skill->W8_Invoke(piece8xy[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
                
                m_bTouchStarted = false;
                m_bIsCycle[touch_cnt%QUEUE_CNT] = false;
                touch_cnt++;
                return;
            }
            ////////////////////////////////////////////////////////////////////////////////
            

            wasFallen[touch_cnt%QUEUE_CNT] = false;
            
            // 피버타임일 경우, 주변부까지 터뜨린다.
            if (isFeverTime)
            {
                // 피버타임의 경우 미리 위치를 저장해두고 사이클 발동 + 궁극 발동 시 사용하자.
                posForFeverTime[touch_cnt%QUEUE_CNT] = piece8xy[touch_cnt%QUEUE_CNT];
                
                // 순차 폭발을 위한 cnt 초기화
                feverBombOrderCnt[touch_cnt%QUEUE_CNT] = 0;
                
                AddPiecesByFeverTime(piece8xy[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
            }
            else
                isFeverRound[touch_cnt%QUEUE_CNT] = false;

            
            // 스킬 발동 try (발동되는 스킬들 조사)
            // 여기서 스킬에 따라 skill Lock이 걸릴 수 있다. (그러면 스킬발동 종료까지 터치 아예 못함)
            m_bSkillLock[touch_cnt%QUEUE_CNT] = false;
            skill->TrySkills(globalType[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT, isFeverTime);
            if (m_bSkillLock[touch_cnt%QUEUE_CNT])
            {
                m_iSkillSP++; // skill semaphore 증가
                wasFallen[touch_cnt%QUEUE_CNT] = true; // 스킬 발동 시 동시에 한붓그리기 못하므로 true로 둔다.
            }
            else
            
            m_iBombCallbackType[touch_cnt%QUEUE_CNT] = 0;
            
            touch_cnt++;
            m_iItemSP++; //m_bIsItemPossible = false;
            
            // 터치 lock을 푼다. (먼저 만든 한붓그리기가 터지고 새로운 피스가 완전히 falling하기 전에 새로운 한붓그리기 가능하게 하기 위하여)
            m_bTouchStarted = false;
            
            // effect 체크 초기화
            effect->InitCheck();
            
            // 스킬 실행 (오토마타 표 참조)
            iTouchRound++;
            skill->SetQueuePos((touch_cnt-1)%QUEUE_CNT);

            m_iState[(touch_cnt-1)%QUEUE_CNT] = SKILL_STOPTIME;
            InvokeSkills((touch_cnt-1)%QUEUE_CNT);

            m_bIsCycle[(touch_cnt-1)%QUEUE_CNT] = false; // cycle 푼다.
        }
        
        // 3개 미만으로 한붓그리기가 되었다면, 원상태로 복구시킨다.
        else
        {
            CancelDrawing();
        }
            
        // 아이템 값 초기화
        m_bIsItemTouched = false;
        item_dx = -999;
        item_dy = -999;
    }
}

void Puzzle::AddPiecesByFeverTime(std::vector<CCPoint> &p, int queue_pos)
{
    int size = p.size();
    int xx = p[size-1].x;
    int yy = p[size-1].y;
    if (m_bIsCycle[queue_pos])
    {
        xx = p[0].x;
        yy = p[0].y;
    }
    
    isFeverRound[queue_pos] = true;
    
    // 5개 이하 : 3*3
    // 8개 이하 : 5*5
    // 그 이상  : 7*7
    int cnt;
    if (size <= 5) cnt = 1;
    else if (size <= 8) cnt = 2;
    else cnt = 3;
    feverBombOrderMax[queue_pos] = cnt;
    
    std::vector<CCPoint> temp;
    temp.clear();
    
    // 변수 초기화
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            feverBombOrder[queue_pos][x][y] = 0;
    
    bool flag;
    int pieceType;
    for (int x = xx-cnt ; x <= xx+cnt ; x++)
    {
        for (int y = yy-cnt ; y <= yy+cnt ; y++)
        {
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            if (x < 0 || y < 0 || x > COLUMN_COUNT-1 || y > ROW_COUNT-1)
                continue;
            
            // lock 걸린 피스도 제외한다. (직전 한붓그리기에서 터지고 아직 안떨어진 것이기에)
            if (m_bLockP8[x][y])
                continue;
            
            pieceType = puzzleP8set->GetType(x, y);
            if (!(pieceType >= PIECE_RED && pieceType <= PIECE_WHITE)) // 일반 피스가 아니면 제외한다.
                continue;
            
            flag = true;
            for (int i = 0 ; i < size ; i++)
            {
                int cx = p[i].x;
                int cy = p[i].y;
                
                if (cx == x && cy == y)
                {
                    flag = false;
                    break;
                }
            }
            if (flag) // (x, y)를 추가한다.
            {
                temp.push_back(ccp(x, y));
                feverBombOrder[queue_pos][x][y] = std::max(abs(x-xx), abs(y-yy));
            }
        }
    }
    
    for (int i = 0 ; i < temp.size() ; i++)
        p.push_back(temp[i]);
    temp.clear();
    
    /////
    /*
    for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
    {
        CCLog("%d %d %d %d %d %d %d", feverBombOrder[queue_pos][0][y], feverBombOrder[queue_pos][1][y], feverBombOrder[queue_pos][2][y],
              feverBombOrder[queue_pos][3][y], feverBombOrder[queue_pos][4][y], feverBombOrder[queue_pos][5][y], feverBombOrder[queue_pos][6][y]);
    }
     */
    /////
}

void Puzzle::CancelDrawing()
{
    if (isCancelling)
        return;
    isCancelling = true;
    
    int x, y;
    CCLog("CancelDrawing = %d", touch_cnt%QUEUE_CNT);
    //CCLog("정령 클릭 후 손 떼지 않고 정령스킬발동완료 후 떼어서 여기로 왔다.");
    //CCLog("%d %d %d", (int)piece8xy[touch_cnt%QUEUE_CNT].size(), (int)piece4xy[touch_cnt%QUEUE_CNT].size(), (int)strap[touch_cnt%QUEUE_CNT].size());
    for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
    {
        x = (int)piece8xy[touch_cnt%QUEUE_CNT][i].x;
        y = (int)piece8xy[touch_cnt%QUEUE_CNT][i].y;
        spriteP8[x][y]->setScale(GetBoardSize()/(float)board_wh);
        spriteP8[x][y]->setColor(ccc3(255,255,255));
        // lock도 해제
        m_bLockP8[x][y]--;
    }
    for (int i = 0 ; i < piece4xy[touch_cnt%QUEUE_CNT].size() ; i++)
    {
        x = (int)piece4xy[touch_cnt%QUEUE_CNT][i].x;
        y = (int)piece4xy[touch_cnt%QUEUE_CNT][i].y;
        puzzleP4set->SetOpacity(x, y, 255);
    }
    for (int i = 0 ; i < strap[touch_cnt%QUEUE_CNT].size() ; i++)
        strap[touch_cnt%QUEUE_CNT][i]->removeFromParentAndCleanup(true);
    
    piece8xy[touch_cnt%QUEUE_CNT].clear();
    piece4xy[touch_cnt%QUEUE_CNT].clear();
    strap[touch_cnt%QUEUE_CNT].clear();
    m_bTouchStarted = false;
    m_bIsCycle[touch_cnt%QUEUE_CNT] = false;
    
    isCancelling = false;
}



void Puzzle::InvokeSkills(int queue_pos)
{
    // ------- 순서도 ------- //
    // 0) '시간을 얼리다' 적용
    // 1) 한붓그리기 위치 폭발 (+ 10개추가점수/별사탕 등도 포함)
    // 2) 사이클 주변부 검사 : 적용되었다면 주변부까지 바로 터뜨리자.
    // Falling...
    // 3) '6개이상 한번더' 검사
    //  - 적용되었다면, Bomb + Falling...
    // 5) 속성별 마지막 스킬 검사
    //  - 적용되었다면, Bomb + Falling...
    
    if (m_iState[queue_pos] == SKILL_STOPTIME) // 제일 처음 : '시간을 얼리다' 검사
    {
        CCLog("state(%d) STOP TIME", queue_pos);

        // W7 (시간을얼리다)
        //if (bFreezeFlag && globalType[queue_pos] == PIECE_BLUE)
        if (skill->IsApplied(14, queue_pos) && !skill->W7_IsFreezed() && skill->W7_GetCnt() < numOfFreezeTime)
        {
            skill->Invoke(14, NULL);
        }
 
        m_iState[queue_pos] = SKILL_BASIC;
        InvokeSkills(queue_pos);
    }
    else if (m_iState[queue_pos] == SKILL_BASIC) // 추가점수, +10 스킬, 기본 한붓그리기 폭발
    {
        CCLog("state(%d) BASIC", queue_pos);
        if (skill->IsApplied(1, queue_pos) || skill->IsApplied(9, queue_pos) || skill->IsApplied(17, queue_pos))
        {
            m_iNextState[queue_pos] = SKILL_CYCLE; // 사이클 주변부 적용
        }
        else
        {
            m_iNextState[queue_pos] = SKILL_DOUBLESIX; // 그게 아니면 바로 6개이상*2 state로 넘어가자.
        }
        
        Lock(queue_pos);
        Bomb(queue_pos, piece8xy[queue_pos]);
        
        // 추가점수 적용되었다면 실행한 후, 한붓그리기 한 것을 터뜨리기
        skill->Invoke(0, queue_pos); skill->Invoke(8, queue_pos); skill->Invoke(16, queue_pos); // 추가점수
        
        // 10개이상제거시추가점수(F3), //10개이상제거시추가별사탕(E4)
        skill->Invoke(2, queue_pos); //skill->Invoke(19, queue_pos);
    }
    else if (m_iState[queue_pos] == SKILL_CYCLE) // 사이클 주변부 터뜨리기
    {
        CCLog("state(%d) CYCLE", queue_pos);

        m_iNextState[queue_pos] = SKILL_DOUBLESIX;
        skill->Invoke(1, queue_pos); skill->Invoke(9, queue_pos); skill->Invoke(17, queue_pos);
    }
    else if (m_iState[queue_pos] == SKILL_DOUBLESIX) // 6개 이상 제거 시, 한번 더 제거
    {
        CCLog("state(%d) DOUBLESIX", queue_pos);
        m_iNextState[queue_pos] = SKILL_FINAL;
        
        skill->Invoke(5, queue_pos); skill->Invoke(13, queue_pos); skill->Invoke(21, queue_pos);

        if (!(skill->IsApplied(5, queue_pos) || skill->IsApplied(13, queue_pos) || skill->IsApplied(21, queue_pos)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    else if (m_iState[queue_pos] == SKILL_FINAL) // 마지막 (속성별 8번) 스킬
    {
        CCLog("state(%d) FINAL", queue_pos);
        m_iNextState[queue_pos] = SKILL_COCOTIME;
        
        //piece8xy[queue_pos] = posForFeverTime[queue_pos];
        //CCLog("size = %d", (int)piece8xy[queue_pos].size());

        skill->Invoke(7, queue_pos); skill->Invoke(15, queue_pos); skill->Invoke(23, queue_pos);
        
        // '고대나무' 스킬을 검사할 때는 미리 state를 넘겨 놓는다.
        //if (!skill->IsApplied(7, queue_pos) && !skill->IsApplied(15, queue_pos))
        //    m_iState[queue_pos] = m_iNextState[queue_pos];
        
        if ((!skill->IsApplied(7, queue_pos) && !skill->IsApplied(23, queue_pos)) ||
            (skill->IsApplied(23, queue_pos) && skill->E8_IsFinished()))
        {
            //CCLog("state(%d) 스킬 파이날 발동되지 않음.");
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            // 참고 : '여신의 은총' 스킬은 발동 유무에 관계없이 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    else if (m_iState[queue_pos] == SKILL_COCOTIME)
    {
        if (skill->IsApplied(6, queue_pos) && !isFeverTime && skill->F7_GetCnt() < numOfCocoTime)
        {
            skill->Invoke(6, queue_pos);
        }
        
        m_iNextState[queue_pos] = SKILL_DONE;
        m_iState[queue_pos] = m_iNextState[queue_pos];
        InvokeSkills(queue_pos);
    }
    
    else if (m_iState[queue_pos] == SKILL_DONE)
    {
        CCLog("state(%d) DONE!", queue_pos);
        // 모든 스킬이 끝났다. 원상태로 되돌리자.
        
        // 피스판 갱신 필요성 검사
        isFalling = false;
        if (skill->IsRenewNeeded())
        {
            CancelDrawing();
            skill->RenewPuzzle(queue_pos);
        }
        
        // skill Lock을 푼다.
        if (m_bSkillLock[queue_pos])
        {
            SetSkillLock(queue_pos, false);
            m_iSkillSP--;
        }
        
        m_iSpiritSP--;
        iTouchRound--;
        
        m_iItemSP--;
        //m_bIsItemPossible = true;
    }
}

void Puzzle::EndRound(int queue_pos)
{
    // 모든 스킬이 끝났다. 원상태로 되돌리자.
    CCLog("state(%d) END ROUND", queue_pos);
    
    // lock을 모두 해제한다. (semaphore를 1씩 감소시킨다)
    int x, y;
    for (int i = 0 ; i < lock8xy[(int)queue_pos].size() ; i++)
    {
        x = lock8xy[(int)queue_pos][i].x;
        y = lock8xy[(int)queue_pos][i].y;
        m_bLockP8[x][y]--;
    }
    lock8xy[(int)queue_pos].clear();
    
    // 피스판 갱신 필요성 검사
    //isFalling = false;
    //if (skill->IsRenewNeeded())
    //{
    //    CancelDrawing();
    //    skill->RenewPuzzle(queue_pos);
    //}
    
    // skill Lock을 푼다.
    if (m_bSkillLock[queue_pos])
    {
        SetSkillLock(queue_pos, false);
        m_iSkillSP--;
    }
    
    m_iSpiritSP--;
    iTouchRound--;
    
    m_iItemSP--;
    //m_bIsItemPossible = true;
}

// 특정 색(type) 피스가 터지는 개수(cnt)를 갱신하고, 그에 맞춰 미션도 갱신한다.
void Puzzle::UpdatePieceBombCnt(int type, int cnt)
{
    if (!(type >= PIECE_RED && type <= PIECE_WHITE))
        return;
    
    iPieceBombCnt[type] += cnt;
    
    // 미션 내용도 같이 갱신
    if ( !isMissionDone &&
         ((missionType == 1 &&
           ((missionRefVal == 1 && type == PIECE_BLUE) ||
            (missionRefVal == 2 && type == PIECE_RED) ||
            (missionRefVal == 3 && type == PIECE_GREEN) ) ) ||
         (missionType == 4)) )
    {
        iMissionCnt += cnt;
        if (iMissionCnt >= missionVal)
            isMissionDone = true;
        
        char n[6];
        sprintf(n, "%d", iMissionCnt);
        pMissionLabel->setString(n);
        
        // icon action
        pMissionSprite->stopAllActions();
        float scale, varScale;
        if (missionType == 1) { scale = 0.5f; varScale = 0.2f; }
        else if (missionType == 4) { scale = 0.85f; varScale = 0.25f; }// 0.5f
        pMissionSprite->setScale(scale + varScale);
        pMissionSprite->setRotation(0);
        
        CCActionInterval* action;
        if (missionType == 1)
            action = CCSpawn::create( CCSequence::create(CCRotateBy::create(0.05f, 10), CCRotateBy::create(0.1f, -20), CCRotateBy::create(0.05f, 10), NULL), CCScaleTo::create(0.2f, scale), NULL);
        else if (missionType == 4)
            action = CCScaleTo::create(0.2f, scale);
        
        pMissionSprite->runAction(action);
        
        // 미션 달성하면 표현
        if (isMissionDone)
            MissionComplete();
    }
}

// 스킬 발동 시 미션을 갱신한다.
void Puzzle::UpdateMissionCountBySkill(int skillNum)
{
    int csi;
    if (skillNum < 8) csi = skillNum+21;
    else if (skillNum < 16) csi = skillNum+3;
    else if (skillNum < 24) csi = skillNum+15;
    
    magicCnt++;
    /*
    if (!isFeverTime && magicCnt < feverStartCnt)
    {
        
        magicCnt++;
        CCLog("매직 카운트 = %d", magicCnt);
    
        // 피버 게이지 증가 action
        CCActionInterval* action = CCMoveBy::create(0.1f, ccp(149.0f / feverStartCnt, 0));
        ((CCScale9Sprite*)spriteClassInfo->FindSpriteByTag(98765))->runAction(action);
    }
    */
    
    if ( !isMissionDone &&
         ((missionType == 2 && missionRefVal == csi) ||
         (missionType == 3)) )
    {
        iMissionCnt++;
        if (iMissionCnt >= missionVal)
            isMissionDone = true;
        
        char n[4];
        sprintf(n, "%d", iMissionCnt);
        pMissionLabel->setString(n);
        
        // icon action
        pMissionSprite->stopAllActions();
        float scale;
        if (missionType == 2) scale = 0.6f;
        else if (missionType == 3) scale = 1.5f;
        pMissionSprite->setScale(scale+0.3f); // 0.5f
        pMissionSprite->setRotation(0);
        pMissionSprite->runAction( CCSpawn::create( CCSequence::create(CCRotateBy::create(0.05f, 10), CCRotateBy::create(0.1f, -20), CCRotateBy::create(0.05f, 10), NULL), CCScaleTo::create(0.2f, scale), NULL) );
        
        // 미션 달성하면 표현
        if (isMissionDone)
            MissionComplete();
    }
}

void Puzzle::MissionComplete()
{
    ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(99997))->setOpacity(0);
    ((CCLabelTTF*)spriteClassInfo->FindLabelByTag(99998))->setOpacity(0);
    pMissionLabel->setAnchorPoint(ccp(0.5, 1));
    pMissionLabel->setPosition(ccp(171+14, 137-2));
    pMissionLabel->setFontSize(32);
    pMissionLabel->setColor(ccc3(255,204,33));
    pMissionLabel->setString("미션 완료!");
    
    CCSprite* sp = CCSprite::create("particles/effect_10.png");
    CCParticleSystem* par = CCParticleFlower::create();
    par->setTexture(sp->getTexture());
    par->setAnchorPoint(ccp(0.5, 0.5));
    par->setPosition(pMissionSprite->getPosition());
    par->setStartSize(65);
    par->setLife(0.4);
    par->setSpeed(180);//120);
    par->setSpeedVar(70);
    ccColor4F startColor = par->getStartColor();
    startColor.r = 0.95f;
    startColor.g = 0.95f;
    startColor.b = 0.95f;
    par->setStartColor(startColor);
    par->setDuration(1.0f);
    par->setAutoRemoveOnFinish(true);
    ((CCSprite*)spriteClassInfo->FindSpriteByName("background/bg_mission.png"))->addChild(par, 1000);
    
    // sound
    sound->PlayVoice(VOICE_MISSIONSUCCESS);
}

void Puzzle::Lock(int queue_pos)
{
    CCLog("Lock() - state(%d) : %d", queue_pos, m_iState[queue_pos]);
    
    // drop되는 모든 영역에 lock을 건다.
    int x, y;
    int lowest[COLUMN_COUNT];
    for (int i = 0 ; i < COLUMN_COUNT ; i++)
    {
        lowest[i] = ROW_COUNT;
    }
    for (int i = 0 ; i < piece8xy[queue_pos].size() ; i++)
    {
        x = piece8xy[queue_pos][i].x;
        y = piece8xy[queue_pos][i].y;
        if (y < lowest[x])
            lowest[x] = y;
    }
    for (x = 0 ; x < COLUMN_COUNT ; x++)
    {
        for (y = lowest[x] ; y < ROW_COUNT ; y++)
        {
            // lock 거는 부분 (semaphore +1)
            m_bLockP8[x][y]++;
            // 나중에 lock 걸린 위치를 정확히 해제하기 위해, 그 위치를 따로 저장.
            if (m_bLockP8[x][y] > 0)
                lock8xy[queue_pos].push_back(ccp(x, y));
        }
    }
    /*for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
    {
        CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
              m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
    }*/
}
void Puzzle::LockEach(int x, int y)
{
    m_bLockP8[x][y]++;
}
void Puzzle::UnLockEach(int x, int y)
{
    m_bLockP8[x][y]--;
}
bool Puzzle::IsLocked(int x, int y)
{
    return m_bLockP8[x][y];
}

void Puzzle::RemoveConnectPieces(std::vector<CCPoint> pos)
{
    // 연결피스를 삭제한다. (단, 연결피스 기준으로 두 대각선 중 한 쌍의 피스가 터지면 없애도록 한다)
    /*
    // 1) 변수 초기화
    for (int x = 0; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            P8Bombed[x][y] = false;
    for (int i = 0 ; i < pos.size() ; i++)
        P8Bombed[(int)pos[i].x][(int)pos[i].y] = true;
    
    // 2) 검사
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            if (P8Bombed[x-1][y] || P8Bombed[x][y-1] || P8Bombed[x-1][y-1] || P8Bombed[x][y])
                puzzleP4set->RemoveChild(x, y);
        }
    }
     */
    for (int i = 0 ; i < pos.size() ; i++)
        RemoveConnectPiecesXY((int)pos[i].x, (int)pos[i].y);
}
void Puzzle::CreateConnectPieces()
{
    // 지워진 연결피스를 새로 만든다
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            if (puzzleP4set->IsRemoved(x, y))
            {
                if (puzzleP4set->CreatePiece(x, y))
                    puzzleP4set->AddChild(x, y);
            }
        }
    }
}
/*
void Puzzle::ReplaceConnectPieces()
{
    // 모든 연결피스를 새로 만든다.
    for (int x = 1 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            puzzleP4set->RemoveChild(x, y);
            puzzleP4set->CreatePiece(x, y);
            puzzleP4set->AddChild(x, y);
        }
    }
}
*/
void Puzzle::RemoveConnectPiecesXY(int x, int y)
{
    if (x > 0 && y > 0 && x < COLUMN_COUNT && y < ROW_COUNT)
        puzzleP4set->RemoveChild(x, y);
    if (x+1 > 0 && y > 0 && x+1 < COLUMN_COUNT && y < ROW_COUNT)
        puzzleP4set->RemoveChild(x+1, y);
    if (x > 0 && y+1 > 0 && x < COLUMN_COUNT && y+1 < ROW_COUNT)
        puzzleP4set->RemoveChild(x, y+1);
    if (x+1 > 0 && y+1 > 0 && x+1 < COLUMN_COUNT && y+1 < ROW_COUNT)
        puzzleP4set->RemoveChild(x+1, y+1);
}


void Puzzle::Bomb(int queue_pos, std::vector<CCPoint> bomb_pos)
{
    int original_bomb_size = bomb_pos.size();
    if (feverBombOrderCnt[(int)queue_pos] == 0)
    {
        //m_bIsItemPossible = false;
        bomb_pos = posForFeverTime[queue_pos];
    }
    
    CCLog("Bomb(%d) start : size = %d", queue_pos, (int)bomb_pos.size());
    
    // 연결피스를 삭제한다.
    RemoveConnectPieces(bomb_pos);
    
  
    // 폭발 사운드
    if (m_iState[queue_pos] == SKILL_BASIC && skill->IsApplied(0, queue_pos) && globalType[queue_pos] == PIECE_RED)
        sound->PlaySkillSound(0);
    else if (skill->W8_IsActive())
        sound->PlayDesignatedSound(153);
    else
        sound->PlayBomb();
    
    
    // 기본 이펙트 적용
    effect->PlayEffect(-100, NULL, bomb_pos);
    
    // 피버타임 이펙트 적용 (마법진)
    if (feverBombOrderCnt[(int)queue_pos] == 0 && bonusTimeState == -999)
    {
        int size = bomb_pos.size();
        if (m_bIsCycle[(int)queue_pos])
            effect->PlayEffect_FeverCircle(bomb_pos[0], bomb_pos[size-1], feverBombOrderMax[(int)queue_pos]);
        else
            effect->PlayEffect_FeverCircle(bomb_pos[size-1], bomb_pos[size-2], feverBombOrderMax[(int)queue_pos]);
    }
    
    // 8각형들을 터뜨린다.
    int x, y;
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax[queue_pos] = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;
        
        // 피버타임 때 터지는 순서 초기화 시키기 (다시 안 터뜨리기 위해)
        feverBombOrder[queue_pos][x][y] = 0;
        
        // 폭파 개수 갱신
        if (!skill->F8_IsReady()) // 붉은용의숨결 시작 떄 싹쓸이하는데, 이 때는 개수를 갱신하면 안 된다.
            UpdatePieceBombCnt(puzzleP8set->GetType(x, y), 1);
        
        // 폭파 시간
        float bombTime = 0.15f;
        if (bonusTimeState == 0 || bonusTimeState == -1) // 보너스폭발 중이면 시간을 2배 늦춘다.
            bombTime = 0.3f;
        else if (m_iState[queue_pos] == SKILL_CYCLE && (globalType[queue_pos] == PIECE_RED || globalType[queue_pos] == PIECE_BLUE))
            bombTime = 0.60f;
        
        
        float delayTime = 0.0f;
        if (feverBombOrderCnt[queue_pos] >= 0)
            delayTime = 0.15f;
        // 그 다음이 사이클 주변부 터지는 스킬이면 조금 딜레이준다.
        else if (m_iNextState[queue_pos] == SKILL_CYCLE)
            delayTime = 0.1f;
        // 6개이상 한번더 (불꽃놀이/얼음비/바람) 스킬 : 시작 전에 딜레이를 조금 준다.
        else if (m_iState[queue_pos] == SKILL_DOUBLESIX)
            delayTime = 0.15f;
        
        // 터지는 액션 (BOMB!)
        CCFiniteTimeAction* action;
        if (m_iState[queue_pos] == SKILL_DOUBLESIX)
        {
            if (globalType[queue_pos] == PIECE_BLUE) // 얼음비 : 터지고 살짝 딜레이를 준다. (이펙트 길게 보여주기 위해)
                action = CCSequence::create( CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                                            CCDelayTime::create(0.7f),
                                            CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos), NULL);
            else // 불꽃놀이/바람 : 시작 전에 딜레이를 주어, 직전에 행해진 한붓그리기 폭발과 간격을 둔다.
                action = CCSequence::create( CCDelayTime::create(delayTime),
                                            CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                                            CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos), NULL);
        }
        else
        {
            action = CCSequence::create(
                                        CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                                        CCDelayTime::create(delayTime), // 그 다음이 사이클 주변부 터지는 스킬이면 조금 딜레이준다.
                                        CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                                        NULL);
        }
        
        spriteP8[x][y]->runAction(action);
    }

    // 용의숨결 시작할 때 전체 폭발은 점수와 관련없으니 무시하자.
    if (skill->F8_IsReady())
        return;
    
    if (bonusTimeState == -1 || bonusTimeState == 0) // 게임이 끝났고, 보너스 폭발
    {
        // update score (피스당 10*MP)
        UpdateScore(1, original_bomb_size * 20*myInfo->GetMPTotal());
        
        if (bonusTimeState == -1 && skill->IsSkillNumberExists(18))
        {
            //UpdateStarCandy(1, original_bomb_size*5);
            //UpdateStarCandy(1, 5*(int)bomb_pos.size());
            effect->ShowStarCandyAll(m_bIsCycle[queue_pos], bomb_pos);
        }
    }
    else if (!isFeverTime || (isFeverTime && feverBombOrderCnt[(int)queue_pos] == 0)) // 피버타임이 아니거나, 피버타임이라도 첫 폭발일 때만
    {
        // update score (1번 스킬 발동 시에는 하면 안 된다)
        if (!skill->IsApplied(0, (int)queue_pos) && !skill->IsApplied(8, (int)queue_pos) && !skill->IsApplied(16, (int)queue_pos) &&
            !skill->W8_IsActive())
        {
            UpdateScore(0, original_bomb_size);
        }
    
        // update starcandy
        //UpdateStarCandy(0, original_bomb_size);
        //UpdateStarCandy(0, (int)bomb_pos.size());
        effect->ShowStarCandy(m_bIsCycle[queue_pos], bomb_pos);
    }
    
    // update combo (한붓그리기를 실제로 해서 터뜨리는 순간에만 콤보로 인정 or 보너스타임 때)
    if ( bonusTimeState == -1 || m_iState[(int)queue_pos] == SKILL_BASIC || skill->W8_IsActive())
        UpdateCombo();
    
    // 터지는 게 없다면 바로 Falling을 시작한다.
    if (bomb_pos.size() == 0)
    {
        // falling queue insertion
        fallingQueue.push((int)queue_pos);
        if (!isFalling)
            FallingProcess();
    }
}

void Puzzle::BombCallback(CCNode* sender, void* queue_pos)
{
    m_iBombCallbackCnt[(int)queue_pos]++;
    
    if (m_iBombCallbackCnt[(int)queue_pos] == m_iBombCallbackCntMax[(int)queue_pos])
    {
        if (bonusTimeState == -1) // 보너스타임 폭발
        {
            std::vector<CCPoint> temp = skill->GetResult();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                int x = (int)temp[i].x;
                int y = (int)temp[i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            temp.clear();
        }
        else if (feverBombOrderCnt[(int)queue_pos] >= 0) // 피버타임 때의 순차폭발이 진행 중일 때
        {
            feverBombOrderCnt[(int)queue_pos]++;
            
            std::vector<CCPoint> next_bomb_pos;
            while (feverBombOrderCnt[(int)queue_pos] <= feverBombOrderMax[(int)queue_pos])
            {
                for (int x = 0 ; x < COLUMN_COUNT ; x++)
                {
                    for (int y = 0 ; y < ROW_COUNT ; y++)
                    {
                        if (feverBombOrder[(int)queue_pos][x][y] == feverBombOrderCnt[(int)queue_pos])
                            next_bomb_pos.push_back(ccp(x, y));
                    }
                }
                if ((int)next_bomb_pos.size() == 0)
                    feverBombOrderCnt[(int)queue_pos]++;
                else
                    break;
            }
            
            //CCLog("callback FEVER : %d , %d", feverBombOrderCnt[(int)queue_pos], feverBombOrderMax[(int)queue_pos]);
            // 다음이 마지막 순차폭발이면 미리 flag를 바꿔 놓는다. (callback 진행하도록)
            if (feverBombOrderCnt[(int)queue_pos] >= feverBombOrderMax[(int)queue_pos])
                feverBombOrderCnt[(int)queue_pos] = -1;
            
            //CCLog("callback FEVER : done ? (%d)", feverBombOrderCnt[(int)queue_pos]);
            
            // 터뜨릴 피스가 없는데 순차폭발 횟수가 이미 다 끝났다면? 바로 fallingCallback 진행해서 다음으로 넘어간다.
            if ((int)next_bomb_pos.size() == 0 && feverBombOrderCnt[(int)queue_pos] == -1)
            {
                for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
                {
                    int x = (int)piece8xy[(int)queue_pos][i].x;
                    int y = (int)piece8xy[(int)queue_pos][i].y;
                    puzzleP8set->RemoveChild(x, y);
                    spriteP8[x][y] = NULL;
                }
                piece8xy[(int)queue_pos].clear();
                
                //if (!isFeverRound[(int)queue_pos])
                //{
                    if (m_iNextState[(int)queue_pos] == SKILL_CYCLE) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
                    {
                        m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                        
                        InvokeSkills((int)queue_pos);
                        ////piece8xy[(int)queue_pos].clear();
                        
                        return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
                    }
                    //else
                    //    piece8xy[(int)queue_pos].clear();
                //}
                
                ////piece8xy[(int)queue_pos].clear();
            }
            else
            {
                // Bomb()로 돌아가 그다음 순차 폭발을 실행한다.
                Bomb((int)queue_pos, next_bomb_pos);
                return;
            }
        }
        
        else if (globalType[(int)queue_pos] == ITEM_STAFF) // 지팡이 아이템 사용 시
        {
            CCLog("bomb callback - ITEM STAFF");
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            piece8xy[(int)queue_pos].clear();
        }
        else if (m_iState[(int)queue_pos] == SKILL_CYCLE) // cycle 주변부 폭발 완료
        {
            CCLog("bomb callback (%d) CYCLE", (int)queue_pos);
            std::vector<CCPoint> temp = skill->A2GetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A2Clear();
        }
        else if (m_iState[(int)queue_pos] == SKILL_BASIC) // 한붓그리기 부분 폭발 완료 (피버타임 순차폭발이 끝났을 경우에도)
        {
            CCLog("bomb callback (%d) BASIC", (int)queue_pos);
            // 추가점수 , +10 은 따로 둔 callback에서 처리하도록 하자
            
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            
            //if (!isFeverRound[(int)queue_pos])
            //{
                if (m_iNextState[(int)queue_pos] == SKILL_CYCLE) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
                {
                    m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                
                    InvokeSkills((int)queue_pos);
                    piece8xy[(int)queue_pos].clear();
                
                    return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
                }
                //else
                //    piece8xy[(int)queue_pos].clear();
            //}
            piece8xy[(int)queue_pos].clear();
        }
        else if (m_iState[(int)queue_pos] == SKILL_DOUBLESIX)
        {
            CCLog("bomb callback (%d) DOUBLESIX", (int)queue_pos);
            std::vector<CCPoint> temp = skill->GetResult();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
        }
        else if (m_iState[(int)queue_pos] == SKILL_COCOTIME)
        {
            CCLog("bomb callback (%d) COCOTIME", (int)queue_pos);
            return; // Falling은 F7 (스킬) 함수쪽에서 한다.
        }
        else if (m_iState[(int)queue_pos] == SKILL_FINAL) // 8번 스킬 폭발 완료
        {
            CCLog("bomb callback (%d) FINAL", (int)queue_pos);
            std::vector<CCPoint> temp;
            if (skill->F8_IsReady()) // 처음 시작 때 전체폭발
                temp = skill->GetResult();
            else // 혜성으로 인한 폭발
                temp = effect->GetDoublePos(sender->getTag());
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
        }
        else // '여신의 은총'
        {
            //CCLog("여신의 은총 : Bomb callback");
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            piece8xy[(int)queue_pos].clear();
        }
        
        
        // falling들어가기 전에 검사 : 직전 한붓그리기가 아직 falling을 시작하지 않았다면, 현재 한붓그리기는 falling을 하지 않는다.
        // 어차피 직전 한붓그리기가 한번에 해결해 줄 것이기 때문에!
        if (!wasFallen[((int)queue_pos-1+QUEUE_CNT) % QUEUE_CNT])
        {
            wasFallen[(int)queue_pos] = true;
            EndRound((int)queue_pos);
            return;
        }
        // falling queue insertion
        fallingQueue.push((int)queue_pos);
        if (!isFalling)
            FallingProcess();
    }
}

void Puzzle::FallingProcess()
{
    CCLog("Falling Process In");
    
    if (fallingQueue.size() > 0)
        isFalling = true;
    else
    {
        isFalling = false;
        return;
    }
    
    int queue_pos = fallingQueue.front();
    wasFallen[queue_pos] = true;
    if (isFeverRound[(int)queue_pos])
        CCLog("Falling Process = %d", queue_pos);
    fallingQueue.pop();
    Falling(queue_pos);
}

void Puzzle::Falling(int queue_pos, int xx)
{
    if (isFeverRound[(int)queue_pos])
        CCLog("Falling 들어옴 : queue_pos %d", queue_pos);
    
    // get the number of total falling objects.
    m_iFallingCallbackCnt = 0;
    m_numOfFallingObjects = 0;
    if (xx != -1)
    {
        m_iFallingCallbackCnt_E8[xx] = 0;
        m_numOfFallingObjects_E8[xx] = 0;
    }
    
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
        for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
			if (spriteP8[x][y] == NULL)
            {
                m_numOfFallingObjects += (ROW_COUNT - y);
                if (xx != -1) m_numOfFallingObjects_E8[xx] += (ROW_COUNT - y);
                if (x == 0 || x == COLUMN_COUNT-1)
                {
                    m_numOfFallingObjects--;
                    if (xx != -1) m_numOfFallingObjects_E8[xx]--;
                }
                break;
            }
        }
    }
    
    // 실제로 떨어지게 한다. (action)
	for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
        int miny = 999;
        for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
			if (spriteP8[x][y] == NULL)
			{
                miny = std::min(miny, y);
                
                int pos;
                int end = (x == 0 || x == COLUMN_COUNT-1) ? ROW_COUNT-1 : ROW_COUNT;
                for (pos = y ; pos < end ; pos++)
                {
                    if (spriteP8[x][pos] != NULL)
                        break;
                }
                
                if (pos >= end)
                {
                    // 더 이상 내려야 할 8각형이 없는 경우 (새로 만들어서 drop시킨다)
                    if (!isGameOver && iNumOfPaintItemNeeded > 0 && y+1 == end) // paint item 필요할 때
                    {
                        iNumOfPaintItemNeeded--;
                        puzzleP8set->CreatePiece(x, y, -ITEM_PAINT_RED);
                    }
                    else if (!isGameOver && iNumOfStaffItemNeeded > 0 && y+1 == end) // staff item 필요할 때
                    {
                        iNumOfStaffItemNeeded--;
                        puzzleP8set->CreatePiece(x, y, ITEM_STAFF);
                    }
                    else // 일반 랜덤 피스 생성
                    {
                        puzzleP8set->CreatePiece(x, y);
                    }
                    puzzleP8set->GetObject(x, y)->SetPosition(SetPiece8Position(x, end));
                    puzzleP8set->AddChild(x, y);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    puzzleP8set->Falling(x, y, x, y, queue_pos, xx);
                }
                else
                {
                    // 떨어지는 경우
                    puzzleP8set->MoveObject(x, y, x, pos);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    spriteP8[x][pos] = NULL;
                    puzzleP8set->Falling(x, y, x, y, queue_pos, xx);
                }
			}
		}
        
        // 떨어지는 모든 피스의 주위 연결피스를 제거한다.
        int maxy = (x == 0 || x == COLUMN_COUNT-1) ? ROW_COUNT-1 : ROW_COUNT;
        //CCLog("x(%d) : %d , %d", x, miny, maxy);
        for (int y = miny ; y < maxy ; y++)
        {
            RemoveConnectPiecesXY(x, y);
        }
	}
}

void Puzzle::FallingCallback(CCNode* sender, void* queue_pos)
{
    int queue = (int)queue_pos;
    int xx = sender->getTag(); // E8 스킬일 때 vertical 라인 idx
    m_iFallingCallbackCnt++;
    if (xx != -1) m_iFallingCallbackCnt_E8[xx]++;
    
	if ((xx == -1 && m_numOfFallingObjects == m_iFallingCallbackCnt) ||
        (xx != -1 && m_numOfFallingObjects_E8[xx] == m_iFallingCallbackCnt_E8[xx]))
	{
        if (isFeverRound[(int)queue_pos])
            CCLog("Falling Callback : %d", (int)queue_pos);
        // drop이 모두 끝나면, diamond들을 다시 검사해서 적절히 바꿔준다.
        CreateConnectPieces();
        //ReplaceConnectPieces();
        
        // lock을 모두 해제한다. (semaphore를 1씩 감소시킨다)
        int x, y;
        for (int i = 0 ; i < lock8xy[queue].size() ; i++)
        {
            x = lock8xy[queue][i].x;
            y = lock8xy[queue][i].y;
            m_bLockP8[x][y]--;
        }
        lock8xy[queue].clear();

        /************ LOCK print ****************/
        for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
        {
            CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
                  m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
        }
        /*****************************************/
        
        // 요정에 의해 생성된 연결피스가 있다면 이제 만들어주자.
        //CCLog("요정에 의해 생성할 수 = %d", iNumOfFairySkillStart);
        /*
        while (iNumOfFairySkillStart > 0)
        {
            iNumOfFairySkillStart--;
            FairySkillAction();
        }
        */
        
        if (bonusTimeState == -1) // 보너스 폭발
        {
            FallingProcess();
            bonusBombCnt--;
            if (bonusBombCnt > 0)
                skill->BonusBomb(0);
            else
                BonusTime(NULL, this); // 다음 보너스 스킬로 넘어가자.
        }
        else if (bonusTimeState == 0 || bonusTimeState == 1) // 라스트 아이템 폭발
        {
            FallingProcess();
            skill->LastItems(bonusTimeState, 0);
        }
        else if (isRenewing)
        {
            // 피스판 갱신 필요성 검사
            isFalling = false;
            if (skill->IsRenewNeeded())
            {
                CancelDrawing();
                skill->RenewPuzzle((int)queue_pos);
            }
        }
        else if (xx != -1)
        {
            // '고대나무' 스킬의 한 line falling이 끝나면, Restart 여부를 확인하고, 끝났다면 라운드 전체를 종료한다.
            skill->E8_DecideRestart(xx);
            if (skill->E8_IsFinished())
            {
                m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                //CCLog("Falling callback (%d) - 다음 스킬 [고대나무 끝났음] : %d", queue, m_iState[(int)queue_pos]);
                InvokeSkills((int)queue_pos);
            }
            return;
        }
        else
        {
            if (m_iState[(int)queue_pos] == SKILL_FINAL && globalType[int(queue_pos)] == PIECE_RED)
            {
                if (skill->F8_IsReady())
                {
                    skill->F8_Start(7, (int)queue_pos);
                }
                else
                {
                    // '붉은 용의 숨결' 발동 중이면, 혜성이 다 떨어졌을 때만 끝내도록 한다.
                    skill->F8_FinishCountUp();
                    if (skill->F8_IsFinished())
                    {
                        effect->Effect7_Clear();
                        GoNextState((int)queue_pos);
                    }
                    else
                    {
                        skill->F8_Bomb_Real();
                    }
                }
            }
            else
            {
                FallingProcess();
        
                // '여신의 은총' 실행 중이면 폭발이 끝났다는 신호를 보낸다.
                if (skill->W8_IsActive())
                {
                    // 피스판 갱신 필요성 검사
                    isFalling = false;
                    if (skill->IsRenewNeeded())
                    {
                        CancelDrawing();
                        skill->RenewPuzzle((int)queue_pos);
                    }
                    
                    skill->W8_BombDone();
                }
                else
                {
                    if (globalType[(int)queue_pos] >= PIECE_RED && globalType[(int)queue_pos] <= PIECE_WHITE)
                        GoNextState((int)queue_pos);
                    else if (globalType[(int)queue_pos] == ITEM_STAFF) // 스태프 아이템을 눌러서 여기까지 왔을 경우
                        m_iItemSP--;
                        //m_bIsItemPossible = true;
                }
            }
        }
    }
}

void Puzzle::GoNextState(int queue_pos)
{
    // 피스 갱신으로 인해 실행된 경우는 무시하자.
    if (m_iState[queue_pos] == SKILL_DONE)
    {
        //m_bIsItemPossible = true;
        m_iItemSP--;
        return;
    }
    
    // 다음 스킬로 넘어간다.
    m_iState[queue_pos] = m_iNextState[queue_pos];
    CCLog("Falling callback (%d) - 다음 스킬 [일반] : %d", queue_pos, m_iState[queue_pos]);
    InvokeSkills(queue_pos);
}

void Puzzle::FallingQueuePushAndFalling(int queue_pos)
{
    // falling들어가기 전에 검사 : 직전 한붓그리기가 아직 falling을 시작하지 않았다면, 현재 한붓그리기는 falling을 하지 않는다.
    // 어차피 직전 한붓그리기가 한번에 해결해 줄 것이기 때문에!
    if (!wasFallen[(queue_pos-1+QUEUE_CNT) % QUEUE_CNT])
    {
        wasFallen[queue_pos] = true;
        /*
        // lock을 모두 해제한다. (semaphore를 1씩 감소시킨다)
        int x, y;
        for (int i = 0 ; i < lock8xy[(int)queue_pos].size() ; i++)
        {
            x = lock8xy[(int)queue_pos][i].x;
            y = lock8xy[(int)queue_pos][i].y;
            m_bLockP8[x][y]--;
        }
        lock8xy[(int)queue_pos].clear();
         */
        return;
    }
    // falling queue insertion
    fallingQueue.push(queue_pos);
    CCLog("FallingQueuePushAndFalling (%d) : %d", queue_pos, isFalling);
    if (!isFalling)
        FallingProcess();
}

void Puzzle::SetRenewFlag(bool flag)
{
    isRenewing = flag;
}
bool Puzzle::IsRenewing()
{
    return isRenewing;
}

void Puzzle::GameEnd(CCNode* sender, void* pointer)
{
    isStartGameEnd = true;
    
    // 인포메이션 바 삭제
    spriteClassInfo->RemoveAllObjects();
    delete spriteClassInfo;
    pScoreLayer->removeAllChildren();
    pScoreLayer->removeFromParentAndCleanup(true);

  //  using_skill_count_list : *배열임, 스킬 활성 수 기입 (배열 key값은 skill_ID)
  //  break_piece_count_list : *배열임, 제거한 피스 수 기입 (배열 key값은 1-물, 2-불, 3-땅)
 
    char temp[100];
    bool flag;
    std::string param = "";
    
    int addedStarcandyBySkill = 0;
    
    int num;
    flag = false;
    for (int i = 0 ; i < NUMOFSKILL ; i++)
    {
        if (skill->GetSkillAppliedCount(i) > 0)
        {
            flag = true;
            num = SkillInfo::ConvertedToOriginal(i);
            sprintf(temp, "using_skill_count_list[%d]=%d&", num, skill->GetSkillAppliedCount(i));
            param += temp;
            addedStarcandyBySkill += (int)skill->GetSkillAppliedCount(i);
        }
    }

    if (!flag)
    {
        sprintf(temp, "using_skill_count_list[0]=0&");
        param += temp;
    }
    
    flag = false;
    for (int i = 0 ; i < 5 ; i++)
    {
        if (iPieceBombCnt[i] > 0)
        {
            flag = true;
            if (i == PIECE_RED) num = 2;
            else if (i == PIECE_BLUE) num = 1;
            else if (i == PIECE_GREEN) num = 3;
            else if (i == PIECE_YELLOW) num = 4;
            else num = 5;
            sprintf(temp, "break_piece_count_list[%d]=%d&", num, iPieceBombCnt[i]);
            param += temp;
        }
    }
    if (!flag)
    {
        sprintf(temp, "break_piece_count_list[0]=0&");
        param += temp;
    }

    // 별사탕 최종개수 계산
    iStarCandy = FakeStarCandy( (int)((float)RealStarCandy() + (float)RealStarCandy()*ADD_STARCANDY_PERCENT/100.0f) );
    iStarCandy = FakeStarCandy( (int)((float)RealStarCandy() + addedStarcandyBySkill) );
    //iStarCandy = FakeStarCandy( (int)((float)RealStarCandy() + addedStarcandyBySkill + (float)RealStarCandy()*ADD_STARCANDY_PERCENT/100.0f) );
    
    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
    param += temp;
    sprintf(temp, "score=%d&", RealScore());
    param += temp;
    sprintf(temp, "starcandy=%d&", RealStarCandy());
    param += temp;
    sprintf(temp, "combo=%d", maxCombo);
    param += temp;
    
    //// Loading 화면으로 MESSAGE request 넘기기
    //Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_PUZZLEEND);
    
    Network::HttpPost(param, URL_GAMEEND, this, httpresponse_selector(Puzzle::onHttpRequestCompleted));
}

void Puzzle::onHttpRequestCompleted(CCNode *sender, void *data)
{
    //if (XMLStatus == -1) // Loading 창 끄기 (게임 시작하는 경우)
    //    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    switch (XMLStatus)
    {
        case 0: XmlParseGameEnd(&xmlDoc); break;
        case 1: XmlParseFriends(&xmlDoc); break;
    }
    XMLStatus++;
}

void Puzzle::XmlParseFriends(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "Puzzle", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 친구관계에서 시간이 중요한 것들 : 포션요청남은시간, 포션보내기남은시간, 토파즈요청남은시간
        // 여기서 지금 시간을 저장해둔다.
        savedTime2 = time(0);
        
        // clear friend-list
        for (int i = 0 ; i < friendList.size() ; i++)
            delete friendList[i];
        friendList.clear();
        
        std::string kakaoId;
        std::string nickname;
        std::string imageUrl;
        int potionMsgStatus;
        int weeklyHighScore;
        int scoreUpdateTime;
        int remainPotionTime;
        int remainRequestPotionTime;
        int remainRequestTopazTime;
        int highScore;
        int certificateType;
        int profileTitleId;
        int fire;
        int water;
        int land;
        int master;
        int fairyId;
        int fairyLevel;
        int skillId;
        int skillLevel;
        
        xml_object_range<xml_named_node_iterator> friends = nodeResult.child("friend-list").children("friend");
        for (xml_named_node_iterator it = friends.begin() ; it != friends.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "kakao-id") kakaoId = ait->as_string();
                else if (name == "nick-name") nickname = ait->as_string();
                else if (name == "profile-image-url") imageUrl = ait->as_string();
                else if (name == "potion-message-receive") potionMsgStatus = ait->as_int();
                else if (name == "remain-potion-send-time") remainPotionTime = ait->as_int();
                else if (name == "remain-request-potion-send-time") remainRequestPotionTime = ait->as_int();
                else if (name == "remain-request-topaz-send-time") remainRequestTopazTime = ait->as_int();
                else if (name == "high-score") highScore = ait->as_int();
                else if (name == "weekly-high-score") weeklyHighScore = ait->as_int();
                else if (name == "score-update-time") scoreUpdateTime = ait->as_int();
                else if (name == "certificate-type") certificateType = ait->as_int();
                else if (name == "name-title-id") profileTitleId = ait->as_int();
                else if (name == "properties-fire") fire = ait->as_int();
                else if (name == "properties-water") water = ait->as_int();
                else if (name == "properties-land") land = ait->as_int();
                else if (name == "properties-master") master = ait->as_int();
                else if (name == "fairy-id") fairyId = ait->as_int();
                else if (name == "fairy-level") fairyLevel = ait->as_int();
                else if (name == "skill-id") skillId = ait->as_int();
                else if (name == "skill-level") skillLevel = ait->as_int();
            }
            
            // nickname 너무 길면 자르자.
            nickname = SubstrNickname(nickname);
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, remainRequestPotionTime, remainRequestTopazTime, weeklyHighScore, highScore, scoreUpdateTime, certificateType, profileTitleId, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            // potion image 처리
            friendList[(int)friendList.size()-1]->SetPotionSprite();
        }
        // sort by { max[weeklyScore], min[scoreUpdateTime] }
        DataProcess::SortFriendListByScore();
        
        
        // 카카오 친구리스트에서 필요한 변수들을 집어넣는다.
        int cnt = KakaoFriends::getInstance()->appFriends->count();
        CCArray* keys = KakaoFriends::getInstance()->appFriends->allKeys();
        std::string id;
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            for (int j = 0 ; j < cnt ; j++)
            {
                id = ((CCString*)keys->objectAtIndex(j))->getCString();
                if (friendList[i]->GetKakaoId() == id)
                {
                    KakaoFriends::Friends* f = (KakaoFriends::Friends*)KakaoFriends::getInstance()->appFriends->objectForKey(id.c_str());
                    friendList[i]->SetKakaoVariables( f->nickname, f->profileImageUrl, f->hashedTalkUserId, f->messageBlocked, f->supportedDevice );
                    break;
                }
            }
        }
        
        
        // 본인이 몇 등인지 검사해서, 기존 순위보다 높으면 랭킹변동 UI를 띄운다.
        isRankUp = false;
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            {
                if (i < curMyRank)
                    isRankUp = true;
                break;
            }
        }
        //CCLog ("IS RANK UP ? = %d", isRankUp);

        Common::ShowNextScene(this, "Puzzle", "PuzzleResult", false);
    }
}

std::string Puzzle::SubstrNickname(std::string nickname)
{
    if (nickname.size() > 20)
    {
        //CCLog("%s", nickname.c_str());
        int i;
        for (i = 0 ; i < nickname.size() ; i++)
        {
            if (i >= 20)
                break;
            if (isascii(nickname[i]) != 1)
                i += 2;
        }
        nickname = nickname.substr(0, i);
        nickname += "...";
    }
    return nickname;
}



void Puzzle::XmlParseGameEnd(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "Puzzle", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // new record 인가?
        int isNewRecord = nodeResult.child("new-record").text().as_int();
        // mission 성공인가?
        int isMissionSuccess = nodeResult.child("mission-success").text().as_int();
        
        // 얻은 돈,포션 값
        int getTopaz = nodeResult.child("game-get").attribute("topaz").as_int();
        int getStarCandy = nodeResult.child("game-get").attribute("star-candy").as_int();
        int getPotion = nodeResult.child("game-get").attribute("potion").as_int();
        int getMP = nodeResult.child("game-get").attribute("mp").as_int();
        
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        /*
        // 포션 갱신
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        if (potionRemainTime > 0)
            myInfo->SetPotion(potion, myInfo->GetRemainPotionTimeNumber());
        else
            myInfo->SetPotion(potion, 0);
        */
        
        // 코코 정보 갱신
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int staffMP = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int fairyMP = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffFailPoint = nodeResult.child("coco").attribute("magic-staff-fail-point").as_int();
        myInfo->SetCoco(mp, staffMP, fairyMP, staffLv, staffFailPoint);
        
        // 시작 유저 보상 관련
        isStartUser = (nodeResult.child("start-user").text().as_int() == 1);
        
        // 포션 보상에 따른 개수 변화
        //int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        //int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        //myInfo->SetPotion(potion, potionRemainTime);
        
        // MP 보상에 따른 변화
        int isPotionMax = nodeResult.child("mp-reward").attribute("potion-max").as_int();
        int addTopazNum = nodeResult.child("mp-reward").attribute("add-topaz-num").as_int();
        //CCLog("%d %d", isPotionMax, addTopazNum);
        myInfo->SetReward(isPotionMax, addTopazNum);
        
        // 추가 점수 % 계산
        float addedMPPercent = ((float)(myInfo->GetMPTotal())/100.0f) + 1.0f;
        
        int practiceUserSkillId = nodeResult.child("coco").attribute("practice-user-skill-id").as_int();
        
        // 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp, isActive, learntime;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "exp") exp = ait->as_int();
                else if (name == "is-active") isActive = ait->as_int();
                else if (name == "learn-time") learntime = ait->as_int();
            }
            myInfo->AddSkill(csi, usi, level, exp, learntime);
            
            // 현재 연습 중인 스킬 id 가져오기
            if (usi == practiceUserSkillId)
            {
                myInfo->SetPracticeSkill(csi, level);
            }
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        isRankUp = false;
        // score 정보 갱신
        int totalScore = nodeResult.child("score").attribute("now-score").as_int();
        int bestCombo = nodeResult.child("score").attribute("best-combo").as_int();
        
        // script (말풍선 내용)
        std::string text = nodeResult.child("script").text().as_string();
        
        // 결과화면에 보낼 데이터 class 생성
        myGameResult = new MyGameResult(getTopaz, getStarCandy, getPotion, getMP, addedMPPercent, RealScore(), totalScore, maxCombo, bestCombo, isMissionSuccess, isNewRecord, text);
        int num;
        for (int i = 0 ; i < NUMOFSKILL ; i++)
        {
            if (skill->GetSkillAppliedCount(i) > 0)
            {
                num = SkillInfo::ConvertedToOriginal(i);
                myGameResult->skillNum.push_back(num);
                myGameResult->skillCnt.push_back(skill->GetSkillAppliedCount(i));
            }
        }
        
        
        int highScore = nodeResult.child("score").attribute("high-score").as_int();
        int weeklyHighScore = nodeResult.child("score").attribute("weekly-high-score").as_int();
        int certificateType = nodeResult.child("score").attribute("certificate-type").as_int();
        int remainWeeklyRankTime = nodeResult.child("score").attribute("remain-weekly-rank-time").as_int();
        myInfo->SetScore(highScore, weeklyHighScore, certificateType, remainWeeklyRankTime);
        // 친구리스트 호출의 필요성에 대한 검사
        bool flag = true;
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            {
                curMyRank = i;
                friendList[i]->SetScore(highScore, weeklyHighScore, certificateType);
                if (i > 0 && weeklyHighScore > friendList[i-1]->GetWeeklyHighScore())
                {
                    flag = false;
                    char temp[50];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
                    param += temp;
                    
                    Network::HttpPost(param, URL_FRIENDLIST, this, httpresponse_selector(Puzzle::onHttpRequestCompleted));
                    
                    break;
                }
            }
        }
        
        // 친구리스트 새로 호출하지 않는다면, 바로 게임결과 화면으로 넘어가자.
        if (flag)
        {
            //// Loading 창 끄기
            //((Loading*)Depth::GetCurPointer())->EndScene();
            
            Common::ShowNextScene(this, "Puzzle", "PuzzleResult", false);
        }
    }
}


PuzzleSkill* Puzzle::GetSkill()
{
    return skill;
}
void Puzzle::SetPiece8xy(int queue_pos, std::vector<CCPoint> pos)
{
    piece8xy[queue_pos] = pos;
}
std::vector<CCPoint> Puzzle::GetPiece8xy(bool afterCast)
{
    if (afterCast)
        return piece8xy[(touch_cnt-1)%QUEUE_CNT];
    return piece8xy[touch_cnt%QUEUE_CNT];
}
std::vector<CCPoint> Puzzle::GetPosForFeverTime(bool afterCast)
{
    if (afterCast)
        return posForFeverTime[(touch_cnt-1)%QUEUE_CNT];
    return posForFeverTime[touch_cnt%QUEUE_CNT];
}

int Puzzle::GetGlobalType()
{
    return globalType[(touch_cnt-1)%QUEUE_CNT];
}

void Puzzle::SetSkillLock(int queue_pos, bool flag)
{
    m_bSkillLock[queue_pos] = flag;
}

bool Puzzle::IsCycle(int queue_pos)
{
    return m_bIsCycle[queue_pos];
}

Sound* Puzzle::GetSound()
{
    return sound;
}

CCLayer* Puzzle::GetPuzzleLayer()
{
    return puzzleLayer;
}
CCLayer* Puzzle::GetCocoLayer()
{
    return cocoLayer;
}
CCLayer* Puzzle::GetFairyLayer()
{
    return fairyLayer;
}

bool Puzzle::IsPaused()
{
    return isInGamePause;
}

void Puzzle::SkillSemaphoreUpdate(int val)
{
    m_iSkillSP += val;
}
bool Puzzle::IsSkillSemaphoreFree()
{
    return m_iSkillSP == 0;
}

void Puzzle::EndScene()
{
    CCLog("Puzzle : EndScene");
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Puzzle");
    
    // layer 배경색을 검은색으로 바꾼다.
    this->setColor(ccc3(0,0,0));
//CCLog("0");
    // 일시정지 화면에서 바로 종료하는 상황일 경우
    if (isInGamePause)
    {
        if (skill->W8_IsActive())
        {
            this->unschedule(schedule_selector(PuzzleSkill::W8_Timer));
            this->unschedule(schedule_selector(PuzzleSkill::W8_AccelTimer));
        }
        if (skill->F8_IsActive())
            this->unschedule(schedule_selector(PuzzleSkill::F8_Comet));
        if (skill->E8_IsActive())
            this->unschedule(schedule_selector(PuzzleSkill::E8_Timer));
        
        this->unschedule(schedule_selector(Puzzle::UpdateTimer));
        this->unschedule(schedule_selector(Puzzle::HintTimer));
        this->unschedule(schedule_selector(Puzzle::ComboTimer));
        if (myInfo->GetActiveFairyId() > 0)
            this->unschedule(schedule_selector(Puzzle::ChangeAnimFairy));
        
        spriteClassInfo->RemoveAllObjects();
        delete spriteClassInfo;
        pScoreLayer->removeAllChildren();
        pScoreLayer->removeFromParentAndCleanup(true);
    }
    isInGamePause = false;
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->stopAllActions();
//CCLog("1");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game3.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/popup.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game2.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game3.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/popup.png");
//CCLog("2");
    switch (myInfo->GetActiveFairyId())
    {
        case 1:
            CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/fairy_flower.plist");
            CCTextureCache::sharedTextureCache()->removeTextureForKey("images/fairy_flower.png");
            break;
        case 3:
            CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/fairy_cloud.plist");
            CCTextureCache::sharedTextureCache()->removeTextureForKey("images/fairy_cloud");
            break;
    }
    
    CCTextureCache::sharedTextureCache()->removeAllTextures();
    
//CCLog("3");
    // delete all objects
    effect->RemoveAllObjects();
    delete effect;
    skill->RemoveAllObjects();
    delete skill;
    sound->StopAllEffects();
    sound->StopBackgroundInGameSound();
    sound->UnLoadInGameSound();
    delete sound;
//CCLog("4");
    puzzleP4set->RemoveAllObjects();
    delete puzzleP4set;
    puzzleP8set->RemoveAllObjects();
    delete puzzleP8set;
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    while (!fallingQueue.empty())
        fallingQueue.pop();
//CCLog("5");
    for (int i = 0 ; i < lock8xy.size() ; i++) lock8xy[i].clear();
    for (int i = 0 ; i < lock4xy.size() ; i++) lock4xy[i].clear();
    for (int i = 0 ; i < piece4xy.size() ; i++) piece4xy[i].clear();
    for (int i = 0 ; i < piece8xy.size() ; i++) piece8xy[i].clear();
    for (int i = 0 ; i < strap.size() ; i++) strap[i].clear();
    lock8xy.clear();
    lock4xy.clear();
    piece4xy.clear();
    piece8xy.clear();

//CCLog("6");
    for (int i = 0 ; i < strap.size() ; i++)
    {
        for (int j = 0 ; j < strap[i].size() ; j++)
            strap[i][j]->removeFromParentAndCleanup(true);
        strap[i].clear();
    }
    strap.clear();

    pTimerLabel->removeFromParentAndCleanup(true);
//CCLog("7");
    boardSP->removeFromParentAndCleanup(true);
    boardClip->removeAllChildren();
    boardClip->removeFromParentAndCleanup(true);
    
    puzzleLayer->removeAllChildren();
    puzzleLayer->removeFromParentAndCleanup(true);
//CCLog("8");
    timerLayer->removeAllChildren();
    timerLayer->removeFromParentAndCleanup(true);
    timerStencil->removeFromParentAndCleanup(true);
    timerClip->removeFromParentAndCleanup(true);

    gaugeLayer->removeAllChildren();
    gaugeLayer->removeFromParentAndCleanup(true);
    gaugeStencil->removeFromParentAndCleanup(true);
    gaugeClip->removeFromParentAndCleanup(true);
    
    for (int i = 0 ; i < coco_sp.size() ; i++)
        coco_sp[i]->removeFromParentAndCleanup(true);
    coco_sp.clear();
    for (int i = 0 ; i < fairy_sp.size() ; i++)
        fairy_sp[i]->removeFromParentAndCleanup(true);
    fairy_sp.clear();
//CCLog("9");
    cocoLayer->removeAllChildren();
    cocoLayer->removeFromParentAndCleanup(true);
    if (myInfo->GetActiveFairyId() > 0)
    {
        fairyShadow->removeFromParentAndCleanup(true);
        fairyLayer->removeAllChildren();
        fairyLayer->removeFromParentAndCleanup(true);
    }
    
//CCLog("10");
    if (!isRebooting)
    {
        if (isRankUp)
            Common::ShowNextScene(this, "Puzzle", "RankUp", true);
        else
            Common::ShowNextScene(this, "Puzzle", "Ranking", true, 1);
    }
}

void Puzzle::EndSceneCallback()
{
}

float Puzzle::GetPieceWidth()
{
    return PIECE8_WIDTH;
}

PuzzleP8Set* Puzzle::GetPuzzleP8Set()
{
    return puzzleP8set;
}
PuzzleP4Set* Puzzle::GetPuzzleP4Set()
{
    return puzzleP4set;
}

void Puzzle::SetSpriteP8Null(int x, int y)
{
    spriteP8[x][y] = NULL;
}

CCSprite* Puzzle::GetSpriteP8(int x, int y)
{
    return spriteP8[x][y];
}

float Puzzle::GetBoardSize()
{
    return boardSize.height;
}

bool Puzzle::IsConnected(int x, int y)
{
    return (puzzleP4set->GetType(x, y) != BLOCKED);
}

Effect* Puzzle::GetEffect()
{
    return effect;
}

void Puzzle::PlayEffect(int skillNum, int queue_pos)
{
    CCLog("Puzzle :: PlayEffect - %d", skillNum);
    if (skillNum == 100)
        effect->PlayEffect(skillNum, queue_pos, piece8xy[0]);
    else if (skillNum == 1 || skillNum == 17)
        effect->PlayEffect(skillNum, queue_pos, skill->A2GetPos());
    else if (skillNum == 9)
        effect->PlayEffect(skillNum, queue_pos, skill->A2GetPos());
    else if (skillNum == 5 || skillNum == 13 || skillNum == 21)
        effect->PlayEffect(skillNum, queue_pos, skill->GetResult()); //skill->A4BGetPos());
    else if (skillNum == 7 || skillNum == 15 || skillNum == 23)
        effect->PlayEffect(skillNum, queue_pos, skill->A8GetPos());
    else
        effect->PlayEffect(skillNum, queue_pos, piece8xy[queue_pos]);
        //effect->PlayEffect(skillNum, queue_pos, piece8xy[(touch_cnt-1)%QUEUE_CNT]);
}

void Puzzle::SwapSpriteP8(int x1, int y1, int x2, int y2)
{
    spriteP8[x1][y1] = puzzleP8set->GetSprite(x1, y1);
    spriteP8[x2][y2] = puzzleP8set->GetSprite(x2, y2);
}

void Puzzle::SetSpiritTouch(bool val)
{
    if (!val)
        m_iSpiritSP--;
    m_bIsSpiritExecuted = false;
    //m_bIsSpiritTouched = val;
}

void Puzzle::SetSpriteP8(int x, int y, CCSprite* sp)
{
    spriteP8[x][y] = sp;
}

bool Puzzle::IsItemClear()
{
    return item_clear;
}
bool Puzzle::IsItemTime()
{
    return item_time;
}
bool Puzzle::IsItemPaint()
{
    return item_paint;
}
bool Puzzle::IsItemStaff()
{
    return item_staff;
}
void Puzzle::UpdateSemaphore(int v)
{
    m_iItemSP += v;
}
//void Puzzle::SetItemPossible(bool flag)
//{
//    m_bIsItemPossible = flag;
//}
CCClippingNode* Puzzle::GetBoardClip()
{
    return boardClip;
}
bool Puzzle::IsGameOver()
{
    return isGameOver;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void PuzzleP8Set::SetGameLayer(Puzzle* layer)
{
    gameLayer = layer;
}

void PuzzleP8Set::SetPuzzleLayer(CCLayer* layer)
{
    puzzleLayer = layer;
}

void PuzzleP8Set::CreatePiece(int x, int y, int type)
{
    object[x][y] = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x, y), gameLayer, 20, gameLayer->GetBoardSize()/(float)gameLayer->board_wh, gameLayer->IsItemClear(), type);
    gameLayer->SetSpriteP8(x, y, object[x][y]->GetPiece());
}

CCSprite* PuzzleP8Set::GetSprite(int x, int y)
{
    return object[x][y]->GetPiece();
}

void PuzzleP8Set::AddChild(int x, int y)
{
    gameLayer->GetBoardClip()->addChild(object[x][y]->GetPiece(), object[x][y]->GetZOrder());
}

void PuzzleP8Set::RemoveChild(int x, int y)
{
    if (gameLayer->GetSpriteP8(x, y) != NULL)
    {
        object[x][y]->GetPiece()->removeFromParentAndCleanup(true);
        object[x][y]->RemovePiece();
        gameLayer->SetSpriteP8Null(x, y);
    }
    if (object[x][y] != NULL)
    {
        delete object[x][y];
        object[x][y] = NULL;
    }
}

int PuzzleP8Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}

PuzzleP8* PuzzleP8Set::GetObject(int x, int y)
{
    return object[x][y];
}

void PuzzleP8Set::MoveObject(int x, int y, int fromX, int fromY)
{
    object[x][y] = object[fromX][fromY];
}

void PuzzleP8Set::SwapObject(int x1, int y1, int x2, int y2)
{
    PuzzleP8* p1 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x1, y1), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)gameLayer->board_wh, gameLayer->IsItemClear(), object[x2][y2]->GetType());
    PuzzleP8* p2 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x2, y2), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)gameLayer->board_wh, gameLayer->IsItemClear(), object[x1][y1]->GetType());
    
    RemoveChild(x1, y1);
    RemoveChild(x2, y2);
    object[x1][y1] = p1;
    object[x2][y2] = p2;
    
    gameLayer->SwapSpriteP8(x1, y1, x2, y2);
    AddChild(x1, y1);
    AddChild(x2, y2);
}
void PuzzleP8Set::Falling(int x, int y, int targetX, int targetY, int queue_pos, int vertical_idx)
{
    CCMoveTo* moveTo = CCMoveTo::create(0.05f, gameLayer->SetPiece8Position(targetX, targetY));
    CCFiniteTimeAction* action = CCSequence::create(
            moveTo,
            CCCallFuncND::create(gameLayer, callfuncND_selector(Puzzle::FallingCallback), (void*)queue_pos),
            NULL);
    object[x][y]->GetPiece()->setTag(vertical_idx); // '고대나무' 스킬에서 line idx를 위한 것
    object[x][y]->GetPiece()->runAction(action);
}
void PuzzleP8Set::RemoveAllObjects()
{
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            // board의 모서리에 있는 4개는 취급하지 않는다.
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            RemoveChild(x, y);
		}
	}
}


////////////////////////////////////////////////////////////////////////

void PuzzleP4Set::SetGameLayer(Puzzle* layer)
{
    gameLayer = layer;
}
void PuzzleP4Set::SetPuzzleLayer(CCLayer* layer)
{
    puzzleLayer = layer;
}
void PuzzleP4Set::CreateObject(int x, int y)
{
    object[x][y] = PuzzleP4::CreateP4(gameLayer, zGameObjectSP);
}
bool PuzzleP4Set::CreatePiece(int x, int y, int type)
{
    if ( (gameLayer->GetPuzzleP8Set()->GetObject(x-1, y) == NULL && !(x-1 == 0 && y == ROW_COUNT-1)) ||
         (gameLayer->GetPuzzleP8Set()->GetObject(x, y) == NULL && !(x == COLUMN_COUNT-1 && y == ROW_COUNT-1)) ||
         (gameLayer->GetPuzzleP8Set()->GetObject(x-1, y-1) == NULL && !(x-1 == 0 && y-1 == 0)) ||
         (gameLayer->GetPuzzleP8Set()->GetObject(x, y-1) == NULL && !(x == COLUMN_COUNT-1 && y-1 == 0)) )
        return false;
    
    // 4각형의 4방향에 있는 8각형들의 type을 구해서 삼각형을 어떻게 만들지 결정한다.
    // 가장자리는 그 주위의 P8 type이 존재하지 않을 수도 있으므로 조건문을 잘 쓰자.
    int lu = (x == 1 && y == ROW_COUNT-1) ? -1 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y);
    int ru = (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) ? -2 : gameLayer->GetPuzzleP8Set()->GetType(x, y);
    int ld = (x == 1 && y == 1) ? -3 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y-1);
    int rd = (x == COLUMN_COUNT-1 && y == 1) ? -4 : gameLayer->GetPuzzleP8Set()->GetType(x, y-1);
    
    // scale & probability
    float diaHalfWidth = gameLayer->GetPieceWidth() / (2.0f + sqrt(2.0f));
    int prob = gameLayer->CONN_PIECE_PROB;
    
    object[x][y]->CreateSprites(x, y, lu, ru, ld, rd, gameLayer->SetPiece4Position(x, y), diaHalfWidth, prob, type);
    return true;
}
int PuzzleP4Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}
void PuzzleP4Set::SetOpacity(int x, int y, int alpha)
{
    if (object[x][y]->GetLeftUp() != NULL)
        object[x][y]->GetLeftUp()->setColor(ccc3(alpha,alpha,alpha));
    if (object[x][y]->GetRightUp() != NULL)
        object[x][y]->GetRightUp()->setColor(ccc3(alpha,alpha,alpha));
    if (object[x][y]->GetLeftDown() != NULL)
        object[x][y]->GetLeftDown()->setColor(ccc3(alpha,alpha,alpha));
    if (object[x][y]->GetRightDown() != NULL)
        object[x][y]->GetRightDown()->setColor(ccc3(alpha,alpha,alpha));
}
void PuzzleP4Set::AddChild(int x, int y)
{
    int zOrder = object[x][y]->GetZOrder();
    if (object[x][y]->GetLeftUp() != NULL)
        puzzleLayer->addChild(object[x][y]->GetLeftUp(), zOrder);
    if (object[x][y]->GetRightUp() != NULL)
        puzzleLayer->addChild(object[x][y]->GetRightUp(), zOrder);
    if (object[x][y]->GetLeftDown() != NULL)
        puzzleLayer->addChild(object[x][y]->GetLeftDown(), zOrder);
    if (object[x][y]->GetRightDown() != NULL)
        puzzleLayer->addChild(object[x][y]->GetRightDown(), zOrder);
}
void PuzzleP4Set::RemoveChild(int x, int y)
{
    if (object[x][y]->GetLeftUp() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetLeftUp(), true);
    if (object[x][y]->GetRightUp() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetRightUp(), true);
    if (object[x][y]->GetLeftDown() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetLeftDown(), true);
    if (object[x][y]->GetRightDown() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetRightDown(), true);
    
    object[x][y]->SetRemoved(true);
    
    object[x][y]->InitChild();
}
void PuzzleP4Set::SetAction(int x, int y, CCActionInterval* action)
{
    if (object[x][y]->GetLeftUp() != NULL)
        object[x][y]->GetLeftUp()->runAction((CCActionInterval*)action->copy());
    if (object[x][y]->GetRightUp() != NULL)
        object[x][y]->GetRightUp()->runAction((CCActionInterval*)action->copy());
    if (object[x][y]->GetLeftDown() != NULL)
        object[x][y]->GetLeftDown()->runAction((CCActionInterval*)action->copy());
    if (object[x][y]->GetRightDown() != NULL)
        object[x][y]->GetRightDown()->runAction((CCActionInterval*)action->copy());
}
void PuzzleP4Set::StopAllActions(int x, int y)
{
    if (object[x][y]->GetLeftUp() != NULL)
        object[x][y]->GetLeftUp()->stopAllActions();
    if (object[x][y]->GetRightUp() != NULL)
        object[x][y]->GetRightUp()->stopAllActions();
    if (object[x][y]->GetLeftDown() != NULL)
        object[x][y]->GetLeftDown()->stopAllActions();
    if (object[x][y]->GetRightDown() != NULL)
        object[x][y]->GetRightDown()->stopAllActions();
}
PuzzleP4* PuzzleP4Set::GetObject(int x, int y)
{
    return object[x][y];
}
void PuzzleP4Set::SetType(int x, int y, int type)
{
    object[x][y]->SetType(type);
}
void PuzzleP4Set::RemoveAllObjects()
{
    for (int x = 1 ; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            RemoveChild(x, y);
            delete object[x][y];
        }
    }
}
bool PuzzleP4Set::IsRemoved(int x, int y)
{
    return object[x][y]->IsRemoved();
}


