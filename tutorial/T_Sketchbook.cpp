#include "T_Sketchbook.h"

static int froms; // 0 : from Ranking , 1 : from GameReady

CCScene* T_Sketchbook::scene(int fromWhere)
{
    froms = fromWhere;
    
    CCScene* pScene = CCScene::create();
    T_Sketchbook* pLayer = T_Sketchbook::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void T_Sketchbook::onEnter()
{
    CCLog("T_Sketchbook : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void T_Sketchbook::onExit()
{
    CCLog("T_Sketchbook : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}


void T_Sketchbook::keyBackClicked()
{
}

static int offsets = -80;

bool T_Sketchbook::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_Sketchbook", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("T_스케치북 : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(T_Sketchbook::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // scrollview init.
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 914-40+offsets));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 5);
    
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 228+12));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollViewSlot, 5);
    
    spriteClass = new SpriteClass();
    spriteClassBook = new SpriteClass();
    spriteClassSlot = new SpriteClass();
    
    InitSprites();
    MakeScrollSlot();
    
    // 연습 중인 스킬이 속한 속성을 우선적으로 보여주는데, 만약 없다면 불/물/땅/궁극 순서대로 연 것 먼저 보여준다.
    int pcsi = myInfo->GetPracticeSkillId();
    if (21 <= pcsi && pcsi <= 28) tabNumber = 0;
    else if (11 <= pcsi && pcsi <= 18) tabNumber = 1;
    else if (31 <= pcsi && pcsi <= 38) tabNumber = 2;
    else if (41 <= pcsi && pcsi <= 45) tabNumber = 3;
    else {
        if (myInfo->IsFire()) tabNumber = 0;
        else if (myInfo->IsWater()) tabNumber = 1;
        else if (myInfo->IsLand()) tabNumber = 2;
        else if (myInfo->IsMaster()) tabNumber = 3;
        else tabNumber = 0;
    }
    curState = -1;
    MakeScroll(tabNumber);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    
    ttr = new CCLayer();
    this->addChild(ttr, 1000);
    
    ttrBg = CCSprite::create("images/tutorial_explain.png");
    ttrBg->setAnchorPoint(ccp(0, 0));
    ttrBg->setPosition(ccp(vo.x, vo.y));
    ttrBg->setScaleX(winSize.width / (float)637);
    ttrBg->setScaleY((float)206 / (float)130);
    ttr->addChild(ttrBg, 100);
    
    ttrArrow = CCSprite::create("images/tutorial_arrow.png");
    ttrArrow->retain();
    
    ttrPos = CCSprite::create("images/tutorial_position.png");
    ttrPos->retain();
    
    ttrCoco = CCSprite::createWithSpriteFrameName("image/coco_ready.png");
    ttrCoco->setAnchorPoint(ccp(0, 0));
    ttrCoco->setPosition(ccp(vo.x, vo.y+5));
    ttr->addChild(ttrCoco, 101);
    
    ttrMsg = CCLabelTTF::create("", fontList[0].c_str(), 32, CCSize(700, 100), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    ttrMsg->setAnchorPoint(ccp(0, 0.5));
    ttrMsg->setPosition(ccp(vo.x+320, vo.y+103));
    ttrMsg->setColor(ccc3(78,47,8));
    ttr->addChild(ttrMsg, 101);
    
    tutorialState = -1;
    TutorialNextState();
    
    return true;
}

