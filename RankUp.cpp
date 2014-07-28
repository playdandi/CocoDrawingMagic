#include "RankUp.h"
#include "Kakao/Plugins/KakaoNativeExtension.h"


CCScene* RankUp::scene()
{
	CCScene* pScene = CCScene::create();
	RankUp* pLayer = RankUp::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void RankUp::onEnter()
{
    CCLog("RankUp :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
    
    // Fade Out (서서히 밝아진다) + 3/2/1/시작 액션으로 이동
    CCActionInterval* action = CCSequence::create(CCFadeOut::create(0.5f), CCCallFuncND::create(this, callfuncND_selector(RankUp::Callback), this), NULL);
    pBlackOpen->runAction(action);
}
void RankUp::Callback(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
    CCActionInterval* action = CCEaseElasticOut::create( CCMoveTo::create(2.0f, ccp(-700, 0)), 2.0f );
    ((RankUp*)p)->spriteClass->layers[1]->runAction(action);
    
    isTouched = false;
    isBoasted = false;
}
void RankUp::onExit()
{
    CCLog("RankUp :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void RankUp::keyBackClicked()
{
}

bool RankUp::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    isBoasted = true;
    
    // make depth tree
    Depth::AddCurDepth("RankUp", this);

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    CCLog("RankUp : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RankUp::Notification), Depth::GetCurName(), NULL);
    
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/rankup.plist");
    
    spriteClass = new SpriteClass();
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
	return true;
}

void RankUp::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 5)
    {
        // 자랑하기 블록
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png1"))->setColor(ccc3(150,150,150));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_boast.png"))->setColor(ccc3(150,150,150));
        isBoasted = true;
    }
    
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        if (idx > -1)
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
        }
    }
}


