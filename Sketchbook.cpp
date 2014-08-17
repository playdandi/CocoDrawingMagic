#include "Sketchbook.h"

static int from; // 0 : from Ranking , 1 : from GameReady

CCScene* Sketchbook::scene(int fromWhere)
{
    from = fromWhere;

    CCScene* pScene = CCScene::create();
    Sketchbook* pLayer = Sketchbook::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

int Sketchbook::FromWhere()
{
    return from;
}

void Sketchbook::onEnter()
{
    //CCLog("Sketchbook : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.1f, ccp(0, 0)), CCScaleTo::create(0.1f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(Sketchbook::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void Sketchbook::SceneCallback()
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    if (myInfo->HasNoProperty())
    {
        // 속성을 처음 익힐 때
        Common::ShowNextScene(this, "Sketchbook", "SelectProperty", false, 0);
    }
    else if (myInfo->IsTimeToFreelyBuyProperty())
    {
        // 속성 하나를 무료로 open 가능! (팝업메시지 띄우기)
        std::vector<int> nullData;
        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_PROPERTY_FREE_MSG, BTN_1, nullData);
    }
    else
    {
        // "모두 5레벨 이상되면 속성 무료 구매 가능하다"는 팝업알림창 띄우기
        ShowHintForBuyingProperty();
    }
}
void Sketchbook::onExit()
{
    //CCLog("Sketchbook : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Sketchbook::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}

int offsets = -80;

bool Sketchbook::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("Sketchbook", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("스케치북 : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Sketchbook::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);

    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);

    // scrollview init.
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 914-40+offsets));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 5);
    
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 228+12));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollViewSlot, 5);
    
    balloon = NULL;
    ball = NULL;
    balloon2 = NULL;
    ball2 = NULL;
    isHintOfMPShown = false;
    
    spriteClass = new SpriteClass();
    spriteClassBook = new SpriteClass();
    spriteClassSlot = new SpriteClass();
    
    InitSprites();
    MakeScrollSlot();
    
    
    // 연습 중인 스킬이 속한 속성을 우선적으로 보여주는데, 만약 없다면 불/물/땅/궁극 순서대로 연 것 먼저 보여준다.
    int pcsi = myInfo->GetPracticeSkillId();
    if (21 <= pcsi && pcsi <= 27) tabNumber = 0;
    else if (11 <= pcsi && pcsi <= 17) tabNumber = 1;
    else if (31 <= pcsi && pcsi <= 37) tabNumber = 2;
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

    return true;
}

bool compare_sb(MySkill* ms1, MySkill* ms2)
{
    return ms1->GetLearnTime() < ms2->GetLearnTime();
}
void Sketchbook::SortMySkillByUserId()
{
    std::sort(sList.begin(), sList.end(), compare_sb);
}

void Sketchbook::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        scrollView->setTouchEnabled(true);
        scrollViewSlot->setTouchEnabled(true);
        //CCLog("Sketchbook : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("Sketchbook : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
        scrollViewSlot->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
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
    else if (param->intValue() == 4)
    {
        // 어떤 스킬을 레벨업하고 돌아오는 경우.
        
        if (myInfo->IsTimeToFreelyBuyProperty())
        {
            // 속성 하나를 무료로 open 가능! (팝업메시지 띄우기)
            std::vector<int> nullData;
            Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_PROPERTY_FREE_MSG, BTN_1, nullData);
        }
    }
    else if (param->intValue() == 5)
    {
        // 속성선택창을 열어야 하는 경우 (무료로 배울 때가 되서)
        Common::ShowNextScene(this, "Sketchbook", "SelectProperty", false, 0);
    }
    else if (param->intValue() == 6)
    {
        // 모두 5레벨 이상되면 속성 무로 구매 가능하다는 팝업알림창 띄우기
        ShowHintForBuyingProperty();
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
        
        ShowHintOfBuyingSlot();
    }
    else if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        scrollViewSlot->setTouchEnabled(true);
        //CCLog("Sketchbook : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}

