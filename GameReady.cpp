#include "GameReady.h"
#include "Ranking.h"
#include "MagicList.h"

static int minusCost_topaz;
static int minusCost_starcandy;


GameReady::~GameReady(void)
{
    //CCLog("GameReady 소멸자");
}

CCScene* GameReady::scene()
{   
    CCScene* pScene = CCScene::create();
    GameReady* pLayer = GameReady::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void GameReady::onEnter()
{
    //CCLog("GameReady :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();

    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(0.05f), CCCallFunc::create(this, callfunc_selector(GameReady::SceneCallback)), NULL );
    tLayer->runAction(action);
    //tLayer->runAction(CCEaseExponentialOut::create(action));
    
    //  spriteClassProperty
    // spriteClass
    // balloon, ball => 이건 fade-in 끝나고 만들까?
}
void GameReady::SceneCallback()
{
    ////CCLog("scene callback");

    ShowRewardPopup();
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
    isStarting = false;
}
void GameReady::onExit()
{
    //CCLog("GameReady :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void GameReady::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndSceneCallback(NULL, this);
}


bool GameReady::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    popupStatus = 0;
    doNotShowExpPopup = false;
    
    idx = -1;
    isTouched = true;
    isScrollViewTouched = true;
    isScrolling = true;
    isStarting = true;
    
    // make depth tree
    Depth::AddCurDepth("GameReady", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("GameReady : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GameReady::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->schedule(schedule_selector(GameReady::PotionTimer), 0.2f);
    
    // 장면 액션 위한 layer
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(0, 0));
    //tLayer->setPosition(ccp(winSize.width/2, 0));
    this->addChild(tLayer, 6);
    
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 488+12));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollViewSlot, 5);
    //this->addChild(scrollViewSlot, 5);
    
    // spriteclass init.
    spriteClass = new SpriteClass();
    spriteClassProperty = new SpriteClass();
    spriteClassFairy = new SpriteClass();
    spriteClassSkill = new SpriteClass();
    spriteClassSlot = new SpriteClass();
    
    fairyLayer = CCLayer::create();
    tLayer->addChild(fairyLayer, 10);
    skillLayer = CCLayer::create();
    tLayer->addChild(skillLayer, 4);
    
    balloon = NULL;
    ball = NULL;
    balloon2 = NULL;
    ball2 = NULL;
    isHintOfMPShown = false;
    
    // item select init.
    for (int i = 0 ; i < 5 ; i++)
    {
        itemSelected[i] = false;
        itemNumberLayer[i] = NULL;
    }
    
    InitSprites();
    
    InitFairy();
    InitSkill();
    MakeScrollSlot();
    return true;
}

void GameReady::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        scrollViewSlot->setTouchEnabled(true);
        //CCLog("GameReady : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        if (param->intValue() == 0)
            RenewInfo();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("GameReady 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        //this->setKeypadEnabled(false);
        //this->setTouchEnabled(false);
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollViewSlot->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        RenewInfo();
    }
    else if (param->intValue() == 3)
    {
        // 코코 정보가 바뀌었을 경우 (지팡이 강화 등으로)
        char name[40];
        sprintf(name, "지팡이 %dLv", myInfo->GetStaffLv());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(-108922))->setString(name);
        ((CCLabelTTF*)spriteClass->FindLabelByTag(-108921))->setString(name);
    }
    else if (param->intValue() == 4)
    {
        // 요정 정보가 바뀐 경우 (요정 선택변경, 요정 강화 등)
        InitFairy();
    }
    else if (param->intValue() == 5)
    {
        // 스킬 정보가 바뀐 경우 (게임준비->스케치북->게임준비 돌아올 때)
        InitSkill();
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 종류 갱신
        InitProperties();
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
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        if (idx > -1)
        {
            ((CCSprite*)spriteClass->FindSpriteByName("button/btn_blue.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gamestart.png"))->setColor(ccc3(255,255,255));
        }
    }
    else if (param->intValue() == 7)
    {
        // 속성을 처음 새로 배우고 인게임 튜토리얼 시작하는 경우
        StartGame();
    }
    else if (param->intValue() == 11)
    {
        // 필요한 팝업창이 있으면 (보상, 스킬연습량 관려) 띄우자.
        ShowRewardPopup();
    }
}


