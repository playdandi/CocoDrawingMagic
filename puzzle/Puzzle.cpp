#include "Puzzle.h"

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
}
void Puzzle::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
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
    
    spriteClass = new SpriteClass();
    
    puzzleLayer = new CCLayer();
    this->addChild(puzzleLayer, 10);
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
    
    skillNum.push_back(4);
    skillNum.push_back(12);
    skillNum.push_back(20);
    //skillNum.push_back(7);
    
    for (int i = 0 ; i < skillNum.size() ; i++) {
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
    SetScore();
    SetCombo();
    SetTimer();
    
    //    puzzleLayer->setScale((double)966/(double)1078);
    //CCMoveBy::create(2.0f, ccp(0, boardSize.height/2)),
    //CCFiniteTimeAction* action = CCSpawn::create(CCFlipY3D::create(2.0f), NULL);
    
    //CCFiniteTimeAction* action = CCRotateBy::create(2.0f, 0, -45);
    //CCAction* rep = CCRepeatForever::create((CCActionInterval*)action);
    //((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->runAction(rep);
    
    
    PlayEffect(100);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    isMagicTime = false;
    isFeverTime = false;
    
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
    
    
    // background
    CCTexture2D* bgCache = CCTextureCache::sharedTextureCache()->addImage("images/game_bg.png");
    CCSprite* bg = new CCSprite();
    bg->initWithTexture(bgCache, CCRectMake(0, 0, 1080, 1920));
    bg->setAnchorPoint(ccp(0, 1));
    bg->setPosition(ccp(0, vs.height+vo.y)); ///-(m_winSize.height-ds.height)/2));
    this->addChild(bg, 0);
    
    
    // 별사탕
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_2.png", ccp(0, 1), ccp(40, vs.height+vo.y-30), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_3.png", ccp(0, 1), ccp(10, vs.height+vo.y-17), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_4.png", ccp(0, 1), ccp(25, vs.height+vo.y-3), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 1), ccp(30, vs.height+vo.y-10), CCSize(200, 75), "", "Puzzle", this, 1) );
    // 스코어
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 1), ccp(250, vs.height+vo.y-10), CCSize(500, 75), "", "Puzzle", this, 1) );
    // 스킬게이지
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_coco.png", ccp(0, 1), ccp(750, vs.height+vo.y-10), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 1), ccp(790, vs.height+vo.y-10), CCSize(200, 75), "", "Puzzle", this, 1) );
    
    // 값들
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("972", fontList[0], 40, ccp(0.5, 0.5), ccp(130, vs.height+vo.y-10-30), ccc3(255,255,255), "", "Puzzle", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("1,596,394", fontList[0], 44, ccp(0.5, 0.5), ccp(500, vs.height+vo.y-10-30), ccc3(255,255,255), "", "Puzzle", this, 3) );
    
    // pause button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/pause.png", ccp(1, 1), ccp(m_winSize.width-5, vs.height+vo.y-5), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    
    // timer bar
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_outer.png", ccp(0.5, 0), ccp(m_winSize.width/2+7, vo.y+35), CCSize(0, 0), "", "Puzzle", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/timer_inner.png", ccp(0.5, 0), ccp(m_winSize.width/2, vo.y+30), CCSize(0, 0), "", "Puzzle", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bar_ice.png", ccp(0.5, 0), ccp(m_winSize.width/2, vo.y), CCSize(0, 0), "", "Puzzle", this, 3) );
    tbSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    
    boardSize = CCSize(vs.height*1.920/2.920 - tbSize.height, vs.height*1.920/2.920 - tbSize.height);
    if (boardSize.height >= 1078.0f)
        boardSize = CCSize(1078.0f, 1078.0f);
    //CCLog("board size = %f", boardSize.height);
    puzzleLayer->setPosition(ccp(m_winSize.width/2, vo.y+tbSize.height+boardSize.height/2));
    
    // puzzle board