void T_Sketchbook::TutorialNextState()
{
    char temp[200];
    tutorialState++;
    
    // 붙어있는 sprite 미리 제거
    if ((tutorialState-1 >= 1 && tutorialState-1 <= 3) || tutorialState-1 == 6 || tutorialState-1 == 8 || tutorialState-1 == 10)
        ttrPos->removeFromParent();
    if (tutorialState-1 == 3 || tutorialState-1 == 10)
        ttrArrow->removeFromParent();
    
    int sNum;
    std::string prop;
    if (tabNumber == 0) { sNum = 21; prop = "불"; }
    else if (tabNumber == 1) { sNum = 11; prop = "물"; }
    else if (tabNumber == 2) { sNum = 31; prop = "땅"; }
    
    CCLog("TutorialNextState() : %d", tutorialState);
    
    switch (tutorialState)
    {
        case 0:
            ttrMsg->setString("여기는 내가 그릴 줄 아는 마법을 모아 놓은 스케치북이야.");
            break;
        case 1:
            sprintf(temp, "나는 %s 속성 마법사이고 '%s'을 그릴 수 있어.", prop.c_str(), SkillInfo::GetSkillInfo(sNum)->GetName().c_str());
            ttrMsg->setString(temp);
            MakeScroll(curState);
            break;
        case 2:
            sprintf(temp, "새로운 마법인 '%s' 스킬을 배워볼까?", SkillInfo::GetSkillInfo(sNum+1)->GetName().c_str());
            ttrMsg->setString(temp);
            MakeScroll(curState);
            break;
        case 3:
            ttrMsg->setString("오른쪽에 있는 습득 버튼을 눌러봐!");
            MakeScroll(curState);
            break;
        case 4:
            ttrMsg->setString("조건이 충족되었으니까 습득 버튼을 눌러!"); // SketchDetail
            break;
        case 5:
            ttrMsg->setString("새로운 마법을 배웠어!"); // NoImage
            break;
        case 6:
            sprintf(temp, "'%s' 스킬에는 자동효과라고 되어있지?\n이것은 장착하지 않아도 자동으로 발동해.", SkillInfo::GetSkillInfo(sNum)->GetName().c_str());
            ttrMsg->setString(temp);
            ttrArrow->setRotation(0);
            MakeScroll(curState);
            break;
        case 7:
            sprintf(temp, "하지만 새로 배운 '%s' 스킬은 슬롯에 장착해야 쓸 수 있어.", SkillInfo::GetSkillInfo(sNum+1)->GetName().c_str());
            ttrMsg->setString(temp);
            break;
        case 8:
            ttrMsg->setString("아래쪽에 있는 바를 눌러봐.");
            
            ttrBg->setPosition(ccp(vo.x, 458));
            ttrCoco->setPosition(ccp(vo.x, 458+5));
            ttrMsg->setPosition(ccp(vo.x+320, 458+103));
            
            ttrPos->setAnchorPoint(ccp(0, 0));
            ttrPos->setPosition(ccp(77, 228));
            ttrPos->setScaleX( (float)792 / (float)162 );
            ttrPos->setScaleY( (float)187 / (float)89 );
            this->addChild(ttrPos, 101);
            ttrArrow->setAnchorPoint(ccp(0.5, 0));
            ttrArrow->setPosition(ccp(77+782-100, 228+187+10));
            CCActionInterval* action;
            action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
            ttrArrow->runAction(CCRepeatForever::create(action));
            this->addChild(ttrArrow, 1000);
            break;
        case 9:
            ttrMsg->setString("여기를 누르면 장착할 수 있어.");
            ttrPos->removeFromParent();
            ttrArrow->removeFromParent();
            break;
        case 10:
            ttrMsg->setString("장착이 완료되었어! 다시 스케치북으로 돌아갈까? 아래쪽의 바를 눌러봐.");
            break;
        case 11:
            ttrMsg->setString("이제 강화하고 싶은 마법을 선택해서 마음껏 마법을 그려보자!");
            ttrBg->setPosition(ccp(vo.x, vo.y));
            ttrCoco->setPosition(ccp(vo.x, vo.y+5));
            ttrMsg->setPosition(ccp(vo.x+320, vo.y+103));
            MakeScroll(curState);
            CCUserDefault::sharedUserDefault()->setBoolForKey("is_tutorial_done", true);
            break;
    }
}