void Sketchbook::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(200);
    this->addChild(pBlack, 0);

    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "Sketchbook", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Sketchbook", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Sketchbook", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "Sketchbook", this, 5, 0, 255, 3) );
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
                    ccp(0, 0), ccp(14, 1586+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1634+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_sketchbook.png",
                    ccp(0, 0), ccp(279, 1632+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    
    // select menu
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
                    ccp(0, 0), ccp(100, 1416+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
                    ccp(0, 0), ccp(296, 1416+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
                    ccp(0, 0), ccp(492, 1416+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png4",
                    ccp(0, 0), ccp(688, 1416+offsets), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire.png",
                    ccp(0, 0), ccp(153, 1460+offsets), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water.png",
                    ccp(0, 0), ccp(350, 1461+offsets), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land.png",
                    ccp(0, 0), ccp(534, 1464+offsets), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master.png",
                    ccp(0, 0), ccp(737, 1463+offsets), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png1", ccp(0, 0), ccp(100+90, 1416+offsets+40), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png2", ccp(0, 0), ccp(296+90, 1416+offsets+40), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png3", ccp(0, 0), ccp(492+90, 1416+offsets+40), CCSize(0, 0), "", "Layer", tLayer, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png4", ccp(0, 0), ccp(688+90, 1416+offsets+40), CCSize(0, 0), "", "Layer", tLayer, 3) );
    
    CheckProperties();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 974+offsets), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(77, 492), CCSize(929, 914+offsets), "", "Layer", tLayer, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
                    ccp(0, 0), ccp(896, 312), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png0",
                    ccp(0, 0), ccp(867, 237), CCSize(136, 63), "", "Layer", tLayer, 1) );
    
    // 슬롯 옆의 {현재 구입한 슬롯수} / {구입 가능한 max 슬롯수}
    char name[7];
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 44, ccp(1, 0), ccp(892+30, 245), ccc3(255,219,53), "", "Layer", tLayer, 5, 0, 255, 100) ); // 현재 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 34, ccp(0, 0), ccp(927+3, 245), ccc3(182,142,142), "", "Layer", tLayer, 5) ); // 젼체 개수
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // 슬롯구입 힌트
    ShowHintOfBuyingSlot();
}

void Sketchbook::ShowHintOfMP()
{
    if (isHintOfMPShown)
        return;
    isHintOfMPShown = true;
    
    if (balloon2 != NULL && ball2 != NULL)
    {
        ball2->removeFromParentAndCleanup(true);
        balloon2->removeFromParentAndCleanup(true);
    }
    balloon2 = NULL;
    ball2 = NULL;
    
    balloon2 = CCScale9Sprite::create("images/tutorial_balloon3.png");
    balloon2->setContentSize(CCSize(600, 200));
    balloon2->setAnchorPoint(ccp(1, 1));
    balloon2->setPosition(ccp(765+200, 1666+35));
    this->addChild(balloon2, 100);
    ball2 = CCLabelTTF::create("MP로 새로운 마법을 배울 수 있고,\n보너스 점수를 증가시켜줘요.", fontList[0].c_str(), 36);
    ball2->setPosition(ccp(600/2, 200/2-30));
    ball2->setColor(ccc3(255,255,255));
    balloon2->addChild(ball2, 101);
    
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
    balloon2->runAction( CCRepeatForever::create(action) );
}

void Sketchbook::ShowHintForBuyingProperty()
{
    int numOfSkills[5] = {0,}; // 속성별 스킬 개수
    int numOfMore5Lv[5] = {0,}; // 속성별 5레벨 이상 스킬 개수
    int idx;
    for (int i = 0 ; i < (int)myInfo->GetSkillList().size() ; i++)
    {
        idx = myInfo->GetSkillList()[i]->GetCommonId() / 10;
        numOfSkills[idx]++;
        if (myInfo->GetSkillList()[i]->GetLevel() >= 5)
            numOfMore5Lv[idx]++;
    }
    
    int propertyCnt = 0;
    if (myInfo->IsFire()) propertyCnt++;
    if (myInfo->IsWater()) propertyCnt++;
    if (myInfo->IsLand()) propertyCnt++;
    if (propertyCnt >= 3) // 보유 속성이 이미 3개면 힌트가 나올 필요 없다.
        return;
    
    // 스킬이 7개 다 존재하면서 전부 5레벨 이상은 아닌, 그런 속성의 개수를 카운트한다.
    int appliedCnt = 0;
    for (int i = 1 ; i <= 3 ; i++)
    {
        if (numOfSkills[i] == 7 && numOfMore5Lv[i] < 7)
            appliedCnt++;
        else if (numOfSkills[i] == 7 && numOfMore5Lv[i] == 7)
            propertyCnt--;
    }
    
    // 보유 속성 수와 위 조건을 만족하는 속성 수가 같다면 힌트 팝업창을 노출하도록 한다.
    int propertyIdx = -1;
    if (appliedCnt > 0 && appliedCnt == propertyCnt)
    {
        for (int i = 1 ; i <= 3 ; i++)
        {
            if (numOfSkills[i] == 7 && numOfMore5Lv[i] < 7)
            {
                propertyIdx = i;
                break;
            }
        }
    }
    
    if (propertyIdx != -1 && !isHintForBuyingNextProperty && !myInfo->IsTimeToFreelyBuyProperty())
    {
        isHintForBuyingNextProperty = true;
        std::vector<int> data;
        data.push_back(propertyIdx); // 1(물), 2(불), 3(땅)
        Common::ShowPopup(this, "Sketchbook", "NoImage", false, HINT_BUY_PROPERTY, BTN_1, data);
    }
}