void GameReady::ShowRewardPopup()
{
    std::vector<int> nullData;
    
    while (popupStatus < 4)
    {
        if (popupStatus == 0) // 초보 유저에 대한 포션 보상
        {
            if (isStartUser)
            {
                //myInfo->SetMsgCnt(myInfo->GetMsgCnt()+1); // 메시지함 개수 1개 추가
                //myInfo->SetPotion(5, 0);
                isStartUser = false;
                Common::ShowPopup(this, "GameReady", "NoImage", false, POTION_REWARD, BTN_1, nullData);
                return;
            }
        }
        else if (popupStatus == 1) // MP 50의 배수, 100의 배수 관련 보상
        {
            if (myInfo->GetRewardTopaz() > 0)
            {
                myInfo->SetMsgCnt(myInfo->GetMsgCnt()+1); // 메시지함 개수 1개 추가
                Common::ShowPopup(this, "GameReady", "NoImage", false, MP_REWARD_100, BTN_1, nullData);
                return;
            }
            else if (myInfo->IsRewardPotion())
            {
                Common::ShowPopup(this, "GameReady", "NoImage", false, MP_REWARD_50, BTN_1, nullData);
                return;
            }
        }
        else if (popupStatus == 2) // 요정을 구입할 수 있는 경우
        {
            if (!isPossibleBuyFairyShown)
            {
                for (int i = 0 ; i < fairyInfo.size() ; i++) // 한 요정이라도 살 수 있는 조건이 충족되면 팝업창을 띄운다 (게임중 한번만)
                {
                    bool flag = false;
                    for (int j = 0 ; j < myInfo->GetFairyList().size() ; j++)
                    {
                        if (myInfo->GetFairyList()[j]->GetId() == fairyInfo[i]->GetId())
                        {
                            flag = true;
                            break;
                        }
                    }
                    
                    if (!flag && myInfo->GetMPTotal() >= fairyInfo[i]->GetRefVal())
                    {
                        char name[30];
                        sprintf(name, "buyingFairy_%d", fairyInfo[i]->GetId());
                        if (!CCUserDefault::sharedUserDefault()->getBoolForKey(name, false))
                        {
                            CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
                            isPossibleBuyFairyShown = true;
                            doNotShowExpPopup = true;
                            std::vector<int> data;
                            data.push_back(i);
                            Common::ShowPopup(this, "GameReady", "NoImage", false, POSSIBLE_BUY_FAIRY, BTN_1, data);
                            return;
                        }
                    }
                }
            }
        }
        else if (popupStatus == 3) // 현재 연습중인 스킬이 max exp일 경우 팝업창
        {
            MySkill* ms = MySkill::GetObj(myInfo->GetPracticeSkillId());
            if (!doNotShowExpPopup)
            {
                if ( ms != NULL && ms->GetExp() == SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()) )
                {
                    doNotShowExpPopup = true;
                    Common::ShowPopup(this, "GameReady", "NoImage", false, PRACTICE_SKILL_FULL_EXP, BTN_1, nullData);
                    return;
                }
            }
            ms = NULL;
        }
        
        popupStatus++;
    }
}

void GameReady::RenewInfo()
{
    // 선택된 아이템들의 cost를 보고, 별사탕/토파즈가 모자라면 취소시킨다.
    char name3[40];
    minusCost_starcandy = 0;
    minusCost_topaz = 0;
    for (int i = 0 ; i < 5 ; i++)
    {
        sprintf(name3, "item_0%d", i);
        itemSelected[i] = CCUserDefault::sharedUserDefault()->getBoolForKey(name3, false);
        if (itemSelected[i] && myInfo->GetItem(i) <= 0)
        {
            if (itemType[i] == ITEM_TOPAZ)
                minusCost_topaz += itemCost[i];
            else if (itemType[i] == ITEM_STARCANDY)
                minusCost_starcandy += itemCost[i];
        }
    }
    if (myInfo->GetTopaz()-minusCost_topaz < 0) // 돈이 모자라니 아이템을 취소시키고 선택 해제.
    {
        for (int i = 0 ; i < 4 ; i++)
        {
            sprintf(name3, "item_0%d", i);
            if (itemType[i] == ITEM_TOPAZ) // 체크 선택모양 해제 및 클라이언트 변수 false
            {
                itemSelected[i] = false;
                CCUserDefault::sharedUserDefault()->setBoolForKey(name3, false);
                ((CCSprite*)spriteClass->FindSpriteByTag(-888*(i+1)))->setOpacity(0);
                ((CCSprite*)spriteClass->FindSpriteByTag(-777*(i+1)))->setOpacity(0);
            }
        }
        minusCost_topaz = 0;
    }
    if (myInfo->GetStarCandy()-minusCost_starcandy < 0) // 마찬가지
    {
        for (int i = 0 ; i < 4 ; i++)
        {
            sprintf(name3, "item_0%d", i);
            if (itemType[i] == ITEM_STARCANDY) // 체크 선택모양 해제 및 클라이언트 변수 false
            {
                itemSelected[i] = false;
                CCUserDefault::sharedUserDefault()->setBoolForKey(name3, false);
                ((CCSprite*)spriteClass->FindSpriteByTag(-888*(i+1)))->setOpacity(0);
                ((CCSprite*)spriteClass->FindSpriteByTag(-777*(i+1)))->setOpacity(0);
            }
        }
        minusCost_starcandy = 0;
    }
    
    // 부스터 아이템을 사용할 수 없는 경우 (연습중 스킬 없음 or 강화에 필요한 연습량이 1 남았을 때)
    ((CCSprite*)spriteClass->FindSpriteByTag(-3334*(4+1)))->setColor(ccc3(255,255,255));
    ((CCSprite*)spriteClass->FindSpriteByTag(-3333*(4+1)))->setColor(ccc3(255,255,255));
    int psid = myInfo->GetPracticeSkillId();
    int plv = myInfo->GetPracticeSkillLv();
    if (psid <= 0 || SkillBuildUpInfo::GetMaxExp(psid, plv) - MySkill::GetObj(psid)->GetExp() <= 1)
    {
        ((CCSprite*)spriteClass->FindSpriteByTag(-3334*(4+1)))->setColor(ccc3(140,140,140));
        ((CCSprite*)spriteClass->FindSpriteByTag(-3333*(4+1)))->setColor(ccc3(140,140,140));
        itemSelected[4] = false;
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_04", false);
        
        int tag = 5;
        if (myInfo->GetItem(tag-1) > 0) // 아이템을 보유하고 있는 경우 원상태로 되돌리기
        {
            itemNumberLayer[tag-1]->removeAllChildren();
            itemNumberLayer[tag-1]->removeFromParentAndCleanup(true);
            itemNumberLayer[tag-1] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(tag-1)));
            itemNumberLayer[tag-1]->setPosition(ccp(84+193*(tag-1)+130, 965));
            tLayer->addChild(itemNumberLayer[tag-1], 10);
        }

        CCSprite* border = (CCSprite*)spriteClass->FindSpriteByName("background/bg_skill_select.png5");
        CCSprite* check = (CCSprite*)spriteClass->FindSpriteByName("icon/icon_check.png5");
        border->setOpacity(0);
        check->setOpacity(0);
    }
    
    
    // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
    ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()-minusCost_topaz).c_str());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()-minusCost_starcandy).c_str());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    
    // 포션 개수에 따른 포션 아이콘 업데이트
    char name[40];
    for (int i = 0 ; i < 5 ; i++)
    {
        sprintf(name, "icon/icon_potion.png%d", i);
        if (i < myInfo->GetPotion())
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        else
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
    }
}