void T_Sketchbook::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        scrollViewSlot->setTouchEnabled(true);
        CCLog("T_Sketchbook : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        scrollViewSlot->setTouchEnabled(true);
        CCLog("T_Sketchbook : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("T_Sketchbook : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
        scrollViewSlot->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        //if (isTutorial) // 튜토리얼 상태 4 -> 5 이동 , 9 -> 10 이동
        TutorialNextState();
        
        // 스킬 정보 갱신
        MakeScroll(curState, true);
    }
    else if (param->intValue() == 3)
    {
        // 돈+mp 정보 갱신
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 개/폐 여부 갱신
        CheckProperties();
    }
    else if (param->intValue() == 9)
    {
        // 스킬 슬롯 정보 갱신
        spriteClassSlot->RemoveAllObjects();
        MakeScrollSlot();
        char name[7];
        sprintf(name, "%d", (int)myInfo->GetSlot().size());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->setString(name);
    }
    else if (param->intValue() == 4)
    {
        // 튜토리얼 상태 한 번 넘기기
        TutorialNextState();
    }
    
    else if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        scrollViewSlot->setTouchEnabled(true);
        CCLog("Sketchbook : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}

void T_Sketchbook::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(200);
    this->addChild(pBlack, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "T_Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "T_Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "T_Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "T_Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "T_Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "T_Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "T_Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "T_Sketchbook", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "T_Sketchbook", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "T_Sketchbook", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "T_Sketchbook", this, 5, 0, 255, 3) );
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
                                                           ccp(0, 0), ccp(14, 1586+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                                                           ccp(0, 0), ccp(875, 1634+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_sketchbook.png",
                                                           ccp(0, 0), ccp(279, 1632+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    
    // select menu
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
                                                           ccp(0, 0), ccp(100, 1416+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
                                                           ccp(0, 0), ccp(296, 1416+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
                                                           ccp(0, 0), ccp(492, 1416+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png4",
                                                           ccp(0, 0), ccp(688, 1416+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire.png",
                                                           ccp(0, 0), ccp(153, 1460+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water.png",
                                                           ccp(0, 0), ccp(350, 1461+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land.png",
                                                           ccp(0, 0), ccp(534, 1464+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master.png",
                                                           ccp(0, 0), ccp(737, 1463+offsets), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(100+90, 1416+offsets+40), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(296+90, 1416+offsets+40), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png3", ccp(0, 0), ccp(492+90, 1416+offsets+40), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png4", ccp(0, 0), ccp(688+90, 1416+offsets+40), CCSize(0, 0), "", "T_Sketchbook", this, 3) );
    
    CheckProperties();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                                                           ccp(0, 0), ccp(49, 458), CCSize(982, 974+offsets), "", "T_Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                                                           ccp(0, 0), ccp(77, 492), CCSize(929, 914+offsets), "", "T_Sketchbook", this, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                                                           ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "T_Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                                                           ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "T_Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
                                                           ccp(0, 0), ccp(896, 312), CCSize(0, 0), "", "T_Sketchbook", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png0",
                                                           ccp(0, 0), ccp(867, 237), CCSize(136, 63), "", "T_Sketchbook", this, 1) );
    // 슬롯 개수 숫자
    char name[7];
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0, 0), ccp(892, 245), ccc3(255,219,53), "", "T_Sketchbook", this, 5, 0, 255, 100) ); // 현재 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(927, 245), ccc3(182,142,142), "", "T_Sketchbook", this, 5) ); // 젼체 개수
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void T_Sketchbook::CheckProperties()
{
    if (myInfo->IsFire())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png1"))->setOpacity(0);
    if (myInfo->IsWater())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png2"))->setOpacity(0);
    if (myInfo->IsLand())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png3"))->setOpacity(0);
    if (myInfo->IsMaster())
        ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_lock_white.png4"))->setOpacity(0);
}

void T_Sketchbook::MakeScroll(int state, bool isFromPopup)
{
    // sprites init
    spriteClassBook->RemoveAllObjects();
    
    // menu change
    SetMenuChange(state);
    
    if (state == 0) MakeScrollBook(2);
    else if (state == 1) MakeScrollBook(1);
    else if (state == 2) MakeScrollBook(3);
    
    curState = state;
}

void T_Sketchbook::SetMenuChange(int state)
{
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png1"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_fire.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png2"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_water.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png3"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_land.png"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png4"))->setColor(ccc3(170,170,170));
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_sketchbook_select.png4"))->setZOrder(0);
    ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_master.png"))->setColor(ccc3(170,170,170));
    
    char name[38];
    sprintf(name, "background/bg_sketchbook_select.png%d", state+1);
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setColor(ccc3(255,255,255));
    ((CCSprite*)spriteClass->FindSpriteByName(name))->setZOrder(2);
    if (state+1 == 1) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_fire.png"))->setColor(ccc3(255,255,255));
    else if (state+1 == 2) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_water.png"))->setColor(ccc3(255,255,255));
    else if (state+1 == 3) ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_land.png"))->setColor(ccc3(255,255,255));
    else ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_property_master.png"))->setColor(ccc3(255,255,255));
}


SkillInfo* T_Sketchbook::GetNextSkillInfo(int state)
{
    if (tabNumber == 0) return SkillInfo::GetSkillInfo(22);
    else if (tabNumber == 1) return SkillInfo::GetSkillInfo(12);
    else if (tabNumber == 2) return SkillInfo::GetSkillInfo(32);
    return NULL;
    /*
    // '?' 스킬 여부
    MySkill* ms;
    for (int i = 0 ; i < skillInfo.size() ; i++)
    {
        if (skillInfo[i]->GetId() / 10 != state)
            continue;
        bool flag = true;
        for (int j = 0 ; j < myInfo->GetSkillList().size() ; j++)
        {
            ms = myInfo->GetSkillList()[j];
            if (skillInfo[i]->GetId() == ms->GetCommonId())
                flag = false;
        }
        if (!flag)
            continue;
        
        for (int j = 0 ; j < myInfo->GetSkillList().size() ; j++)
        {
            ms = myInfo->GetSkillList()[j];
            if (skillInfo[i]->GetRequiredSkillId() == ms->GetCommonId())
            {
                return skillInfo[i];
            }
        }
    }
    return NULL;
     */
}

void T_Sketchbook::MakeScrollBook(int idx)
{
    std::vector<MySkill*> ms;
    //int numOfList = myInfo->GetSkillList().size();
    int numOfList = 1;
    for (int i = 0 ; i < numOfList ; i++)
    {
        if (myInfo->GetSkillList()[i]->GetCommonId() / 10 == idx)
        {
            ms.push_back(myInfo->GetSkillList()[i]);
            CCLog("%d", myInfo->GetSkillList()[i]->GetCommonId());
        }
    }
    //numOfList = ms.size();
    //numOfList++;

    numOfList = 2;
    
    int numOfMaxSkill = 7;
    
    containerBook = CCLayer::create();
    containerBook->setContentSize(CCSizeMake(929, std::min(numOfList, numOfMaxSkill)*206));
    
    SkillInfo* sInfo;
    int id;
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList; i++)
    {
        if (i == numOfList-1)
        {
            sInfo = GetNextSkillInfo(idx);
            if (sInfo == NULL || numOfList == numOfMaxSkill+1)
                continue;
        }
        else
            sInfo = SkillInfo::GetSkillInfo(ms[i]->GetCommonId());
        
        //CCLog("next skill = (%d), %s", sInfo->GetId(), sInfo->GetName().c_str());
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setPosition(ccp(27, (std::min(numOfList, numOfMaxSkill)-i-1)*206));
        containerBook->addChild(itemLayer, 5*10-i);
        spriteClassBook->layers.push_back(itemLayer);
        
        
        if (i == numOfList-1)
            id = sInfo->GetId();
        else
            id = ms[i]->GetCommonId();
        sprintf(name, "background/bg_board_brown.png%d", i+3);
        spriteClassBook->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(0, 0), CCSize(872, 206), "", "Layer", itemLayer, 5, 0, 255, id) );
        
        // 튜토리얼에 따라 영역표시를 해 준다.
        if ( (tutorialState == 1 && i == 0) || (tutorialState == 2 && i == 1) )
        {
            CCLog("스케치북 tutorialState = %d", tutorialState);
            ttrPos->setAnchorPoint(ccp(0, 0));
            ttrPos->setPosition(ccp(0, 0));
            ttrPos->setScaleX( (float)892 / (float)162 );
            ttrPos->setScaleY( (float)216 / (float)89 );
            itemLayer->addChild(ttrPos, 1005);
        }
        
        // 스킬 배경
        sprintf(name, "icon/icon_skill_division_red.png%d", i+3);
        spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(25, 132), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        sprintf(name, "background/bg_skill_brown.png%d", i+3);
        spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(25, 51), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        // 스킬 문양
        if (i == numOfList-1) // '?' 스킬인 경우
        {
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(68, 80), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        }
        else // 그 외
        {
            sprintf(name2, "skill_%d.png", ms[i]->GetCommonId());
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(25+8, 51+8), CCSize(0, 0), "", "Layer", itemLayer, 5) );
        }
        
        // 스킬 레벨
        if (i < numOfList-1)
        {
            int offset = 0;
            sprintf(name, "number/level_lv.png%d", i+3);
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53, 41), CCSize(0,0), "", "Layer", itemLayer, 6) );
            if (ms[i]->GetLevel() >= 10)
            {
                sprintf(name, "number/level_%d.png", ms[i]->GetLevel() / 10);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53+43+3, 41), CCSize(0,0), "",   "Layer", itemLayer, 6) );
                offset = spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width;
            }
            sprintf(name, "number/level_%d.png", ms[i]->GetLevel() % 10);
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(53+43+3+offset, 41), CCSize(0,0), "", "Layer", itemLayer, 6) );
        }
        
        // 스킬 이름 배경
        sprintf(name, "background/bg_gameready_name.png%d", i+3);
        spriteClassBook->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(169, 116), CCSize(442, 69), "", "Layer", itemLayer, 5) );
        // 스킬 이름
        CCPoint pos = spriteClassBook->FindParentCenterPos(name);
        spriteClassBook->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 48, ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), ccc3(255,255,255), name, "1", NULL, 5, 1) );
        
        // 스킬 간략 설명
        if (i < numOfList-1)
            spriteClassBook->spriteObj.push_back( SpriteObject::CreateLabel(SkillInfo::GetShortDesc(sInfo->GetId()), fontList[0], 30, ccp(0.5, 0.5), ccp(169+(int)pos.x, 92), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        else
            spriteClassBook->spriteObj.push_back( SpriteObject::CreateLabel("스킬을 배우면 알 수 있어요", fontList[0], 30, ccp(0.5, 0.5), ccp(169+(int)pos.x, 92), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        
        // '패시브' 스킬에 대해 '자동효과' 문구 넣기
        if (!sInfo->IsActive() && i < numOfList-1)
        {
            sprintf(name2, "icon/icon_auto_effect.png%d", i+3);
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(114, 163), CCSize(0, 0), "", "Layer", itemLayer, 6) );
            
            // 튜토리얼에 따라 영역표시를 한다. ('자동효과'에 영역표시 (첫 스킬에만))
            if ( (tutorialState == 6 && i == 0) )
            {
                CCLog("스케치북 tutorialState = %d", tutorialState);
                ttrPos->setAnchorPoint(ccp(0, 0));
                ttrPos->setPosition(ccp(104, 153));
                ttrPos->setScaleX( (float)122 / (float)162 );
                ttrPos->setScaleY( (float)48 / (float)89 );
                itemLayer->addChild(ttrPos, 1005);
            }
        }
        
        // 연습량 프로그레스바
        sprintf(name, "background/bg_progress_bar.png%d", i+3);
        spriteClassBook->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(133, 28), CCSize(412, 31), "", "Layer", itemLayer, 5) );
        
        
        // 연습량 프로그레스바 안의 바
        if (i < numOfList-1)
        {
            float percentage = ((float)ms[i]->GetExp() / (float)SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()))   ;
            CCSprite* bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, (float)(412-8)*percentage, 31-12));
            bar->setAnchorPoint(ccp(0, 0));
            bar->setPosition(ccp(133+4, 28+8));
            bar->setColor(ccc3(255,255,255));
            if (ms[i]->GetExp() == SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()))
                bar->setColor(ccc3(255,219,53));
            itemLayer->addChild(bar, 10);
        }
        
        // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
        if (i < numOfList-1)
        {
            sprintf(name, "%d", ms[i]->GetExp());
            sprintf(name2, "/%d", SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()));
        }
        else
        {
            sprintf(name, "0");
            sprintf(name2, "/0");
        }
        spriteClassBook->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(1, 0), ccp(582-3, 31), ccc3(255,255,255), "", "Layer", itemLayer, 5) );
        spriteClassBook->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(582, 25), ccc3(182,142,142), "", "Layer", itemLayer, 5) );
        
        if (i < numOfList-1)
        {
            /*
            // 'MASTER' 버튼
            if (SkillBuildUpInfo::IsMastered(ms[i]->GetCommonId(), ms[i]->GetLevel()))
            {
                sprintf(name, "button/btn_skill_master.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 61), CCSize(213, 95), "", "Layer", itemLayer, 5) );
                sprintf(name2, "letter/letter_master.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), name, "0", NULL, 5, 1) );
            }
            // '레벨업' 버튼
            else if (ms[i]->GetExp() == SkillBuildUpInfo::GetMaxExp(ms[i]->GetCommonId(), ms[i]->GetLevel()))
            {
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, ms[i]->GetCommonId()) ); // 태그에 common_id를 둔다.
                sprintf(name2, "letter/letter_buildup_red.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5, 1) );
                sprintf(name2, "icon/icon_levelup.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(-4, 73-20), CCSize(0, 0), name, "0", NULL, 5, 1) );
                
                CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
                ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->runAction( CCRepeatForever::create(action) );
            }
            // '연습' 버튼
            else
            {*/
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, -ms[i]->GetUserId()) ); // 태그에 user_id를 음수로 둔다.
                sprintf(name2, "letter/letter_practice.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5, 1) );
            //}
        }
        else
        {
            // '?'스킬의 요구조건을 모두 충족한 경우
            //if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP() &&
            //    MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
            //{
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
                
                CCPoint p = spriteClassBook->FindParentCenterPos(name);
                sprintf(name2, "letter/letter_get.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), ccp(p.x, p.y+3), CCSize(0, 0), name, "0", NULL, 5, 1) );
                ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->setScale(0.95f);
            //}
            /*
            // 아닌 경우
            else
            {
                sprintf(name, "button/btn_green_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
                CCPoint p = spriteClassBook->FindParentCenterPos(name);
                sprintf(name2, "letter/letter_require.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), ccp(p.x, p.y+3), CCSize(0, 0), name, "0", NULL, 5, 1) );
            }*/
        }
        
        // 튜토리얼에 따라 화살표 + 영역표시를 표시한다.
        if ( (tutorialState == 3 && i == 1) || (tutorialState == 11 && i == 0) )
        {
            ttrArrow->setAnchorPoint(ccp(0.5, 0));
            ttrArrow->setRotation(180);
            ttrArrow->setPosition(ccp(633+233/2, 51-10));
            CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, 5)), CCMoveBy::create(0.5f, ccp(0, -5)), NULL);
            ttrArrow->runAction(CCRepeatForever::create(action));
            itemLayer->addChild(ttrArrow, 5001);
            
            ttrPos->setAnchorPoint(ccp(0, 0));
            ttrPos->setPosition(ccp(633, 51));
            ttrPos->setScaleX( (float)233 / (float)162 );
            ttrPos->setScaleY( (float)115 / (float)89 );
            itemLayer->addChild(ttrPos, 5001);
        }
    }
    
    // sprite들 add child
    for (int i = 0 ; i < spriteClassBook->spriteObj.size() ; i++)
        spriteClassBook->AddChild(i);
    
    if (curState != -1)
    {
        scrollView->getContainer()->removeAllChildren();
        scrollView->getContainer()->removeFromParent();
    }
    // scrollView에 container를 새 layer로 설정.
    scrollView->setContainer(containerBook);
    scrollView->setContentSize(containerBook->getContentSize());
    // offset 변경
    scrollView->setContentOffset(ccp(0, scrollView->minContainerOffset().y), false);
}

