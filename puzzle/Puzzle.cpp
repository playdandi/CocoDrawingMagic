#include "Puzzle.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

enum
{
	zBackground = 0,
	zGameObject = 100,
    zGameObjectSP = 101,
    zPieceConn = 3,
};

CCScene* Puzzle::scene()
{
    CCLog("Puzzle : scene");
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
    //CCDirector::sharedDirector()->end();
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
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Puzzle::Notification), "Puzzle", NULL);
    
    
    sound = new Sound();
    sound->PreLoadInGameSound();
    effect = new Effect();
    effect->Init(effect, this);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game3.plist");
    
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
    
    //    puzzleLayer->setScale((double)966/(double)1078);
    //CCMoveBy::create(2.0f, ccp(0, boardSize.height/2)),
    //CCFiniteTimeAction* action = CCSpawn::create(CCFlipY3D::create(2.0f), NULL);
    
    //CCFiniteTimeAction* action = CCRotateBy::create(2.0f, 0, -45);
    //CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    //((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->runAction(rep);
    
    PlayEffect(100, NULL);
    
    isMagicTime = false;
    isFeverTime = false;
    
    m_iSpiritSP = 0;
    
    W3_total = 0;
    W4_total = 0;
    
    pBlackOpen = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlackOpen->setAnchorPoint(ccp(0, 0));
    pBlackOpen->setPosition(ccp(0, 0));
    pBlackOpen->setColor(ccc3(0,0,0));
    this->addChild(pBlackOpen, 3000);
    
    isFalling = false;
    isGameOver = false;
    isInGamePause = false;
    
    iTouchRound = 0;
    isMissionSuccess = false;
    
    XMLStatus = 0;
    
	return true;
}

void Puzzle::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        m_bTouchStarted = false;
        CCLog("GameReady : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        sound->ResumeBackgroundInGameSound();
        
        // 정령 다시 움직이기
        if (skill->IsSpiritAlive(0))
            effect->GetSpirit(0)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(1))
            effect->GetSpirit(1)->resumeSchedulerAndActions();
        if (skill->IsSpiritAlive(2))
            effect->GetSpirit(2)->resumeSchedulerAndActions();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("Puzzle 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        sound->StopBackgroundInGameSound();
        
        // 종료하고 Ranking으로 돌아가자.
        CCLog("Puzzle 종료. Ranking으로 돌아감.");
        this->EndScene();
    }
}

void Puzzle::InitSkills()
{
    std::vector<int> skillNum, skillProb, skillLv;

    int id;
    for (int i = 0 ; i < inGameSkill.size() ; i++)
    {
        id = inGameSkill[i];
        MySkill* ms = MySkill::GetObj(id);
        if (ms != NULL)
        {
            if (id >= 21 && id <= 28) {
                skillNum.push_back(id-21);
                if (id == 27) skillProb.push_back(10);
                else skillProb.push_back(100);
            }
            else if (id >= 11 && id <= 18) {
                skillNum.push_back(id-3);
                if (id == 17) skillProb.push_back(10);
                else skillProb.push_back(100);
            }
            else if (id >= 31 && id <= 38) {
                skillNum.push_back(id-15);
                if (id == 38) skillProb.push_back(100);
                else skillProb.push_back(100);
            }
            CCLog("적용 스킬 : %d", id);
            skillLv.push_back(ms->GetLevel());
        }
    }
    
    skill->Init(skillNum, skillProb, skillLv);
}

void Puzzle::InitInfoBar()
{
    int offset = 20;
    //spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png1", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+vs.height-50-offset/2), CCSize(1075,75+25+offset), "", "Puzzle", this, 6) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png1", ccp(0.5, 1), ccp(m_winSize.width/2, vo.y+vs.height), CCSize(1075,75+25+offset), "", "Puzzle", this, 6) );
    
    // 별사탕 숫자배경
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png1", ccp(0, 0.5), ccp(100-20, vo.y+vs.height-47-offset/2), CCSize(160, 50+10), "", "Puzzle", this, 6) );
    
    // 별사탕
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(1, 0.5), ccp(50, vo.y+vs.height-30), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(1, 0.5), ccp(125-20, vs.height+vo.y-60-offset/2 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_3.png", ccp(1, 0.5), ccp(95-20, vs.height+vo.y-47-offset/2 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_4.png", ccp(1, 0.5), ccp(110-20, vs.height+vo.y-28-offset/2 ), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    // 게이지바 배경
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png2", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+vs.height-45-offset/2 ), CCSize(164, 35+15+offset/2), "", "Puzzle", this, 6) );
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_gauge.png", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+vs.height-45+2-offset/2 ), CCSize(149, 35+offset/2), "", "Puzzle", this, 7) );
    // 게이지바 아이콘(모자)
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_gauge_big.png", ccp(1, 0.5), ccp(m_winSize.width-170-82+20 +5, vo.y+vs.height-47-offset/2), CCSize(0, 0), "", "Puzzle", this, 8) );
    
    // pause button
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_pause.png", ccp(1, 0.5), ccp(m_winSize.width-25, vs.height+vo.y-50+5-offset/2), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    // 값들 (별사탕 개수)
    spriteClassInfo->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 40, ccp(1, 0.5), ccp(100+140-20, vo.y+vs.height-47-offset/2+3), ccc3(255,255,255), "", "Puzzle", this, 20, 0, 255, 1) ); // font 원래 40
    
    // 미션창
    spriteClassInfo->spriteObj.push_back( SpriteObject::Create(0, "mission_window.png", ccp(1, 1), ccp(m_winSize.width-40, vo.y+vs.height-50-50+7 -30-offset/2), CCSize(0, 0), "", "Puzzle", this, 6) );
    ((CCSprite*)spriteClassInfo->FindSpriteByName("mission_window.png"))->setScale(1.15f);
    
    
    for (int i = 0 ; i < spriteClassInfo->spriteObj.size(); i++)
        spriteClassInfo->AddChild(i);
}

void Puzzle::InitSprites()
{
    //CCLog("Init Sprites");
    //CCLog("Real : %f , %f", m_winSize.width, m_winSize.height);
    vs = CCDirector::sharedDirector()->getVisibleSize();
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    //CCLog("visible : %f , %f", vs.width, vs.height);
    //CCLog("visible point : %f , %f", vo.x, vo.y);
    
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
    //vs.height+vo.y-480
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
        
        this->unschedule(schedule_selector(Puzzle::CocoAnim));
    }
    
    cocoFrameNumber++;
}

