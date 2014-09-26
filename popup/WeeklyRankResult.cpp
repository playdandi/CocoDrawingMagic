#include "WeeklyRankResult.h"

CCScene* WeeklyRankResult::scene()
{
    CCScene* pScene = CCScene::create();
    WeeklyRankResult* pLayer = WeeklyRankResult::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void WeeklyRankResult::onEnter()
{
    //CCLog("WeeklyRankResult :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    isTouched = false;
    
    // 학위를 받았다면 학위수여 화면으로 이동.
    if (certificateType > 0)
    {
        std::vector<int> nullData;
        Common::ShowPopup(this, "WeeklyRankResult", "NoImage", false, GET_DEGREE, BTN_1, nullData);
    }
}
void WeeklyRankResult::onExit()
{
    //CCLog("WeeklyRankResult :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void WeeklyRankResult::keyBackClicked()
{
}


bool WeeklyRankResult::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("WeeklyRankResult", this);
    
    this->setTouchEnabled(true);
    //this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("WeeklyRankResult : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(WeeklyRankResult::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    Common::AddSpriteFramesWithFile("rank");
    //CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/rank.plist");
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
}

void WeeklyRankResult::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("WeeklyRankResult : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("WeeklyRankResult : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        if (idx > -1)
        {
            ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_gameready.png"))->setColor(ccc3(255,255,255));
        }
    }
}

