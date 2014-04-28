#include "Puzzle.h"

enum
{
	zBackground = 0,
	zGameObject = 100,
    zGameObjectSP = 101,
    zPieceConn = 3,
};

int drop_order;
pthread_t thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
    
    // Fade Out (서서히 밝아진다)
    pBlackOpen->runAction(CCFadeOut::create(0.5f));
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
    CCDirector::sharedDirector()->end();
}

bool Puzzle::init()
{
    CCLog("puzzle init");
    
	if (CCLayer::init() == false)
	{
		return false;
	}
    
    sound = new Sound();
    sound->PreLoadInGameSound();
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
        sound->PlayBackgroundInGameSound();
    
    effect = new Effect();
    effect->Init(effect, this);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/game2.plist");
    CCLog("========================================");
    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    CCLog("========================================");
    
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
    std::vector<int> skillNum, skillProb, skillLv;
    skillNum.push_back(0);
    skillNum.push_back(8);
    skillNum.push_back(16);
    
    skillNum.push_back(1);
    skillNum.push_back(9);
    skillNum.push_back(17);
    
    skillNum.push_back(5);
    skillNum.push_back(13);
    skillNum.push_back(21);
    
    skillNum.push_back(4); // 정령
    skillNum.push_back(12); // 정령
    
    skillNum.push_back(10); // 콤보비례 추가점수
    skillNum.push_back(11); // 콤보비례 추가사탕
    
    
    skillNum.push_back(14); // 시간을 얼리다.

    skillNum.push_back(2); // 10개이상 추가점수
    skillNum.push_back(19); // 10개이상 추가별사탕

    skillNum.push_back(6); // coco time
    
    skillNum.push_back(7); // 막타
    skillNum.push_back(15);
    skillNum.push_back(23);
    
    for (int i = 0 ; i < skillNum.size() ; i++) {
        if (skillNum[i] == 6 || skillNum[i] == 14)
            skillProb.push_back(10);
        else
            skillProb.push_back(100);
        skillLv.push_back(4);
    }
    skill->Init(skillNum, skillProb, skillLv);
    
    /* color data 수집
    for (int i = 0; i <= 45; i++) {
        res_allCnt.push_back(0);
        std::vector<int> temp;
        for (int j = 0 ; j < 5; j++)
            temp.push_back(0);
        res_colorCnt.push_back(temp);
    }
    res_cycleCnt = 0;
    */
    
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
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
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
    
	return true;
}