void Puzzle::InitFairy()
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

void Puzzle::ChangeAnimFairy(float f)
{
    
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
            puzzleP4set->CreatePiece(x, y);
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
    
    m_bTouchStarted = true;
    m_iSkillSP = 0;
    m_bIsSpiritExecuted = false;
    m_iSpiritSP = 0;
}

void Puzzle::SetScoreAndStarCandy()
{
    iScore = 0;
    char n[10];
    sprintf(n, "%d", iScore);
    pScoreLayer = Common::MakeScoreLayer(iScore);
    CCSize s = pScoreLayer->getContentSize();
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93));
    this->addChild(pScoreLayer, 6);
    
    iStarCandy = 0;
    pStarCandyLabel = (CCLabelTTF*)spriteClassInfo->FindLabelByTag(1);
}

void Puzzle::UpdateScore(int type, int data)
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
    pScoreLayer->setPosition(ccp(m_winSize.width/2-s.width/2-30, vo.y+vs.height-93));
    this->addChild(pScoreLayer, 6);
}

void Puzzle::UpdateStarCandy(int type, int data)
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

void Puzzle::SetCombo()
{
    iCombo = 0;
    pComboLabel = CCLabelTTF::create("", fontList[2].c_str(), 50);
    pComboLabel->setAnchorPoint(ccp(0, 0));
    pComboLabel->setColor(ccc3(0,0,0));

    CCPoint pos = cocoLayer->getPosition();
    pComboLabel->setPosition(ccp(pos.x, pos.y+330));
    pComboLabel->setOpacity(0);
    this->addChild(pComboLabel, 3000);
}

int Puzzle::GetCombo()
{
    return iCombo;
}

void Puzzle::UpdateCombo()
{
    iCombo++;
    char temp[11];
    sprintf(temp, "%d Combo!", iCombo);
    pComboLabel->setString(temp);
    
    iComboTimer = 0;
    //this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    if (iCombo == 1)
    {
        // 처음 콤보가 적용될 때만 타이머 적용을 하면 된다.
        this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    }
    
    if (iCombo > 0 && iCombo % 10 == 0)
    {
        // 10의 배수마다 W3(콤보비례추가점수)스킬 발동
        skill->Invoke(10, NULL);
        W3_total += skill->W3GetScore();
        UpdateScore(1, skill->W3GetScore());
    }
    else if (iCombo % 10 == 5)
    {
        // 5, 15,... 마다 W4(콤보비례추가별사탕)스킬 발동
        skill->Invoke(11, NULL);
        W4_total += skill->W4GetCandy();
        UpdateStarCandy(1, skill->W4GetCandy());
    }
    
    pComboLabel->stopAllActions();
    pComboLabel->setOpacity(0);
    
    CCFiniteTimeAction* action = CCSequence::create(CCFadeIn::create(0.3f), CCFadeOut::create(0.3f), NULL);
    pComboLabel->runAction(action);
}

void Puzzle::ComboTimer(float f)
{
    iComboTimer += 100;
    if (iComboTimer >= 2000)
    {
        iCombo = 0;
        pComboLabel->setOpacity(0);
        this->unschedule(schedule_selector(Puzzle::ComboTimer));
    }
}

void Puzzle::ReadyAndStart(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true); // pBlackOpen 제거
 
    /*
    readyTimeLabel = CCLabelTTF::create("셋", fontList[0].c_str(), 82);
    readyTimeLabel->setAnchorPoint(ccp(0.5,0.5));
    readyTimeLabel->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    readyTimeLabel->setOpacity(0);
    this->addChild(readyTimeLabel, 5000);
    
    iReadyTime = 3000;
    ((Puzzle*)pointer)->schedule(schedule_selector(Puzzle::Ready), 0.1f);
    */
    
    sound->PlayVoice(VOICE_READY);
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName("letter_ready.png");
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(ccp(m_winSize.width/2+20, vo.y+tbSize.height+boardSize.height+120+180));
    sprite->setScale(1.2f);
    sprite->setOpacity(0);
    this->addChild(sprite, 5000);

    CCActionInterval* action = CCSequence::create( CCFadeIn::create(0.5f), CCDelayTime::create(0.8f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::Ready_C), NULL), NULL);
    sprite->runAction(action);
}
void Puzzle::Ready_C(CCNode* sender, void* p)
{
    sound->PlayVoice(VOICE_GO);
    
    sender->removeFromParentAndCleanup(true);
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName("letter_go.png");
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120+180));
    sprite->setScale(1.2f);
    //sprite->setOpacity(0);
    this->addChild(sprite, 5000);
    
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyCallback), this), NULL);
    sprite->runAction(action);
    /*
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.2f), CCEaseBackIn::create(CCMoveBy::create(0.7f, ccp(0, 1200))), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyCallback), this), NULL);
    sprite->runAction(action);
     */
}
void Puzzle::Ready(float f)
{
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.15f), CCEaseBackIn::create(CCMoveBy::create(0.3f, ccp(0, 1200))), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyCallback), this), NULL);
    readyTimeLabel->runAction(action);
    
    /*
    if (iReadyTime % 1000 == 0)
    {
        if (iReadyTime == 3000) readyTimeLabel->setString("셋");
        else if (iReadyTime == 2000) readyTimeLabel->setString("둘");
        else if (iReadyTime == 1000) readyTimeLabel->setString("하나");
        else readyTimeLabel->setString("수업 시작!");
        readyTimeLabel->setOpacity(255);
        
        if (iReadyTime == 0) // 게임 시작!
        {
            this->unschedule(schedule_selector(Puzzle::Ready));
            
            CCActionInterval* action = CCSequence::create(CCDelayTime::create(0.15f), CCEaseBackIn::create(CCMoveBy::create(0.3f, ccp(0, 1200))), CCCallFuncND::create(this, callfuncND_selector(Puzzle::ReadyCallback), this), NULL);
            readyTimeLabel->runAction(action);
        }
    }
    iReadyTime -= 250;
    */
}
void Puzzle::ReadyCallback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
    
    // 배경음 재생
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
    {
        if (!isInGamePause)
            sound->PlayBackgroundInGameSound();
    }
    
    m_bTouchStarted = false;
    ((Puzzle*)pointer)->schedule(schedule_selector(Puzzle::UpdateTimer), 0.1f);
}