//        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0, 0), ccp(57, vo.y+130), CCSize(0, 0), "", "Layer", puzzleLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0.5, 0.5), ccp(0, 0), CCSize(0, 0), "", "Layer", puzzleLayer, 1) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/board.png"))->setScale((float)boardSize.height/(float)1078);
    //else
    //    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/board.png", ccp(0, 0), ccp(1, vo.y+130), CCSize(0, 0), "", "Layer", puzzleLayer, 1) );
        
    
    PIECE8_WIDTH = (float)152 * (float)boardSize.height/(float)1078;
    PIECE8_HEIGHT = (float)152 * (float)boardSize.height/(float)1078;
    PIECE8_FRAME_WIDTH = (float)154 * (float)boardSize.height/(float)1078;;
    PIECE8_FRAME_HEIGHT = (float)154 * (float)boardSize.height/(float)1078;;
    
    /*
    // (temporary) fever time button
    fever = CCSprite::createWithSpriteFrameName("pieces/mt_1.png");
    fever->setScale(0.6f);
    fever->setAnchorPoint(ccp(0, 0));
    fever->setPosition(ccp(850, 1680));
    this->addChild(fever, 10);
    */
    
    // mission
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/mission.png", ccp(0, 0), ccp(46, 1290), CCSize(0, 0), "", "Puzzle", this, 1) );
    // 코코 그림자, 마법진
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/coco_shadow.png", ccp(0, 0), ccp(155, vs.height+vo.y-480), CCSize(0, 0), "", "Puzzle", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/magic_circle.png", ccp(0, 0), ccp(330, vs.height+vo.y-500), CCSize(0, 0), "", "Puzzle", this, 1) );
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    //effect->SetSpirit(0);
    //effect->SetSpirit(1);
    //effect->SetSpirit(2);
    //CCLog("Init Sprites Done");
    
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
    // 오른쪽 상단 (물5: 파도타기)
    CCTexture2D* bg = CCTextureCache::sharedTextureCache()->addImage("images/bg2.png");
    pado = new CCSprite();
    pado->initWithTexture(bg, CCRectMake(0, 0, 90, 90));
    pado->setAnchorPoint(ccp(0, 0));
    pado->setColor(ccc3(80, 80, 80));
    pado->setPosition(ccp(768-90, 768-90));
    this->addChild(pado);
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
    //cocoLayer->setPosition(ccp(100, 1380));
    
    
    
    cocoLayer->setPosition(ccp(100, vs.height+vo.y-500));
    
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
    
    // 그림자
    fairyShadow = CCSprite::createWithSpriteFrameName("anim/fairy/sun_shadow.png");
    fairyShadow->setAnchorPoint(ccp(0.5, 0.5));
    fairyShadow->setPosition(ccp(0, -150));
    fairyShadow->setScale(1.25f);
    fairyLayer->addChild(fairyShadow, 999);
    
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
    //fairyLayer->setPosition(ccp(800, 1350));
    fairyLayer->setPosition(ccp(800, vs.height+vo.y-530));
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
    }
    drop_order = 0;
    touch_cnt = 0;
    cur_priority = 0;
    max_priority = 0;
    
    m_bTouchStarted = false;
    m_bIsCycle = false;
    m_bSkillLock = false;
    isPadoRunning = false;
}

void Puzzle::SetScore()
{
    /*
    iScore = 0;
    pScoreLabel = CCLabelTTF::create("0", fontList[2].c_str(), 50);
    pScoreLabel->setAnchorPoint(ccp(0, 0));
    pScoreLabel->setPosition(ccp(PIECE8_WIDTH-6, 8*PIECE8_HEIGHT-6));
    this->addChild(pScoreLabel);
    */
}

void Puzzle::UpdateScore(int numOfPiece)
{
    /*
    iScore += numOfPiece*100;
    char s[10];
    sprintf(s, "%d", iScore);
    pScoreLabel->setString(s);
     */
}

void Puzzle::SetCombo()
{
    /*
    iCombo = 0;
    pComboLabel = CCLabelTTF::create("", fontList[2].c_str(), 50);
    pComboLabel->setAnchorPoint(ccp(0, 0));
    pComboLabel->setPosition(ccp(PIECE8_WIDTH-6, 7*PIECE8_HEIGHT+6+10));
    pComboLabel->setOpacity(0);
    this->addChild(pComboLabel);
    */
}

int Puzzle::GetCombo()
{
    return iCombo;
}