void GameReady::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);

    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "GameReady", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "GameReady", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "GameReady", this, 5, 0, 255, 3) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 687), CCSize(982, 725), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(77, 1126), CCSize(929, 268), "", "Layer", tLayer, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(77, 726), CCSize(929, 200), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2", ccp(0, 0), ccp(92, 1143), CCSize(309, 236), "", "Layer", tLayer, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(112, 1165), CCSize(274, 53), "", "Layer", tLayer, 4, 128) );
    
    
    // property 문양 (mp 바로 밑에)
    InitProperties();
    
    // roof
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "Layer", tLayer, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    // 버튼들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // make potion
    char name[40], name2[40];
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "Layer", tLayer, 4) );
        
        sprintf(name2, "icon/icon_potion.png%d", i);
        if (i < myInfo->GetPotion())
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 1000, 1, 255) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 1000, 1, 0) );
    }
    
    // potion-remain-time
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainPotionTime(), fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "Layer", tLayer, 5, 0, 255, 4) );

    // strap, 제일 아래 버튼 등
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_blue.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Layer", tLayer, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_gameready.png", ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Layer", tLayer, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Layer", tLayer, 11) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png", ccp(0, 0), ccp(408+11, 1143+165), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(408+55, 1143+187), CCSize(0, 0), "", "Layer", tLayer, 5) );
    

    // 선택된 아이템들의 cost를 보고, 별사탕/토파즈가 모자라면 취소시킨다.
    char name3[40];
    int minus_starcandy = 0;
    int minus_topaz = 0;
    for (int i = 0 ; i < 4 ; i++) // 부스터 아이템은 제외
    {
        sprintf(name3, "item_0%d", i);
        itemSelected[i] = CCUserDefault::sharedUserDefault()->getBoolForKey(name3, false);
        if (itemSelected[i] && myInfo->GetItem(i) <= 0)
        {
            if (itemType[i] == ITEM_TOPAZ)
                minus_topaz += itemCost[i];
            else if (itemType[i] == ITEM_STARCANDY)
                minus_starcandy += itemCost[i];
        }
    }

    // 부스터 아이템은 항상 클라이언트 데이터 false
    itemSelected[4] = false;
    CCUserDefault::sharedUserDefault()->setBoolForKey("item_04", false);
    
    // 5 items
    int alpha;
    for (int i = 0 ; i < 5; i++)
    {
        sprintf(name3, "item_0%d", i);
        itemSelected[i] = CCUserDefault::sharedUserDefault()->getBoolForKey(name3, false);
        
        // 아이템이 0개인데 돈은 모자라고 선택되어 있는 경우, 클라이언트에 false로 저장한다.
        if ( (itemCost[i] == ITEM_STARCANDY && myInfo->GetStarCandy() < minus_starcandy) ||
             (itemCost[i] == ITEM_TOPAZ && myInfo->GetTopaz() < minus_topaz) )
        {
            CCUserDefault::sharedUserDefault()->setBoolForKey(name3, false);
            itemSelected[i] = false;
        }
    
        sprintf(name, "background/bg_skill_brown.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(84+193*i, 972), CCSize(0, 0), "", "Layer", tLayer, 5) );

        if (itemSelected[i]) alpha = 255;
        else alpha = 0;
        
        sprintf(name2, "background/bg_skill_select.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 6, 1, alpha, -888*(i+1)) ); // tag = -888*(i+1)
        
        sprintf(name3, "icon/icon_check.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name3, ccp(0, 0), ccp(100, 110), CCSize(0, 0), name2, "0", NULL, 6, 2, alpha, -777*(i+1)) ); // tag = -777*(i+1)
        
        // 아이템 배경
        sprintf(name2, "background/bg_skill_yellow.png%d", i+1);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 5, 1, 255, -3333*(i+1)) ); // tag = -555*(i+1)
        
        
        // 아이템 아이콘
        CCPoint p = spriteClass->FindParentCenterPos(name);
        if (i == 0)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_clear.png", ccp(0.5, 0.5), ccp(p.x+1, p.y+2), CCSize(0, 0), name, "0", NULL, 6, 1) );
        else if (i == 1)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_time.png", ccp(0.5, 0.5), p, CCSize(0, 0), name, "0", NULL, 6, 1) );
        else if (i == 2)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_paint.png", ccp(0.5, 0.5), p, CCSize(0, 0), name, "0", NULL, 6, 1) );
        else if (i == 3)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_staff.png", ccp(0.5, 0.5), p, CCSize(0, 0), name, "0", NULL, 6, 1) );
        else if (i == 4)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_item_booster.png", ccp(0.5, 0.5), ccp(p.x-3, p.y-3), CCSize(0, 0), name, "0", NULL, 6, 1, 255, -3334*(i+1)) ); // tag = -333*(i+1)
        
        if (myInfo->GetItem(i) > 0)
        {
            // 아이템 별 남은 개수
            if (itemSelected[i])
                itemNumberLayer[i] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(i)-1));
            else
                itemNumberLayer[i] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(i)));
            itemNumberLayer[i]->setPosition(ccp(84+193*i+130, 965));
            tLayer->addChild(itemNumberLayer[i], 10);
        }
        else
        {
            // 남은 개수가 0이면 가격을 적는다 (별사탕 x개 or 토파즈 y개)
            if (itemType[i] == ITEM_STARCANDY)
            {
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(84+193*i-15, 925), CCSize(0, 0), "", "Layer", tLayer, 5) );
                spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(itemCost[i]), fontList[0], 36, ccp(0, 0), ccp(84+193*i +45, 935), ccc3(255,255,255), "", "Layer", tLayer, 5) );
            }
            else if (itemType[i] == ITEM_TOPAZ)
            {
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(84+193*i-15+30, 925), CCSize(0, 0), "", "Layer", tLayer, 5) );
                spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(itemCost[i]), fontList[0], 36, ccp(0, 0), ccp(84+193*i +45+30, 935), ccc3(255,255,255), "", "Layer", tLayer, 5) );
            }
        }
    }
    
    // 부스터 아이템을 사용할 수 없는 경우 (연습중 스킬 없음 or 강화에 필요한 연습량이 1 남았을 때)
    int psid = myInfo->GetPracticeSkillId();
    int plv = myInfo->GetPracticeSkillLv();
    if (psid <= 0 || SkillBuildUpInfo::GetMaxExp(psid, plv) - MySkill::GetObj(psid)->GetExp() <= 1)
    {
        ((CCSprite*)spriteClass->FindSpriteByTag(-3334*(4+1)))->setColor(ccc3(140,140,140));
        ((CCSprite*)spriteClass->FindSpriteByTag(-3333*(4+1)))->setColor(ccc3(140,140,140));
    }
    
    // 아이템별 설명
    int idx = -1;
    for (int i = 4 ; i >= 0 ; i--) // 선택된 아이템 중 가장 처음 것을 구한다.
        if (itemSelected[i])
            idx = i;
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(ItemDesc(idx).c_str(), fontList[0], 36, ccp(0.5, 0), ccp(winSize.width/2+10, 736), ccc3(78,47,8), CCSize(880, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5, 0, 255, 10101) ); // tag = 10101
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png5", ccp(0, 0), ccp(49, 458), CCSize(982, 223), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(77, 488), CCSize(782, 177), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", ccp(0, 0), ccp(896, 572), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png4", ccp(0, 0), ccp(867, 497), CCSize(136, 63), "", "Layer", tLayer, 1) );
    
    // 슬롯 옆의 {현재 구입한 슬롯수} / {구입 가능한 max 슬롯수}
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 44, ccp(1, 0), ccp(892+30, 505), ccc3(255,219,53), "", "Layer", tLayer, 5, 0, 255, 100) ); // 현재 슬롯 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 34, ccp(0, 0), ccp(927+3, 505), ccc3(182,142,142), "", "Layer", tLayer, 5) ); // 젼체 슬롯 개수
    

    // 게임시작 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(318, 193), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gamestart.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // 스케치북 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_sketchbook.png", ccp(0, 0), ccp(67, 1920-1569-96), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_sketchbook.png", ccp(0, 0), ccp(53, 1920-1674-44), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // 오늘의 별사탕 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_todaycandy.png", ccp(0, 0), ccp(886-50, 1920-1674-44), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // 코코 그림
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_ready.png", ccp(0.5, 0.5), ccp(250, 1190+100), CCSize(0, 0), "", "Layer", tLayer, 3) );
    ((CCSprite*)spriteClass->FindSpriteByName("image/coco_ready.png"))->setScale(0.85f);
    
    // 지팡이 레벨 표시
    sprintf(name, "지팡이 %dLv", myInfo->GetStaffLv());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1190), ccc3(121,71,0), "", "Layer", tLayer, 5, 0, 255, -108922) ); // tag = -108922
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), ccp(250, 1193), ccc3(255,219,53), "", "Layer", tLayer, 5, 0, 255, -108921) ); // tag = -108921
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // 아이템 관련 바뀐 것들 체크
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    // 게임시작 버튼 움직이기
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_blue.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5,0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gamestart.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action->copy()));
    
    
    // 슬롯구입 힌트
    ShowHintOfBuyingSlot();
}