void Puzzle::SetTimer()
{
    CCScale9Sprite* timerbar = ((CCScale9Sprite*)spriteClass->FindSpriteByName("background/bg_bar_timer.png"));
    CCPoint p = timerbar->getPosition();
    CCSize s = timerbar->getContentSize();
    
    iTimer = 1000 * PUZZLE_TIME;
    char n[5];
    sprintf(n, "%d", PUZZLE_TIME);
    pTimerLabel = CCLabelTTF::create(n, fontList[0].c_str(), 30);
    pTimerLabel->setAnchorPoint(ccp(0.5, 0.5));
    pTimerLabel->setColor(ccc3(0,0,0));
    //pTimerLabel->setPosition(ccp(40, 300));
    //ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1000, 22)
    //pTimerLabel->setPosition(ccp(p.x+s.width/2-30, p.y));
    pTimerLabel->setPosition(ccp(m_winSize.width/2 + 1000/2 - 20, vo.y+31.5f+5));
    this->addChild(pTimerLabel, 1000);
}

void Puzzle::UpdateTimer(float f)
{
    //////////////////////////////////////// 시간을 정지해야 하는 상황들 ////////////////////////////////////
    // Pause 상태에는 시간을 정지시킨다.
    if (isInGamePause)
        return;
    // Magic Time 발동 중에는 시간이 정지된다.
    if (isMagicTime)
        return;
    
    // "W7 : 시간을 얼리다" 스킬이 적용 중이면 5초 동안 정지한다.
    if (skill->W7GetVar())
    {
        skill->W7SetTime(skill->W7GetTime()-100);
        if (skill->W7GetTime() == 0)
        {
            skill->W7SetVar(); // false로 바꿔 다시 시간을 없앤다.
            PlayEffect(14, NULL);
        }
        return;
    }
    
    // "W8 : 여신의 은총" 스킬이 적용 중이면 정지한다.
    if (skill->W8_IsActive())
        return;
    
    // 한붓그리기 직후 발동된 active한 스킬이 하나라도 있으면 정지.
    if (m_iSkillSP > 0)
        return;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    iTimer -= 100;
    
    // timer 이동
    float delta = (float)1000/(PUZZLE_TIME*10);
    CCPoint tl = timerLayer->getPosition();
    CCPoint tp = timerClip->getPosition();
    timerClip->setPosition(ccp(tp.x-delta, tp.y));
    timerLayer->setPosition(ccp(tl.x+delta, tl.y));
    pTimerLabel->setPosition(ccp(pTimerLabel->getPosition().x-delta, pTimerLabel->getPosition().y));
    
    // 5초 남았을 때 이펙트+목소리 표현
    if (iTimer == 5000)
    {
        sound->PlayVoice(VOICE_TIMELIMIT);
        
        timelimit = CCSprite::createWithSpriteFrameName("letter_timelimit.png");
        timelimit->setAnchorPoint(ccp(0.5, 0.5));
        CCPoint p = SetTouch8Position(0, 0);
        timelimit->setPosition(ccp(p.x+30, p.y+30));
        timelimit->setScale(1.2f);
        this->addChild(timelimit, 5000);
        
        CCActionInterval* action = CCSequence::create(CCRotateBy::create(0.05f, -10), CCRotateBy::create(0.1f, 20), CCRotateBy::create(0.1f, -20), CCRotateBy::create(0.05f, 10), NULL);
        timelimit->runAction(action);
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
        
        if (iTimer == 0) // game over
        {
            isGameOver = true;
            
            this->unschedule(schedule_selector(Puzzle::UpdateTimer));
            this->setTouchEnabled(false);
            this->setKeypadEnabled(false);
            sound->StopBackgroundSound();
            
            spriteClassInfo->RemoveAllObjects();
            delete spriteClassInfo;
            pScoreLayer->removeAllChildren();
            pScoreLayer->removeFromParentAndCleanup(true);
            
            // 그리고 있던 것들 모두 취소
            CancelDrawing();
            timelimit->removeFromParentAndCleanup(true);
            
            overBg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
            overBg->setAnchorPoint(ccp(0, 0));
            overBg->setPosition(ccp(0, 0));
            overBg->setColor(ccc3(0,0,0));
            overBg->setOpacity(220);
            this->addChild(overBg, 5000);
            
            CCSprite* sprite = CCSprite::createWithSpriteFrameName("icon_gameover.png");
            sprite->setAnchorPoint(ccp(0.5, 0.5));
            sprite->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
            this->addChild(sprite, 5000);
            
            CCActionInterval* action = CCSequence::create(CCDelayTime::create(2.0f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::GameOver_Callback), this), NULL);
            sprite->runAction(action);
            
            // sound
            sound->PlayVoice(VOICE_GAMEOVER);
            
            /*
            // TIME UP 메시지 띄운 후, 게임결과화면 로딩 위해 game_end protocol 호출
            CCLabelTTF* timeup = CCLabelTTF::create("수업 종료!", fontList[0].c_str(), 84);
            //timeup->setColor(ccc3());
            timeup->setAnchorPoint(ccp(0.5, 0.5));
            timeup->setPosition(ccp(-300, m_winSize.height/2));
            this->addChild(timeup, 5000);
            CCActionInterval* action = CCSequence::create(CCMoveTo::create(0.25f, ccp(m_winSize.width/2, m_winSize.height/2)), CCDelayTime::create(1.3f), CCMoveTo::create(0.25f, ccp(m_winSize.width+300, m_winSize.height/2)), CCCallFuncND::create(this, callfuncND_selector(Puzzle::GameEnd), this), NULL);
            timeup->runAction(action);
            */
        }
        
        // 정령 준비 발동 ('시간을 얼리다' 발동 중에는 NO!)
        if (!skill->W7GetVar())
        {
            skill->SpiritTry(0, 0);
            skill->SpiritTry(1, 0);
            skill->SpiritTry(2, 0);
        }
    }
    
    if (isFeverTime)
    {
        feverRemainTime -= 100;
        if (feverRemainTime == 0)
        {
            isFeverTime = false;
            for (int x = 1; x < COLUMN_COUNT ; x++)
            {
                for (int y = 1 ; y < ROW_COUNT ; y++)
                {
                    CCSprite* bomb = CCSprite::create("images/bomb.png");
                    bomb->setPosition(SetPiece4Position(x, y));
                    this->addChild(bomb, 3000);
                    feverSpr.push_back(bomb);
                    
                    puzzleP4set->SetType(x, y, -puzzleP4set->GetType(x, y));
                    //if (puzzleP4set->GetType(x, y) != BLOCKED)
                    //{
                        if (puzzleP4set->GetObject(x, y) != NULL)
                            puzzleP4set->RemoveChild(x, y);
                        puzzleP4set->CreatePiece(x, y, puzzleP4set->GetType(x, y));
                        puzzleP4set->AddChild(x, y);
                    //}
                }
            }
            
            for (int i = 0 ; i < feverSpr.size() ; i++)
                this->removeChild(feverSpr[i]);
            feverSpr.clear();
        }
    }
}