void Puzzle::UpdateCombo()
{
    /*
    iCombo++;
    char temp[11];
    sprintf(temp, "%d Combo!", iCombo);
    pComboLabel->setString(temp);
    pComboLabel->setOpacity(255);
    
    iComboTimer = 0;
    this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    if (iCombo == 1)
    {
        // 처음 콤보가 적용될 때만 타이머 적용을 하면 된다.
        this->schedule(schedule_selector(Puzzle::ComboTimer), 0.1f);
    }
     */
    
    //CCFiniteTimeAction* action =
    //    CCSequence::create(CCFadeIn::create(0.15f), CCFadeOut::create(0.15f), NULL);
    //pComboLabel->runAction(action);
}

void Puzzle::ComboTimer(float f)
{
    iComboTimer += 100;
    if (iComboTimer >= 3000)
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
    pTimerLabel->setPosition(ccp(40, 240));
    this->addChild(pTimerLabel);
    
    this->schedule(schedule_selector(Puzzle::UpdateTimer), 0.1f);
}

void Puzzle::UpdateTimer(float f)
{
    // Magic Time 발동 중에는 시간이 정지된다.
    if (isMagicTime)
        return;
    
    /*
    if (skill->W5GetVar()) { // "W5_시간얼리기" 스킬이 적용 중이면 2배속 느리게.
        CCLog("update timer -50");
        iTimer -= 50;
    }
    else
        iTimer -= 100;
     */
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
        
        // 정령 준비 발동
        skill->SpiritTry(0);
        skill->SpiritTry(1);
        skill->SpiritTry(2);
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
    cocoFrameNumber = 0;
    
    if (m_bSkillLock) // skill 발동 중이면 무조건 터치 금지.
        return false;
    
    if (m_bTouchStarted)
        return false;
    m_bTouchStarted = true; // touch lock 걸기
    
    CCPoint point = pTouch->getLocation();
    CCLog("Puzzle :: %d , %d", (int)point.x, (int)point.y);
    
    // pause button
    if (((CCSprite*)spriteClass->FindSpriteByName("background/pause.png"))->boundingBox().containsPoint(point))
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
    
    /*
    if (pado->boundingBox().containsPoint(point))
    {
        if (isPadoRunning)
            return false;
        isPadoRunning = true;
        
        m_bTouchStarted = true;
        m_bSkillLock = true;
        
        // 물 5 : 파도타기
        //skill->Try(12);
        skill->Invoke(12);
        
        m_pado_callback_cnt = 0;
        
        std::vector<CCPoint> t = skill->A4AGetPos();
        int x, y;
        for (int i = 0 ; i < t.size() ; i++) {
            x = (int)t[i].x;
            y = (int)t[i].y;
            CCFiniteTimeAction* action = CCSequence::create(CCScaleTo::create(0.2f, 0.0f),
                CCCallFuncND::create(this, callfuncND_selector(Puzzle::tCB), NULL), NULL);
            m_pBoard[x][y]->runAction(action);
        }
        
        if (t.size() == 0) {
            m_bSkillLock = false;
            m_bTouchStarted = false;
            isPadoRunning = false;
        }
        
        return false;
    }
    */
    
    // 어느 퍼즐인지 찾기
    CCPoint boardPos = BoardStartPosition(point);
    int x = (int)boardPos.x;
    int y = (int)boardPos.y;
    
    if (x == -1 && y == -1) // 찾지 못했다면 퍼즐 피스를 클릭한 것이 아니므로 그냥 종료.
        return (m_bTouchStarted = false);
    
    if (m_bLockP8[x][y] > 0) // locked면 중지.
        return (m_bTouchStarted = false);
    
    
    // 3가지 정령 중 하나를 터치할 때 동작한다.
    if ((x == 0 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) || (x == COLUMN_COUNT && y == 0))
    {
        if (x == 0 && y == ROW_COUNT-1)
            skill->Invoke(4);
        else if (x == COLUMN_COUNT-1 && y == ROW_COUNT-1)
            skill->Invoke(12);
        else if (x == COLUMN_COUNT-1 == y == 0)
            skill->Invoke(20);
        return true;
    }
    
    
    m_bIsCycle = false;
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

    // global piece type 지정
    globalType[touch_cnt%QUEUE_CNT] = puzzleP8set->GetType(x, y);

    // 0.9배 축소 action
    spriteP8[x][y]->setScale(spriteP8[x][y]->getScale() * 0.9f);
    spriteP8[x][y]->setOpacity(100);
    
    sound->PlayPieceClick(piece8xy[touch_cnt%QUEUE_CNT].size());
    
    return true;
}