void GameReady::ShowHintOfBuyingSlot()
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
        balloon->setContentSize(CCSize(600, 180));
        balloon->setAnchorPoint(ccp(1, 0));
        balloon->setPosition(ccp(896+100, 572+55));
        this->addChild(balloon, 100);
        ball = CCLabelTTF::create("여기서 슬롯을 구매하여\n두번째 발동스킬을 장착하세요!", fontList[0].c_str(), 36);
        ball->setPosition(ccp(600/2, 180/2+30));
        ball->setColor(ccc3(255,255,255));
        balloon->addChild(ball, 101);
        
        CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
        balloon->runAction( CCRepeatForever::create(action) );
    }
}

void GameReady::ShowHintOfMP()
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

void GameReady::InitProperties()
{
    spriteClassProperty->RemoveAllObjects();
    
    CCPoint pos = ccp(848, 1611);
    if (myInfo->IsFire())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png1"), CCSize(0, 0), "background/bg_property.png1", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    if (myInfo->IsWater())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png2"), CCSize(0, 0), "background/bg_property.png2", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    if (myInfo->IsLand())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), pos, CCSize(0, 0), "", "GameReady", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png3"), CCSize(0, 0), "background/bg_property.png3", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    
    for (int i = 0 ; i < spriteClassProperty->spriteObj.size() ; i++)
        spriteClassProperty->AddChild(i);
}



