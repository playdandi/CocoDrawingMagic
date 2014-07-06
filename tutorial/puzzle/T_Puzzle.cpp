#include "T_Puzzle.h"

enum
{
	zBackground = 0,
	zGameObject = 100,
    zGameObjectSP = 101,
    zPieceConn = 3,
};

static int ptype[7][7] = {
    -1, 1, 0, 2, 4, 4, -1,
    2, 0, 4, 2, 0, 0, 1,
    0, 1, 4, 1, 4, 0, 1,
    0, 1, 3, 3, 1, 2, 3,
    1, 1, 1, 0, 3, 0, 4,
    2, 1, 2, 4, 4, 1, 2,
    -1, 0, 3, 0, 0, 4, -1};

CCScene* T_Puzzle::scene()
{
	CCScene* pScene = CCScene::create();
    
	T_Puzzle* pLayer = T_Puzzle::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void T_Puzzle::onEnter()
{
    CCLog("T_Puzzle : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
    
    // Fade Out (서서히 밝아진다) + 3/2/1/시작 액션으로 이동
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(T_Puzzle::TutorialStart), this), NULL);
    pBlackOpen->runAction(action);
}
void T_Puzzle::onExit()
{
    CCLog("T_Puzzle : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void T_Puzzle::keyBackClicked()
{
    if (!isInGamePause)
        PauseGame();
}

bool T_Puzzle::init()
{
	if (!CCLayerColor::initWithColor(ccc4(43, 93, 151, 255))) // 파란 배경
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_Puzzle", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(T_Puzzle::Notification), "T_Puzzle", NULL);
    
    
    sound = new Sound();
    sound->PreLoadInGameSound();
    effect = new T_Effect();
    effect->Init(effect, this);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game2.plist");
    // game3.plist는 loading화면에서 preload했음.
    
    
    spriteClassInfo = new SpriteClass();
    spriteClass = new SpriteClass();
    
    puzzleLayer = new CCLayer();
    this->addChild(puzzleLayer, 20);
    puzzleP8set = new T_PuzzleP8Set();
    puzzleP8set->SetGameLayer(this);
    puzzleP8set->SetPuzzleLayer(puzzleLayer);
    puzzleP4set = new T_PuzzleP4Set();
    puzzleP4set->SetGameLayer(this);
    puzzleP4set->SetPuzzleLayer(puzzleLayer);
    
    // skill algorithm
    skill = new T_PuzzleSkill();
    skill->SetGameLayer(this);
    InitSkills();
    
	m_winSize = CCDirector::sharedDirector()->getWinSize();
    srand(time(NULL));
    InitSprites();
    InitCoco();
    InitFairy();
    InitBoard();
    SetScoreAndStarCandy();
    SetTimer();
    InitTutorial();
    
    pBlackOpen = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlackOpen->setAnchorPoint(ccp(0, 0));
    pBlackOpen->setPosition(ccp(0, 0));
    pBlackOpen->setColor(ccc3(0,0,0));
    this->addChild(pBlackOpen, 3000);
    
    isFalling = false;
    isInGamePause = false;
    
    isCancelling = false;
    //m_iSpiritSP = 0;
    
	return true;
}

void T_Puzzle::InitTutorial()
{
    ttrBg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width-250, 150));
    ttrBg->setAnchorPoint(ccp(0.5, 1));
    ttrBg->setPosition(ccp(m_winSize.width/2, vo.y+vs.height-7-120-30));
    ttrBg->setColor(ccc3(0,0,0));
    ttrBg->setOpacity(0);
    this->addChild(ttrBg, 3001);
    
    CCSize size = ttrBg->getContentSize();
    ttrMsg = CCLabelTTF::create("", fontList[0].c_str(), 32, CCSize(700, 150), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    ttrMsg->setAnchorPoint(ccp(0.5, 0.5));
    ttrMsg->setPosition(ccp(size.width/2, size.height/2));
    ttrMsg->setColor(ccc3(255,255,255));
    ttrBg->addChild(ttrMsg, 3002);
    ttrBg->setOpacity(0);
    
    ttrArrow = CCSprite::create("images/tutorial_arrow.png");
    ttrArrow->setRotation(180);
    ttrArrow->retain();
    
    isSkipPossible = false;
    CCLog("is skip possible = %d", CCUserDefault::sharedUserDefault()->getBoolForKey("is_inGameTutorial_seen", false));
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("is_inGameTutorial_seen", false))
    {
        isSkipPossible = true;
        ttrSkip = CCSprite::create("images/tutorial_explain.png");
        ttrSkip->setAnchorPoint(ccp(1, 0));
        ttrSkip->setPosition(ccp(m_winSize.width, vo.y));
        ttrSkip->setScaleX((float)400 / (float)637);
        ttrSkip->setScaleY((float)100 / (float)130);
        this->addChild(ttrSkip, 3001);
    }
}

void T_Puzzle::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        ((T_Puzzle*)Depth::GetCurPointer())->setTouchEnabled(true);
        ((T_Puzzle*)Depth::GetCurPointer())->setKeypadEnabled(true);
        this->setTouchPriority(Depth::GetCurPriority());
        m_bTouchStarted = false;
        CCLog("T_Puzzle : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        //sound->ResumeBackgroundInGameSound();
        sound->ResumeAllEffects();
        /*
        // 정령 다시 움직이기
        if (skill->IsSpiritAlive(0))
            effect->GetSpirit(0)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(1))
            effect->GetSpirit(1)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(2))
            effect->GetSpirit(2)->resumeSchedulerAndActions();
        */
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("T_Puzzle 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        ((T_Puzzle*)depth[depth.size()-2]->GetCurPointer())->setTouchEnabled(false);
        ((T_Puzzle*)depth[depth.size()-2]->GetCurPointer())->setKeypadEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        //sound->StopBackgroundInGameSound();
        sound->ResumeAllEffects();
        
        // 종료하고 Ranking으로 돌아가자.
        CCLog("T_Puzzle 종료. Ranking으로 돌아감.");
        this->TutorialEnd();
    }
}

void T_Puzzle::InitSkills()
{
    // 마법불꽃, 태양열, 붉은용의숨결 (모두 만렙) 을 넣는다.
    std::vector<int> skillNum, skillProb, skillLv;
    
    skillNum.push_back(0);
    skillNum.push_back(1);
    skillNum.push_back(7);
    
    skillProb.push_back(100);
    skillProb.push_back(100);
    skillProb.push_back(100);
    
    skillLv.push_back(5);
    skillLv.push_back(5);
    skillLv.push_back(5);
 
    skill->Init(skillNum, skillProb, skillLv);
}