void Puzzle::tCB(CCNode* sender, void* pos)
{
    /*
    m_pado_callback_cnt++;
    
    if (m_pado_callback_cnt == skill->A4AGetPos().size())
    {
        int x, y;
        std::vector<CCPoint> t = skill->A4AGetPos();
        for (int i = 0 ; i < t.size() ; i++) {
            x = (int)t[i].x;
            y = (int)t[i].y;
            pieceLayer->removeChild(m_pBoard[x][y], true);
            m_pBoard[x][y] = NULL;
            m_pBoard[x][y] = PuzzleP8::Create(this, 3);
            m_pBoard[x][y]->setPosition(SetPiece8Position(x, y));
            pieceLayer->addChild(m_pBoard[x][y], zGameObject);
        }
        
        m_bSkillLock = false;
        m_bTouchStarted = false;
        isPadoRunning = false;
    }
     */
}


void Puzzle::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_bTouchStarted && !m_bIsCycle)
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
                    m_bIsCycle = true;
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
                    m_bIsCycle = true;
                }
            }
            
            if (flag)
            {
                if (m_bIsCycle)
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
    if (m_bTouchStarted)
    {
        // 3개 이상 한붓그리기가 되었다면, 스킬 적용 여부를 판단하고, 콤보 처리와 함께 bomb를 시작한다.
        if (piece8xy[touch_cnt%QUEUE_CNT].size() >= 3)
        {
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
            
            for (int i = 0 ; i < strap[touch_cnt%QUEUE_CNT].size() ; i++)
                strap[touch_cnt%QUEUE_CNT][i]->removeFromParentAndCleanup(true);
            strap[touch_cnt%QUEUE_CNT].clear();
            
            // 스킬 발동 try
            skill->TrySkills(globalType[touch_cnt%QUEUE_CNT]);
            
            m_iBombCallbackType[touch_cnt%QUEUE_CNT] = 0;
            m_bTouchStarted = false;
            
            touch_cnt++;
            
            // effect 체크 초기화
            effect->InitCheck();
            
            // sound bomb
            sound->PlayBomb();
            
            // 스킬 실행 (오토마타 표 참조)
            m_iState = 1;
            InvokeSkills();
            
            m_bIsCycle = false;
        }
        
        // 3개 미만으로 한붓그리기가 되었다면, 원상태로 복구시킨다.
        else
        {
            int x, y;
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
            m_bIsCycle = false;
        }
    }
}