void Puzzle::GameOver_Callback(CCNode* sender, void* pointer)
{
    sender->removeFromParentAndCleanup(true);
    
    CCSprite* sprite = CCSprite::createWithSpriteFrameName("icon_bonustime.png");
    sprite->setAnchorPoint(ccp(0.5, 0.5));
    sprite->setPosition(ccp(m_winSize.width/2, m_winSize.height/2));
    this->addChild(sprite, 5000);
    
    CCActionInterval* action = CCSequence::create(CCDelayTime::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(Puzzle::GameEnd), this), NULL);
    sprite->runAction(action);
    
    // sound
    sound->PlayVoice(VOICE_BONUS);
}

/*
CCPoint Puzzle::SetPiece8Position(int x, int y)
{
    CCPoint vo = CCDirector::sharedDirector()->getVisibleOrigin();
    
    // start + (x,y)좌표만큼 이동 + AP로 인해 절반 더 이동 + 보정치
    //int posX =   1 + x*PIECE8_WIDTH  + PIECE8_WIDTH/2  + 7;
    int posX = 85 + x*PIECE8_WIDTH + PIECE8_WIDTH/2;
    //CCLog("(%d, %d) : %d", x, y, posX);
    //int posY = 220 + y*PIECE8_HEIGHT + PIECE8_HEIGHT/2 + 6;
    int posY = vo.y+130 + y*PIECE8_HEIGHT + PIECE8_HEIGHT/2 + 6;
    return ccp(posX, posY);
}
 CCPoint Puzzle::SetPiece4Position(int x, int y)
 {
 // start + (x,y)좌표만큼 이동 + 보정치
 int posX =   1 + x*PIECE8_WIDTH  + 7;
 int posY = 220 + y*PIECE8_HEIGHT + 6;
 return ccp(posX, posY);
 }
*/

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
    int posX = (x-3)*PIECE8_WIDTH - PIECE8_WIDTH/2;
    int posY = (y-3)*PIECE8_HEIGHT - PIECE8_HEIGHT/2;
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
            
            int d = GetDist(SetTouch8Position(x, y), point);
            if (d < dist && globalType[touch_cnt%QUEUE_CNT] == puzzleP8set->GetType(x, y))
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
    isInGamePause = true;
    sound->PauseBackgroundInGameSound();
    
    // 정령 멈추기
    if (skill->IsSpiritAlive(0))
        effect->GetSpirit(0)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(1))
        effect->GetSpirit(1)->pauseSchedulerAndActions();
    if (skill->IsSpiritAlive(2))
        effect->GetSpirit(2)->pauseSchedulerAndActions();
    
    Common::ShowNextScene(this, "Puzzle", "PuzzlePause", false, vo.y+tbSize.height+boardSize.height+60);
}

bool Puzzle::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isGameOver)
        return false;
    CCPoint point = pTouch->getLocation();
    //CCLog("Puzzle :: %d , %d", (int)point.x, (int)point.y);
    

    // 보드판과 상관없는 것들 (lock에 제한받지 않는 것들)
    // 1) pause button
    if (((CCSprite*)spriteClassInfo->FindSpriteByName("icon/icon_pause.png"))->boundingBox().containsPoint(point))
    {
        /*
        m_bTouchStarted = true;
        isMagicTime = true;
        
        magicTimeBg = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
        magicTimeBg->setPosition(ccp(0, 0));
        magicTimeBg->setAnchorPoint(ccp(0, 0));
        magicTimeBg->setColor(ccc3(0, 0, 0));
        magicTimeBg->setOpacity(0);
        this->addChild(magicTimeBg, 3000);
        
        m_iStartMagicTimeStatus = 1;
        this->schedule(schedule_selector(Puzzle::StartMagicTime), 0.1f);
        */
        
        PauseGame();
        return true;
    }
    
    cocoFrameNumber = 0;
    
    CCLog("touchBegan (%d) : %d %d %d %d", touch_cnt%QUEUE_CNT, m_iSkillSP, m_bTouchStarted, m_iSpiritSP, m_bIsSpiritExecuted);
    
    // 스킬 발동 중 or touchEnd가 되기 전까지 or 정령스킬 실행 중이면 터치 금지
    if (m_iSkillSP > 0 || m_bTouchStarted || m_bIsSpiritExecuted)
        return false;
    
    m_bTouchStarted = true; // 터치 시작 lock 걸기
    
    
    /*
    if (fever->boundingBox().containsPoint(point))
    {
        isFeverTime = true;
        feverRemainTime = 5000;
        
        for (int x = 1; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                //CCSprite* bomb = CCSprite::create("images/bomb.png");
                //bomb->setPosition(SetPiece4Position(x, y));
                //this->addChild(bomb, 3000);
                //feverSpr.push_back(bomb);
     
                CCSprite* dia = CCSprite::create("images/fever_dia.png");
                dia->setPosition(SetPiece4Position(x, y));
                this->addChild(dia, 3000);
                feverSpr.push_back(dia);
                
                puzzleP4set->SetType(x, y, -puzzleP4set->GetType(x, y));
                //if (puzzleP4set->GetType(x, y) != BLOCKED)
                //{
                if (puzzleP4set->GetObject(x, y) != NULL)
                    puzzleP4set->RemoveChild(x, y);
                //puzzleP4set->CreatePiece(x, y, puzzleP4set->GetType(x, y));
                //puzzleP4set->AddChild(x, y);
                //}
            }
        }
    }
    */
    
    // 어느 퍼즐인지 찾기
    CCPoint boardPos = BoardStartPosition(point);
    int x = (int)boardPos.x;
    int y = (int)boardPos.y;
    
    if (x == -1 && y == -1) // 찾지 못했다면 퍼즐 피스를 클릭한 것이 아니므로 그냥 종료.
        return (m_bTouchStarted = false);
    
    if (m_bLockP8[x][y] > 0) // 터치한 locked면 중지.
        return (m_bTouchStarted = false);
    
    
    // 3가지 정령 중 하나를 터치할 때 동작한다. (좌표 기준인데, 정령이 없으면 아예 시도하지 않는다)
    if ((x == 0 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == 0))
    {
        CCLog("정령 부분 터치함");
        if (m_iSpiritSP == 0 && !skill->W8_IsActive()) // '여신의 은총' 실행 중이면 중지.
        {
            //CCLog("정령 되나요?");
            // 정령이 살아있지 않다면 터치 종료.
            if (x == 0 && y == ROW_COUNT-1 && !skill->IsSpiritAlive(0))
                return (m_bTouchStarted = false);
            else if (x == COLUMN_COUNT-1 && y == ROW_COUNT-1 && !skill->IsSpiritAlive(1))
                return (m_bTouchStarted = false);
            else if (x == COLUMN_COUNT-1 && y == 0 && !skill->IsSpiritAlive(2))
                return (m_bTouchStarted = false);
            
            m_bTouchStarted = false;
            
            //CCLog("가자 정령!");
            // semaphore 1 증가 + 실행상태 lock 걸기
            m_iSpiritSP++;
            m_bIsSpiritExecuted = true;
            
            // 각 정령에 관련된 스킬 발동!
            if (x == 0 && y == ROW_COUNT-1)
                skill->Invoke(4, NULL);
            else if (x == COLUMN_COUNT-1 && y == ROW_COUNT-1)
                skill->Invoke(12, NULL);
            else if (x == COLUMN_COUNT-1 && y == 0)
                skill->Invoke(20, NULL);
        }
        
        return (m_bTouchStarted = false);
    }
    
    
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
    spriteP8[x][y]->setOpacity(100);
    
    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size());
    
    return true;
}