void T_Puzzle::InitInfoBar()
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
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_gauge.png", ccp(0.5, 0.5), ccp(m_winSize.width-170 - 149.0f/2.0f, vo.y+vs.height-45+2-offset/2-7 ), CCSize(149, 35+offset/2), "", "Layer", gaugeLayer, 7) );
    // 게이지바 clipping하기
    gaugeStencil = CCDrawNode::create();
    CCPoint pos = ccp(m_winSize.width-170 - 149.0f/2.0f, vo.y+vs.height-45+2-offset/2-7 - (35+offset/2)/2.0f );
    CCPoint ver[] = { ccp(pos.x, pos.y), ccp(pos.x+149, pos.y), ccp(pos.x+149, pos.y+35+offset/2), ccp(pos.x, pos.y+35+offset/2) };
    gaugeStencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    gaugeClip = CCClippingNode::create(gaugeStencil);
    gaugeClip->addChild(gaugeLayer);
    this->addChild(gaugeClip, 7);
    
    
    // 게이지바 아이콘(모자)
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_gauge_big.png", ccp(1, 0.5), ccp(m_winSize.width-170-82+20 +5, vo.y+vs.height-47-offset/2-7), CCSize(0, 0), "", "Puzzle", this, 8) );
    
    // pause button
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_pause.png", ccp(1, 0.5), ccp(m_winSize.width-25, vs.height+vo.y-50+5-offset/2-7), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    // 값들 (별사탕 개수)
    spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 40, ccp(1, 0.5), ccp(100+140-20, vo.y+vs.height-47-offset/2+3-7), ccc3(255,255,255), "", "Puzzle", this, 20, 0, 255, 1) ); // font 원래 40
    
    for (int i = 0 ; i < spriteClassInfo->spriteObj.size(); i++)
        spriteClassInfo->AddChild(i);
}

void T_Puzzle::InitSprites()
{
    vs = CCDirector::sharedDirector()->getVisibleSize();
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    
    InitInfoBar();
    
    // timer bar 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png2", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f), CCSize(1044, 63), "", "Puzzle", this, 6) ); // 4
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png3", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1010, 35), "", "Puzzle", this, 6) ); // 4
    
    // 실제 timer bar
    timerLayer = CCLayer::create();
    timerLayer->setAnchorPoint(ccp(0,0));
    timerLayer->setPosition(ccp(0,0));
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_timer.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1000, 22), "", "Layer", timerLayer, 6) ); // 4
    
    tbSize = CCSize(1044, 63+20);
    //tbSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    
    // timer clock icon
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/clock.png", ccp(0.5, 0.5), ccp(m_winSize.width/2 + 1000/2 - 20-17-57/(float)2, vo.y+31.5f+5), CCSize(0,0), "", "Puzzle", this, 1001) );
    pClock = ((CCSprite*)spriteClass->FindSpriteByName("icon/clock.png"));
    
    // timer bar에 clipping node 만들기
    timerStencil = CCDrawNode::create();
    CCPoint pos = ccp(m_winSize.width/2 - 1000/2, vo.y+31.5f+5 - 22/2);
    CCPoint ver[] = { ccp(pos.x, pos.y), ccp(pos.x+1000, pos.y), ccp(pos.x+1000, pos.y+22), ccp(pos.x, pos.y+22) };
    timerStencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    timerClip = CCClippingNode::create(timerStencil);
    timerClip->addChild(timerLayer);
    this->addChild(timerClip, 1000);
    
    // 화면 비율에 맞춰 board size 만들기
    boardSize = CCSize(vs.height*1.920/2.920 - tbSize.height, vs.height*1.920/2.920 - tbSize.height);
    if (boardSize.height >= 1076.0f)
        boardSize = CCSize(1076.0f, 1076.0f);
    puzzleLayer->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height/2));
    
    // puzzle board
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0.5, 0.5), ccp(0, 0), CCSize(0, 0), "", "Layer", puzzleLayer, 20) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->setScale((float)boardSize.height/(float)1076);
    
    // 화면 비율에 맞춰 piece 1개의 size 지정하기
    PIECE8_WIDTH = (float)152 * (float)boardSize.height/(float)1076;
    PIECE8_HEIGHT = (float)152 * (float)boardSize.height/(float)1076;
    PIECE8_FRAME_WIDTH = (float)154 * (float)boardSize.height/(float)1076;
    PIECE8_FRAME_HEIGHT = (float)154 * (float)boardSize.height/(float)1076;
    CCLog("%f %f", PIECE8_WIDTH, PIECE8_HEIGHT);
    
    // 구름
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_cloud_near.png", ccp(1, 0), ccp(m_winSize.width, vo.y+vs.height*1.920f/2.920f+350), CCSize(0, 0), "", "Puzzle", this, 1) );
    // 배경 그림
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_upper.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height-30), CCSize(0, 0), "", "Puzzle", this, 0) );
    
    // 퍼즐판 뒤 잔디
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_grass.png", ccp(0, 1), ccp(0, vo.y+tbSize.height+boardSize.height+80), CCSize(0, 0), "", "Puzzle", this, 5) ); // 2
    // 길바닥
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_floor.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height+15), CCSize(0, 0), "", "Puzzle", this, 4) );
    floorSize = ((CCSprite*)spriteClass->FindSpriteByName("bg_floor.png"))->getContentSize();
    // 길바닥 위 작은 잔디
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_grass_mini.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height-5), CCSize(0, 0), "", "Puzzle", this, 3) );
    // 산
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_mountain.png", ccp(1, 0), ccp(m_winSize.width, vo.y+tbSize.height+boardSize.height+floorSize.height), CCSize(0, 0), "", "Puzzle", this, 2) );
    // 성
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_castle.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height+floorSize.height-35), CCSize(0, 0), "", "Puzzle", this, 2) );
    
    // 코코 그림자, 요정 그림자
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/coco_shadow.png", ccp(0.5, 0.5), ccp(215, vo.y+tbSize.height+boardSize.height+95), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "sun/sun_shadow.png", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+tbSize.height+boardSize.height+95), CCSize(0, 0), "", "Puzzle", this, 20) );
    ((CCSprite*)spriteClass->FindSpriteByName("sun/sun_shadow.png"))->setScale(1.25f);
    
    // 마법진
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/magic_circle.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120), CCSize(0, 0), "", "Puzzle", this, 4) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/magic_circle.png"))->setScale(0.9f);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    // 잔디 액션
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +3)), CCMoveBy::create(1.0f, ccp(0, -3)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    ((CCSprite*)spriteClass->FindSpriteByName("bg_grass.png"))->runAction(rep);
}

void T_Puzzle::InitCoco()
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

void T_Puzzle::CocoAnim(float f)
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
        
        this->unschedule(schedule_selector(T_Puzzle::CocoAnim));
    }
    
    cocoFrameNumber++;
}

