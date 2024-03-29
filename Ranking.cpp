#include "Ranking.h"
#include "pugixml/pugixml.hpp"
#include "curl/curl.h"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

static int fromWhere;

enum
{
	zBackground = 0,
	zGameObject = 1,
    zGameObjectSP = 2,
    zPieceConn = 3,
};

CCScene* Ranking::scene(int from)
{
    fromWhere = from;
	CCScene* pScene = CCScene::create();
	Ranking* pLayer = Ranking::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Ranking::onEnter()
{
    //CCLog("Ranking :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
    
    isOnceScrollViewTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
    isKeybackTouched = false;
    
    if (fromWhere == 1)
        Common::ShowNextScene(this, "Ranking", "GameReady", false);
    else
        ShowPopup();
}
void Ranking::onPause()
{
    //CCLog("Ranking : onPause");
}
void Ranking::onExit()
{
    //CCLog("Ranking :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Ranking::keyBackClicked()
{
    //CCLog("%d %d", isKeybackTouched, isTouched);
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playWarning();    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Ranking", "NoImage", false, POPUP_EXIT, BTN_2, nullData);
}

bool Ranking::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isOnceScrollViewTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    isTouched = true;
    isKeybackTouched = true;
    
    popupStatus = 0;
    menuInSetting = -1;
    
    // make depth tree (처음 시작이니까 clear하고 진행)
    Depth::ClearDepth();
    Depth::AddCurDepth("Ranking", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    //CCLog("Ranking : touch prio = %d", this->getTouchPriority());
    
    srand(time(NULL));
    
    // 퍼즐게임이 끝나고 돌아온 경우 다시 spriteFrameCache를 불러와야 한다.
    if (fromWhere != -1)
    {
        //CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_1.plist");
        //CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_2.plist");
        Common::AddSpriteFramesWithFile("texture_1");
        Common::AddSpriteFramesWithFile("texture_2");
    }
    Common::AddSpriteFramesWithFile("fairy_each");
    // //CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/fairy.plist");
    //CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/fairy_each.plist");
    
    //CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    
    spriteClass = new SpriteClass();
    spriteClassProperty = new SpriteClass();
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Ranking::Notification), "Ranking", NULL);
    
    // 인게임에서 돌아온 경우 potion timer 시간 갱신한다.
    if (fromWhere != -1)
        RenewAllTime();
    
    balloon = NULL;
    ball = NULL;
    isHintOfMPShown = false;
    
    idx = -1;
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // 모든 시간에 대한 타이머 작동
    this->schedule(schedule_selector(Ranking::PotionTimer), 1.0f);
    this->schedule(schedule_selector(Ranking::ProfileTimer), 1.0f);
    
    // 효과음 , 배경음은 클라이언트에 user data로 보관해야 한다. 기본 세팅을 위한 코드.
    bool opt0 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_0", true);
    bool opt1 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_0", opt0);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_1", opt1);
    
    sound = new Sound();
    sound->PreLoadSound();
    if (opt1)
    {
        sound->ResumeBackgroundSound(); // iphone bug? (사운드 강제로 잡기 위한 것)
        sound->StopBackgroundSound();
        sound->PlayBackgroundSound();
    }

	return true;
}