void Sketchbook::CheckProperties()
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

void Sketchbook::MakeScroll(int state, bool isFromPopup)
{
    // select button을 눌렀을 때 이 함수를 통해 어떤 layer를 보여줄 지 결정한다.
    if (curState == state && !isFromPopup)
        return;

    // sprites init
    spriteClassBook->RemoveAllObjects();
    
    // menu change
    SetMenuChange(state);

    // 선택된 속성의 마법들 보여주기
    if (state == 0) MakeScrollBook(2);
    else if (state == 1) MakeScrollBook(1);
    else if (state == 2) MakeScrollBook(3);
    //else MakeScrollBook(4);
    
    curState = state;
}

void Sketchbook::SetMenuChange(int state)
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


SkillInfo* Sketchbook::GetNextSkillInfo(int state)
{
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
}

void Sketchbook::MakeScrollBook(int idx)
{
    std::vector<MySkill*> ms;
    int numOfList = (int)myInfo->GetSkillList().size();
    for (int i = 0 ; i < numOfList ; i++)
    {
        if (myInfo->GetSkillList()[i]->GetCommonId() / 10 == idx)
            ms.push_back(myInfo->GetSkillList()[i]);
    }
    numOfList = ms.size();
    numOfList++;
    
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
            spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(114-12, 163), CCSize(0, 0), "", "Layer", itemLayer, 6) );
            ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->setScale(1.2f);
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
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(-4, 73), CCSize(0, 0), name, "0", NULL, 5, 1) );
                
                CCActionInterval* action = CCSequence::create(CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
                ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->runAction( CCRepeatForever::create(action) );
            }
            // '연습' 버튼
            else
            {
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, -ms[i]->GetUserId()) ); // 태그에 user_id를 음수로 둔다.
                
                if (ms[i]->GetCommonId() != myInfo->GetPracticeSkillId()) // 연습중이 아님.
                {
                    sprintf(name2, "letter/letter_practice.png%d", i+3);
                    spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5, 1) );
                }
                else // 연습중인 스킬임을 표시하자.
                {
                    sprintf(name2, "letter/letter_practicing.png%d", i+3);
                    spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0), ccp(spriteClassBook->spriteObj[spriteClassBook->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), name, "0", NULL, 5, 1) );
                    ////CCLog("연습 스킬? %d , %d", ms[i]->GetCommonId(), myInfo->GetPracticeSkillId());
                    ((CCSprite*)spriteClassBook->FindSpriteByName(name))->setColor(ccc3(140,140,140));
                    ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->setColor(ccc3(140,140,140));
                    ((CCSprite*)spriteClassBook->FindSpriteByName(name))->setTag(0); // 클릭하지 못하도록 함
                }
                
            }
        }
        else
        {
            // '?'스킬의 요구조건을 모두 충족한 경우
            // 조건1) prerequisite 스킬의 레벨이 요구레벨 이상!
            // 조건2) 내 MP가 요구MP 이상! (결제를 통해 얻은 속성의 수에 따라 요구MP 할인이 있을 수 있다)
            
            // 지금 이 스킬을 배우는데 요구되는 MP
            sList = myInfo->GetSkillList();
            SortMySkillByUserId();
            int requiredMP = SkillBuildupMPInfo::RequiredMP(sList, sInfo->GetId());
            
            if (MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv() &&
                myInfo->GetMPTotal() >= requiredMP)
            {
                sprintf(name, "button/btn_red_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
                
                CCPoint p = spriteClassBook->FindParentCenterPos(name);
                sprintf(name2, "letter/letter_get.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), ccp(p.x, p.y+3), CCSize(0, 0), name, "0", NULL, 5, 1) );
                ((CCSprite*)spriteClassBook->FindSpriteByName(name2))->setScale(0.95f);
            }
            // 아닌 경우
            else
            {
                sprintf(name, "button/btn_green_mini.png%d", i+3);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(633, 51), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, sInfo->GetId()) );
                CCPoint p = spriteClassBook->FindParentCenterPos(name);
                sprintf(name2, "letter/letter_require.png%d", i);
                spriteClassBook->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), ccp(p.x, p.y+3), CCSize(0, 0), name, "0", NULL, 5, 1) );
            }
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