void RankUp::InitSprites()
{
    // background
    pBackground = CCSprite::create("images/main_background.png");
    pBackground->setAnchorPoint(ccp(0, 0));
    pBackground->setPosition(ccp(0, 0));
    this->addChild(pBackground, 0);
    
    CCLog("=========================================================");
    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    CCLog("=========================================================");
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap_yellow.png", ccp(0, 0), ccp(14, 1506), CCSize(0, 0), "", "RankUp", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap_title_rankup.png", ccp(0.5, 0), ccp(winSize.width/2, 1552), CCSize(0, 0), "", "RankUp", this, 1) );

    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "RankUp", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow_mini.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "RankUp", this, 1) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "RankUp", this, 1) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "RankUp", this, 1) );
    
    // 자랑하기 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png1", ccp(0, 0), ccp(126, 711), CCSize(0, 0), "", "RankUp", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_boast.png", ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2-1, 24), CCSize(0, 0), "button/btn_red_mini.png1", "0", NULL, 5, 1) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png2", ccp(0, 0), ccp(717+5, 711), CCSize(0, 0), "", "RankUp", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png2", "0", NULL, 5, 1) );
    
    // 내 순위와 점수 찾기
    int myRank;
    int myScore;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
        {
            myScore = friendList[i]->GetWeeklyHighScore();
            myRank = i+1;
            break;
        }
    }
    // 점수
    char number[20];
    sprintf(number, "%d", myScore);
    CCLayer* pScoreLayer = Common::MakeImageNumberLayer(number, 2);
    float offset = pScoreLayer->getContentSize().width/2;
    pScoreLayer->setAnchorPoint(ccp(0, 0));
    pScoreLayer->setPosition(ccp(winSize.width/2-offset, 840));
    spriteClass->layers.push_back(pScoreLayer);
    this->addChild(pScoreLayer, 6);
    
    // 순위
    sprintf(number, "%d위", myRank);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(number, fontList[0], 48, ccp(0.5, 0.5), ccp(winSize.width/2, 970), ccc3(78,47,8), "", "RankUp", this, 5) );
    
    // 프로필
    CCLayer* profileLayer = CCLayer::create();
    spriteClass->layers.push_back(profileLayer);
    this->addChild(profileLayer, 6);
    
    
    int height = 1100;
    char name[40];
    ProfileSprite* psp = ProfileSprite::GetObj(friendList[myRank]->GetImageUrl());

    if (friendList[myRank]->GetImageUrl() != "" && psp->IsLoadingDone())
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5, 0.5), ccp(winSize.width/2, height), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 1.45f*0.95f) );
        sprintf(name, "background/bg_profile.png1");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), ccp(winSize.width/2, height), CCSize(0,0), "", "Layer", profileLayer, 6, 1) );
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setScale(1.45f);
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5, 0.5), ccp(winSize.width/2, height), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 1.2f) );
    }
    
    int dist = 700;
    psp = ProfileSprite::GetObj(friendList[myRank-1]->GetImageUrl());

    if (friendList[myRank-1]->GetImageUrl() != "" && psp->IsLoadingDone())
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5, 0.5), ccp(winSize.width/2+dist, height), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 1.45f*0.95f) );
        sprintf(name, "background/bg_profile.png2");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), ccp(winSize.width/2+dist, height), CCSize(0,0), "", "Layer", profileLayer, 6, 1) );
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setScale(1.45f);
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5, 0.5), ccp(winSize.width/2+dist, height), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 1.3f) );
    }
    
    pBlackOpen = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlackOpen->setAnchorPoint(ccp(0, 0));
    pBlackOpen->setPosition(ccp(0, 0));
    pBlackOpen->setColor(ccc3(0,0,0));
    this->addChild(pBlackOpen, 4000);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    /*
    // 시상대
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "podium.png", ccp(0.5, 0), ccp(winSize.width/2, 450), CCSize(0, 0), "", "RankUp", this, 1) );
    
    // 자랑하기 왼쪽
    CCPoint p;
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "btn_red_mini.png1", ccp(0.5, 0), ccp(240, 220), CCSize(0, 0), "", "RankUp", this, 1) );
    p = spriteClass->FindParentCenterPos("btn_red_mini.png1");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter_boast.png1", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "btn_red_mini.png1", "0", NULL, 1) );

    // 자랑하기 오른쪽
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "btn_red_mini.png2", ccp(0.5, 0), ccp(490, 220), CCSize(0, 0), "", "RankUp", this, 1) );
    p = spriteClass->FindParentCenterPos("btn_red_mini.png2");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter_boast.png2", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "btn_red_mini.png2", "0", NULL, 1) );
     
    // 닫기
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "btn_system.png", ccp(0.5, 0), ccp(785, 220), CCSize(0, 0), "", "RankUp", this, 1) );
    p = spriteClass->FindParentCenterPos("btn_system.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter_close.png", ccp(0.5, 0.5), ccp(p.x, p.y+5), CCSize(0, 0), "btn_system.png", "0", NULL, 1) );
    
    
    // 등수(숫자) 3개 label
    int myRank;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            myRank = i;
    }
    char name[20];
    sprintf(name, "%d", myRank+1);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 72, ccp(0.5,0.5), ccp(797, 725), ccc3(0,0,0), "", "RankUp", this, 5) );
    sprintf(name, "%d", myRank+2);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 64, ccp(0.5,0.5), ccp(495, 602), ccc3(0,0,0), "", "RankUp", this, 5) );
    sprintf(name, "%d", myRank+3);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0.5,0.5), ccp(243, 530), ccc3(0,0,0), "", "RankUp", this, 5) );
    
    // 3명 프로필 사진
    std::vector<CCPoint> pos;
    pos.push_back(ccp(797, 970));
    pos.push_back(ccp(797, 870));
    pos.push_back(ccp(495, 747));
    pos.push_back(ccp(243, 650));
    for (int i = myRank ; i <= myRank+2 && i < friendList.size(); i++)
    {
        // 프로필 이미지
        CCSprite* profile = ProfileSprite::GetProfile(friendList[i]->GetImageUrl());
        if (friendList[i]->GetImageUrl() != "")
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5, 0.5), pos[i-myRank], CCSize(0,0), "", "RankUp", this, 5, 0, 255, 0.85f, i) );
            sprintf(name, "bg_profile.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0.5, 0.5), pos[i-myRank], CCSize(0, 0), "", "RankUp", this, 6, 1, 255, i+10000) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5, 0.5), pos[i-myRank], CCSize(0,0), "", "RankUp", this, 5, 0, 255, 1.0f, i) );
        }
        
        // 이동 액션
        CCActionInterval* action;
        if (i == myRank)
            action = CCSequence::create(CCMoveTo::create(1.5f, pos[i-myRank+1]), NULL);
        else
            action = CCSequence::create(CCMoveTo::create(1.0f, ccp(pos[i-myRank+1].x, pos[i-myRank].y)), CCDelayTime::create(0.2f), CCMoveTo::create(0.3f, pos[i-myRank+1]), NULL);
        
        ((CCSprite*)spriteClass->FindSpriteByTag(i))->runAction(action);
        if (friendList[i]->GetImageUrl() != "")
            ((CCSprite*)spriteClass->FindSpriteByTag(i+10000))->runAction((CCActionInterval*)action->copy()->autorelease());
    }
    */
}