void WeeklyRankResult::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "WeeklyRankResult", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "bg_weekly_rank.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "WeeklyRankResult", this, 0) );
    
    // 스트랩
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_rank.png", ccp(0, 0), ccp(118, 1920-441-88), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0.5, 0.5), ccp(winSize.width/2, 275), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    CCPoint p = spriteClass->FindParentCenterPos("button/btn_red.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm.png", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_red.png", "0", NULL, 5, 1) );
    
    // 확인 버튼 젤리 움직임
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"))->runAction(CCRepeatForever::create(action));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action->copy()));
    
 
    // 트로피
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon_weekly_rank_trophy.png", ccp(0.5, 0), ccp(winSize.width/2, 750), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    // 1,2,3등 등수 icon
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_1.png", ccp(0.5,0.5), ccp(winSize.width/2+5, 1030), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_2.png", ccp(0.5,0.5), ccp(243, 945), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_3.png", ccp(0.5,0.5), ccp(832, 870), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    char name[40];
    
    std::vector<CCPoint> p1, p2, p3;
    p1.push_back(ccp(winSize.width/2+5, 1270));
    p1.push_back(ccp(243, 1145));
    p1.push_back(ccp(832, 1080));
    p2.push_back(ccp(winSize.width/2+5, 1175));
    p2.push_back(ccp(243, 1052));
    p2.push_back(ccp(832, 990));
    p3.push_back(ccp(winSize.width/2+5, 1130));
    p3.push_back(ccp(243, 1015));
    p3.push_back(ccp(832, 955));
    std::vector<CCSize> s;
    s.push_back(CCSize(210, 45));
    s.push_back(CCSize(165, 40));
    s.push_back(CCSize(165, 40));
    for (int i = 0 ; i < 3 && i < lastWeeklyRank.size() ; i++)
    {
        if (lastWeeklyRank[i]->GetScore() != -1)
        {
            // 프로필 사진
            if (lastWeeklyRank[i]->GetProfileUrl() != "")
            {
                spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, ProfileSprite::GetProfile(lastWeeklyRank[i]->GetProfileUrl()), ccp(0.5, 0.5), p1[i], CCSize(0,0), "", "WeeklyRankResult", this, 5, 0, 255, 0.95f) );
                sprintf(name, "background/bg_profile.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), p1[i], CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
            }
            else
            {
                spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, ProfileSprite::GetProfile(lastWeeklyRank[i]->GetProfileUrl()), ccp(0.5, 0.5), p1[i], CCSize(0,0), "", "WeeklyRankResult", this, 5) );
            }
            
            // 이름
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(lastWeeklyRank[i]->GetNickname(), fontList[0], 32, ccp(0.5,0.5), p2[i], ccc3(78,47,8), "", "WeeklyRankResult", this, 5) );
            sprintf(name, "background/bg_dontknow_1.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0.5,0.5), p3[i], s[i], "", "WeeklyRankResult", this, 5, 0, 210) );
            p = spriteClass->FindParentCenterPos(name);
            
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(lastWeeklyRank[i]->GetScore()), fontList[0], 30, ccp(0.5,0.5), p, ccc3(255,255,255), name, "1", NULL, 5, 1) );
            
            if (myRank == i+1) // 내 등수가 3등 이내일 경우, 프로필 위치에 이펙트 빔을 쏜다.
            {
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "effect_beam.png", ccp(0.5,0.5), p1[i], CCSize(0,0), "", "WeeklyRankResult", this, 4) );
                CCFiniteTimeAction* action = CCRotateBy::create(1.0f, 60);
                CCActionInterval* rep = CCRepeatForever::create((CCActionInterval*)action);
                ((CCSprite*)spriteClass->FindSpriteByName("effect_beam.png"))->runAction(rep);
            }
        }
    }
    
    // 본인 등수 : 배경 + x위 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png1", ccp(0.5, 1), ccp(winSize.width/2, 750), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );
    
    sprintf(name, "%d", myRank);
    CCLayer* rank = Common::MakeImageNumberLayer(name, 0);
    rank->setAnchorPoint(ccp(0, 0));
    rank->setPosition(ccp(300, 698.5f-25));
    this->addChild(rank, 6);
    spriteClass->layers.push_back(rank);
    int rankSize = rank->getContentSize().width;

    // '위'
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter_wi.png", ccp(0, 0.5), ccp(300+rankSize, 698.5f), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    // 지난주 최고점수
    sprintf(name, "%d", myLastWeekHighScore);
    CCLayer* lastWeekHighScore = Common::MakeImageNumberLayer(name, 0);
    lastWeekHighScore->setAnchorPoint(ccp(0.5, 0.5));
    lastWeekHighScore->setPosition(ccp(500, 698.5f-27));
    this->addChild(lastWeekHighScore, 6);
    spriteClass->layers.push_back(lastWeekHighScore);
    
    if (rewardType > 0)
    {
        // 보상 (opt.) : 배경 + 보상그림 + 보상 숫자
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0.5, 1), ccp(winSize.width/2, 635), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0.5, 0.5), ccp(420, 635-52.5f), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
        int numOfStarCandy = 0;
        if (rewardType == 1) numOfStarCandy = 10000;
        else if (rewardType == 2) numOfStarCandy = 7000;
        else if (rewardType == 3) numOfStarCandy = 5000;
        else if (rewardType == 4) numOfStarCandy = 3000;
        
        sprintf(name, "%d", numOfStarCandy);
        CCLayer* starcandy = Common::MakeImageNumberLayer(name, 0);
        starcandy->setAnchorPoint(ccp(0.5, 0.5));
        starcandy->setPosition(ccp(500, 635-52.5f-27));
        this->addChild(starcandy, 6);
        spriteClass->layers.push_back(starcandy);
        
        //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(numOfStarCandy), fontList[0], 56, ccp(0.5, 0.5), ccp(600+3, 635-52.5f-3), ccc3(0,0,0), "", "WeeklyRankResult", this, 5) );
        //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(numOfStarCandy), fontList[0], 56, ccp(0.5, 0.5), ccp(600, 635-52.5f), ccc3(255,255,255), "", "WeeklyRankResult", this, 5) );
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool WeeklyRankResult::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("weekly : touch began");
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
            }
        }
    }
    
    return true;
}


void WeeklyRankResult::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void WeeklyRankResult::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        switch (kind)
        {
            case BTN_MENU_CONFIRM:
                EndScene();
                break;
        }
    }
    
    isTouched = false;
}

void WeeklyRankResult::EndScene()
{
    //sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
    
    
    // 다른 팝업창 관련 (Ranking)
    param = CCString::create("11");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
}