void T_Sketchbook::MakeScrollSlot()
{
    int numOfList = myInfo->GetSlot().size();
    
    // container 초기화
    containerSlot = CCLayer::create();
    containerSlot->setContentSize(CCSizeMake(numOfList*(146+5), 146));
    
    char fname[50], fname2[20];
    int scid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerSlot, 3) );
        
        
        //scid = myInfo->GetSlot()[i]->GetCommonId();
        //CCLog("slot common id = %d", scid);
        //if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
        CCLog("%d %d", i, tutorialState);
        if (tutorialState >= 11 && i == 0)
        {
            if (tabNumber == 0) scid = 22;
            else if (tabNumber == 1) scid = 12;
            else if (tabNumber == 2) scid = 32;
            sprintf(fname2, "skill_%d.png", scid);
            spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0.5), spriteClassSlot->FindParentCenterPos(fname), CCSize(0, 0), fname, "0", NULL, 4, 1) );
        }
    }
    
    for (int i = 0 ; i < spriteClassSlot->spriteObj.size() ; i++)
        spriteClassSlot->AddChild(i);
    
    // scrollView의 container 재설정
    scrollViewSlot->setContainer(containerSlot);
    scrollViewSlot->setContentSize(containerSlot->getContentSize());
}

void T_Sketchbook::SetTouchLock(bool val)
{
    isTouched = val;
}