void T_Puzzle::InitFairy()
{
    fairyLayer = CCLayer::create();
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("sun/sun_face.png");
    face->setAnchorPoint(ccp(0.5, 0.5));
    face->setPosition(ccp(0, 0));
    fairyLayer->addChild(face, 1000);
    CCSprite* mouth = CCSprite::createWithSpriteFrameName("sun/sun_mouth.png");
    mouth->setAnchorPoint(ccp(0.5, 0.5));
    mouth->setPosition(ccp(0, -30));
    fairyLayer->addChild(mouth, 1000);
    
    CCLayer* eyeLayer = CCLayer::create();
    CCSprite* eye1 = CCSprite::createWithSpriteFrameName("sun/sun_eye.png");
    eye1->setAnchorPoint(ccp(0.5, 0.5));
    eye1->setPosition(ccp(0, 0));
    eyeLayer->addChild(eye1, 1000);
    CCSprite* eye2 = CCSprite::createWithSpriteFrameName("sun/sun_eye.png");
    eye2->setAnchorPoint(ccp(0.5, 0.5));
    eye2->setPosition(ccp(70, 0));
    eyeLayer->addChild(eye2, 1000);
    eyeLayer->setPosition(ccp(-50, 0));
    eyeLayer->setTag(0);
    fairyLayer->addChild(eyeLayer, 1000);
    
    fairy_sp.clear();
    fairy_sp.push_back(face);
    fairy_sp.push_back(mouth);
    fairy_sp.push_back(eye1);
    fairy_sp.push_back(eye2);
    
    float r = 110.0;
    float x, y;
    for (int i = 0 ; i < 8 ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("sun/sun_head.png");
        temp->setAnchorPoint(ccp(0.5, 0.5));
        if (i == 0)      { x = 0; y = r; }
        else if (i == 1) { x = r/sqrt(2); y = r/sqrt(2); }
        else if (i == 2) { x = r; y = 0; }
        else if (i == 3) { x = r/sqrt(2); y = -r/sqrt(2); }
        else if (i == 4) { x = 0; y = -r; }
        else if (i == 5) { x = -r/sqrt(2); y = -r/sqrt(2); }
        else if (i == 6) { x = -r; y = 0; }
        else if (i == 7) { x = -r/sqrt(2); y = r/sqrt(2); }
        temp->setPosition(ccp(x, y));
        temp->setRotation(45*i);
        fairy_sp.push_back(temp);
        fairyLayer->addChild(temp, 1000);
    }
    
    fairyLayer->setScale(0.8f);
    fairyLayer->setPosition(ccp(m_winSize.width-280, vo.y+tbSize.height+boardSize.height+60));
    this->addChild(fairyLayer, 100);
    
    // action
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, +5)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    fairyLayer->runAction(rep);
}

void T_Puzzle::ChangeAnimFairy(float f)
{
    
}

void T_Puzzle::InitBoard()
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
            
            puzzleP8set->CreatePiece(x, y, ptype[y][x]);
            spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
            spriteP8[x][y]->setColor(ccc3(180,180,180));
            puzzleP8set->AddChild(x, y);
		}
	}
    
    // 4각형
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            puzzleP4set->CreatePiece(x, y);
            puzzleP4set->AddChild(x, y);
            puzzleP4set->SetOpacity(x, y, 180);
        }
    }
    
    
    // 4각형, 8각형의 lock(semaphore)을 위한 것들
    for (int x = 0 ; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            m_bLockP8[x][y] = 1; // 처음에 락 걸어두기

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
        
        // skill state 초기화
        m_iState[i] = -1;
        m_iNextState[i] = -1;
        // cycle
        m_bIsCycle[i] = false;
        // skill lock
        m_bSkillLock[i] = false;
    }
    
    touch_cnt = 0;
    
    m_bTouchStarted = false;
    m_iSkillSP = 0;
    //m_bIsSpiritExecuted = false;
    //m_iSpiritSP = 0;
}

void T_Puzzle::SetScoreAndStarCandy()
{
    iScore = 0;
    char n[10];
    sprintf(n, "%d", iScore);
    pScoreLayer = Common::MakeScoreLayer(iScore);
    CCSize s = pScoreLayer->getContentSize();
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93-7));
    this->addChild(pScoreLayer, 6);
    
    iStarCandy = 0;
    pStarCandyLabel = (CCLabelTTF*)spriteClassInfo->FindLabelByTag(1);
}

void T_Puzzle::UpdateScore(int type, int data)
{
    //CCLog("before : %d %d", iScore, data);
    if (type == 0)
    {
        if (data < 3)
            iScore += data * 1000;
        else
            // 기본점수 : (50 + 15(n-3)^1.2) * n
            iScore += ( (50 + 15*pow(data-3, 1.20)) * data );
    }
    else if (type == 1)
    {
        // 기본 스킬에 의한 추가점수 (A1)
        iScore += data;
    }
    //CCLog("after : %d", iScore);
    
    pScoreLayer->removeAllChildren();
    pScoreLayer->removeFromParentAndCleanup(true);
    
    char n[12];
    sprintf(n, "%d", iScore);
    pScoreLayer = Common::MakeScoreLayer(iScore);
    CCSize s = pScoreLayer->getContentSize();
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93-7));
    this->addChild(pScoreLayer, 6);
}

void T_Puzzle::UpdateStarCandy(int type, int data)
{
    if (type == 0)
    {
        if (data <= 3)
            return;
        iStarCandy += ((data-3)*(data-4)/2 + 1);
    }
    else if (type == 1)
    {
        iStarCandy += data;
    }
    char s[5];
    sprintf(s, "%d", iStarCandy);
    pStarCandyLabel->setString(s);
}

void T_Puzzle::SetTimer()
{
    CCScale9Sprite* timerbar = ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_timer.png"));
    CCPoint p = timerbar->getPosition();
    CCSize s = timerbar->getContentSize();
    
    iTimer = 1000 * PUZZLE_TIME;
}

void T_Puzzle::TutorialStart(CCNode* sender, void* pointer)
{
    CCUserDefault::sharedUserDefault()->setBoolForKey("is_inGameTutorial_seen", true);
    
    sender->removeFromParentAndCleanup(true);
    T_Puzzle* pThis = (T_Puzzle*)pointer;
    
    ttrBg->setOpacity(255);
    ttrMsg->setOpacity(255);
    
    pThis->ttrState = -1;
    pThis->TutorialNextState();
}