void Sketchbook::ShowHintOfBuyingSlot()
{
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    balloon = NULL;
    ball = NULL;
    bool flag = false;
    for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
    {
        if (myInfo->GetSkillList()[i]->GetCommonId() % 10 == 3) // 정령 스킬이 있는가?
        {
            flag = true;
            break;
        }
    }
    if (myInfo->GetSlot().size() == 1 && flag)
    {
        balloon = CCScale9Sprite::create("images/tutorial_balloon2.png");
        balloon->setContentSize(CCSize(600, 140));
        balloon->setAnchorPoint(ccp(1, 0));
        balloon->setPosition(ccp(896+100, 312+55));
        this->addChild(balloon, 100);
        ball = CCLabelTTF::create("여기서 슬롯을 구매하여\n두번째 발동스킬을 장착하세요!", fontList[0].c_str(), 36);
        ball->setPosition(ccp(600/2, 140/2+30));
        ball->setColor(ccc3(255,255,255));
        balloon->addChild(ball, 101);
        
        CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
        balloon->runAction( CCRepeatForever::create(action) );
    }
}


void Sketchbook::MakeScrollSlot()
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
        
        scid = myInfo->GetSlot()[i]->GetCommonId();
        ////CCLog("slot common id = %d", scid);
        if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
        {
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

void Sketchbook::SetTouchLock(bool val)
{
    isTouched = val;
}

bool Sketchbook::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isHintOfMPShown) // MP힌트 표시되고 있으면 없애기.
    {
        if (balloon2 != NULL && ball2 != NULL)
        {
            ball2->removeFromParentAndCleanup(true);
            balloon2->removeFromParentAndCleanup(true);
        }
        balloon2 = NULL;
        ball2 = NULL;
        isHintOfMPShown = false;
    }
    
    if (isTouched)
        return false;
    isTouched = true;
    isSlotTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    // 슬롯구매 힌트 있으면 안 보이게 한다.
    if (balloon != NULL && ball != NULL)
    {
        balloon->setOpacity(0);
        ball->setOpacity(0);
    }
    
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isSlotTouched = true;
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 35) == "background/bg_sketchbook_select.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                int number = atoi(spriteClass->spriteObj[i]->name.substr(35,36).c_str());

                if (number == 4) // 마스터 속성의 경우 업데이트 예정
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "Sketchbook", "NoImage", false, WILL_BE_UPDATED, BTN_1, nullData);
                }
                
                // 열려있는 속성은 그 스케치북을 보여준다.
                else if ( (number == 1 && myInfo->IsFire()) || (number == 2 && myInfo->IsWater()) ||
                    (number == 3 && myInfo->IsLand()) || (number == 4 && myInfo->IsMaster()) )
                {
                    MakeScroll(number-1);
                }
                else
                {
                    // 잠긴 속성의 경우,
                    if (myInfo->IsTimeToFreelyBuyProperty()) // 무료로 속성을 배울 수 있는 경우
                    {
                        Common::ShowNextScene(this, "Sketchbook", "SelectProperty", false, 0);
                    }
                    else // 돈 주고 사야 하는 경우
                    {
                        std::vector<int> data;
                        data.push_back(number); // 불(1), 물(2), 땅(3), 마스터(4)
                        data.push_back(SkillPropertyInfo::GetCost(number)); // 가격
                        // 토파즈로 구매한 속성 개수
                        int count = 0;
                        if (myInfo->IsFireByTopaz()) count++;
                        if (myInfo->IsWaterByTopaz()) count++;
                        if (myInfo->IsLandByTopaz()) count++;
                        if (count+1 == 1)
                            data.push_back( SkillBuildupMPInfo::GetObj(1)->GetDiscount1() );
                        else if (count+1 == 2)
                            data.push_back( SkillBuildupMPInfo::GetObj(1)->GetDiscount2() );
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_PROPERTY_TRY, BTN_2, data);
                    }
                }

                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                return true;
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
                    Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 1);
                    else
                        Common::ShowPopup(this, "Sketchbook", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 1);
                }
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Sketchbook", "BuyTopaz", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Sketchbook", "BuyStarCandy", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png3") // MP hint 보여주기
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                ShowHintOfMP();
                return true;
            }
        }
    }
    
    return true;
}