void Ranking::RenewAllTime()
{
    int deltaTime = time(0) - savedTime;
    int deltaTime2 = time(0) - savedTime2;
    int deltaPotionTime = time(0) - savedMyPotionTime;

    //CCLog("%d %d %d", deltaTime, deltaTime2, savedMyPotionTime);
    //CCLog("바꾸기 전의 remainpotiontime = %d", myInfo->GetRemainPotionTimeNumber());
    isInGame = false;
    
    // 인게임에서 pause상태로 아주 오래동안 있다가 끝난 경우도 있을 수 있다. (deltaTime 값이 크겠지)
    // 따라서 갱신된 '포션남은시간'이 음수가 될 수 있는데, 12분씩 추가하면서 포션 값도 바꾸며 제자리를 찾아준다.
    int potion = myInfo->GetPotion();
    int remainPotionTime = myInfo->GetRemainPotionTimeNumber();
    if (savedMyPotionTime == -1)
        remainPotionTime -= deltaTime;
    else // 게임이 끝나고 돌아왔을 때는 이 변경값을 따른다.
        remainPotionTime -= deltaPotionTime;
    while (potion < 5 && remainPotionTime < 0) // 예외 = game_end에서 포션 개수가 갱신되었기 때문에, 이미 개수가 5개보다 많을 수도 있다.
    {
        remainPotionTime += 720;
        potion++;
        if (potion >= 5)
        {
            remainPotionTime = 0;
            break;
        }
    }
    myInfo->SetPotion(potion, std::max(0, remainPotionTime));
    
    // 주간랭킹 남은시간 갱신
    int remainWeeklyRankTime = myInfo->GetRemainWeeklyRankTimeInt() - deltaTime;
    myInfo->SetRemainWeeklyRankTime(std::max(remainWeeklyRankTime, 0));
    
    
    // 각 친구마다 포션 전송 남은시간 + 포션 요청 남은시간 + 토파즈 요청 남은시간 갱신
    int remainRequestPotionTime, remainRequestTopazTime;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        remainPotionTime = friendList[i]->GetRemainPotionTime() - deltaTime2;
        remainRequestPotionTime = friendList[i]->GetRemainRequestPotionTime() - deltaTime2;
        remainRequestTopazTime = friendList[i]->GetRemainRequestTopazTime() - deltaTime2;
        friendList[i]->SetRemainPotionTime(std::max(remainPotionTime, 0));
        friendList[i]->SetRemainRequestPotionTime(std::max(remainRequestPotionTime, 0));
        friendList[i]->SetRemainRequestTopazTime(std::max(remainRequestTopazTime, 0));
    }
}


void Ranking::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        //CCLog("Ranking : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        //this->setKeypadEnabled(true);
        //this->setTouchEnabled(true);
        this->setTouchPriority(Depth::GetCurPriority());
        scrollView->setTouchEnabled(true);
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("Ranking : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
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
        
        // message Count 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setString(Common::MakeComma(myInfo->GetMsgCnt()).c_str());
        if (myInfo->GetMsgCnt() == 0)
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(0);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(0);
        }
        else
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(255);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(255);
        }
        
        // 포션 보내기에 대한 정보 업데이트
        //for (int i = 0 ; i < friendList.size() ; i++)
        //    friendList[i]->SetPotionSprite();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("Ranking 터치 비활성");
        //this->setKeypadEnabled(false);
        //this->setTouchEnabled(false);
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        //CCLog("Ranking Noti(2) : 토파즈 = %d", myInfo->GetTopaz());
        //CCLog("토파즈 string = %s", Common::MakeComma(myInfo->GetTopaz()).c_str());
        
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
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
        
        // message Count 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setString(Common::MakeComma(myInfo->GetMsgCnt()).c_str());
        if (myInfo->GetMsgCnt() == 0)
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(0);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(0);
        }
        else
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(255);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(255);
        }
    }
    else if (param->intValue() == 3)
    {
        pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
        pBlack->setPosition(ccp(0, 0));
        pBlack->setAnchorPoint(ccp(0, 0));
        pBlack->setColor(ccc3(0, 0, 0));
        this->addChild(pBlack, 0);
    }
    else if (param->intValue() == 4)
    {
        // 인게임 시작
        EndScene();
    }
    else if (param->intValue() == 5)
    {
        // 인게임 중이 아닐 때, 모든 시간 타이머를 갱신하기 위함
        RenewAllTime();
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 종류 갱신
        InitProperties();
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        //this->setKeypadEnabled(true);
        //this->setTouchEnabled(true);
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"))->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gameready.png"))->setColor(ccc3(255,255,255));
    }
    else if (param->intValue() == 11)
    {
        ShowPopup();
    }
}