void T_Puzzle::TutorialNextState()
{
    ttrState++;
    
    CCLog("인게임 튜토리얼 이번 상태 : %d", ttrState);
    switch (ttrState)
    {
        case 0:
            ttrMsg->setString("안녕, 난 코코라고 해. 넌 누구니? 이상하게 생겼네 ㅋ");
            break;
        case 1:
            ttrMsg->setString("너라니! 난 너의 마법 선생님이라구! 지금은 이상한 마법에 걸려서 이렇게 됐지만...");
            break;
        case 2:
            ttrMsg->setString("난 마법을 배우고 싶어서 여기 왔는데...");
            break;
        case 3:
            ttrMsg->setString("괜찮아. 비록 나에게 걸린 마법을 풀 수 없지만, 마법을 가르쳐 줄 수는 있지!");
            break;
        case 4:
            ttrMsg->setString("정말? 와~ 다행이다. 어떻게 해야 해?");
            break;
        case 5:
            ttrMsg->setString("먼저 피스를 그리는 방법을 보여줄게.");
            break;
        case 6: // 피스 그리기
            spriteP8[2][3]->setColor(ccc3(255,255,255));
            spriteP8[3][3]->setColor(ccc3(255,255,255));
            spriteP8[4][4]->setColor(ccc3(255,255,255));
            puzzleP4set->SetOpacity(4, 4, 255);
            UnLockEach(2, 3);
            UnLockEach(3, 3);
            UnLockEach(4, 4);
            break;
        case 7:
            ttrMsg->setString("같은 색깔 피스를 3개 이상 따라 그리면 피스를 터뜨릴 수 있어.");
            break;
        case 8:
            ttrMsg->setString("아하, 그렇구나!");
            break;
        case 9:
            ttrMsg->setString("나는 불 속성 마법사라서 붉은 피스를 그리면 마법을 시전할 수 있어!");
            break;
        case 10:
            ttrMsg->setString("붉은색 피스를 따라 그려봐.");
            break;
        case 11: // 붉은색 기본 마법 위한 한붓그리기
            spriteP8[3][5]->setColor(ccc3(255,255,255));
            spriteP8[4][5]->setColor(ccc3(255,255,255));
            spriteP8[5][4]->setColor(ccc3(255,255,255));
            puzzleP4set->SetOpacity(5, 5, 255);
            UnLockEach(3, 5);
            UnLockEach(4, 5);
            UnLockEach(5, 4);
            break;
        case 12:
            ttrMsg->setString("잘했어! 마법을 사용해서 추가점수를 얻었어!");
            break;
        case 13:
            char temp[150];
            if (myInfo->IsFire())
                sprintf(temp, "그렇구나! 나는 불 속성 마법사니까, 너처럼 붉은색 피스를 없애면 추가점수를 얻을 수 있겠구나!");
            else if (myInfo->IsWater())
                sprintf(temp, "그렇구나! 나는 물 속성 마법사니까, 푸른색 피스를 없애면 추가점수를 얻을 수 있겠구나!");
            else if (myInfo->IsLand())
                sprintf(temp, "그렇구나! 나는 땅 속성 마법사니까, 초록색 피스를 없애면 추가점수를 얻을 수 있겠구나!");
            ttrMsg->setString(temp);
            break;
        case 14:
            ttrMsg->setString("너 생각보다 똑똑하구나? 좋아! 그럼 마법을 하나 더 보여줄게.");
            break;
        case 15:
            ttrMsg->setString("아래에 보이는 붉은 피스를 처음과 끝이 연결되도록, 삼각형으로 그려봐!");
            break;
        case 16: // 사이클 한붓그리기
            spriteP8[4][1]->setColor(ccc3(255,255,255));
            spriteP8[5][1]->setColor(ccc3(255,255,255));
            spriteP8[5][2]->setColor(ccc3(255,255,255));
            puzzleP4set->SetOpacity(5, 2, 255);
            UnLockEach(4, 1);
            UnLockEach(5, 1);
            UnLockEach(5, 2);
            break;
        case 17:
            ttrMsg->setString("우와~ 나도 이런 마법을 쓸 수 있어?");
            break;
        case 18:
            ttrMsg->setString("하하, 물론이지. 이것 말고도 세상에는 숨겨진 마법이 무궁무진하다구!");
            break;
        case 19:
            ttrMsg->setString("이야, 신난다!");
            break;
        case 20:
            ttrMsg->setString("그럼 나는 내게 걸린 저주를 푸는 마법을 연구하러 가야 해. 나머지 마법은 스스로 찾아봐!");
            break;
        case 21:
            ttrMsg->setString("마지막으로 화려한 마법을 보여주고 갈게. 그럼 안녕!");
            break;
        case 22: // 붉은 용의 숨결 시전
            ttrMsg->removeFromParentAndCleanup(true);
            ttrBg->removeFromParentAndCleanup(true);
            ReverseColor();
            skill->Invoke(7, touch_cnt%QUEUE_CNT);
            break;
        case 23: // 튜토리얼 종료. 실제 게임 시작.
            EndScene();
            break;
    }
}


void T_Puzzle::ReverseColor()
{
	for (int x = 0 ; x < COLUMN_COUNT ; x++)
	{
		for (int y = 0 ; y < ROW_COUNT ; y++)
		{
            if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) ||
                (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                continue;
            
            spriteP8[x][y]->setColor(ccc3(255,255,255));
		}
	}
    
    for (int x = 1; x < COLUMN_COUNT ; x++)
        for (int y = 1 ; y < ROW_COUNT ; y++)
            puzzleP4set->SetOpacity(x, y, 255);
}


// 터치에 대한 좌표 계산 (보드판 [x][y]의 중심 좌표(실제 좌표시스템의 좌표)를 리턴해줌.
CCPoint T_Puzzle::SetTouch8Position(int x, int y)
{
    CCPoint relative = SetPiece8Position(x, y);
    CCPoint layer = puzzleLayer->getPosition();
    return ccp(relative.x + layer.x, relative.y + layer.y);
}
// 터치에 대한 좌표 계산 (보드판 [x][y]의 중심 좌표(실제 좌표시스템의 좌표)를 리턴해줌.
CCPoint T_Puzzle::SetTouch4Position(int x, int y)
{
    CCPoint relative = SetPiece4Position(x, y);
    CCPoint layer = puzzleLayer->getPosition();
    return ccp(relative.x + layer.x, relative.y + layer.y);
}

CCPoint T_Puzzle::SetPiece8Position(int x, int y)
{
    int posX = (x-3)*PIECE8_WIDTH;
    int posY = (y-3)*PIECE8_HEIGHT;
    return ccp(posX, posY);
}
CCPoint T_Puzzle::SetPiece4Position(int x, int y)
{
    float posX = (float)(x-3)*PIECE8_WIDTH - PIECE8_WIDTH/2.0f;
    float posY = (float)(y-3)*PIECE8_HEIGHT - PIECE8_HEIGHT/2.0f;
    return ccp(posX, posY);
}

bool T_Puzzle::IsValidInSquare(CCPoint center, CCPoint point)
{
    // 처음 터치할 때만 보는 것이므로, 완전한 정4각형 범위까지 허용한다.
    //return abs(center.x-point.x)+abs(center.y-point.y) < PIECE8_WIDTH-1;
    if ((center.x-PIECE8_WIDTH/2 < point.x && point.x < center.x+PIECE8_WIDTH/2) &&
        (center.y-PIECE8_WIDTH/2 < point.y && point.y < center.y+PIECE8_WIDTH/2)) {
        return true;
    }
    return false;
}

CCPoint T_Puzzle::BoardStartPosition(CCPoint point)
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


int T_Puzzle::GetDist(CCPoint center, CCPoint point)
{
    int dist = (center.x-point.x)*(center.x-point.x) + (center.y-point.y)*(center.y-point.y);
    if (dist > PIECE8_WIDTH*PIECE8_WIDTH + (PIECE8_HEIGHT/2)*(PIECE8_HEIGHT/2))
        return 999999999;
    return dist;
}

CCPoint T_Puzzle::BoardMovePosition(CCPoint point)
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
            
            int d = GetDist(SetTouch8Position(x, y), point);
            if ( d < dist && (globalType[touch_cnt%QUEUE_CNT] == puzzleP8set->GetType(x, y)) )
            {
                dist = d;
                resultXY = ccp(x, y);
            }
		}
	}
    return resultXY;
}


void T_Puzzle::PauseGame()
{
    if (isInGamePause)
        return;
    isInGamePause = true;
    
    sound->PauseBackgroundInGameSound();
    
    CancelDrawing();
    
    // 정령 멈추기
    /*
    if (skill->IsSpiritAlive(0))
        effect->GetSpirit(0)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(1))
        effect->GetSpirit(1)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(2))
        effect->GetSpirit(2)->pauseSchedulerAndActions();
    */
    
    Common::ShowNextScene(this, "T_Puzzle", "T_PuzzlePause", false, vo.y+tbSize.height+boardSize.height+60);
}
void T_Puzzle::SkipGame()
{
    CancelDrawing();
    
    Common::ShowNextScene(this, "T_Puzzle", "T_Skip", false, vo.y+tbSize.height+boardSize.height+60);
}

void T_Puzzle::StopAllActionsAtPieces()
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
}