void Puzzle::InvokeSkills()
{
    if (m_iState == 1)
    {
        CCLog("state 1");
        // W5(시간을얼리다), E5(끈질긴생명력)
        skill->Invoke(14);
        skill->Invoke(22);
        
        // 마지막(8번) 스킬이 적용되었다면,
        if (skill->IsApplied(7) || skill->IsApplied(15) || skill->IsApplied(23))
        {
            // cycle(2A)이 적용되었다면,
            if (skill->IsApplied(1) || skill->IsApplied(9) || skill->IsApplied(17))
            {
                m_iState = 2;
            }
            else // 아니라면,
            {
                m_iState = 3;
            }
        }
        else
        {
            // cycle이 적용됨과 관계없이, 한붓그리기 한 것을 터뜨리기
            m_iState = 4;
        }
        
        InvokeSkills();
    }
    else if (m_iState == 2) // 사이클 주변부 터뜨리기
    {
        CCLog("state 2");
        if (skill->IsApplied(7) || skill->IsApplied(15) || skill->IsApplied(23)) // 8번 적용된 경우 8번 '준비'
        {
            m_iNextState = 3;
        }
        else
        {
            m_iNextState = 5;
        }
        
        /*
        // 물 스킬 사이클이 적용된 경우, 파도가 나아갈 방향에 미리 피스를 생성해 둔다. (터진 부분에만)
        if (skill->IsApplied(9))
        {
            std::vector<CCPoint> pos = piece8xy[(touch_cnt-1)%QUEUE_CNT];
            
            CCPoint last = pos[pos.size()-1];
            CCPoint before = pos[pos.size()-2];
            CCPoint delta = ccp((int)last.x-(int)before.x, (int)last.y-(int)before.y);
            
            int size = skill->GetSkillLevel(9);
            int cnt = 0;
            int x = (int)last.x + (int)delta.x;
            int y = (int)last.y + (int)delta.y;
            while (cnt < size && !(x == (int)last.x && y == (int)last.y))
            {
                if ((x == 0 && y == 0) || (x == 0 && y == ROW_COUNT-1) || (x == COLUMN_COUNT-1 && y == 0) || (x == COLUMN_COUNT-1 && y == ROW_COUNT-1))
                {
                    x += (int)delta.x;
                    y += (int)delta.y;
                }
                if (x < 0) x = COLUMN_COUNT-1;
                if (x >= COLUMN_COUNT) x = 0;
            }
            
            //int queue_pos = (touch_cnt-1)%QUEUE_CNT;
            //piece8xy[(int)queue_pos].clear();
        }
        */
        // 8번, cycle 모두 적용된 경우 : cycle에서 나온 주변부 터뜨리기
        skill->Invoke(1); skill->Invoke(9); skill->Invoke(17);
    }
    else if (m_iState == 3) // 마지막 8번 스킬
    {
        CCLog("state 3");
        m_iNextState = 5;
        
        // 8번 적용된 경우 : 8번 스킬 '준비' & '실행'
        skill->Invoke(7); skill->Invoke(15); skill->Invoke(23);
    }
    else if (m_iState == 4) // 추가점수, +10 스킬 (기본 한붓그리기 폭발)
    {
        CCLog("state 4");
        if (skill->IsApplied(1) || skill->IsApplied(9) || skill->IsApplied(17))
        {
            m_iNextState = 2;
        }
        else
        {
            m_iNextState = 5;
        }
        
        Lock((touch_cnt-1)%QUEUE_CNT);
        
        // 기본 이펙트 적용
        effect->PlayEffect(-100, piece8xy[(touch_cnt-1)%QUEUE_CNT]);
        
        // 추가점수, +10 스킬 적용되었다면 실행한 후, 한붓그리기 한 것을 터뜨리기
        skill->Invoke(0); skill->Invoke(8); skill->Invoke(16); // 추가점수
        skill->Invoke(11); skill->Invoke(19); // +10
        
        Bomb(piece8xy[(touch_cnt-1)%QUEUE_CNT]);
    }
    else if (m_iState == 5) // 6개 이상 제거 시, 한번 더 제거
    {
        CCLog("state 5");
        m_iNextState = 6;
        
        // 6개 이상 제거 시, 한 번 더 제거
        skill->Invoke(5); skill->Invoke(13); skill->Invoke(21);

        if (!(skill->IsApplied(5) || skill->IsApplied(13) || skill->IsApplied(21)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
    }
    else if (m_iState == 6)
    {
        CCLog("state 6");
        m_iNextState = 7;
        m_iState = m_iNextState;
        
        InvokeSkills();
        
        /*
        // 불지르기, 물내리기, 땅꺼지기
        skill->Invoke(4); skill->Invoke(20); //skill->Invoke(12); skill->Invoke(20);
        
        if (!(skill->IsApplied(4) || skill->IsApplied(12) || skill->IsApplied(20)))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
         */
    }
    else if (m_iState == 7)
    {
        CCLog("state 7");
        m_iNextState = 8;
        
        // 코코타임
        skill->Invoke(6);
        
        if (!skill->IsApplied(6))
        {
            // 스킬이 발동되지 않았으면 다음으로 넘긴다.
            m_iState = m_iNextState;
            InvokeSkills();
        }
    }
    else if (m_iState == 8)
    {
        CCLog("state 8");
        m_iNextState = -1;
        
        // MT*2
        skill->Invoke(3);
        
        SetSkillLock(false);
    }
}

void Puzzle::Lock(int queue_pos)
{
    // 스킬 발동이 된 경우는 이미 전체가 lock이므로 이 함수를 실행할 필요가 없다.
    //if (m_bSkillLock)
    //    return;
    CCLog("Lock() - state : %d", m_iState);
    
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

void Puzzle::Bomb(std::vector<CCPoint> bomb_pos)
{
    CCLog("Bomb");
    int queue_pos = (touch_cnt-1)%QUEUE_CNT;

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
  
    // 8각형들을 터뜨린다.
    m_iBombCallbackCnt[queue_pos] = 0;
    m_iBombCallbackCntMax = bomb_pos.size();
    for (int i = 0 ; i < bomb_pos.size() ; i++)
    {
        x = (int)bomb_pos[i].x;
        y = (int)bomb_pos[i].y;

        // 8각형 주변의 diamond중에 터지지 않은 diamond를 마저 지운다.
        if (x > 0 && y > 0 && x < COLUMN_COUNT && y < ROW_COUNT) // leftdown
            puzzleP4set->RemoveChild(x, y);
        if (x > 0 && y+1 > 0 && x < COLUMN_COUNT && y+1 < ROW_COUNT) // leftup
            puzzleP4set->RemoveChild(x, y+1);
        if (x+1 > 0 && y > 0 && x+1 < COLUMN_COUNT && y < ROW_COUNT) // rightdown
            puzzleP4set->RemoveChild(x+1, y);
        if (x+1 > 0 && y+1 > 0 && x+1 < COLUMN_COUNT && y+1 < ROW_COUNT) // rightup
            puzzleP4set->RemoveChild(x+1, y+1);
        
        float bombTime = 0.15f;
        if (m_iState == 2)
            bombTime = 0.60f;
        
        /*
        // 터뜨리는 action
        if (m_iState == 2 && globalType[queue_pos] == 2)
        {
            //CCActionInterval* ripple = CCRipple3D::create(bombTime, CCSizeMake(30, 30), SetPiece8Position(x, y), PIECE8_WIDTH/2, 2, 10);
            //CCActionInterval* ripple = CCMoveBy::create(bombTime, ccp(100, 0));
            CCActionInterval* ripple = CCLiquid::create(bombTime, CCSizeMake(30, 30), 4, 160);
            CCFiniteTimeAction* action = CCSequence::create(
                CCSpawn::create(ripple, CCScaleTo::create(bombTime, 0.3f), CCFadeOut::create(bombTime), NULL),
                CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                NULL);
            spriteP8[x][y]->runAction(action);
        }
        else
        {*/
            CCFiniteTimeAction* action = CCSequence::create(
                CCSpawn::create(CCScaleTo::create(bombTime, 1.5f), CCFadeOut::create(bombTime), NULL),
                CCCallFuncND::create(this, callfuncND_selector(Puzzle::BombCallback), (void*)queue_pos),
                NULL);
            spriteP8[x][y]->runAction(action);
        //}
    }

    // update score
    UpdateScore(bomb_pos.size());
    // update combo
    UpdateCombo();
    
    // 터지는 게 없다면 바로 Falling을 시작한다.
    if (bomb_pos.size() == 0)
        Falling((int)queue_pos);
}

void Puzzle::BombCallback(CCNode* sender, void* queue_pos)
{
    m_iBombCallbackCnt[(int)queue_pos]++;
    
    if (m_iBombCallbackCnt[(int)queue_pos] == m_iBombCallbackCntMax)
    {
        if (m_iState == 2) // cycle 주변부 폭발 완료
        {
            CCLog("bomb callback 2");
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A2AGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
            //skill->A2AClear();
        }
        else if (m_iState == 3) // 8번 스킬 폭발 완료
        {
            CCLog("bomb callback 3");
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A8GetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
            //skill->A8Clear();
        }
        else if (m_iState == 4) // 한붓그리기 부분 폭발 완료
        {
            CCLog("bomb callback 4");
            // 추가점수 , +10 은 따로 둔 callback에서 처리하도록 하자
            
            for (int i = 0 ; i < piece8xy[(int)queue_pos].size() ; i++)
            {
                int x = (int)piece8xy[(int)queue_pos][i].x;
                int y = (int)piece8xy[(int)queue_pos][i].y;
                puzzleP8set->RemoveChild(x, y);
                spriteP8[x][y] = NULL;
            }
            
            if (m_iNextState == 2) // 그 다음이 cycle 주변부를 바로 터뜨려야 한다면
            {
                CCLog("bomb callback 4 --> next : cycle boundary");
                m_iState = m_iNextState;
                
                InvokeSkills();
                piece8xy[(int)queue_pos].clear();
                
                return; // drop을 하지 않고 바로 터뜨려야 하므로, Falling()으로 넘어가지 않는다.
            }
            else
                piece8xy[(int)queue_pos].clear();
        }
        else if (m_iState == 5)
        {
            CCLog("bomb callback 5");
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A4BGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
            //skill->A4BClear(); // 6개 이상 한 번 더 터진 후
        }
        else if (m_iState == 6)
        {
            CCLog("bomb callback 6");
            std::vector<CCPoint> temp = skill->GetResult(); //skill->A4AGetPos();
            for (int i = 0 ; i < temp.size() ; i++)
            {
                puzzleP8set->RemoveChild((int)temp[i].x, (int)temp[i].y);
                spriteP8[(int)temp[i].x][(int)temp[i].y] = NULL;
            }
            temp.clear();
            skill->ResultClear();
            //skill->A4AClear();
        }
        else if (m_iState == 7)
        {
            CCLog("bomb callback 7");
        }
        
        // 새로운 8각형 piece들의 drop을 시작한다.
        Falling((int)queue_pos);
    }
}

void Puzzle::Falling(int queue_pos)
{
    // queue에서 현재 순서(order)인 것을 찾는다. 없으면 대기한다.
    while (queue_pos == drop_order)
    {
        break;
    }
    
    CCLog("Falling : priority (%d) is started", drop_order);
    
    
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
    //CCLog("total falling objects : %d", m_numOfFallingObjects);
    
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
        if (!m_bSkillLock)
            drop_order++;
        
        // 아직 발동할 스킬이 남아있으면 실행한다.
        if (m_iNextState != -1)
        {
            m_iState = m_iNextState;
            InvokeSkills();
        }
        else
        {
            SetSkillLock(false);
        }
    }
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
/*
int Puzzle::GetP8Type(int x, int y)
{
    return m_pBoard[x][y]->GetType();
}


PuzzleP8* Puzzle::GetP8(int x, int y)
{
    return m_pBoard[x][y];
}
*/
void Puzzle::SetSkillLock(bool flag)
{
    m_bSkillLock = flag;
}

bool Puzzle::IsCycle()
{
    return m_bIsCycle;
}

Sound* Puzzle::GetSound()
{
    return sound;
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

void Puzzle::PlayEffect(int skillNum)
{
    CCLog("Puzzle :: PlayEffect - %d", skillNum);
    if (skillNum == 100)
        effect->PlayEffect(skillNum, piece8xy[0]);
    else if (skillNum == 1 || skillNum == 17)
        effect->PlayEffect(skillNum, skill->GetResult()); //skill->A2AGetPos());
    else if (skillNum == 9)
        effect->PlayEffect(skillNum, skill->GetResult(), (touch_cnt-1)%QUEUE_CNT); //skill->A2AGetPos(), (touch_cnt-1)%QUEUE_CNT);
    else if (skillNum == 5 || skillNum == 13 || skillNum == 21)
        effect->PlayEffect(skillNum, skill->GetResult()); //skill->A4BGetPos());
    else if (skillNum == 7 || skillNum == 15 || skillNum == 23)
        effect->PlayEffect(skillNum, skill->GetResult()); //skill->A8GetPos());
    else
        effect->PlayEffect(skillNum, piece8xy[(touch_cnt-1)%QUEUE_CNT]);
}

void Puzzle::SwapSpriteP8(int x1, int y1, int x2, int y2)
{
    spriteP8[x1][y1] = puzzleP8set->GetSprite(x1, y1);
    spriteP8[x2][y2] = puzzleP8set->GetSprite(x2, y2);
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

void PuzzleP8Set::CreatePiece(int x, int y)
{
    object[x][y] = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x, y), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1078);
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
    PuzzleP8* p1 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x1, y1), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1078, object[x1][y1]->GetType());
    PuzzleP8* p2 = PuzzleP8::CreateP8(ccp(0.5, 0.5), gameLayer->SetPiece8Position(x2, y2), gameLayer, zGameObject, gameLayer->GetBoardSize()/(float)1078, object[x1][y1]->GetType());
    
    RemoveChild(x1, y1);
    RemoveChild(x2, y2);
    object[x1][y1] = p1;
    object[x2][y2] = p2;
    gameLayer->SwapSpriteP8(x1, y1, x2, y2);
    
    
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