bool T_Sketchbook::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isSlotTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCLog("touch began");
    
    CCPoint point = pTouch->getLocation();
    
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isSlotTouched = true;
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    
    CCLog("tutorial = %d", tutorialState);
    if (tutorialState <= 2 || tutorialState == 6 || tutorialState == 7)
    {
        TutorialNextState();
        isTouched = false;
        isSlotTouched = false;
        isScrolling = false;
        isScrollViewTouched = false;
        return false;
    }
    return true;
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 35) == "background/bg_sketchbook_select.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                int number = atoi(spriteClass->spriteObj[i]->name.substr(35,36).c_str());
                if (number == 4)
                    continue;
                
                if ( (number == 1 && myInfo->IsFire()) || (number == 2 && myInfo->IsWater()) ||
                    (number == 3 && myInfo->IsLand()) || (number == 4 && myInfo->IsMaster()) )
                {
                    MakeScroll(number-1);
                }
                else
                {
                    std::vector<int> data;
                    data.push_back(number); // 불(1), 물(2), 땅(3), 마스터(4)
                    data.push_back(SkillPropertyInfo::GetCost(number)); // 가격
                    Common::ShowPopup(this, "T_Sketchbook", "NoImage", false, BUY_PROPERTY_TRY, BTN_2, data);
                }
                break;
            }
        }
        /*
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                // 슬롯 구매
                std::vector<int> data;
                if ((int)myInfo->GetSlot().size() >= (int)skillSlotInfo.size())
                    Common::ShowPopup(this, "T_Sketchbook", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "T_Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 1);
                    else
                        Common::ShowPopup(this, "T_Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 1);
                }
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "T_Sketchbook", "BuyTopaz", false, 0);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "T_Sketchbook", "BuyStarCandy", false, 0);
                break;
            }
        }
         */
    }
    
    return true;
}