bool T_Puzzle::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    /*
    // 보드판과 상관없는 것들 (lock에 제한받지 않는 것들)
    // 1) pause button
    CCRect rect = ((CCSprite*)spriteClassInfo->FindSpriteByName("icon/icon_pause.png"))->boundingBox();
    rect.setRect(rect.getMinX()-50, rect.getMinY()-50, rect.getMaxX()-rect.getMinX()+100, rect.getMaxY()-rect.getMinY()+100);
    if (rect.containsPoint(point))
    {
        PauseGame();
        return true;
    }
    */
    // 2) 튜토리얼 건너뛰기
    if (isSkipPossible)
    {
        if (ttrSkip->boundingBox().containsPoint(point))
        {
            SkipGame();
            return false;
        }
    }
    
    
    CCLog("ttrState = %d", ttrState);
    CCLog("touchBegan (%d) : %d %d", touch_cnt%QUEUE_CNT, m_iSkillSP, m_bTouchStarted);
    
    // 아래의 튜토리얼 상태일 때는 대화만 넘긴다.
    if (ttrState <= 5 || (ttrState >= 7 && ttrState <= 10) || (ttrState >= 12 && ttrState <= 15) || (ttrState >= 17 && ttrState <= 21) )
    {
        TutorialNextState();
        return false;
    }
    
    // 스킬 발동 중 or touchEnd가 되기 전까지 or 정령스킬 실행 중이면 터치 금지
    if (m_iSkillSP > 0 || m_bTouchStarted)
        return false;
    
    m_bTouchStarted = true; // 터치 시작 lock 걸기
    
    
    // 어느 퍼즐인지 찾기
    CCPoint boardPos = BoardStartPosition(point);
    int x = (int)boardPos.x;
    int y = (int)boardPos.y;
    
    if (x == -1 && y == -1) // 찾지 못했다면 퍼즐 피스를 클릭한 것이 아니므로 그냥 종료.
        return (m_bTouchStarted = false);
    
    if (m_bLockP8[x][y] > 0) // 터치한 locked면 중지.
        return (m_bTouchStarted = false);
    
    /*
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
            
            m_bTouchStarted = false;
            
            // semaphore 1 증가 + 실행상태 lock 걸기
            m_iSpiritSP++;
            m_bIsSpiritExecuted = true;
            
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
     */
    
    m_bIsCycle[touch_cnt%QUEUE_CNT] = false;
    m_bLockP8[x][y]++; // lock 걸기
    
    // 벡터 초기화
    piece8xy[touch_cnt%QUEUE_CNT].clear();
    piece4xy[touch_cnt%QUEUE_CNT].clear();
    strap[touch_cnt%QUEUE_CNT].clear();
    
    // 8각형 piece를 터뜨릴 목록에 추가
    piece8xy[touch_cnt%QUEUE_CNT].push_back(ccp(x, y));
    
    // 마법띠 동그라미 처음에 띄우기
    CCSprite* sp = CCSprite::createWithSpriteFrameName("pieces/strap_connector.png");
    sp->setPosition(SetTouch8Position(x, y));
    this->addChild(sp, 1000);
    strap[touch_cnt%QUEUE_CNT].push_back(sp);
    
    CCLog ("touch began (%d) : %d %d", touch_cnt%QUEUE_CNT, x, y);
    // global piece type 지정
    globalType[touch_cnt%QUEUE_CNT] = puzzleP8set->GetType(x, y);
    
    // 0.9배 축소 action
    spriteP8[x][y]->setScale(spriteP8[x][y]->getScale() * 0.9f);
    spriteP8[x][y]->setColor(ccc3(140,140,140));
    
    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size());
    
    return true;
}

void T_Puzzle::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (m_iSkillSP > 0)
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
                if (m_bIsCycle[touch_cnt%QUEUE_CNT])
                {
                    // cycle을 표시해주는 액션이 들어간다.
                    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size()+1);
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

void T_Puzzle::CheckUselessDiaPieces()
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

void T_Puzzle::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (m_iSkillSP > 0)
        return;
    
    if (m_bTouchStarted)
    {
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
            // 사이클 튜토리얼 시 반드시 사이클을 만들어야 한다.
            if (ttrState == 16)
            {
                if (!m_bIsCycle[touch_cnt%QUEUE_CNT])
                {
                    CancelDrawing();
                    return;
                }
            }
            // 일단 정령 스킬부터 semaphore 증가.
            // m_iSpiritSP++;
            
            // 다시 락 걸어둔다.
            for (int x = 0 ; x < COLUMN_COUNT ; x++)
                for (int y = 0 ; y < ROW_COUNT ; y++)
                    m_bLockP8[x][y] = 1;
            
            // 다시 한붓그린 부분 lock을 푼다
            int x, y;
            for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                x = piece8xy[touch_cnt%QUEUE_CNT][i].x;
                y = piece8xy[touch_cnt%QUEUE_CNT][i].y;
               // m_bLockP8[x][y]--;
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
            
            
            // 스킬 발동 try (발동되는 스킬들 조사)
            // 여기서 스킬에 따라 skill Lock이 걸릴 수 있다. (그러면 스킬발동 종료까지 터치 아예 못함)
            m_bSkillLock[touch_cnt%QUEUE_CNT] = false;
            skill->TrySkills(globalType[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
            if (m_bSkillLock[touch_cnt%QUEUE_CNT])
            {
                m_iSkillSP++; // skill semaphore 증가
                //CCLog("스킬 lock 걸림 (%d)", touch_cnt%QUEUE_CNT);
            }
            else
                //CCLog("스킬 발동 X (%d)", touch_cnt%QUEUE_CNT);
            
            m_iBombCallbackType[touch_cnt%QUEUE_CNT] = 0;
            
            touch_cnt++;
            
            // 터치 lock을 푼다. (먼저 만든 한붓그리기가 터지고 새로운 피스가 완전히 falling하기 전에 새로운 한붓그리기 가능하게 하기 위하여)
            m_bTouchStarted = false;
            
            // effect 체크 초기화
            effect->InitCheck();
            
            // 스킬 실행 (오토마타 표 참조)
            iTouchRound++;
            skill->SetQueuePos((touch_cnt-1)%QUEUE_CNT);
            
            m_iState[(touch_cnt-1)%QUEUE_CNT] = SKILL_BASIC;
            InvokeSkills((touch_cnt-1)%QUEUE_CNT);
            
            m_bIsCycle[(touch_cnt-1)%QUEUE_CNT] = false; // cycle 푼다.
        }
        
        // 3개 미만으로 한붓그리기가 되었다면, 원상태로 복구시킨다.
        else
        {
            CancelDrawing();
        }
    }
}