void Puzzle::InitSprites()
{
    CCLog("Init Sprites");
    CCLog("Real : %f , %f", m_winSize.width, m_winSize.height);
    vs = CCDirector::sharedDirector()->getVisibleSize();
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    CCLog("visible : %f , %f", vs.width, vs.height);
    CCLog("visible point : %f , %f", vo.x, vo.y);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png1", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+vs.height-37.5f), CCSize(1075,75), "", "Puzzle", this, 5) );
    
    // 별사탕 숫자배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png1", ccp(0.5, 0.5), ccp(150, vo.y+vs.height-36), CCSize(160, 50), "", "Puzzle", this, 6) );
    
    // 별사탕
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(1, 0.5), ccp(150, vo.y+vs.height-30), CCSize(0, 0), "", "Puzzle", this, 20) );
    /*
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(0, 1), ccp(40, vs.height+vo.y-30), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_3.png", ccp(0, 1), ccp(10, vs.height+vo.y-17), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_4.png", ccp(0, 1), ccp(25, vs.height+vo.y-3), CCSize(0, 0), "", "Puzzle", this, 20) );
     */
    
    // 게이지바 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png2", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+vs.height-36), CCSize(164,35), "", "Puzzle", this, 6) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_bar_gauge.png", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+vs.height-36), CCSize(0,0), "", "Puzzle", this, 7) );
    // 게이지바 아이콘(모자)
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_gauge.png", ccp(1, 0.5), ccp(m_winSize.width-170-82, vo.y+vs.height-37.5f), CCSize(0, 0), "", "Puzzle", this, 8) );
    
    // pause button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_pause.png", ccp(1, 0.5), ccp(m_winSize.width-25, vs.height+vo.y-37.5f+5), CCSize(0, 0), "", "Puzzle", this, 20) );
    
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 1), ccp(30, vs.height+vo.y-10), CCSize(200, 75), "", "Puzzle", this, 19) );
    // 스코어
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 1), ccp(250, vs.height+vo.y-10), CCSize(500, 75), "", "Puzzle", this, 19) );
    
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 1), ccp(790, vs.height+vo.y-10), CCSize(200, 75), "", "Puzzle", this, 19) );
    
    // 값들 (별사탕 개수, 점수)
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 36, ccp(0.5, 0.5), ccp(100, vo.y+vs.height-37.5f), ccc3(255,255,255), "", "Puzzle", this, 20, 0, 255, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("0", fontList[0], 44, ccp(0.5, 0.5), ccp(m_winSize.width/2, vs.height+vo.y-37), ccc3(255,255,255), "", "Puzzle", this, 20, 0, 255, 2) );
    
    // timer bar
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_outer.png2", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f), CCSize(1044, 63), "", "Puzzle", this, 4) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_inner.png3", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1010, 35), "", "Puzzle", this, 4) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_bar_timer.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+31.5f+5), CCSize(1000, 22), "", "Puzzle", this, 4) );
    
    
    
    // timer bar
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_outer.png", ccp(0.5, 0), ccp(m_winSize.width/2+7, vo.y+35), CCSize(0, 0), "", "Puzzle", this, 4) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_inner.png", ccp(0.5, 0), ccp(m_winSize.width/2, vo.y+30), CCSize(0, 0), "", "Puzzle", this, 3) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bar_ice.png", ccp(0.5, 0), ccp(m_winSize.width/2, vo.y), CCSize(0, 0), "", "Puzzle", this, 3, 0, 0) );
    
    //tbSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    tbSize = CCSize(1044, 63+20);
    
    boardSize = CCSize(vs.height*1.920/2.920 - tbSize.height, vs.height*1.920/2.920 - tbSize.height);
    if (boardSize.height >= 1076.0f)
        boardSize = CCSize(1076.0f, 1076.0f);
    //CCLog("board size = %f", boardSize.height);
    puzzleLayer->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height/2));
    
    // puzzle board