void Ranking::ShowPopup()
{
    if (popupStatus == 0)
    {
        popupStatus++;
        if (!myInfo->IsWeeklyRankReward() && myInfo->GetLastWeeklyHighScore() != -1)
            Common::ShowNextScene(this, "Ranking", "WeeklyRankResult", false);
        else
            ShowPopup();
    }
    else if (popupStatus == 1)
    {
        bool flag = true;
        for (int i = 0 ; i < noticeList.size() ; i++)
        {
            if (noticeList[i]->isShown)
                continue;
            noticeList[i]->isShown = true;

            flag = false;
            
            if (noticeList[i]->link != "")
                noticeList[i]->message += "\n(확인 버튼을 누르면 웹페이지로 연결합니다)";
            
            //Network::replaceAll(noticeList[i]->message, "\n", "\n");
            //CCLog("%s", noticeList[i]->message.c_str());
            //CCLog("%d", (int)noticeList[i]->message.find("\n"));
            
            char s[20];
            sprintf(s, "noticelist_%d", noticeList[i]->id);
            long lastTime = CCUserDefault::sharedUserDefault()->getIntegerForKey(s, -1);
            
            if (lastTime == -1 || time(0)-lastTime > 60*60*24)
            {
                // 한번도 본 적 없거나, 해제 24시간 지난 상황
                std::vector<int> data;
                data.push_back(i); // idx
                Common::ShowPopup(this, "Ranking", "NoImage", false, POPUP_NOTICE, BTN_1, data);
                break;
            }
        }
        if (flag)
        {
            popupStatus++;
            ShowPopup();
        }
    }
    else if (popupStatus == 2)
    {
        if (myInfo->IsTodayFirst() && !isAttendRewardShown)
        {
            isAttendRewardShown = true;
            Common::ShowNextScene(this, "Ranking", "AttendReward", false);
        }
        popupStatus++;
    }
}

void Ranking::InitProperties()
{
    spriteClassProperty->RemoveAllObjects();
    
    CCPoint pos = ccp(848, 1611);
    if (myInfo->IsFire())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png1"), CCSize(0, 0), "background/bg_property.png1", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    if (myInfo->IsWater())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png2"), CCSize(0, 0), "background/bg_property.png2", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    if (myInfo->IsLand())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png3"), CCSize(0, 0), "background/bg_property.png3", "0", NULL, 5, 1) );
        pos.x += 57;
    }
    
    for (int i = 0 ; i < spriteClassProperty->spriteObj.size() ; i++)
        spriteClassProperty->AddChild(i);
}

void Ranking::InitSprites()
{
    // background
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    pBackground = CCSprite::create("images/splash/main_background.png");
    pBackground->setAnchorPoint(ccp(0, 0));
    pBackground->setPosition(ccp(0, 0));
    this->addChild(pBackground, 0);
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);

    char name[30], name2[30];
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 3) );
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Ranking", this, 0) );
    
    // property 문양 (mp 바로 밑에)
    InitProperties();
    
    // 메인 지붕
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    // 플러스 아이콘들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 스트랩
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_rank.png", ccp(0, 0), ccp(118, 1920-441-88), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_rank_time.png", ccp(0, 0), ccp(423, 1391), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 스케치북 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_sketchbook.png", ccp(0, 0), ccp(67, 1920-1569-96), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_sketchbook.png", ccp(0, 0), ccp(53, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );

    // 친구추가 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png", ccp(0, 0), ccp(912, 1920-1582-86), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invitefriend.png", ccp(0, 0), ccp(886, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );
    if (isGuestLogin)
    {
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_addfriend.png"))->setColor(ccc3(140,140,140));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_invitefriend.png"))->setColor(ccc3(140,140,140));
    }
    
    
    // 게임준비 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0, 0), ccp(319, 191), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gameready.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 세팅 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_setting.png", ccp(0, 0), ccp(750, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    // 메시지함 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_msg.png", ccp(0, 0), ccp(750+90+35, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 메시지 빨간배경 + 숫자
    int opacity = 255;
    if (myInfo->GetMsgCnt() == 0)
        opacity = 0;
    CCSize msgSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_msg_setting.png", ccp(0, 0), ccp(msgSize.width-30, msgSize.height-30), CCSize(0, 0), "button/btn_msg.png", "0", NULL, 5, 1, opacity) );
    sprintf(name, "%d", myInfo->GetMsgCnt());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_msg_setting.png"), ccc3(255,255,255), "background/bg_msg_setting.png", "0", NULL, 5, 2, opacity, 0) );
    
    // make potion
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "Ranking", this, 4) );
        
        sprintf(name2, "icon/icon_potion.png%d", i);
        if (i < myInfo->GetPotion())
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 255) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 0) );
    }

    // potion-remain-time
    //CCLog("%s", myInfo->GetRemainPotionTime().c_str());
    //CCLog("%d", myInfo->GetRemainPotionTimeNumber());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainPotionTime(), fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 4) );
    
    // 주간랭킹 남은시간
    if (myInfo->GetRemainWeeklyRankTimeInt() < 86400) // '시간' 단어가 들어갈 경우
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 36, ccp(0.5, 0.5), ccp(498, 1450), ccc3(152, 49, 64), "", "Ranking", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 36, ccp(0.5, 0.5), ccp(498, 1453), ccc3(255,255,255), "", "Ranking", this, 5) );
    }
    else // '일' 단어가 들어갈 경우
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1440), ccc3(152, 49, 64), "", "Ranking", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1443), ccc3(255,255,255), "", "Ranking", this, 5) );
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("남음", fontList[0], 30, ccp(0, 0), ccp(550, 1400), ccc3(212, 212, 212), "", "Ranking", this, 5) );
    
    
    // 버튼 젤리 움직임
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5,0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gameready.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action->copy()));
    
    // 인게임 튜토리얼을 완료하지 않은 경우, 처음에 MP에 대한 말풍선을 보여준다.
    if (!CCUserDefault::sharedUserDefault()->getBoolForKey("is_inGameTutorial_done", false))
        ShowHintOfMP();
}