void GameReady::InitFairy()
{
    // init
    spriteClassFairy->RemoveAllObjects();
    CCNode* pic = fairyLayer->getChildByTag(99999);
    if (pic != NULL)
        pic->removeAllChildren();
    fairyLayer->removeAllChildren();
    
    fairyLayer->setAnchorPoint(ccp(0, 0));
    fairyLayer->setPosition(ccp(408, 1143));
    
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3", ccp(0, 0), ccp(0, 0), CCSize(309, 236), "", "Layer", fairyLayer, 4) );
    
    fairyLayer->setContentSize(CCSizeMake(309, 236));
    
    int fid = myInfo->GetActiveFairyId();
    int flv = myInfo->GetActiveFairyLevel();
    //CCLog("fid flv : %d %d", fid, flv);
    FairyInfo* f = FairyInfo::GetObj(fid);
    
    // 요정 그림
    CCLayer* picture = Fairy::GetFairy(fid);
    picture->setAnchorPoint(ccp(0, 0));
    switch (fid)
    {
        case 1:
            picture->setPosition(ccp(309/2+10, 236/2+23));
            picture->setScale(0.63f);
            break;
        case 2:
            picture->setPosition(ccp(309/2, 236/2+15));
            picture->setScale(0.7f);
            break;
        case 3:
            picture->setPosition(ccp(309/2, 236/2+23));
            picture->setScale(0.8f);
            break;
        default:
            picture->setPosition(ccp(309/2, 236/2+23));
            picture->setScale(0.9f);
            break;
    }
 
    picture->setTag(99999);
    fairyLayer->addChild(picture, 5);
    
    // 요정 등급
    char fname[30];
    if (fid > 0)
    {
        if (f->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png");
        else if (f->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png");
        else if (f->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png");
        else if (f->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png");
        spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(11, 165), CCSize(0, 0), "", "Layer", fairyLayer, 5) );
    }
    
    if (fid > 0)
    {
        // 요정 레벨 (+그 배경)
        spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(55, 187), CCSize(0, 0), "", "Layer", fairyLayer, 6) );
        sprintf(fname, "%d Lv", flv);
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 24, ccp(0.5, 0.5), ccp(97, 202), ccc3(255,255,255), "", "Layer", fairyLayer, 6) );
    }
    
    if (fid > 0)
    {
        // 요정 이름
        if (fid == 0) sprintf(fname, "요정 없음");
        else sprintf(fname, "%s", f->GetName().c_str());
        spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 30, ccp(0, 0), ccp(13, 85), ccc3(255,255,255), "", "Layer", fairyLayer, 6) );
    }
    
    // 요정 특수능력 (+그 배경)
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(19, 22), CCSize(274, 53), "", "Layer", fairyLayer, 6) );
    
    if (fid > 0)
        sprintf(fname, "%s", FairyInfo::GetAbilityDesc(f->GetType(), false).c_str());
    else
        sprintf(fname, "요정 없음");
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2), ccc3(121,71,0), "", "Layer", fairyLayer, 6) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2+3), ccc3(255,219,53), "", "Layer", fairyLayer, 6) );
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
        spriteClassFairy->AddChild(i);
}