void T_Sketchbook::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_Sketchbook::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
    
    CCLog("touch ended");
    
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && isSlotTouched && scrollViewSlot->boundingBox().containsPoint(point))
    {
        if (tutorialState == 8)
            TutorialNextState();
        else
        {
            isTouched = false;
            isScrolling = false;
            isSlotTouched = false;
            isScrollViewTouched = false;
            return;
        }
        
        sound->playBoardMove(); // 이 scene만 사운드가 다르다.
        Common::ShowNextScene(this, "T_Sketchbook", "T_MagicList", false, 1);
    }
    
    CCPoint p;
    for (int i = 0 ; i < spriteClassBook->spriteObj.size() ; i++)
    {
        if (spriteClassBook->spriteObj[i]->name.substr(0, 23) == "button/btn_red_mini.png")
        {
            p = spriteClassBook->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassBook->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                // 튜토리얼 진행중일 경우, 적절한 체크 후 state를 넘긴다.
                if (tutorialState == 3 && atoi(spriteClassBook->spriteObj[i]->name.substr(23).c_str()) == 1+3)
                    TutorialNextState();
                else if (tutorialState == 11 && atoi(spriteClassBook->spriteObj[i]->name.substr(23).c_str()) == 0+3)
                    ;//CCUserDefault::sharedUserDefault()->setBoolForKey("tutorial_sketchbook", true); // 튜토리얼 완료를 클라이언트에 저장.
                else
                    continue;
                
                sound->playClick();
                int id = spriteClassBook->spriteObj[i]->sprite->getTag();
                
                // '연습' 누를 때
                if (id < 0) // 이때 id는 user_id
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "T_Sketchbook", "Loading", false, LOADING_MESSAGE);
                    
                    char temp[255];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    param += temp;
                    sprintf(temp, "user_skill_id=%d", -id); // 양수로 고치자.
                    param += temp;
                    
                    Network::HttpPost(param, URL_PRACTICE_SKILL, this, httpresponse_selector(T_Sketchbook::onHttpRequestCompleted));
                }
                // '연습중' 버튼이면 동작하지 않도록 한다.
                else if (id == 0)
                {
                    break;
                }
                // 연습량이 다 차서 레벨업을 해야 하는 경우 (강화)
                else // 이때 id는 common_id
                {
                    Common::ShowNextScene(this, "T_Sketchbook", "T_SketchDetail", false, id);
                }
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isSlotTouched = false;
    isScrollViewTouched = false;
}