void Puzzle::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isGameOver || m_iSkillSP > 0 || m_bIsSpiritExecuted)
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
        
        //CCLog("move : %d %d", x, y);
        
        // 선택된 피스와 바로 직전 피스의 type이 같고, 맨하탄distance가 2 이하인 경우
        if ((puzzleP8set->GetType(x, y) == puzzleP8set->GetType(beforeX, beforeY) &&
            abs(x-beforeX)+abs(y-beforeY) <= 2))// ||
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
                {
                    CCLog("cycle : %d , %d", x, y);
                    m_bIsCycle[touch_cnt%QUEUE_CNT] = true;
                }
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
                {
                    CCLog("cycle : %d , %d", x, y);
                    m_bIsCycle[touch_cnt%QUEUE_CNT] = true;
                }
            }
            
            if (flag)
            {
                if (m_bIsCycle[touch_cnt%QUEUE_CNT])
                {
                    // cycle을 표시해주는 액션이 들어간다.
                    //CCLog("cycle : %d %d", x, y);
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
                    spriteP8[x][y]->setOpacity(140);
                    
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
                }
            }
        }
	}
}

void Puzzle::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isGameOver || m_iSkillSP > 0 || m_bIsSpiritExecuted)
        return;

    if (m_bTouchStarted)
    {
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
            // 일단 정령 스킬부터 semaphore 증가.
            if (!skill->W8_IsActive())
                m_iSpiritSP++;
            
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
            
            // '여신의 은총' 발동 중이면 다음을 행한다.
            if (skill->W8_IsActive())
            {
                skill->W8_Invoke(piece8xy[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
                m_bTouchStarted = false;
                m_bIsCycle[touch_cnt%QUEUE_CNT] = false;
                return;
            }
            
            // 스킬 발동 try (발동되는 스킬들 조사)
            // 여기서 스킬에 따라 skill Lock이 걸릴 수 있다. (그러면 스킬발동 종료까지 터치 아예 못함)
            m_bSkillLock[touch_cnt%QUEUE_CNT] = false;
            skill->TrySkills(globalType[touch_cnt%QUEUE_CNT], touch_cnt%QUEUE_CNT);
            if (m_bSkillLock[touch_cnt%QUEUE_CNT])
            {
                m_iSkillSP++; // skill semaphore 증가
                CCLog("스킬 lock 걸림 (%d)", touch_cnt%QUEUE_CNT);
            }
            else
                CCLog("스킬 발동 X (%d)", touch_cnt%QUEUE_CNT);
            
            m_iBombCallbackType[touch_cnt%QUEUE_CNT] = 0;
            
            touch_cnt++;
            
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
    }
}

void Puzzle::CancelDrawing()
{
    int x, y;
    CCLog("CancelDrawing = %d", touch_cnt%QUEUE_CNT);
    //CCLog("정령 클릭 후 손 떼지 않고 정령스킬발동완료 후 떼어서 여기로 왔다.");
    //CCLog("%d %d %d", (int)piece8xy[touch_cnt%QUEUE_CNT].size(), (int)piece4xy[touch_cnt%QUEUE_CNT].size(), (int)strap[touch_cnt%QUEUE_CNT].size());
    for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
    {
        x = (int)piece8xy[touch_cnt%QUEUE_CNT][i].x;
        y = (int)piece8xy[touch_cnt%QUEUE_CNT][i].y;
        spriteP8[x][y]->setScale(spriteP8[x][y]->getScale() / 0.9f);
        spriteP8[x][y]->setOpacity(255);
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
    // 4) '코코타임' 검사
    //  - 적용되었다면, Bomb + Falling...
    // 5) 속성별 마지막 스킬 검사
    //  - 적용되었다면, Bomb + Falling...
    
    if (m_iState[queue_pos] == SKILL_STOPTIME) // 제일 처음 : '시간을 얼리다' 검사
    {
        CCLog("state(%d) STOPTIME", queue_pos);
        // W5(시간을얼리다)
        skill->Invoke(14, NULL);
 
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
        
        //if (m_iSkillSP == 0) // 스킬 semaphore가 1 이상이면, 굳이 터치lock을 할 필요가 없다. (전체 lock이 되어있으니까)
            Lock(queue_pos);
        
        // 기본 이펙트 적용
        effect->PlayEffect(-100, NULL, piece8xy[queue_pos]);
        
        // 추가점수 적용되었다면 실행한 후, 한붓그리기 한 것을 터뜨리기
        skill->Invoke(0, queue_pos); skill->Invoke(8, queue_pos); skill->Invoke(16, queue_pos); // 추가점수
        
        // 10개이상제거시추가점수(F3), 10개이상제거시추가별사탕(E4)
        skill->Invoke(2, queue_pos); skill->Invoke(19, queue_pos);
        
        Bomb(queue_pos, piece8xy[queue_pos]);
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
        m_iNextState[queue_pos] = SKILL_COCOTIME;
        
        skill->Invoke(5, queue_pos); skill->Invoke(13, queue_pos); skill->Invoke(21, queue_pos);

        if (!(skill->IsApplied(5, queue_pos) || skill->IsApplied(13, queue_pos) || skill->IsApplied(21, queue_pos)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    else if (m_iState[queue_pos] == SKILL_COCOTIME) // 코코타임
    {
        CCLog("state(%d) COCOTIME", queue_pos);
        m_iNextState[queue_pos] = SKILL_FINAL;
        
        skill->Invoke(6, queue_pos);
        
        if (!skill->IsApplied(6, queue_pos))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    else if (m_iState[queue_pos] == SKILL_FINAL) // 마지막 (속성별 8번) 스킬
    {
        CCLog("state(%d) FINAL", queue_pos);
        m_iNextState[queue_pos] = SKILL_DONE;

        skill->Invoke(7, queue_pos); skill->Invoke(15, queue_pos); skill->Invoke(23, queue_pos);
        
        // '고대나무' 스킬을 검사할 때는 미리 state를 넘겨 놓는다.
        if (!skill->IsApplied(7, queue_pos) && !skill->IsApplied(15, queue_pos))
            m_iState[queue_pos] = m_iNextState[queue_pos];
        
        //if (!skill->IsApplied(7, queue_pos) && !skill->IsApplied(15, queue_pos) && !skill->IsApplied(23, queue_pos))
        if ((!skill->IsApplied(7, queue_pos) && !skill->IsApplied(23, queue_pos)) ||
            (skill->IsApplied(23, queue_pos) && skill->E8_IsFinished()))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            // 참고 : '여신의 은총' 스킬은 발동 유무에 관계없이 다음으로 넘긴다.
            // 참고 : '고대나무' 스킬은 발동되더라도 초록 피스가 하나도 없다면 역시 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    
    else if (m_iState[queue_pos] == SKILL_DONE)
    {
        CCLog("state(%d) DONE!", queue_pos);
        // 모든 스킬이 끝났다. 원상태로 되돌리자.
        
        // skill Lock을 푼다.
        if (m_bSkillLock[queue_pos])
        {
            SetSkillLock(queue_pos, false);
            m_iSkillSP--;
        }
        
        m_iSpiritSP--;
        
        iTouchRound--;
    }
}

void Puzzle::Lock(int queue_pos)
{
    // 스킬 발동이 된 경우는 이미 전체가 lock이므로 이 함수를 실행할 필요가 없다.
    //if (m_bSkillLock)
    //    return;
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
}
void Puzzle::LockEach(int x, int y)
{
    m_bLockP8[x][y]++;
}
void Puzzle::UnLockEach(int x, int y)
{
    m_bLockP8[x][y]--;
}

void Puzzle::Bomb(int queue_pos, std::vector<CCPoint> bomb_pos)
{
    CCLog("Bomb(%d) start : size = %d", queue_pos, (int)bomb_pos.size());
    //Lock(queue_pos);
    
    int x, y;
    
    // FeverTime : 폭탄 범위에 있는 것들을 같이 포함시켜 동시에 폭발하도록 하자.
    if (isFeverTime)
    {
        /*
        bool flag;
        for (int i = 0 ; i < piece4xy[queue_pos].size() ; i++)
        {
            x = (int)piece4xy[queue_pos][i].x;
            y = (int)piece4xy[queue_pos][i].y;
            
            flag = true;
            for (int j = 0 ; j < piece8xy[queue_pos].size() ; j++)
            {
                
            }
        }
        */
    }
    
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
        sound->PlaySkillSound(0);
    else
        sound->PlayBomb();
        
    
    // 8각형들을 터뜨린다.
    float delayTime = 0.0f;
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax[queue_pos] = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;

        float bombTime = 0.15f;
        if (m_iState[queue_pos] == SKILL_CYCLE)
            bombTime = 0.60f;
        else if (m_iState[queue_pos] == SKILL_FINAL && globalType[queue_pos] == PIECE_RED)
            bombTime = 0.05f;
        
        // 그 다음이 사이클 주변부 터지는 스킬이면 조금 딜레이준다.
        if (m_iNextState[queue_pos] == SKILL_CYCLE)
            delayTime = 0.1f;
        
        // 터지는 액션
        CCFiniteTimeAction* action;
        if (m_iState[queue_pos] == SKILL_DOUBLESIX && globalType[queue_pos] == PIECE_BLUE) // 6개이상 한번더(blue) : 터지고 살짝 딜레이를 준다.
        {
            action = CCSequence::create(
                        CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                                        CCDelayTime::create(0.7f),
                        CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                        NULL);
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
    
    // show starcandy
    effect->ShowStarCandy(bomb_pos);

    if (m_iState[queue_pos] == SKILL_FINAL && globalType[queue_pos] == PIECE_RED)
        return;
    
    // update score
    UpdateScore(0, bomb_pos.size());
    // update starcandy
    UpdateStarCandy(0, bomb_pos.size());
    // update combo
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
        else if (m_iState[(int)queue_pos] == SKILL_BASIC) // 한붓그리기 부분 폭발 완료
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
        else if (m_iState[(int)queue_pos] == SKILL_DOUBLESIX)
        {
            CCLog("bomb callback (%d) DOUBLESIX", (int)queue_pos);
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A4BGetPos();
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
            std::vector<CCPoint> temp = skill->A8GetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A8Clear();
            
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            piece8xy[(int)queue_pos].clear();
        }
        else // '여신의 은총'
        {
            CCLog("여신의 은총 : Bomb callback");
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            piece8xy[(int)queue_pos].clear();
        }
        
        // falling queue insertion
        fallingQueue.push((int)queue_pos);
        if (!isFalling)
            FallingProcess();
    }
}

void Puzzle::FallingProcess()
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

void Puzzle::Falling(int queue_pos, int xx)
{
    // CCLog("Falling 들어옴 : queue_pos , drop_order : %d %d", queue_pos, drop_order%QUEUE_CNT);
    
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
                    if (spriteP8[x][pos] != NULL) //m_pBoard[x][pos] != NULL)
                        break;
                }
                
                if (pos >= end)
                {
                    // 더 이상 내려야 할 8각형이 없는 경우 (새로 만들어서 drop시킨다)
                    puzzleP8set->CreatePiece(x, y);
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
	}
    
    //pthread_mutex_unlock(&falling);
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
                }
            }
        }
        
        //if (m_iSkillSP == 0)
        //{
            // lock을 모두 해제한다. (semaphore를 1씩 감소시킨다)
            int x, y;
            //CCLog("lock size : %d", (int)lock8xy[queue_pos].size());
            for (int i = 0 ; i < lock8xy[queue].size() ; i++)
            {
                x = lock8xy[queue][i].x;
                y = lock8xy[queue][i].y;
                m_bLockP8[x][y]--;
            }
            lock8xy[queue].clear();
        //}
        
        /************ LOCK print ****************/
        /*
        for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
        {
            CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
                  m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
        }
        */
        /*****************************************/

        
        // '고대나무' 스킬의 한 line falling이 끝나면, Restart 여부를 확인하고, 끝났다면 라운드 전체를 종료한다.
        //if (m_iState[(int)queue_pos] == SKILL_FINAL && globalType[(int)queue_pos] == PIECE_GREEN)
        if (xx != -1)
        {
            skill->E8_DecideRestart(xx);
            if (skill->E8_IsFinished())
            {
                m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                CCLog("Falling callback (%d) - 다음 스킬 [고대나무 끝났음] : %d", queue, m_iState[(int)queue_pos]);
                InvokeSkills((int)queue_pos);
            }
            return;
        }
        else
        {
            FallingProcess();
        
            // 다음 스킬로 넘어간다. ('여신의 은총' 실행 중에는 할 필요 없다)
            if (!skill->W8_IsActive())
            {
                m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                CCLog("Falling callback (%d) - 다음 스킬 [일반] : %d", queue, m_iState[(int)queue_pos]);
                InvokeSkills((int)queue_pos);
            }
        }
    }
}

void Puzzle::FallingQueuePushAndFalling(int queue_pos)
{
    // falling queue insertion
    fallingQueue.push(queue_pos);
    if (!isFalling)
        FallingProcess();
}



void Puzzle::GameEnd(CCNode* sender, void* pointer)
{
    overBg->removeFromParentAndCleanup(true);
    
    sender->removeFromParentAndCleanup(true);
    
    //http://14.63.225.203/cogma/game/game_end.php?kakao_id=1000&score=150&mission=0&starcandy=10
    char temp[255];
    std::string url = "http://14.63.225.203/cogma/game/game_end.php?";
    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
    url += temp;
    sprintf(temp, "score=%d&", iScore);
    url += temp;
    sprintf(temp, "mission=%d&", isMissionSuccess);
    url += temp;
    sprintf(temp, "starcandy=%d", iStarCandy);
    url += temp;
    CCLog("url = %s", url.c_str());
    
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(this, httpresponse_selector(Puzzle::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
}

void Puzzle::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    switch (XMLStatus)
    {
        case 0: XmlParseGameEnd(dumpData, buffer->size()); break;
        case 1: XmlParseFriends(dumpData, buffer->size()); break;
        default: ParseProfileImage(dumpData, buffer->size(), atoi(res->getHttpRequest()->getTag())); break;
    }
    XMLStatus++;
}

void Puzzle::XmlParseFriends(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        int kakaoId;
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
        int fire;
        int water;
        int land;
        int master;
        int fairyId;
        int fairyLevel;
        int skillId;
        int skillLevel;
        
        // clear friend-list
        for (int i = 0 ; i < friendList.size() ; i++)
            delete friendList[i];
        friendList.clear();
        
        profileCnt = profiles.size();
        
        xml_object_range<xml_named_node_iterator> friends = nodeResult.child("friend-list").children("friend");
        for (xml_named_node_iterator it = friends.begin() ; it != friends.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "kakao-id") kakaoId = ait->as_int();
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
                else if (name == "properties-fire") fire = ait->as_int();
                else if (name == "properties-water") water = ait->as_int();
                else if (name == "properties-land") land = ait->as_int();
                else if (name == "properties-master") master = ait->as_int();
                else if (name == "fairy-id") fairyId = ait->as_int();
                else if (name == "fairy-level") fairyLevel = ait->as_int();
                else if (name == "skill-id") skillId = ait->as_int();
                else if (name == "skill-level") skillLevel = ait->as_int();
            }
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, remainRequestPotionTime, remainRequestTopazTime, weeklyHighScore, highScore, scoreUpdateTime, certificateType, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            // potion image 처리
            friendList[(int)friendList.size()-1]->SetPotionSprite();
            
            if (ProfileSprite::GetProfile(imageUrl) == NULL) // 프로필 sprite에 모은다.
                profiles.push_back( new ProfileSprite(imageUrl) );
        }
        // sort by { max[weeklyScore], min[scoreUpdateTime] }
        DataProcess::SortFriendListByScore();
        
        // get image by url
        char tag[5];
        bool flag = true;
        for (int i = 0 ; i < profiles.size() ; i++)
        {
            if (profiles[i]->GetProfileUrl() != "" && profiles[i]->GetProfile() == NULL)
            {
                flag = false;
                // get profile image sprite from URL
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(profiles[i]->GetProfileUrl().c_str());
                req->setRequestType(CCHttpRequest::kHttpGet);
                req->setResponseCallback(this, httpresponse_selector(Puzzle::onHttpRequestCompleted));
                sprintf(tag, "%d", i);
                req->setTag(tag);
                CCHttpClient::getInstance()->send(req);
                req->release();
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
        CCLog ("IS RANK UP ? = %d", isRankUp);
        
        // 새로 받을 프로필 이미지가 없다면, 바로 게임결과 화면으로 넘어가자.
        if (flag)
            Common::ShowNextScene(this, "Puzzle", "PuzzleResult", false);
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}

void Puzzle::ParseProfileImage(char* data, int size, int idx)
{
    // make texture2D
    CCImage* img = new CCImage;
    img->initWithImageData(data, size);
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
    
    // set CCSprite
    profiles[idx]->SetSprite(texture);
    
    // 새 프로필을 다 받으면, 게임결과 화면으로 넘어가자.
    profileCnt++;
    if (profileCnt == (int)profiles.size())
        Common::ShowNextScene(this, "Puzzle", "PuzzleResult", false);
}

void Puzzle::XmlParseGameEnd(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        // new record 인가?
        isNewRecord = nodeResult.child("new-record").text().as_int();
        
        // 이전 값 저장 (MP는 프로토콜 바뀌면 제대로 받도록 하자)
        prevTopaz = myInfo->GetTopaz();
        prevStarCandy = myInfo->GetStarCandy();
        prevMP = myInfo->GetMPTotal();
        
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp, isActive;
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
            }
            myInfo->AddSkill(csi, usi, level, exp);
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        isRankUp = false;
        // score 정보 갱신
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
                    std::string url = "http://14.63.225.203/cogma/game/get_friendslist.php?";
                    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(Puzzle::onHttpRequestCompleted));
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                    break;
                }
            }
        }
        
        // 친구리스트 새로 호출하지 않는다면, 바로 게임결과 화면으로 넘어가자.
        if (flag)
            Common::ShowNextScene(this, "Puzzle", "PuzzleResult", false);
    }
    else
    {
        // failed msg
        CCLog("Puzzle : failed code = %d", code);
    }
}