void GameReady::InitSkill()
{
    // init
    spriteClassSkill->RemoveAllObjects();
    skillLayer->removeAllChildren();
    
    skillLayer->setAnchorPoint(ccp(0, 0));
    skillLayer->setPosition(ccp(725, 1143));
    
    int sid = myInfo->GetPracticeSkillId();
    int slv = myInfo->GetPracticeSkillLv();
    //CCLog("sid, siv = %d %d", sid, slv);
    
    // 배경판
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png4", ccp(0, 0), ccp(0, 0), CCSize(263, 236), "", "Layer", skillLayer, 1) );
    
    skillLayer->setContentSize(CCSizeMake(263, 236));
    
    // 스킬 배경
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2", ccp(0, 0), ccp(19, 22), CCSize(228, 53), "", "Layer", skillLayer, 5) );
    CCPoint pos = spriteClassSkill->FindParentCenterPos("background/bg_gameready_name.png2");
    if (sid == 0) // 배운 스킬이 없는 경우 '스킬 없음' 표시
    {
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel("스킬 없음", fontList[2], 30, ccp(0.5, 0.5), ccp(pos.x, pos.y+2), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 2) );
    }
    
    // 스킬 문양 (+그 배경판)
    spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(63, 81), CCSize(0, 0), "", "Layer", skillLayer, 5) );
    if (sid > 0) // 스킬이 있는 경우에만 실제 문양
    {
        char skillName[20];
        // 스킬 옆에 삼각형 아이콘 색깔 (속성별로)
        switch (sid / 10)
        {
            case 2: // fire
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_red.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            case 1: // water
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_blue.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            case 3: // land
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_green.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
            default: // master
                spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_purple.png", ccp(0, 0), ccp(63, 161), CCSize(0, 0), "", "Layer", skillLayer, 5) );
                break;
        }
        // 스킬 문양
        sprintf(skillName, "skill_%d.png", sid);
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0.5, 0.5), spriteClassSkill->FindParentCenterPos("background/bg_skill_brown.png"), CCSize(0, 0), "background/bg_skill_brown.png", "0", NULL, 5, 1) );
        // 스킬 이름
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel(DataProcess::FindSkillNameById(sid), fontList[2], 30, ccp(0.5, 0.5), ccp(19+228/2,22+53/2), ccc3(255,255,255), "", "Layer", skillLayer, 7) );
        
        
        // Lv. <- 이 그림
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(19, 850-772), CCSize(0, 0), "", "Layer", skillLayer, 5) );
        
        char name[40];
        int offset = 0;
        if (slv >= 10) // 10의 자리
        {
            sprintf(name, "number/level_%d.png", slv / 10);
            spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(19+43+3, 850-772), CCSize(0,0), "",   "Layer", skillLayer, 6) );
            offset = spriteClassSkill->spriteObj[spriteClassSkill->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(skillName, "number/level_%d.png", slv % 10);
        spriteClassSkill->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0, 0), ccp(19+43+3+offset, 850-772), CCSize(0, 0), "", "Layer", skillLayer, 5) );
        
        // 연습량
        sprintf(skillName, "%d", MySkill::GetObj(sid)->GetExp());
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel(skillName, fontList[0], 32, ccp(1, 0), ccp(797-699+100, 850-772), ccc3(255,219,53), "", "Layer", skillLayer, 5) ); // 현재 연습량
        sprintf(skillName, "/%d", SkillBuildUpInfo::GetMaxExp(sid, slv));
        spriteClassSkill->spriteObj.push_back( SpriteObject::CreateLabel(skillName, fontList[0], 28, ccp(0, 0), ccp(797-699+105, 850-772+3), ccc3(255,255,255), "", "Layer", skillLayer, 5) ); // 이 레벨의 max 경험치량
    }
    
    for (int i = 0 ; i < spriteClassSkill->spriteObj.size() ; i++)
        spriteClassSkill->AddChild(i);
    
    
    if (sid > 0 && MySkill::GetObj(sid)->GetExp() > 0)
    {
        // 연습 중인 스킬이 있으며, 연습량이 0보다 클 때
        sp9 = CCScale9Sprite::createWithSpriteFrameName("background/bg_gameready_name.png");
        //sp9->setContentSize(CCSize(228 * MySkill::GetObj(sid)->GetExp() / SkillBuildUpInfo::GetMaxExp(sid, slv), 53));
        float width = 228.0f * (float)MySkill::GetObj(sid)->GetExp() / (float)SkillBuildUpInfo::GetMaxExp(sid, slv);
        sp9->setContentSize(CCSize(228, 53));
        sp9->setAnchorPoint(ccp(0,0));
        sp9->setPosition(ccp(19, 22));
        sp9->setColor(ccc3(170,170,170));

        stencil = CCDrawNode::create();
        CCPoint ver[] = { ccp(19, 22+3), ccp(19+width, 22+3), ccp(19+width, 22+3+53), ccp(19, 22+3+53) };
        stencil->drawPolygon(ver, 4, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
        clip = CCClippingNode::create(stencil);
        clip->addChild(sp9, 5);
        skillLayer->addChild(clip, 5);
    }
}


