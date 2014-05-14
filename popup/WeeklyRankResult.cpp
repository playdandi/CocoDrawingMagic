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
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0.5, 0.5), ccp(winSize.width/2, 275), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    CCPoint p = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
 
    // 트로피
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon_weekly_rank_trophy.png", ccp(0.5, 0), ccp(winSize.width/2, 750), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    char name[40], name2[40];
    
    // 1등 (프로필사진)
    sprintf(name, "background/bg_profile_noimage.png1");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5,0.5), ccp(winSize.width/2+5, 1270), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    p = spriteClass->FindParentCenterPos(name);
    sprintf(name2, "background/bg_profile.png1");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), p, CCSize(0, 0), name, "0", NULL, 5, 1) );
    
    // 2등 (프로필사진)
    sprintf(name, "background/bg_profile_noimage.png2");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5,0.5), ccp(243, 1145), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    p = spriteClass->FindParentCenterPos(name);
    sprintf(name2, "background/bg_profile.png2");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), p, CCSize(0, 0), name, "0", NULL, 5, 1) );
    
    // 3등 (프로필사진)
    sprintf(name, "background/bg_profile_noimage.png3");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5,0.5), ccp(832, 1080), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    p = spriteClass->FindParentCenterPos(name);
    sprintf(name2, "background/bg_profile.png3");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5,0.5), p, CCSize(0, 0), name, "0", NULL, 5, 1) );
    
    // 1,2,3등 등수 icon
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_1.png", ccp(0.5,0.5), ccp(winSize.width/2+5, 1030), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_2.png", ccp(0.5,0.5), ccp(243, 945), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_3.png", ccp(0.5,0.5), ccp(832, 870), CCSize(0, 0), "", "WeeklyRankResult", this, 5) );
    
    // 1등 이름 + 점수배경 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("1등한 사람의 이름", fontList[0], 32, ccp(0.5,0.5), ccp(winSize.width/2+5, 1175), ccc3(78,47,8), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png1", ccp(0.5,0.5), ccp(winSize.width/2+5, 1130), CCSize(210, 45), "", "WeeklyRankResult", this, 5, 0, 210) );
    p = spriteClass->FindParentCenterPos("background/bg_dontknow_1.png1");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("52,987,090", fontList[0], 30, ccp(0.5,0.5), p, ccc3(255,255,255), "background/bg_dontknow_1.png1", "1", NULL, 5, 1) );
    
    // 2등 이름 + 점수배경 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("2등한 사람의 이름", fontList[0], 28, ccp(0.5,0.5), ccp(243, 1052), ccc3(78,47,8), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png2", ccp(0.5,0.5), ccp(243, 1015), CCSize(165, 40), "", "WeeklyRankResult", this, 5, 0, 210) );
    p = spriteClass->FindParentCenterPos("background/bg_dontknow_1.png2");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("33,333,333", fontList[0], 27, ccp(0.5,0.5), p, ccc3(255,255,255), "background/bg_dontknow_1.png2", "1", NULL, 5, 1) );
    
    // 3등 이름 + 점수배경 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("3등한 사람의 이름", fontList[0], 28, ccp(0.5,0.5), ccp(832, 990), ccc3(78,47,8), "", "WeeklyRankResult", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png3", ccp(0.5,0.5), ccp(832, 955), CCSize(165, 40), "", "WeeklyRankResult", this, 5, 0, 210) );
    p = spriteClass->FindParentCenterPos("background/bg_dontknow_1.png3");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("1,598,006", fontList[0], 27, ccp(0.5,0.5), p, ccc3(255,255,255), "background/bg_dontknow_1.png3", "1", NULL, 5, 1) );
    
    
    // 본인 등수 : 배경 + x위 + 점수
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png1", ccp(0.5, 1), ccp(winSize.width/2, 750), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );
    
    // 보상 (opt.) : 배경 + 보상그림 + 보상 숫자
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0.5, 1), ccp(winSize.width/2, 750-105-10), CCSize(750, 105), "", "WeeklyRankResult", this, 5) );
    
    // 1~3등 중에 내가 있다면, 축하 이펙트 rotation 하기

    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
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
            spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
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
    
    Common::ShowNextScene(this->getParent(), "Ranking", "GetDegree", false);
    
    this->removeFromParentAndCleanup(true);
}