void T_Puzzle::CancelDrawing()
{
    if (isCancelling)
        return;
    isCancelling = true;
    
    int x, y;
    for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
    {
        x = (int)piece8xy[touch_cnt%QUEUE_CNT][i].x;
        y = (int)piece8xy[touch_cnt%QUEUE_CNT][i].y;
        spriteP8[x][y]->setScale(GetBoardSize()/(float)1076);
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



void T_Puzzle::InvokeSkills(int queue_pos)
{
    if (m_iState[queue_pos] == SKILL_BASIC)
    {
        CCLog("state(%d) BASIC", queue_pos);
        if (skill->IsApplied(1, queue_pos) || skill->IsApplied(9, queue_pos) || skill->IsApplied(17, queue_pos))
        {
            m_iNextState[queue_pos] = SKILL_CYCLE;
        }
        else
        {
            m_iNextState[queue_pos] = SKILL_DONE;
        }
        
        Lock(queue_pos);
        
        skill->Invoke(0, queue_pos);
        
        Bomb(queue_pos, piece8xy[queue_pos]);
    }
    else if (m_iState[queue_pos] == SKILL_CYCLE) // 사이클 주변부 터뜨리기
    {
        CCLog("state(%d) CYCLE", queue_pos);
        
        m_iNextState[queue_pos] = SKILL_DONE;
        skill->Invoke(1, queue_pos);// skill->Invoke(9, queue_pos); skill->Invoke(17, queue_pos);
    }
    /*
    else if (m_iState[queue_pos] == SKILL_FINAL) // 마지막 (속성별 8번) 스킬
    {
        CCLog("state(%d) FINAL", queue_pos);
        m_iNextState[queue_pos] = SKILL_DONE;

        skill->Invoke(7, queue_pos);
 
        if (!skill->IsApplied(7, queue_pos))
        {
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    */
    
    else if (m_iState[queue_pos] == SKILL_DONE)
    {
        CCLog("state(%d) DONE!", queue_pos);
      
        // skill Lock을 푼다.
        if (m_bSkillLock[queue_pos])
        {
            SetSkillLock(queue_pos, false);
            m_iSkillSP--;
        }
        
        //m_iSpiritSP--;
        iTouchRound--;
        
        // 튜토리얼 넘기기
        TutorialNextState();
    }
}


void T_Puzzle::Lock(int queue_pos)
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
    for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
    {
        CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
              m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
    }
}
void T_Puzzle::LockEach(int x, int y)
{
    m_bLockP8[x][y]++;
}
void T_Puzzle::UnLockEach(int x, int y)
{
    m_bLockP8[x][y]--;
}

void T_Puzzle::Bomb(int queue_pos, std::vector<CCPoint> bomb_pos)
{
    CCLog("Bomb(%d) start : size = %d", queue_pos, (int)bomb_pos.size());
    
    int x, y;
    
    // 연결피스를 삭제한다. (단, 연결피스 기준으로 두 대각선 중 한 쌍의 피스가 터지면 없애도록 한다)
    // 1) 변수 초기화
    for (int x = 0; x < COLUMN_COUNT ; x++)
        for (int y = 0 ; y < ROW_COUNT ; y++)
            P8Bombed[x][y] = false;
    P8Bombed[0][0] = P8Bombed[0][ROW_COUNT-1] = P8Bombed[COLUMN_COUNT-1][0] = P8Bombed[COLUMN_COUNT-1][ROW_COUNT-1] = true;
    for (int i = 0 ; i < bomb_pos.size() ; i++)
        P8Bombed[(int)bomb_pos[i].x][(int)bomb_pos[i].y] = true;
    // 2) 검사
    for (int x = 1; x < COLUMN_COUNT ; x++)
    {
        for (int y = 1 ; y < ROW_COUNT ; y++)
        {
            if ( (P8Bombed[x-1][y] && P8Bombed[x][y-1]) || (P8Bombed[x-1][y-1] && P8Bombed[x][y]) )
                puzzleP4set->RemoveChild(x, y);
        }
    }
    
    // sound bomb
    if (m_iState[queue_pos] == SKILL_BASIC && skill->IsApplied(0, queue_pos) && globalType[queue_pos] == PIECE_RED)
        sound->PlaySkillSound(0); //sound->PlayVoice(VOICE_EIT2);
    //else if (F8_idx != -1)
    //    ;
    else
        sound->PlayBomb();
    
    
    // 기본 이펙트 적용
    effect->PlayEffect_Default(bomb_pos);
    
    // 8각형들을 터뜨린다.
    float delayTime = 0.0f;
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax[queue_pos] = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;
        
        float bombTime = 0.15f;
        if (m_iState[queue_pos] == SKILL_CYCLE && (globalType[queue_pos] == PIECE_RED || globalType[queue_pos] == PIECE_BLUE))
            bombTime = 0.60f;
        //else if (F8_idx != -1)
        //    bombTime = 0.05f;
        
        // 그 다음이 사이클 주변부 터지는 스킬이면 조금 딜레이준다.
        if (m_iNextState[queue_pos] == SKILL_CYCLE)
            delayTime = 0.1f;
        
        // 터지는 액션 (BOMB!)
        CCFiniteTimeAction* action;
        action = CCSequence::create(
                            CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                            CCDelayTime::create(delayTime), // 그 다음이 사이클 주변부 터지는 스킬이면 조금 딜레이준다.
                            CCCallFuncND::create(this, callfuncND_selector(T_Puzzle::BombCallback), (void*)queue_pos),
                            NULL);
        
        //if (F8_idx != -1)
        //    spriteP8[x][y]->setTag(F8_idx);
        spriteP8[x][y]->runAction(action);
    }
    
    // update score
    UpdateScore(0, bomb_pos.size());
    
    // update starcandy
    UpdateStarCandy(0, bomb_pos.size());
}

void T_Puzzle::BombCallback(CCNode* sender, void* queue_pos)
{
    m_iBombCallbackCnt[(int)queue_pos]++;
    
    if (m_iBombCallbackCnt[(int)queue_pos] == m_iBombCallbackCntMax[(int)queue_pos])
    {
        if (m_iState[(int)queue_pos] == SKILL_CYCLE) // cycle 주변부 폭발 완료
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
            
            if (m_iNextState[(int)queue_pos] == SKILL_CYCLE) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
            {
                m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                
                InvokeSkills((int)queue_pos);
                piece8xy[(int)queue_pos].clear();
                
                return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
            }
            else
                piece8xy[(int)queue_pos].clear();
        }
        else if (m_iState[(int)queue_pos] == SKILL_FINAL) // 8번 스킬 폭발 완료
        {
            CCLog("bomb callback (%d) FINAL", (int)queue_pos);
            
            std::vector<CCPoint> temp = effect->GetDoublePos(sender->getTag());
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
        }
        
        // falling queue insertion
        fallingQueue.push((int)queue_pos);
        if (!isFalling)
            FallingProcess();
    }
}

void T_Puzzle::FallingProcess()
{
    if (fallingQueue.size() > 0)
        isFalling = true;
    else
    {
        isFalling = false;
        return;
    }
    
    int queue_pos = fallingQueue.front();
    fallingQueue.pop();
    Falling(queue_pos);
}

void T_Puzzle::Falling(int queue_pos)
{
    CCLog("Falling 들어옴 : queue_pos %d", queue_pos);
    
    // get the number of total falling objects.
    m_iFallingCallbackCnt = 0;
    m_numOfFallingObjects = 0;
    
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
                if (x == 0 || x == COLUMN_COUNT-1)
                {
                    m_numOfFallingObjects--;
                }
                break;
            }
        }
    }
    
    int type;
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
                    
                    type = -1;
                    if (ttrState == 6)
                    {
                        type = rand() % TYPE_COUNT;
                        if (type == 0) type = 2;
                    }
                    //else if (ttrState < 22)
                    //    type = PIECE_RED;
                        
                    puzzleP8set->CreatePiece(x, y, type);
                    puzzleP8set->GetObject(x, y)->SetPosition(SetPiece8Position(x, end));
                    puzzleP8set->AddChild(x, y);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    if (ttrState <= 21)
                        spriteP8[x][y]->setColor(ccc3(180,180,180));
                    puzzleP8set->Falling(x, y, x, y, queue_pos);
                }
                else
                {
                    // 떨어지는 경우
                    puzzleP8set->MoveObject(x, y, x, pos);
                    spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
                    spriteP8[x][pos] = NULL;
                    puzzleP8set->Falling(x, y, x, y, queue_pos);
                }
			}
		}
	}
}