void Ranking::ProfileTimer(float f)
{
    // 프로필 사진 왼쪽 위 지점과 스크롤뷰 위치를 비교한다.
    // 음수가 되면, 아래에 있던 프로필이 스크롤뷰에 보이기 시작했다는 의미 -> 프로필 로딩 시작.
    CCPoint p;
    float h;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
        
        // 화면에 보이는 스프라이트 교체 (한번만 시행)
        if (psp != NULL && psp->IsLoadingDone() && !psp->IsLoadingDoneForRanking())
        {
            if (spriteClass == NULL)
                return;
            spriteClass->ChangeSprite(-888*(i+1), psp->GetProfile()); //profiles[i]->GetProfile());
            ((CCSprite*)spriteClass->FindSpriteByTag(-777*(i+1)))->setOpacity(255);
            psp->SetLoadingDoneForRanking(true);
            continue;
        }
        
        if (psp->IsLoadingStarted() || psp->IsLoadingDone())
            continue;
        
        p = friendList[i]->GetProfile()->convertToNodeSpace(scrollView->getPosition());
        h = friendList[i]->GetProfile()->getContentSize().height;
        
        if (p.y - h < 0)
        {
            psp->SetLoadingStarted(true);
            psp->SetLoadingDoneForRanking(true);
            
            char tag[6];
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(psp->GetProfileUrl().c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Ranking::onHttpRequestCompletedNoEncrypt));
            sprintf(tag, "%d", i);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
    }
}
void Ranking::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[110*110*2];
    
    // 프로필 사진 받아오기 실패
    if (!res || !res->isSucceed())
    {
        //CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    // make texture2D
    CCImage* img = new CCImage;
    img->initWithImageData(dumpData, (int)buffer->size());
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
    
    // set CCSprite (profile 모음 리스트에 갱신)
    int index = atoi(res->getHttpRequest()->getTag());
    for (int i = 0 ; i < profiles.size() ; i++)
    {
        if (profiles[i]->GetProfileUrl() == friendList[index]->GetImageUrl())
        {
            profiles[i]->SetSprite(texture);
            profiles[i]->SetLoadingDone(true);
            // 화면에 보이는 스프라이트 교체
            if (spriteClass == NULL)
                return;
            spriteClass->ChangeSprite(-888*(index+1), profiles[i]->GetProfile());
            ((CCSprite*)spriteClass->FindSpriteByTag(-777*(index+1)))->setOpacity(255);
            break;
        }
    }
}