void Sketchbook::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
}

void Sketchbook::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
    
    //CCLog("touch ended");
    
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && isSlotTouched && scrollViewSlot->boundingBox().containsPoint(point))
    {
        sound->playBoardMove(); // 이 scene만 사운드가 다르다.
        Common::ShowNextScene(this, "Sketchbook", "MagicList", false, 1);
    }
    
    CCPoint p;
    for (int i = 0 ; i < spriteClassBook->spriteObj.size() ; i++)
    {
        if (spriteClassBook->spriteObj[i]->name.substr(0, 29) == "background/bg_board_brown.png")
        {
            // 스케치북에서 버튼 외의 영역을 눌렀을 때 스킬상세 팝업창이 뜨게 하자.
            p = spriteClassBook->spriteObj[i]->sprite9->convertToNodeSpace(point);
            CCSize size = spriteClassBook->spriteObj[i]->sprite9->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= 600 && (int)p.y <= size.height)
            {
                sound->playClick();
                int id = spriteClassBook->spriteObj[i]->sprite9->getTag();
                Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id);
                break;
            }
        }
        else if (spriteClassBook->spriteObj[i]->name.substr(0, 23) == "button/btn_red_mini.png")
        {
            p = spriteClassBook->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassBook->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                int id = spriteClassBook->spriteObj[i]->sprite->getTag();
                
                // '연습' 누를 때
                if (id < 0) // 이때 id는 user_id
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, "Sketchbook", "Loading", false, LOADING_MESSAGE);
                    
                    char temp[255];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                    param += temp;
                    sprintf(temp, "user_skill_id=%d", -id); // 양수로 고치자.
                    param += temp;

                    Network::HttpPost(param, URL_PRACTICE_SKILL, this, httpresponse_selector(Sketchbook::onHttpRequestCompleted));
                }
                // '연습중' 버튼이면 동작하지 않도록 한다.
                else if (id == 0)
                {
                    break;
                }
                // 연습량이 다 차서 레벨업을 해야 하는 경우 (강화)
                else // 이때 id는 common_id
                {
                    Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id);
                }
                break;
            }
        }
        else if (spriteClassBook->spriteObj[i]->name.substr(0, 25) == "button/btn_green_mini.png")
        {
            p = spriteClassBook->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassBook->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                
                // 요구조건을 만족한 경우, 만족하지 않은 경우 둘 다 팝업창을 띄운다.
                int id = spriteClassBook->spriteObj[i]->sprite->getTag();
                Common::ShowNextScene(this, "Sketchbook", "SketchDetail", false, id);
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isSlotTouched = false;
    isScrollViewTouched = false;
}

void Sketchbook::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Sketchbook::scrollViewDidZoom(CCScrollView* view)
{
}


void Sketchbook::EndScene()
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
    
    if (balloon != NULL && ball != NULL) // 슬롯구매 힌트 있으면 지운다.
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    ball = NULL;
    balloon = NULL;
    
    if (balloon2 != NULL && ball2 != NULL) // MP 힌트 있으면 지운다.
    {
        ball2->removeFromParentAndCleanup(true);
        balloon2->removeFromParentAndCleanup(true);
    }
    balloon2 = NULL;
    ball2 = NULL;

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
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void Sketchbook::EndSceneCallback()
{
}

void Sketchbook::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParsePracticeSkill(&xmlDoc);
}

void Sketchbook::XmlParsePracticeSkill(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "Sketchbook", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
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
        if (from == 0) // Sketchbook없애고, 그 부모인 Ranking에 GameReady를 입힌다.
        {
            CCNode* parent = this->getParent();
            EndScene();
            Common::ShowNextScene(parent, "Ranking", "GameReady", false);
        }
        else if (from == 1) // Sketchbook 없앤다.
        {
            // GameReady에 연습 스킬 정보 갱신하기
            CCString* param = CCString::create("5");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
            EndScene();
        }
    }
}