void T_Puzzle::FallingCallback(CCNode* sender, void* queue_pos)
{
    int queue = (int)queue_pos;
    m_iFallingCallbackCnt++;
    
    if (m_numOfFallingObjects == m_iFallingCallbackCnt)
	{
        // drop이 모두 끝나면, diamond들을 다시 검사해서 적절히 바꿔준다.
		for (int x = 1 ; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (puzzleP4set->GetType(x, y) != BLOCKED)
                {
                    if (puzzleP4set->GetObject(x, y) != NULL)
                        puzzleP4set->RemoveChild(x, y);
                    puzzleP4set->CreatePiece(x, y, puzzleP4set->GetType(x, y));
                    puzzleP4set->AddChild(x, y);
                    puzzleP4set->SetOpacity(x, y, 180);
                }
            }
        }
        
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
        /*
         for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
         {
         CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
         m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
         }
         */
        /*****************************************/
        
        
        //if (m_iState[(int)queue_pos] == SKILL_FINAL && globalType[int(queue_pos)] == PIECE_RED)
        if (ttrState == 22)
        {
            // '붉은 용의 숨결' 발동 중이면, 혜성이 다 떨어졌을 때만 끝내도록 한다.
            skill->F8_FinishCountUp();
            if (skill->F8_IsFinished())
            {
                effect->Effect7_Clear();
                //GoNextState((int)queue_pos);
                TutorialEnd();
            }
            else
            {
                skill->F8_Bomb_Real();
            }
            //FallingProcess();
        }
        else
        {
            FallingProcess();
            GoNextState((int)queue_pos);
        }
    }
}

void T_Puzzle::GoNextState(int queue_pos)
{
    // 피스 갱신으로 인해 실행된 경우는 무시하자.
    if (m_iState[queue_pos] == SKILL_DONE)
        return;
    
    // 다음 스킬로 넘어간다.
    m_iState[queue_pos] = m_iNextState[queue_pos];
    //CCLog("Falling callback (%d) - 다음 스킬 [일반] : %d", queue_pos, m_iState[queue_pos]);
    InvokeSkills(queue_pos);
}

void T_Puzzle::FallingQueuePushAndFalling(int queue_pos)
{
    // falling queue insertion
    fallingQueue.push(queue_pos);
    //CCLog("FallingQueuePushAndFalling : %d", isFalling);
    if (!isFalling)
        FallingProcess();
}



T_PuzzleSkill* T_Puzzle::GetSkill()
{
    return skill;
}
void T_Puzzle::SetPiece8xy(int queue_pos, std::vector<CCPoint> pos)
{
    piece8xy[queue_pos] = pos;
}
std::vector<CCPoint> T_Puzzle::GetPiece8xy(bool afterCast)
{
    if (afterCast)
        return piece8xy[(touch_cnt-1)%QUEUE_CNT];
    return piece8xy[touch_cnt%QUEUE_CNT];
}

int T_Puzzle::GetGlobalType()
{
    return globalType[(touch_cnt-1)%QUEUE_CNT];
}

void T_Puzzle::SetSkillLock(int queue_pos, bool flag)
{
    m_bSkillLock[queue_pos] = flag;
}

bool T_Puzzle::IsCycle(int queue_pos)
{
    return m_bIsCycle[queue_pos];
}

Sound* T_Puzzle::GetSound()
{
    return sound;
}

CCLayer* T_Puzzle::GetPuzzleLayer()
{
    return puzzleLayer;
}
CCLayer* T_Puzzle::GetCocoLayer()
{
    return cocoLayer;
}
CCLayer* T_Puzzle::GetFairyLayer()
{
    return fairyLayer;
}

bool T_Puzzle::IsPaused()
{
    return isInGamePause;
}

void T_Puzzle::SkillSemaphoreUpdate(int val)
{
    m_iSkillSP += val;
}
bool T_Puzzle::IsSkillSemaphoreFree()
{
    return m_iSkillSP == 0;
}


void T_Puzzle::TutorialEnd()
{
    pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlackClose->setPosition(ccp(0, 0));
    pBlackClose->setAnchorPoint(ccp(0, 0));
    pBlackClose->setColor(ccc3(0, 0, 0));
    pBlackClose->setOpacity(0);
    this->addChild(pBlackClose, 7000);
    
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(T_Puzzle::EndScene), this), NULL);
    pBlackClose->runAction(action);
}

void T_Puzzle::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Puzzle");
    
    // layer 배경색을 검은색으로 바꾼다.
    this->setColor(ccc3(0,0,0));
    
    // 일시정지 화면에서 바로 종료하는 상황일 경우
    isInGamePause = false;
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->stopAllActions();
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game2.plist");
    //CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game3.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game2.png");
    //CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game3.png");
    
    // delete all objects
    effect->RemoveAllObjects();
    delete effect;
    skill->RemoveAllObjects();
    delete skill;
    sound->StopAllEffects();
    sound->StopBackgroundInGameSound();
    sound->UnLoadInGameSound();
    delete sound;
    
    puzzleP4set->RemoveAllObjects();
    delete puzzleP4set;
    puzzleP8set->RemoveAllObjects();
    delete puzzleP8set;
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    while (!fallingQueue.empty())
        fallingQueue.pop();
    
    for (int i = 0 ; i < lock8xy.size() ; i++) lock8xy[i].clear();
    for (int i = 0 ; i < lock4xy.size() ; i++) lock4xy[i].clear();
    for (int i = 0 ; i < piece4xy.size() ; i++) piece4xy[i].clear();
    for (int i = 0 ; i < piece8xy.size() ; i++) piece8xy[i].clear();
    for (int i = 0 ; i < strap.size() ; i++) strap[i].clear();
    lock8xy.clear();
    lock4xy.clear();
    piece4xy.clear();
    piece8xy.clear();
    strap.clear();
    
    for (int i = 0 ; i < strap.size() ; i++)
    {
        for (int j = 0 ; j < strap[i].size() ; j++)
            strap[i][j]->removeFromParentAndCleanup(true);
        strap[i].clear();
    }
    strap.clear();
    
    puzzleLayer->removeAllChildren();
    puzzleLayer->removeFromParentAndCleanup(true);
    
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
    
    cocoLayer->removeAllChildren();
    cocoLayer->removeFromParentAndCleanup(true);
    fairyLayer->removeAllChildren();
    fairyLayer->removeFromParentAndCleanup(true);
    
    if (!isRebooting)
    {
        // 클라이언트의 튜토리얼 value가 이미 true라면, 이는 환경설정에서 들어왔다는 의미. UI로 돌아간다.
        if (CCUserDefault::sharedUserDefault()->getBoolForKey("is_inGameTutorial_done", false))
            Common::ShowNextScene(this, "T_Puzzle", "Ranking", true, 0);
        // 처음 튜토리얼에 들어왔다는 의미. 실제 인게임을 시작한다.
        else
            Common::ShowNextScene(this, "T_Puzzle", "Loading", true);
        
        CCUserDefault::sharedUserDefault()->setBoolForKey("is_inGameTutorial_done", true);
        isInGameTutorial = false;
    }
}