void GameReady::MakeScrollSlot()
{
    scrollViewSlot->removeAllChildren();
    
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


std::string GameReady::ItemDesc(int idx)
{
    // '코코가 그리는 마법에 오신 것을 환영합니다!'
    switch (idx)
    {
        case 0: return "[4색 피스]\n게임 중 흰색 피스가 등장하지 않아요."; break;
        case 1: return "[+5초 시계]\n게임 시간이 5초 늘어나요."; break;
        case 2: return "[색깔붓]\n원하는 방향으로 같은 색깔을 만드는 색깔붓이 등장해요."; break;
        case 3: return "[마법지팡이]\n노란색 또는 흰색을 모두 터뜨리는 지팡이가 등장해요."; break;
        case 4: return "[부스터]\n미션성공 시 연습중인 마법의 경험치가 3배 증가해요."; break;
    }
    return "코코가 그리는 마법에 오신 것을 환영합니다!";
}

void GameReady::PotionTimer(float f)
{
    int remainTime, potion;
    
    potion = myInfo->GetPotion();
    remainTime = myInfo->GetRemainPotionTimeNumber();
    
    if (potion <= 5)
    {
        if (potion > 0 && remainTime == 720-1)
        {
            //CCLog("GameReady: PotionTimer -> potion = %d", potion);
            char name[25];
            sprintf(name, "icon/icon_potion.png%d", potion-1);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        }
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    }
}


bool GameReady::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
    
    if (isTouched || isStarting)
        return false;
    isTouched = true;
    isScrollViewTouched = false;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    
    // 슬롯구매 힌트 있으면 안 보이게 한다.
    if (balloon != NULL && ball != NULL)
    {
        balloon->setOpacity(0);
        ball->setOpacity(0);
    }
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                callbackType = 0;
                EndSceneCallback(NULL, this);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false, 0);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false, 0);
                break;
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
        else if (spriteClass->spriteObj[i]->name == "background/bg_potion_time.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyPotion", false, 0);
                break;
            }
        }
        // item select 관련
        else if (spriteClass->spriteObj[i]->name.substr(0, 29) == "background/bg_skill_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                std::string name = spriteClass->spriteObj[i]->name;
                int tag = name[name.size()-1] - '0';
                
                // 부스터 아이템을 사용할 수 없는 경우 (연습중 스킬 없음 or 강화에 필요한 연습량이 1 남았을 때)
                if (tag == 5)
                {
                    int psid = myInfo->GetPracticeSkillId();
                    int plv = myInfo->GetPracticeSkillLv();
                    if (psid <= 0 || SkillBuildUpInfo::GetMaxExp(psid, plv) - MySkill::GetObj(psid)->GetExp() <= 1)
                        return true;
                }
                
                sound->playClick();
                
                char n1[35], n2[35], itemName[8];
                sprintf(n1, "background/bg_skill_select.png%c", name[name.size()-1]);
                sprintf(n2, "icon/icon_check.png%c", name[name.size()-1]);
                
                if (!itemSelected[tag-1]) // 비선택 -> 선택
                {
                    if (myInfo->GetItem(tag-1) <= 0)
                    {
                        if (itemType[tag-1] == ITEM_STARCANDY)
                        {
                            if (myInfo->GetStarCandy()-minusCost_starcandy < itemCost[tag-1]) // 별사탕이 모자란 경우
                            {
                                std::vector<int> nullData;
                                Common::ShowPopup(this, "GameReady", "NoImage", false, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
                                return true;
                            }
                        }
                        else if (itemType[tag-1] == ITEM_TOPAZ)
                        {
                            if (myInfo->GetTopaz()-minusCost_topaz < itemCost[tag-1]) // 토파즈가 모자란 경우
                            {
                                std::vector<int> nullData;
                                Common::ShowPopup(this, "GameReady", "NoImage", false, NEED_TO_BUY_TOPAZ, BTN_2, nullData);
                                return true;
                            }
                        }
                    }
                    else
                    {
                        // 아이템 남은 개수 변경 (실제로는 아님)
                        itemNumberLayer[tag-1]->removeAllChildren();
                        itemNumberLayer[tag-1]->removeFromParentAndCleanup(true);
                        itemNumberLayer[tag-1] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(tag-1)-1));
                        itemNumberLayer[tag-1]->setPosition(ccp(84+193*(tag-1)+130, 965));
                        tLayer->addChild(itemNumberLayer[tag-1], 10);
                    }
                }
                else // 선택 -> 비선택
                {
                    if (myInfo->GetItem(tag-1) > 0) // 아이템을 보유하고 있는 경우
                    {
                        // 아이템 남은 개수 변경 (실제로는 아님)
                        itemNumberLayer[tag-1]->removeAllChildren();
                        itemNumberLayer[tag-1]->removeFromParentAndCleanup(true);
                        itemNumberLayer[tag-1] = Common::MakeItemNumberLayer(Common::MakeComma(myInfo->GetItem(tag-1)));
                        itemNumberLayer[tag-1]->setPosition(ccp(84+193*(tag-1)+130, 965));
                        tLayer->addChild(itemNumberLayer[tag-1], 10);
                    }
                }
                
                
                itemSelected[tag-1] = !itemSelected[tag-1];
                
                // 아이템 설명 변경
                ((CCLabelTTF*)spriteClass->FindLabelByTag(10101))->setString(ItemDesc(tag-1).c_str());
                
                // user default에도 바로바로 저장.
                sprintf(itemName, "item_0%d", tag-1);
                CCUserDefault::sharedUserDefault()->setBoolForKey(itemName, itemSelected[tag-1]);
                
                CCString* param = CCString::create("2");
                CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
                
                CCSprite* border = (CCSprite*)spriteClass->FindSpriteByName(n1);
                CCSprite* check = (CCSprite*)spriteClass->FindSpriteByName(n2);
                border->setOpacity(255-border->getOpacity());
                check->setOpacity(255-check->getOpacity());
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_blue.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                if (myInfo->GetPotion() > 0)
                {
                    spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gamestart.png"))->setColor(ccc3(170,170,170));
                    rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                    kind = BTN_MENU_GAMESTART;
                    idx = i;
                    return true;
                }
                else
                {
                    // 포션 부족 - 구매창 유도 팝업
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "GameReady", "NoImage", false, NEED_TO_BUY_POTION, BTN_2, nullData);
                    return true;
                }
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_board_brown.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 1);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_sketchbook.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                //if (myInfo->HasNoProperty()) // 속성이 없으면 속성선택창을 띄운다.
                //    Common::ShowNextScene(this, "GameReady", "SelectProperty", false, 0);
                if (!CCUserDefault::sharedUserDefault()->getBoolForKey("is_tutorial_done", false) &&
                         !myInfo->HasNoProperty() && myInfo->GetSkillList().size() <= 2) // 튜토리얼 시작한다.
                    Common::ShowNextScene(this, "GameReady", "T_Sketchbook", false, 1);
                else
                    Common::ShowNextScene(this, "GameReady", "Sketchbook", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_todaycandy.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 3);
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
                    Common::ShowPopup(this, "GameReady", "NoImage", false, BUY_SKILLSLOT_FULL, BTN_1, data);
                else
                {
                    data.push_back((int)myInfo->GetSlot().size()+1);
                    data.push_back(SkillSlotInfo::GetCost((int)myInfo->GetSlot().size()+1));
                    if (SkillSlotInfo::GetCostType((int)myInfo->GetSlot().size()+1) == 1)
                        Common::ShowPopup(this, "GameReady", "NoImage", false, BUY_SKILLSLOT_BY_STARCANDY_TRY, BTN_2, data, 2);
                    else
                        Common::ShowPopup(this, "GameReady", "NoImage", false, BUY_SKILLSLOT_BY_TOPAZ_TRY, BTN_2, data, 2);
                }
                return true;
            }
        }

    }
    
    if (fairyLayer->boundingBox().containsPoint(point))
    {
        sound->playClickboard();
        Common::ShowNextScene(this, "GameReady", "CocoRoom", false, 2);
        return true;
    }
    else if (skillLayer->boundingBox().containsPoint(point))
    {
        sound->playClickboard();
        //if (myInfo->HasNoProperty()) // 속성이 없으면 속성선택창을 띄운다.
        //    Common::ShowNextScene(this, "GameReady", "SelectProperty", false, 0);
        if (!CCUserDefault::sharedUserDefault()->getBoolForKey("is_tutorial_done", false) &&
                 !myInfo->HasNoProperty() && myInfo->GetSkillList().size() <= 2) // 튜토리얼 시작한다.
            Common::ShowNextScene(this, "GameReady", "T_Sketchbook", false, 1);
        else
            Common::ShowNextScene(this, "GameReady", "Sketchbook", false, 0);
        return true;
    }
 
    return true;
}