PuzzleSkill* Puzzle::GetSkill()
{
    return skill;
}
std::vector<CCPoint> Puzzle::GetPiece8xy(bool afterCast)
{
    if (afterCast)
        return piece8xy[(touch_cnt-1)%QUEUE_CNT];
    return piece8xy[touch_cnt%QUEUE_CNT];
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


void Puzzle::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Puzzle");
    
    // layer 배경색을 검은색으로 바꾼다.
    this->setColor(ccc3(0,0,0));
    
    // 일시정지 화면에서 바로 종료하는 상황일 경우
    if (isInGamePause)
        this->unschedule(schedule_selector(Puzzle::UpdateTimer));
    isInGamePause = false;
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game3.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game2.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game3.png");
    
    // delete all objects
    effect->RemoveAllObjects();
    delete effect;
    skill->RemoveAllObjects();
    delete skill;
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

    pComboLabel->removeFromParentAndCleanup(true);
    pTimerLabel->removeFromParentAndCleanup(true);
    
    puzzleLayer->removeAllChildren();
    puzzleLayer->removeFromParentAndCleanup(true);
    
    timerLayer->removeAllChildren();
    timerLayer->removeFromParentAndCleanup(true);
    timerStencil->removeFromParentAndCleanup(true);
    timerClip->removeFromParentAndCleanup(true);
    
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

    //CCSprite* magicTimeBg;
    //CCSprite* fever;
    //std::vector<CCSprite*> feverSpr;
    
    //CCLog("Puzzle - EndScene Done");
    
    if (!isRebooting)
    {
        //isRankUp = true;
        if (isRankUp)
            Common::ShowNextScene(this, "Puzzle", "RankUp", true);
        else
            Common::ShowNextScene(this, "Puzzle", "Ranking", true, 1);
    }
}