void Ranking::MakeScroll()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    
    int numOfList = friendList.size();
    
    char rankNum[3], name[40], score[12];
    for (int i = 0 ; i < numOfList ; i++)
    {
        ////CCLog("imageurl(%d) : %s", i, friendList[i]->GetImageUrl().c_str());
        CCLayer* profileLayer = CCLayer::create();
        profileLayer->setContentSize(CCSizeMake(862, 166));
        profileLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(profileLayer, 5);
        spriteClass->layers.push_back(profileLayer);
        
        // my profile bg
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_my_profile.png", ccp(0, 0), ccp(0, 15), CCSize(862, 166-10), "", "Layer", profileLayer, 3) );
        }
        
        // rank number
        if (i < 3) // 1~3위는 이미지 사용
        {
            CCPoint pos;
            if (i == 0) pos = ccp(0, 65);
            else pos = ccp(8, 77);
            char number[28];
            sprintf(number, "icon/icon_rank_number_%d.png", i+1);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, number, ccp(0, 0), pos, CCSize(0, 0), "", "Layer", profileLayer, 5) );
        }
        else // 4위부터는 일반 font 사용
        {
            sprintf(rankNum, "%d", i+1);
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(rankNum, fontList[0], 48, ccp(0, 0), ccp(32, 115), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
        }
        
        // 프로필 이미지
        sprintf(name, "background/bg_profile.png%d", i);
        ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
        //CCLog("url = %s , %d", friendList[i]->GetImageUrl().c_str(), psp->IsLoadingDone());
        if (friendList[i]->GetImageUrl() != "" && psp->IsLoadingDone())
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(102+5, 36+11), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 0.95f) );
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(102, 36), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 1.0f, -888*(i+1)) ); // tag = -888 * (i+1)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255, -777*(i+1)) ); // tag = -777 * (i+1)
        }
        // 친구리스트에 포인터 저장.
        friendList[i]->SetProfile( spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite );
        

        // user name
        friendList[i]->GetNicknameLabel()->setAnchorPoint(ccp(0, 0));
        friendList[i]->GetNicknameLabel()->setPosition(ccp(252, 110));
        friendList[i]->GetNicknameLabel()->setColor(ccc3(78,47,8));
        profileLayer->addChild(friendList[i]->GetNicknameLabel(), 5);
        
        // user score
        sprintf(score, "%d", friendList[i]->GetWeeklyHighScore());
        CCLayer* scoreLayer = Common::MakeImageNumberLayer(score, 0);
        scoreLayer->setPosition(ccp(282, 36));
        profileLayer->addChild(scoreLayer, 5);

        // potion
        if (friendList[i]->GetPotionSprite() != NULL)
            profileLayer->addChild(friendList[i]->GetPotionSprite(), 5);
        
        // potion timer (안 보여도 그냥 포지션만 잡아 놓는다)
        friendList[i]->GetPotionLabelMin()->setPosition(ccp(765, 80));
        profileLayer->addChild(friendList[i]->GetPotionLabelMin(), 6);
        friendList[i]->GetPotionLabelMin()->setString("60:");
        friendList[i]->GetPotionLabelSec()->setPosition(ccp(773, 52));
        profileLayer->addChild(friendList[i]->GetPotionLabelSec(), 6);
        friendList[i]->GetPotionLabelSec()->setString("00");
        if (friendList[i]->GetRemainPotionTime() == 0)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(0);
            friendList[i]->GetPotionLabelSec()->setOpacity(0);
        }
        
        // dotted line
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    this->addChild(scrollView, 5);
}