//        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0, 0), ccp(57, vo.y+130), CCSize(0, 0), "", "Layer", puzzleLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0.5, 0.5), ccp(0, 0), CCSize(0, 0), "", "Layer", puzzleLayer, 20) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->setScale((float)boardSize.height/(float)1076);
    //else
    //    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0, 0), ccp(1, vo.y+130), CCSize(0, 0), "", "Layer", puzzleLayer, 1) );
        
    
    PIECE8_WIDTH = (float)152 * (float)boardSize.height/(float)1076;
    PIECE8_HEIGHT = (float)152 * (float)boardSize.height/(float)1076;
    PIECE8_FRAME_WIDTH = (float)154 * (float)boardSize.height/(float)1076;
    PIECE8_FRAME_HEIGHT = (float)154 * (float)boardSize.height/(float)1076;
    
    // 구름
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_cloud_near.png", ccp(1, 0), ccp(m_winSize.width, vo.y+vs.height*1.920f/2.920f+350), CCSize(0, 0), "", "Puzzle", this, 1) );
    // 배경 그림
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_upper.png", ccp(0, 0), ccp(0, vo.y+tbSize.height+boardSize.height-30), CCSize(0, 0), "", "Puzzle", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_grass.png", ccp(0, 1), ccp(0, vo.y+tbSize.height+boardSize.height+80), CCSize(0, 0), "", "Puzzle", this, 2) );
    
    // mission
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/mission.png", ccp(0, 0), ccp(46, 1290), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    // 코코 그림자, 요정 그림자
    //vs.height+vo.y-480
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/coco_shadow.png", ccp(0.5, 0.5), ccp(215, vo.y+tbSize.height+boardSize.height+95), CCSize(0, 0), "", "Puzzle", this, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "anim/fairy/sun_shadow.png", ccp(0.5, 0.5), ccp(m_winSize.width-170, vo.y+tbSize.height+boardSize.height+95), CCSize(0, 0), "", "Puzzle", this, 20) );
    ((CCSprite*)spriteClass->FindSpriteByName("anim/fairy/sun_shadow.png"))->setScale(1.25f);
    
    // 마법진
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/magic_circle.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height+120), CCSize(0, 0), "", "Puzzle", this, 1) );
    //vs.height+vo.y-440
    
    ((CCSprite*)spriteClass->FindSpriteByName("background/magic_circle.png"))->setScale(0.9f);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    // 잔디 액션
    CCFiniteTimeAction* action = CCSequence::create(CCMoveBy::create(1.0f, ccp(0, +3)), CCMoveBy::create(1.0f, ccp(0, -3)), NULL);
    CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    ((CCSprite*)spriteClass->FindSpriteByName("bg_grass.png"))->runAction(rep);
    
    
    /*
    pieceLayer = CCLayer::create();
    pieceLayer->setPosition(ccp(0, 0));
    CCSize visibleSize(1078, 1078);
    
    CCDrawNode* stencil = CCDrawNode::create();
    CCPoint ver[] = {ccp(0, 0), ccp(768, 0), ccp(768, 6*108+6), ccp(768-6-108+32, 6*108+6),
        ccp(768-6-108, 6*108+6+32), ccp(768-6-108, 7*108+6-32), ccp(768-6-108-32, 7*108+6),
        ccp(6+108+32, 7*108+6), ccp(6+108, 7*108+6-32), ccp(6+108, 6*108+6+32),
        ccp(6+108-32, 6*108+6), ccp(6+32, 6*108+6), ccp(6, 6*108+6-32)};
    stencil->drawPolygon(ver, 13, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    
    CCClippingNode* clip = CCClippingNode::create(stencil);
    clip->addChild(pieceLayer);
    addChild(clip);
    */
    
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
    //cocoLayer->setPosition(ccp(100, vs.height+vo.y-500));
    cocoLayer->setPosition(ccp(100, vo.y+tbSize.height+boardSize.height+60));
    this->addChild(cocoLayer, 1000);
    
    cocoFrameNumber = 0;
    
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
    
    CCSprite* face = CCSprite::createWithSpriteFrameName("anim/fairy/sun_face.png");
    face->setAnchorPoint(ccp(0.5, 0.5));
    face->setPosition(ccp(0, 0));
    fairyLayer->addChild(face, 1000);
    CCSprite* mouth = CCSprite::createWithSpriteFrameName("anim/fairy/sun_mouth.png");
    mouth->setAnchorPoint(ccp(0.5, 0.5));
    mouth->setPosition(ccp(0, -30));
    fairyLayer->addChild(mouth, 1000);
    
    CCLayer* eyeLayer = CCLayer::create();
    CCSprite* eye1 = CCSprite::createWithSpriteFrameName("anim/fairy/sun_eye.png");
    eye1->setAnchorPoint(ccp(0.5, 0.5));
    eye1->setPosition(ccp(0, 0));
    eyeLayer->addChild(eye1, 1000);
    CCSprite* eye2 = CCSprite::createWithSpriteFrameName("anim/fairy/sun_eye.png");
    eye2->setAnchorPoint(ccp(0.5, 0.5));
    eye2->setPosition(ccp(70, 0));
    eyeLayer->addChild(eye2, 1000);
    eyeLayer->setPosition(ccp(-50, 0));
    eyeLayer->setTag(0);
    fairyLayer->addChild(eyeLayer, 1000);
    
    float r = 110.0;
    float x, y;
    for (int i = 0 ; i < 8 ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("anim/fairy/sun_head.png");
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
        fairyLayer->addChild(temp, 1000);
    }
    
    fairyLayer->setScale(0.8f);
    //fairyLayer->setPosition(ccp(800, vs.height+vo.y-530));
    fairyLayer->setPosition(ccp(m_winSize.width-280, vo.y+tbSize.height+boardSize.height+60));
    this->addChild(fairyLayer, 1000);
    
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
    CCLog("Init board");
    
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
    
    drop_order = 0;
    touch_cnt = 0;
    
    m_bTouchStarted = false;
    m_iSkillSP = 0;
    m_bIsSpiritExecuted = false;
    m_iSpiritSP = 0;
}

void Puzzle::SetScoreAndStarCandy()
{
    iScore = 0;
    pScoreLabel = (CCLabelTTF*)spriteClass->FindLabelByTag(2);
    iStarCandy = 0;
    pStarCandyLabel = (CCLabelTTF*)spriteClass->FindLabelByTag(1);
}