void Puzzle::EndSceneCallback()
{
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
    object[x][y] = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x, y), gameLayer, 20, gameLayer->GetBoardSize()/(float)1076, type);
    gameLayer->SetSpriteP8(x, y, object[x][y]->GetPiece());
}

CCSprite* PuzzleP8Set::GetSprite(int x, int y)
{
    return object[x][y]->GetPiece();
}

void PuzzleP8Set::AddChild(int x, int y)
{
    puzzleLayer->addChild(object[x][y]->GetPiece(), object[x][y]->GetZOrder());
}

void PuzzleP8Set::RemoveChild(int x, int y)
{
    puzzleLayer->removeChild(object[x][y]->GetPiece(), true);
    object[x][y]->RemovePiece();
    delete object[x][y];
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
    PuzzleP8* p1 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x1, y1), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1076, object[x2][y2]->GetType());
    PuzzleP8* p2 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x2, y2), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1076, object[x1][y1]->GetType());
    
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
    CCMoveTo* moveTo = CCMoveTo::create(0.1f, gameLayer->SetPiece8Position(targetX, targetY));
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
void PuzzleP4Set::CreatePiece(int x, int y, int type)
{
    // 처음에만 object를 생성한다.
    if (type == -100)
        object[x][y] = PuzzleP4::CreateP4(gameLayer, zGameObjectSP, type);

    // 4각형의 4방향에 있는 8각형들의 type을 구해서 삼각형을 어떻게 만들지 결정한다.
    // 가장자리는 그 주위의 P8 type이 존재하지 않을 수도 있으므로 조건문을 잘 쓰자.
    int lu = (x == 1 && y == ROW_COUNT-1) ? -1 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y);
    int ru = (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) ? -2 : gameLayer->GetPuzzleP8Set()->GetType(x, y);
    int ld = (x == 1 && y == 1) ? -3 : gameLayer->GetPuzzleP8Set()->GetType(x-1, y-1);
    int rd = (x == COLUMN_COUNT-1 && y == 1) ? -4 : gameLayer->GetPuzzleP8Set()->GetType(x, y-1);
    
    //float offsetX = (float)39.7/(float)40;
    //float offsetY = (float)0.2 /(float)40;
    int offsetX = 1;
    int offsetY = 0;
    object[x][y]->CreateSprites(x, y, lu, ru, ld, rd, ccp(offsetX, offsetY), gameLayer->SetPiece4Position(x, y));
}
int PuzzleP4Set::GetType(int x, int y)
{
    return object[x][y]->GetType();
}
void PuzzleP4Set::SetOpacity(int x, int y, int alpha)
{
    object[x][y]->GetLeftUp()->setOpacity(alpha);
    object[x][y]->GetRightUp()->setOpacity(alpha);
    object[x][y]->GetLeftDown()->setOpacity(alpha);
    object[x][y]->GetRightDown()->setOpacity(alpha);
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
    
    object[x][y]->InitChild();
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