void Ranking::PotionTimer(float f)
{
    char time_c[5];
    int remainTime, potion;
    
    potion = myInfo->GetPotion();
    if (potion < 5) // 5개 이상 되면 할 필요없음.
    {
        // 나의 포션 정보 갱신
        remainTime = myInfo->GetRemainPotionTimeNumber();
        if (remainTime == 0)
        {
            remainTime = 720;
            potion++;
            
            char name[25];
            sprintf(name, "icon/icon_potion.png%d", potion-1);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        }
        myInfo->SetPotion(potion, remainTime-1);
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    }
    
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        // remain request topaz time
        remainTime = friendList[i]->GetRemainRequestTopazTime();
        if (remainTime > 0)
            friendList[i]->SetRemainRequestTopazTime(remainTime-1);
        
        // remain request potion time
        remainTime = friendList[i]->GetRemainRequestPotionTime();
        if (remainTime > 0)
            friendList[i]->SetRemainRequestPotionTime(remainTime-1);
        
        // remain potion time
        remainTime = friendList[i]->GetRemainPotionTime();
        if (remainTime > 0)
            friendList[i]->SetRemainPotionTime(remainTime-1);
        // 시간을 나타내야 하는 포션은 타이머를 보여준다.
        if (remainTime-1 > 0) // && friendList[i]->GetPotionMsgStatus() == 1)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(255);
            friendList[i]->GetPotionLabelSec()->setOpacity(255);
            
            if ((remainTime-1)/60 < 10)
                sprintf(time_c, "0%d:", (remainTime-1)/60);
            else
                sprintf(time_c, "%d:", (remainTime-1)/60);
            friendList[i]->GetPotionLabelMin()->setString(time_c);
            
            if ((remainTime-1)%60 < 10)
                sprintf(time_c, "0%d", (remainTime-1)%60);
            else
                sprintf(time_c, "%d", (remainTime-1)%60);
            friendList[i]->GetPotionLabelSec()->setString(time_c);
        }
        
        // 이제 막 00:00가 되었다면 potion sprite를 바꾼다.
        if (remainTime-1 == 0)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(0);
            friendList[i]->GetPotionLabelSec()->setOpacity(0);
            friendList[i]->SetPotionSprite();
        }
    }
}

void Ranking::ShowHintOfMP()
{
    if (isHintOfMPShown)
        return;
    isHintOfMPShown = true;
    
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    balloon = NULL;
    ball = NULL;
    
    balloon = CCScale9Sprite::create("images/tutorial/tutorial_balloon3.png");
    balloon->setContentSize(CCSize(600, 200));
    balloon->setAnchorPoint(ccp(1, 1));
    balloon->setPosition(ccp(765+200, 1666+35));
    this->addChild(balloon, 100);
    ball = CCLabelTTF::create("MP로 새로운 마법을 배울 수 있고,\n보너스 점수를 증가시켜줘요.", fontList[0].c_str(), 36);
    ball->setPosition(ccp(600/2, 200/2-30));
    ball->setColor(ccc3(255,255,255));
    balloon->addChild(ball, 101);
    
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
    balloon->runAction( CCRepeatForever::create(action) );
}



bool Ranking::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isHintOfMPShown) // MP힌트 표시되고 있으면 없애기.
    {
        if (balloon != NULL && ball != NULL)
        {
            ball->removeFromParentAndCleanup(true);
            balloon->removeFromParentAndCleanup(true);
        }
        balloon = NULL;
        ball = NULL;
        isHintOfMPShown = false;
    }
    
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    // scrollview touch check
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gameready.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_GAMEREADY;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png1")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_topinfo.png2")
        {
            if (spriteClass->spriteObj[i]->sprite9->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false, 0);
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
        else if (spriteClass->spriteObj[i]->name == "background/bg_potion_time.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyPotion", false, 0);
                return true;
            }
        }
        
        else if (spriteClass->spriteObj[i]->name == "button/btn_msg.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Message", false);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_setting.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Setting", false);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_sketchbook.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                if (myInfo->HasNoProperty()) // 속성이 없으면 속성선택창을 띄운다.
                    Common::ShowNextScene(this, "Ranking", "SelectProperty", false, 0);
                else if (!CCUserDefault::sharedUserDefault()->getBoolForKey("is_tutorial_done", false) &&
                         !myInfo->HasNoProperty() && myInfo->GetSkillList().size() <= 2) // 튜토리얼 시작한다.
                    Common::ShowNextScene(this, "Ranking", "T_Sketchbook", false, 0);
                else
                    Common::ShowNextScene(this, "Ranking", "Sketchbook", false, 0);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_addfriend.png")
        {
            if (isGuestLogin) // 게스트로그인이면 터치 못하게 함.
                continue;
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                if (myInfo->GetHashedTalkUserId() == "") // 카카오톡 탈퇴한 경우 친구초대 못함.
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "Ranking", "NoImage", false, KAKAOTALK_UNKNOWN, BTN_1, nullData);
                }
                else
                    Common::ShowNextScene(this, "Ranking", "InviteFriend", false);
                return true;
            }
        }
    }
    
    return true;
}