void Puzzle::UpdateScore(int type, int data)
{
    if (type == 0)
    {
        // 기본점수 : (50 + 15(n-3)^1.2) * n
        iScore += ( (50 + 15*pow(data-3, 1.20)) * data );
    }
    else if (type == 1)
    {
        // 기본 스킬에 의한 추가점수 (A1)
        iScore += data;
    }
    char s[10];
    sprintf(s, "%d", iScore);
    pScoreLabel->setString(s);
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
    //CCLog("pos : %d %d", (int)pos.x, (int)pos.y+330);
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

void Puzzle::SetTimer()
{
    iTimer = 60000*5;
    pTimerLabel = CCLabelTTF::create("60", fontList[2].c_str(), 50);
    pTimerLabel->setAnchorPoint(ccp(0.5, 0.5));
    pTimerLabel->setPosition(ccp(40, 300));
    this->addChild(pTimerLabel);
    
    this->schedule(schedule_selector(Puzzle::UpdateTimer), 0.1f);
}

void Puzzle::UpdateTimer(float f)
{
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

    iTimer -= 100;
    if (iTimer % 1000 == 0)
    {
        char temp[3];
        sprintf(temp, "%d", iTimer/1000);
        pTimerLabel->setString(temp);
        
        if (iTimer == 0)
        {
            // game end
            this->unschedule(schedule_selector(Puzzle::UpdateTimer));
            this->setTouchEnabled(false);
            this->setKeypadEnabled(false);
            
            // 게임결과화면 보여줌
            //CCScene* pScene = PuzzleResult::scene();
            //this->addChild(pScene, 2000, 2000);
            
            // go to Ranking Scene
            sound->StopBackgroundSound();
            Common::ShowNextScene(this, "Puzzle", "Ranking", true);
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


bool Puzzle::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    //CCLog("Puzzle :: %d , %d", (int)point.x, (int)point.y);
    
    /*
    // 보드판과 상관없는 것들 (lock에 제한받지 않는 것들)
    // 1) pause button
    // pause button
    if (((CCSprite*)spriteClass->FindSpriteByName("icon/icon_pause.png"))->boundingBox().containsPoint(point))
    {
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
        
        return true;
    }
    */
    
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
        if (m_iSpiritSP == 0)
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
    if (m_iSkillSP > 0 || m_bIsSpiritExecuted)
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
    if (m_iSkillSP > 0 || m_bIsSpiritExecuted)
        return;

    if (m_bTouchStarted)
    {
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
            // 일단 정령 스킬부터 semaphore 증가.
            m_iSpiritSP++;
            
            // data log 남기는 부분 (어느 색깔을 몇 번 한붓그리기했나)
            //res_allCnt[piece8xy[touch_cnt%QUEUE_CNT].size()]++;
            //CCLog("global = %d", globalType[touch_cnt%QUEUE_CNT]);
            //CCLog("touch_cnt remain QUEUE_CNT = %d", touch_cnt%QUEUE_CNT);
            //CCLog("hmm? : %d", (int)piece8xy[touch_cnt%QUEUE_CNT].size());
            //res_colorCnt[piece8xy[touch_cnt%QUEUE_CNT].size()][globalType[touch_cnt%QUEUE_CNT]]++;
            //if (m_bIsCycle)
            //  res_cycleCnt++;
            
            // 다시 한붓그린 부분 lock을 푼다
            for (int i = 0 ; i < piece8xy[touch_cnt%QUEUE_CNT].size() ; i++)
            {
                int x = piece8xy[touch_cnt%QUEUE_CNT][i].x;
                int y = piece8xy[touch_cnt%QUEUE_CNT][i].y;
                m_bLockP8[x][y]--;
            }
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
            
            // sound bomb
            sound->PlayBomb();
            
            // 스킬 실행 (오토마타 표 참조)
            WaitThread((touch_cnt-1)%QUEUE_CNT);
            //skill->SetQueuePos((touch_cnt-1)%QUEUE_CNT);
            //m_iState[(touch_cnt-1)%QUEUE_CNT] = 1;
            //InvokeSkills((touch_cnt-1)%QUEUE_CNT);
            
            //m_bIsCycle[(touch_cnt-1)%QUEUE_CNT] = false; // cycle 푼다.
        }
        
        // 3개 미만으로 한붓그리기가 되었다면, 원상태로 복구시킨다.
        else
        {
            int x, y;
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
    }
}

void Puzzle::InvokeSkills(int queue_pos)
{
    if (m_iState[queue_pos] == 1)
    {
        CCLog("state(%d) 1", queue_pos);
        // W5(시간을얼리다)
        skill->Invoke(14, NULL);
        
        // 마지막(8번) 스킬이 적용되었다면,
        if (skill->IsApplied(7, queue_pos) || skill->IsApplied(15, queue_pos) || skill->IsApplied(23, queue_pos))
        {
            // cycle(2A)이 적용되었다면,
            if (skill->IsApplied(1, queue_pos) || skill->IsApplied(9, queue_pos) || skill->IsApplied(17, queue_pos))
            {
                m_iState[queue_pos] = 2;
            }
            else // 아니라면,
            {
                m_iState[queue_pos] = 3;
            }
        }
        else
        {
            // cycle이 적용됨과 관계없이, 한붓그리기 한 것을 터뜨리기
            m_iState[queue_pos] = 4;
        }
        
        InvokeSkills(queue_pos);
    }
    else if (m_iState[queue_pos] == 2) // 사이클 주변부 터뜨리기
    {
        CCLog("state(%d) 2", queue_pos);
        // '8번' 스킬 적용된 경우
        if (skill->IsApplied(7, queue_pos) || skill->IsApplied(15, queue_pos) || skill->IsApplied(23, queue_pos))
        {
            m_iNextState[queue_pos] = 3;
        }
        else
        {
            m_iNextState[queue_pos] = 5;
        }
   
        //WaitThread(queue_pos);
        //WaitOrder(queue_pos);
        
        // 8번, cycle 모두 적용된 경우 : cycle에서 나온 주변부 터뜨리기
        skill->Invoke(1, queue_pos); skill->Invoke(9, queue_pos); skill->Invoke(17, queue_pos);
    }
    else if (m_iState[queue_pos] == 3) // 마지막 8번 스킬
    {
        CCLog("state(%d) 3", queue_pos);
        m_iNextState[queue_pos] = 5;
        
        //WaitThread(queue_pos);
        //CCLog("state(%d) 3 : wait done, get started", queue_pos);
        //WaitOrder(queue_pos);
        
        // 8번 적용된 경우 : 8번 스킬 '준비' & '실행'
        skill->Invoke(7, queue_pos); skill->Invoke(15, queue_pos); skill->Invoke(23, queue_pos);
    }
    else if (m_iState[queue_pos] == 4) // 추가점수, +10 스킬 (기본 한붓그리기 폭발)
    {
        CCLog("state(%d) 4", queue_pos);
        if (skill->IsApplied(1, queue_pos) || skill->IsApplied(9, queue_pos) || skill->IsApplied(17, queue_pos))
        {
            m_iNextState[queue_pos] = 2;
        }
        else
        {
            m_iNextState[queue_pos] = 5;
        }
        
        Lock(queue_pos);
        
        // 기본 이펙트 적용
        effect->PlayEffect(-100, NULL, piece8xy[queue_pos]);
        
        // 추가점수 적용되었다면 실행한 후, 한붓그리기 한 것을 터뜨리기
        skill->Invoke(0, queue_pos); skill->Invoke(8, queue_pos); skill->Invoke(16, queue_pos); // 추가점수
        
        // 10개이상제거시추가점수(F3), 10개이상제거시추가별사탕(E4)
        skill->Invoke(2, queue_pos); skill->Invoke(19, queue_pos);
        
        Bomb(queue_pos, piece8xy[queue_pos]);
    }
    else if (m_iState[queue_pos] == 5) // 6개 이상 제거 시, 한번 더 제거
    {
        CCLog("state(%d) 5", queue_pos);
        m_iNextState[queue_pos] = 7;
        
        //WaitThread(queue_pos);
        //WaitOrder(queue_pos);
        
        // 6개 이상 제거 시, 한 번 더 제거
        skill->Invoke(5, queue_pos); skill->Invoke(13, queue_pos); skill->Invoke(21, queue_pos);

        if (!(skill->IsApplied(5, queue_pos) || skill->IsApplied(13, queue_pos) || skill->IsApplied(21, queue_pos)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    else if (m_iState[queue_pos] == 7)
    {
        CCLog("state(%d) 7", queue_pos);
        m_iNextState[queue_pos] = -1;
        //m_iNextState = 8;
        
        //WaitThread(queue_pos);
        //WaitOrder(queue_pos);
        
        // 코코타임
        skill->Invoke(6, queue_pos);
        
        if (!skill->IsApplied(6, queue_pos))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState[queue_pos] = m_iNextState[queue_pos];
            InvokeSkills(queue_pos);
        }
    }
    //else if (m_iState == 8)
    //{
        /*
        CCLog("state 8");
        m_iNextState = -1;
        
        // MT*2
        skill->Invoke(3);
        
        SetSkillLock(false);
        */
    //}
    
    else if (m_iState[queue_pos] == -1)
    {
        CCLog("state(%d) -1 : DONE", queue_pos);
        // 모든 스킬이 끝났다. 원상태로 되돌리자.
        
        // skill Lock을 푼다.
        if (m_bSkillLock[queue_pos])
        {
            SetSkillLock(queue_pos, false);
            m_iSkillSP--;
        }
        //SetSkillLock(queue_pos, false);
        
        // 다음 대기 중인 drop queue가 실행될 수 있도록 한다.
        drop_order++;
        
        m_iSpiritSP--;
        
        CCLog("WaitOrder (%d) : unLock", queue_pos);
        pthread_mutex_unlock(&mutex);
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

void Puzzle::Bomb(int queue_pos, std::vector<CCPoint> bomb_pos)
{
    CCLog("Bomb(%d)", queue_pos);
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
  
    // 8각형들을 터뜨린다.
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax[queue_pos] = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;

        /*
        // 8각형 주변의 diamond중에 터지지 않은 diamond를 마저 지운다.
        if (x > 0 && y > 0 && x < COLUMN_COUNT && y < ROW_COUNT) // leftdown
            puzzleP4set->RemoveChild(x, y);
        if (x > 0 && y+1 > 0 && x < COLUMN_COUNT && y+1 < ROW_COUNT) // leftup
            puzzleP4set->RemoveChild(x, y+1);
        if (x+1 > 0 && y > 0 && x+1 < COLUMN_COUNT && y < ROW_COUNT) // rightdown
            puzzleP4set->RemoveChild(x+1, y);
        if (x+1 > 0 && y+1 > 0 && x+1 < COLUMN_COUNT && y+1 < ROW_COUNT) // rightup
            puzzleP4set->RemoveChild(x+1, y+1);
        */
        float bombTime = 0.15f;
        if (m_iState[queue_pos] == 2)
            bombTime = 0.60f;
        
        // 터지는 액션
        CCFiniteTimeAction* action;
        if (m_iState[queue_pos] == 5 && globalType[queue_pos] == PIECE_BLUE) // 6개이상 한번더 : 터지고 살짝 딜레이를 준다.
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
                        CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                        NULL);
        }
        spriteP8[x][y]->runAction(action);
    }
    
    // show starcandy
    effect->ShowStarCandy(bomb_pos);

    // update score
    UpdateScore(0, bomb_pos.size());
    // update starcandy
    UpdateStarCandy(0, bomb_pos.size());
    // update combo
    UpdateCombo();
    
    // 터지는 게 없다면 바로 Falling을 시작한다.
    if (bomb_pos.size() == 0)
        Falling((int)queue_pos);
}

void Puzzle::BombCallback(CCNode* sender, void* queue_pos)
{
    m_iBombCallbackCnt[(int)queue_pos]++;
    
    if (m_iBombCallbackCnt[(int)queue_pos] == m_iBombCallbackCntMax[(int)queue_pos])
    {
        if (m_iState[(int)queue_pos] == 2) // cycle 주변부 폭발 완료
        {
            CCLog("bomb callback 2 (%d)", (int)queue_pos);
            std::vector<CCPoint> temp = skill->A2GetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->A2Clear();
        }
        else if (m_iState[(int)queue_pos] == 3) // 8번 스킬 폭발 완료
        {
            CCLog("bomb callback 3 (%d)", (int)queue_pos);
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
        else if (m_iState[(int)queue_pos] == 4) // 한붓그리기 부분 폭발 완료
        {
            CCLog("bomb callback 4 (%d)", (int)queue_pos);
            // 추가점수 , +10 은 따로 둔 callback에서 처리하도록 하자
            
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            
            if (m_iNextState[(int)queue_pos] == 2) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
            {
                CCLog("bomb callback 4 nextcycle (%d)", (int)queue_pos);
                m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
                
                InvokeSkills((int)queue_pos);
                piece8xy[(int)queue_pos].clear();
                
                return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
            }
            else
                piece8xy[(int)queue_pos].clear();
        }
        else if (m_iState[(int)queue_pos] == 5)
        {
            CCLog("bomb callback 5 (%d)", (int)queue_pos);
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A4BGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
        }
        else if (m_iState[(int)queue_pos] == 6)
        {
            CCLog("bomb callback 6 (%d)", (int)queue_pos);
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A4AGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
        }
        else if (m_iState[(int)queue_pos] == 7)
        {
            CCLog("bomb callback 7 (%d)", (int)queue_pos);
            //skill->F7_Callback(this, skill);
            skill->F7_Continue(skill, (int)queue_pos);
            return; // Falling은 F7 (스킬) 함수쪽에서 한다.
        }
        
        // 새로운 8각형 piece들의 drop을 시작한다.
        Falling((int)queue_pos);
    }
}

void Puzzle::Falling(int queue_pos)
{
    CCLog("Falling 들어옴 : queue_pos , drop_order : %d %d", queue_pos, drop_order%QUEUE_CNT);
    //WaitThread(queue_pos);
    //WaitOrder(queue_pos);

    CCLog("Falling : drop_order (%d) is started", drop_order%QUEUE_CNT);
    
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
                    m_numOfFallingObjects--;
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

void Puzzle::FallingCallback(CCNode* sender, void* queue_pos)
{
    int queue = (int)queue_pos;
    m_iFallingCallbackCnt++;
	if (m_numOfFallingObjects == m_iFallingCallbackCnt)
	{
        CCLog("Falling callback (%d) - start", queue);
        // drop이 모두 끝나면, diamond들을 다시 검사해서 적절히 바꿔준다.
		for (int x = 1 ; x < COLUMN_COUNT ; x++)
        {
            for (int y = 1 ; y < ROW_COUNT ; y++)
            {
                if (puzzleP4set->GetType(x, y) != BLOCKED) //m_pBoardSP[x][y]->GetType() != BLOCKED)
                {
                    if (puzzleP4set->GetObject(x, y) != NULL)
                        puzzleP4set->RemoveChild(x, y);
                    puzzleP4set->CreatePiece(x, y, puzzleP4set->GetType(x, y));
                    puzzleP4set->AddChild(x, y);
                }
            }
        }
        
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
        
        /************ LOCK print ****************/
        /*
        for (int y = ROW_COUNT-1 ; y >= 0 ; y--)
        {
            CCLog("%d %d %d %d %d %d %d", m_bLockP8[0][y], m_bLockP8[1][y], m_bLockP8[2][y],
                  m_bLockP8[3][y], m_bLockP8[4][y], m_bLockP8[5][y], m_bLockP8[6][y]);
        }
        */
        /*****************************************/

        // 마지막으로 current priority를 올려, 다음 대기 중인 drop queue가 실행될 수 있도록 한다.
        //if (!m_bSkillLock)
        //    drop_order++;
        
        // 다음 스킬로 넘어간다.
        m_iState[(int)queue_pos] = m_iNextState[(int)queue_pos];
        CCLog("Falling callback (%d) - 다음 스킬 : %d", queue, m_iState[(int)queue_pos]);
        InvokeSkills((int)queue_pos);
        
        // 아직 발동할 스킬이 남아있으면 실행한다.
        /*
        if (m_iNextState != -1)
        {
            m_iState = m_iNextState;
            InvokeSkills();
            return;
        }
        else
        {
            SetSkillLock(false);
        }
        */
    }
}



struct TempData
{
    int queue_pos;
    int drop_order;
};

//void* WaitOrder(void *arg)
void Puzzle::WaitOrder(int queue_pos)
{
    CCLog("WaitOrder (%d) : Try Lock", queue_pos);
    // queue에서 현재 순서(order)인 것을 찾는다. 없으면 대기한다.
    //while(pthread_mutex_trylock(&mutex) != 0);
    int trylock = pthread_mutex_lock(&mutex);
    CCLog("WaitOrder(%d) : trylock = %d", queue_pos, trylock);
    
    //if (queue_pos == drop_order%QUEUE_CNT)
    //{
    
        //int lock  = pthread_mutex_lock(&mutex);
        //CCLog("WaitOrder (%d) : lockNumber = %d", queue_pos, lock);
    //}
    CCLog("WaitOrder (%d) : %d %d", queue_pos, queue_pos, drop_order%QUEUE_CNT);

    skill->SetQueuePos((touch_cnt-1)%QUEUE_CNT);
    m_iState[(touch_cnt-1)%QUEUE_CNT] = 1;
    InvokeSkills((touch_cnt-1)%QUEUE_CNT);
    
    m_bIsCycle[(touch_cnt-1)%QUEUE_CNT] = false; // cycle 푼다.
    /*
    while(1)
    {
        if (queue_pos == drop_order%QUEUE_CNT)
            break;
        //CCLog("WaitOrder... (%d)", queue_pos);
        
    }
    */
    //if (queue_pos == drop_order%QUEUE_CNT)
    //{
    
    //}
    CCLog("WaitOrder (%d) : Done", queue_pos);
    
    //CCLog("WaitOrder (%d) : Thread exit", queue_pos);
    //pthread_exit(NULL);
    //return NULL;
}

void Puzzle::WaitThread(int queue_pos)
{
    WaitOrder(queue_pos);
    //pthread_t thread;
    //TempData* temp = new TempData();
    //temp->queue_pos = queue_pos;
    //temp->drop_order = drop_order;
    //thread = NULL;
    //pthread_create(&thread, NULL, &Puzzle::WaitOrder, (void*)queue_pos);
    //WaitOrde
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
    //CCString* param = CCString::create("0");
    //CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    //CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "GameReady");
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/game_bg.png");
    
    this->removeAllChildren();
//    scrollView->removeFromParent();
    
    //this->removeFromParentAndCleanup(true);
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


///////////////////////////////////////////////////////////////////////
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
    
    
    //!@#
    
  //  puzzleP8set->CreatePiece(x, y);
  //  spriteP8[x][y] = puzzleP8set->GetSprite(x, y);
//    puzzleP8set->AddChild(x, y);
    
    
    /*
    //PuzzleP8* temp = object[x1][y1];
    int tempType = object[x1][y1]->GetType();
    int tempZorder = object[x1][y1]->GetZOrder();
    //CCSprite* tempSprite =
    CCSprite* tempSprite1 = CCSprite::createWithTexture(object[x1][y1]->GetPiece()->getTexture());
    CCSprite* tempSprite2 = CCSprite::createWithTexture(object[x2][y2]->GetPiece()->getTexture());

    //object[x1][y1]->GetPiece()->removeFromParentAndCleanup(true);
    //CCTexture2D* tempTexture = object[x1][y1]->GetPiece()->getTexture();
    
    object[x1][y1]->SetType(object[x2][y2]->GetType());
    object[x1][y1]->SetZOrder(object[x2][y2]->GetZOrder());
    object[x1][y1]->SetPiece(tempSprite2);
    //object[x1][y1]->SetPieceTexture(object[x2][y2]->GetPiece()->getTexture());
    
    object[x2][y2]->SetType(tempType);
    object[x2][y2]->SetZOrder(tempZorder);
    object[x2][y2]->SetPiece(tempSprite1);
    
    CCLog("swap : (x1, y1) : (%d, %d)", (int)object[x1][y1]->GetPiece()->getPositionX(), (int)object[x1][y1]->GetPiece()->getPositionY());
    */
    
}

void PuzzleP8Set::Falling(int x, int y, int targetX, int targetY, int queue_pos)
{
    CCMoveTo* moveTo = CCMoveTo::create(0.1f, gameLayer->SetPiece8Position(targetX, targetY));
    CCFiniteTimeAction* action = CCSequence::create(
            moveTo,
            CCCallFuncND::create(gameLayer, callfuncND_selector(Puzzle::FallingCallback), (void*)queue_pos),
            NULL);
    object[x][y]->GetPiece()->runAction(action);
}

void PuzzleP8Set::FallingCompleted(CCNode* sender, void* queue_pos)
{
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
