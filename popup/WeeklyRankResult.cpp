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
    CCLog("WeeklyRankResult :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void WeeklyRankResult::onExit()
{
    CCLog("WeeklyRankResult :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void WeeklyRankResult::keyBackClicked()
{
    EndScene();
}


bool WeeklyRankResult::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("WeeklyRankResult");
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("WeeklyRankResult : touch prio = %d", this->getTouchPriority());
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/rank.plist");
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
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
                spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, ProfileSprite::GetProfile(lastWeeklyRank[i]->GetProfileUrl()), ccp(0.5, 0.5), p1[i], CCSize(0,0), "", "WeeklyRankResult", this, 5, 0, 255, 0.85f) );
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
                CCFiniteTimeAction* action = CCRotateBy::create(1.0f, 180);
                CCActionInterval* rep = CCRepeatForever::create((CCActionInterval*)action);
                ((CCSprite*)spriteClass->FindSpriteByName("effect_beam.png"))->runAction(rep);
            }
        }
    }
    
    // 본인 등수 : 배경 + x위 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png1", ccp(0.5, 1), ccp(winSize.width/2, 750), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );

    sprintf(name, "%d위", myRank);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 56, ccp(0.5, 0.5), ccp(300+3, 698.5f-3), ccc3(0,0,0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 56, ccp(0.5, 0.5), ccp(300, 698.5f), ccc3(255,255,255), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myLastWeekHighScore), fontList[0], 56, ccp(0.5, 0.5), ccp(600+3, 698.5f-3), ccc3(0,0,0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myLastWeekHighScore), fontList[0], 56, ccp(0.5, 0.5), ccp(600, 698.5f), ccc3(255,255,255), "", "WeeklyRankResult", this, 5) );
    
    if (rewardType > 0)
    {
        // 보상 (opt.) : 배경 + 보상그림 + 보상 숫자
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0.5, 1), ccp(winSize.width/2, 635), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0.5, 0.5), ccp(420, 635-52.5f), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
        int numOfStarCandy;
        if (rewardType == 1) numOfStarCandy = 10000;
        else if (rewardType == 2) numOfStarCandy = 7000;
        else if (rewardType == 3) numOfStarCandy = 5000;
        else if (rewardType == 4) numOfStarCandy = 3000;
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(numOfStarCandy), fontList[0], 56, ccp(0.5, 0.5), ccp(600+3, 635-52.5f-3), ccc3(0,0,0), "", "WeeklyRankResult", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(numOfStarCandy), fontList[0], 56, ccp(0.5, 0.5), ccp(600, 635-52.5f), ccc3(255,255,255), "", "WeeklyRankResult", this, 5) );
    }
    
    // 1~3등 중에 내가 있다면, 축하 이펙트 rotation 하기 + 색종이 떨어뜨리기

    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        CCLog("%s", spriteClass->spriteObj[i]->name.c_str());
        spriteClass->AddChild(i);
    }
}


bool WeeklyRankResult::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_red.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                break;
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
    isTouched = false;
}

void WeeklyRankResult::EndScene()
{
    sound->playClick();
    
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
    
    Common::ShowNextScene(this->getParent(), "Ranking", "GetDegree", false);
    
    this->removeFromParentAndCleanup(true);
}