void T_Puzzle::EndSceneCallback()
{
}

float T_Puzzle::GetPieceWidth()
{
    return PIECE8_WIDTH;
}

T_PuzzleP8Set* T_Puzzle::GetPuzzleP8Set()
{
    return puzzleP8set;
}
T_PuzzleP4Set* T_Puzzle::GetPuzzleP4Set()
{
    return puzzleP4set;
}

void T_Puzzle::SetSpriteP8Null(int x, int y)
{
    spriteP8[x][y] = NULL;
}

CCSprite* T_Puzzle::GetSpriteP8(int x, int y)
{
    return spriteP8[x][y];
}

float T_Puzzle::GetBoardSize()
{
    return boardSize.height;
}

bool T_Puzzle::IsConnected(int x, int y)
{
    return (puzzleP4set->GetType(x, y) != BLOCKED);
}

T_Effect* T_Puzzle::GetEffect()
{
    return effect;
}

void T_Puzzle::SwapSpriteP8(int x1, int y1, int x2, int y2)
{
    spriteP8[x1][y1] = puzzleP8set->GetSprite(x1, y1);
    spriteP8[x2][y2] = puzzleP8set->GetSprite(x2, y2);
}

void T_Puzzle::SetSpriteP8(int x, int y, CCSprite* sp)
{
    spriteP8[x][y] = sp;
}

/*
void T_Puzzle::SetSpiritTouch(bool val)
{
    if (!val)
        m_iSpiritSP--;
    m_bIsSpiritExecuted = false;
    //m_bIsSpiritTouched = val;
}
*/




///////////////////////////////////////////////////////////////////////////////////////////////

void T_PuzzleP8Set::SetGameLayer(T_Puzzle* layer)
{
    gameLayer = layer;
}

void T_PuzzleP8Set::SetPuzzleLayer(CCLayer* layer)
{
    puzzleLayer = layer;
}


void T_PuzzleP8Set::CreatePiece(int x, int y, int type)
{
    object[x][y] = T_PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x, y), gameLayer, 20, gameLayer->GetBoardSize()/(float)1076, type);
    gameLayer->SetSpriteP8(x, y, object[x][y]->GetPiece());
}

CCSprite* T_PuzzleP8Set::GetSprite(int x, int y)
{
    return object[x][y]->GetPiece();
}

void T_PuzzleP8Set::AddChild(int x, int y)
{
    puzzleLayer->addChild(object[x][y]->GetPiece(), object[x][y]->GetZOrder());
}

void T_PuzzleP8Set::RemoveChild(int x, int y)
{
    puzzleLayer->removeChild(object[x][y]->GetPiece(), true);
    object[x][y]->RemovePiece();
    delete object[x][y];
}

int T_PuzzleP8Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}

T_PuzzleP8* T_PuzzleP8Set::GetObject(int x, int y)
{
    return object[x][y];
}

void T_PuzzleP8Set::MoveObject(int x, int y, int fromX, int fromY)
{
    object[x][y] = object[fromX][fromY];
}

void T_PuzzleP8Set::SwapObject(int x1, int y1, int x2, int y2)
{
    T_PuzzleP8* p1 = T_PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x1, y1), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1076, object[x2][y2]->GetType());
    T_PuzzleP8* p2 = T_PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x2, y2), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1076, object[x1][y1]->GetType());
    
    RemoveChild(x1, y1);
    RemoveChild(x2, y2);
    object[x1][y1] = p1;
    object[x2][y2] = p2;
    
    gameLayer->SwapSpriteP8(x1, y1, x2, y2);
    AddChild(x1, y1);
    AddChild(x2, y2);
}
void T_PuzzleP8Set::Falling(int x, int y, int targetX, int targetY, int queue_pos)
{
    CCMoveTo* moveTo = CCMoveTo::create(0.1f, gameLayer->SetPiece8Position(targetX, targetY));
    CCFiniteTimeAction* action = CCSequence::create( moveTo, CCCallFuncND::create(gameLayer, callfuncND_selector(T_Puzzle::FallingCallback), (void*)queue_pos), NULL);
    object[x][y]->GetPiece()->runAction(action);
}
void T_PuzzleP8Set::RemoveAllObjects()
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

void T_PuzzleP4Set::SetGameLayer(T_Puzzle* layer)
{
    gameLayer = layer;
}
void T_PuzzleP4Set::SetPuzzleLayer(CCLayer* layer)
{
    puzzleLayer = layer;
}
void T_PuzzleP4Set::CreatePiece(int x, int y, int type, int designatedType)
{
    // 처음에만 object를 생성한다.
    if (type == -100)
        object[x][y] = T_PuzzleP4::CreateP4(gameLayer, zGameObjectSP, type, designatedType);
    
    // 4각형의 4방향에 있는 8각형들의 type을 구해서 삼각형을 어떻게 만들지 결정한다.
    // 가장자리는 그 주위의 P8 type이 존재하지 않을 수도 있으므로 조건문을 잘 쓰자.
    int lu = (x == 1 && y == ROW_COUNT-1) ? -1 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y);
    int ru = (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) ? -2 : gameLayer->GetPuzzleP8Set()->GetType(x, y);
    int ld = (x == 1 && y == 1) ? -3 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y-1);
    int rd = (x == COLUMN_COUNT-1 && y == 1) ? -4 : gameLayer->GetPuzzleP8Set()->GetType(x, y-1);
    
    // scale
    float diaHalfWidth = gameLayer->GetPieceWidth() / (2.0f + sqrt(2.0f));
    
    int offsetX = 1;
    int offsetY = 0;
    object[x][y]->CreateSprites(x, y, lu, ru, ld, rd, ccp(offsetX, offsetY), gameLayer->SetPiece4Position(x, y), diaHalfWidth);
}
int T_PuzzleP4Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}
void T_PuzzleP4Set::SetOpacity(int x, int y, int alpha)
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
void T_PuzzleP4Set::AddChild(int x, int y)
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
void T_PuzzleP4Set::RemoveChild(int x, int y)
{
    if (object[x][y]->GetLeftUp() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetLeftUp(), true);
    if (object[x][y]->GetRightUp() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetRightUp(), true);
    if (object[x][y]->GetLeftDown() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetLeftDown(), true);
    if (object[x][y]->GetRightDown() != NULL)
        puzzleLayer->removeChild(object[x][y]->GetRightDown(), true);
    
    object[x][y]->InitChild();
}
void T_PuzzleP4Set::SetAction(int x, int y, CCActionInterval* action)
{
    object[x][y]->GetLeftUp()->runAction((CCActionInterval*)action->copy());
    object[x][y]->GetRightUp()->runAction((CCActionInterval*)action->copy());
    object[x][y]->GetLeftDown()->runAction((CCActionInterval*)action->copy());
    object[x][y]->GetRightDown()->runAction((CCActionInterval*)action->copy());
}
void T_PuzzleP4Set::StopAllActions(int x, int y)
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
T_PuzzleP4* T_PuzzleP4Set::GetObject(int x, int y)
{
    return object[x][y];
}
void T_PuzzleP4Set::SetType(int x, int y, int type)
{
    object[x][y]->SetType(type);
}
void T_PuzzleP4Set::RemoveAllObjects()
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