void Ranking::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void Ranking::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    CCPoint p;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        p = friendList[i]->GetProfile()->convertToNodeSpace(point);
        // convertToNodeSpace 설명
        // ex) sprite1->convertToNodeSpace(sprite2->getPosition());
        // sprite1 좌측하단부터 sprite2의 anchorPoint까지의 거리
        CCSize size = friendList[i]->GetProfile()->getContentSize();
        if (isScrollViewTouched && !isScrolling &&
            (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
        {
            sound->playClick();
            Common::ShowNextScene(this, "Ranking", "Profile", false, i);
        }
        
        if (friendList[i]->GetPotionSprite() != NULL)
        {
            // 포션 보낼 수 있는 상태 & 남은시간 = 0 일 때만 가능하다.
            p = friendList[i]->GetPotionSprite()->convertToNodeSpace(point);
            CCSize size = friendList[i]->GetPotionSprite()->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height &&
                friendList[i]->GetRemainPotionTime() == 0)
            {
                sound->playClick();
                std::vector<int> data;
                data.push_back(i);
                Common::ShowPopup(this, "Ranking", "NoImage", false, POTION_SEND_TRY, BTN_2, data);
                return;
            }
        }
    }
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gameready.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        switch (kind)
        {
            case BTN_MENU_GAMEREADY:
                if (myInfo->HasNoProperty()) // 속성이 없으면 속성선택창을 띄운다.
                    Common::ShowNextScene(this, "Ranking", "SelectProperty", false, 0);
                else
                    Common::ShowNextScene(this, "Ranking", "GameReady", false, -1);
                break;
        }
    }
    
    isOnceScrollViewTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
}


void Ranking::EndScene()
{
    sound->StopBackgroundSound();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->unschedule(schedule_selector(Ranking::PotionTimer));
    this->unschedule(schedule_selector(Ranking::ProfileTimer));
    
    // MP 힌트 있으면 지운다.
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    balloon = NULL;
    ball = NULL;
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassProperty->RemoveAllObjects();
    delete spriteClassProperty;
    spriteClass = NULL;
    spriteClassProperty = NULL;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParent();
    
    if (!isRebooting)
        pBlack->removeFromParentAndCleanup(true);
    pBackground->removeFromParentAndCleanup(true);
    
    Common::RemoveSpriteFramesWithFile("texture_1");
    Common::RemoveSpriteFramesWithFile("texture_2");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/main_background.png");
    
    CCTextureCache::sharedTextureCache()->removeAllTextures();
    
    // PotionTimer scheduler가 딱 정지된 시점에서 현재 시간을 저장해 둔다.
    // 인게임이 끝나고 다시 UI로 돌아올 때, 벌어진 시간을 갱신해야 하기 떄문이다.
    savedTime = savedTime2 = time(0);
    savedMyPotionTime = -1;
    isInGame = true;
    
    if (!isRebooting)
    {
        // 인게임 튜토리얼을 한 적이 없다면, 튜토리얼로 들어간다.
        if (!CCUserDefault::sharedUserDefault()->getBoolForKey("is_inGameTutorial_done", false) ||
            menuInSetting == 0)
        {
            isInGameTutorial = true;
            
            if (menuInSetting == 0)
                Common::ShowNextScene(this, "Ranking", "T_Puzzle", true, 1); // 1 : 세팅의 튜토리얼에서 옴.
            else
                Common::ShowNextScene(this, "Ranking", "T_Puzzle", true, 0); // 0 : 실제 처음 튜토리얼 진행하려고 옴.
        }
        else
            Common::ShowNextScene(this, "Ranking", "LoadingPuzzle", true);
    }
}

CCScrollView* Ranking::GetScrollView()
{
    return scrollView;
}

void Ranking::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
    ////CCLog("min %f", scrollView->minContainerOffset().y);
    ////CCLog("max %f", scrollView->maxContainerOffset().y);
}

void Ranking::scrollViewDidZoom(CCScrollView* view)
{
}