bool RankUp::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png2") // 확인
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
            }
        }
        else if (!isBoasted && spriteClass->spriteObj[i]->name == "button/btn_red_mini.png1") // 자랑하기
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_boast.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_BOAST;
                idx = i;
                return true;
            }
        }
    }
    
    return true;
}


void RankUp::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void RankUp::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
        if (!isBoasted)
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_boast.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            EndScene();
        }
        else if (kind == BTN_MENU_BOAST)
        {
            int idx;
            for (int i = 0 ; i < friendList.size() ; i++)
            {
                if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
                {
                    idx = i+1; // 추월한 사람의 idx
                    break;
                }
            }
            
            if (friendList[idx]->GetPotionMsgStatus() > 0 && !friendList[idx]->IsMessageBlocked())
            {
                std::vector<int> data;
                data.push_back(idx);
                Common::ShowPopup(this, "RankUp", "NoImage", false, RANKUP_BOAST, BTN_1, data);
            }
            else
            {
                // 카카오톡 안에서 수신거부 or 환경설정 내 거부 <- 둘 중 하나라도 하면 수신거부를 시켜야 한다.
                std::vector<int> nullData;
                Common::ShowPopup(this, "RankUp", "NoImage", false, RANKUP_BOAST_REJECTED, BTN_1, nullData);
            }
        }
    }
            
    isTouched = false;
}

void RankUp::onSendLinkMessageComplete()
{
    CCLog("onSendLinkMessageComplete");
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
}
void RankUp::onSendLinkMessageErrorComplete(char const *status, char const *error)
{
    CCLog("onSendLinkMessageErrorComplete : %s, %s", status, error);
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
}

void RankUp::EndScene()
{
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlackClose->setPosition(ccp(0, 0));
    pBlackClose->setAnchorPoint(ccp(0, 0));
    pBlackClose->setColor(ccc3(0, 0, 0));
    pBlackClose->setOpacity(0);
    this->addChild(pBlackClose, 5005);
    
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.0f), CCCallFuncND::create(this, callfuncND_selector(RankUp::EndSceneCallback), this), NULL);
    pBlackClose->runAction(action);
}

void RankUp::EndSceneFromReboot() // 강제 리부팅 시
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    //pBlackClose->removeFromParentAndCleanup(true);
    pBackground->removeFromParentAndCleanup(true);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/rankup.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/rankup.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/main_background.png");
}

void RankUp::EndSceneCallback(CCNode* sender, void* pointer)
{
    RankUp* pThis = (RankUp*)pointer;
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(pThis, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();

    pThis->setKeypadEnabled(false);
    pThis->setTouchEnabled(false);
    
    // remove all CCNodes
    pThis->spriteClass->RemoveAllObjects();
    delete pThis->spriteClass;
    pThis->pBlackClose->removeFromParentAndCleanup(true);
    pThis->pBackground->removeFromParentAndCleanup(true);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/rankup.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/rankup.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/main_background.png");
    
    Common::ShowNextScene(pThis, "RankUp", "Ranking", true, 1);
}