void GameReady::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void GameReady::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isStarting)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && isScrollViewTouched && scrollViewSlot->boundingBox().containsPoint(point))
    {
        sound->playBoardMove(); // 이 scene만 사운드가 다르다.
        Common::ShowNextScene(this, "GameReady", "MagicList", false, 0);
        return;
    }
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gamestart.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        switch (kind)
        {
            case BTN_MENU_GAMESTART:
                StartGame();
                break;
        }
    }
    
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
}

void GameReady::StartGame()
{
    isKeybackTouched = true; // 시작하면 백버튼 터치 못하게.
    isStarting = true;
    
    sound->playGameStart();
    
    CCPoint p = ccp(530+50, 1508+15);
    if (myInfo->GetPotion() <= 5)
        p = ccp(89+83*(myInfo->GetPotion()-1)+60/2, 1480+82/2);
    
    CCParticleSystemQuad* m_emitter = CCParticleSystemQuad::create("particles/gamestart.plist");
    m_emitter->setAnchorPoint(ccp(0.5, 0.5));
    m_emitter->setPosition(p);
    this->addChild(m_emitter, 2000);
    m_emitter->setAutoRemoveOnFinish(true);
    
    if (myInfo->GetPotion() > 5)
        myInfo->SetPotion(myInfo->GetPotion()-1, 0);
    else if (myInfo->GetPotion() == 5)
        myInfo->SetPotion(myInfo->GetPotion()-1, 720);
    else
        myInfo->SetPotion(myInfo->GetPotion()-1, -1);
    
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    if (myInfo->GetPotion() < 5)
    {
        char name[25];
        sprintf(name, "icon/icon_potion.png%d", myInfo->GetPotion());
        CCActionInterval* action = CCFadeOut::create(0.2f);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->runAction(action);
    }
    
    pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlackClose->setPosition(ccp(0, 0));
    pBlackClose->setAnchorPoint(ccp(0, 0));
    pBlackClose->setColor(ccc3(0, 0, 0));
    pBlackClose->setOpacity(0);
    this->addChild(pBlackClose, 1000);
    
    callbackType = 1;
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.8f), CCCallFuncND::create(this, callfuncND_selector(GameReady::EndSceneCallback), this), NULL);
    pBlackClose->runAction(action);
}

void GameReady::EndScene()
{
}

void GameReady::EndSceneCallback(CCNode* sender, void* pointer)
{
    GameReady* pThis = (GameReady*)pointer;
    if (sender != NULL)
        sender->removeFromParentAndCleanup(true);
    
    pThis->unschedule(schedule_selector(GameReady::PotionTimer));
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(pThis, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCString* param;
    if (pThis->callbackType == 1) // 게임시작 버튼을 누른 경우
    {
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    pThis->setKeypadEnabled(false);
    pThis->setTouchEnabled(false);

    
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
    
    // remove all CCNodes
    for (int i = 0 ; i < 5 ; i++)
    {
        if (itemNumberLayer[i] != NULL)
        {
            itemNumberLayer[i]->removeAllChildren();
            itemNumberLayer[i]->removeFromParentAndCleanup(true);
        }
    }
    pThis->spriteClass->RemoveAllObjects();
    delete pThis->spriteClass;
    pThis->spriteClassProperty->RemoveAllObjects();
    delete pThis->spriteClassProperty;
    pThis->spriteClassFairy->RemoveAllObjects();
    delete pThis->spriteClassFairy;
    pThis->spriteClassSkill->RemoveAllObjects();
    delete pThis->spriteClassSkill;
    pThis->spriteClassSlot->RemoveAllObjects();
    delete pThis->spriteClassSlot;
    
    scrollViewSlot->getContainer()->removeAllChildren();
    scrollViewSlot->removeAllChildren();
    scrollViewSlot->removeFromParent();
    
    pThis->pBlack->removeFromParentAndCleanup(true);
    CCNode* pic = pThis->fairyLayer->getChildByTag(99999);
    if (pic != NULL)
        pic->removeAllChildren();
    pThis->fairyLayer->removeAllChildren();
    pThis->fairyLayer->removeFromParentAndCleanup(true);
    pThis->skillLayer->removeAllChildren();
    pThis->skillLayer->removeFromParentAndCleanup(true);
    
    pThis->tLayer->removeAllChildren();
    pThis->tLayer->removeFromParentAndCleanup(true);
    
    pThis->removeFromParentAndCleanup(true);
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    
    if (pThis->callbackType == 1)
    {
        // Ranking도 끝낸다.
        param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
}

void GameReady::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void GameReady::scrollViewDidZoom(CCScrollView* view)
{
}