void T_Sketchbook::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void T_Sketchbook::scrollViewDidZoom(CCScrollView* view)
{
}


void T_Sketchbook::EndScene()
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    if (strcmp(Depth::GetCurName(), "GameReady") == 0)
    {
        param = CCString::create("9"); // slot 갱신
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    ttrArrow->release();
    ttrArrow->removeFromParentAndCleanup(true);
    ttrPos->release();
    ttrPos->removeFromParentAndCleanup(true);
    ttrCoco->removeFromParentAndCleanup(true);
    ttrMsg->removeFromParentAndCleanup(true);
    ttrBg->removeFromParentAndCleanup(true);
    ttr->removeAllChildren();
    ttr->removeFromParentAndCleanup(true);
    tutorialState = -100;
    isTutorial = false;
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassBook->RemoveAllObjects();
    delete spriteClassBook;
    spriteClassSlot->RemoveAllObjects();
    delete spriteClassSlot;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParent();
    scrollViewSlot->getContainer()->removeAllChildren();
    scrollViewSlot->removeAllChildren();
    scrollViewSlot->removeFromParent();
    
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void T_Sketchbook::EndSceneCallback()
{
}

void T_Sketchbook::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParsePracticeSkill(&xmlDoc);
}

void T_Sketchbook::XmlParsePracticeSkill(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "T_Sketchbook", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        int puid = nodeResult.child("coco").attribute("practice-user-skill-id").as_int();
        
        // myInfo에 현재 연습 스킬 정보 갱신
        for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
        {
            MySkill* ms = myInfo->GetSkillList()[i];
            if (ms->GetUserId() == puid)
                myInfo->SetPracticeSkill(ms->GetCommonId(), ms->GetLevel());
        }
        
        // 장면 전환
        if (froms == 0) // Sketchbook없애고, 그 부모인 Ranking에 GameReady를 입힌다.
        {
            CCNode* parent = this->getParent();
            EndScene();
            Common::ShowNextScene(parent, "Ranking", "GameReady", false);
        }
        else if (froms == 1) // Sketchbook 없앤다.
        {
            // GameReady에 연습 스킬 정보 갱신하기
            CCString* param = CCString::create("5");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
            EndScene();
        }
    }
}