#include "RankUp.h"

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

    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    CCLog("RankUp : touch prio = %d", this->getTouchPriority());
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/rankup.plist");

    
    spriteClass = new SpriteClass();
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();

    isTouched = false;
    
	return true;
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
    // 279
    
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
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool RankUp::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "btn_system.png")
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


void RankUp::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void RankUp::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    /*
    CCPoint p;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        p = friendList[i]->GetProfile()->convertToNodeSpace(point);
        CCSize size = friendList[i]->GetProfile()->getContentSize();
        if (isScrollViewTouched && !isScrolling &&
            (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
        {
            sound->playClick();
            Common::ShowNextScene(this, "RankUp", "Profile", false, i);
        }
        
        if (friendList[i]->GetPotionSprite() != NULL)
        {
            // 포션 보낼 수 있는 상태 & 남은시간 = 0 일 때만 가능하다.
            p = friendList[i]->GetPotionSprite()->convertToNodeSpace(point);
            CCSize size = friendList[i]->GetPotionSprite()->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height &&
                friendList[i]->GetPotionMsgStatus() == 1 && friendList[i]->GetRemainPotionTime() == 0)
            {
                sound->playClick();
                std::vector<int> data;
                data.push_back(i);
                Common::ShowPopup(this, "RankUp", "NoImage", false, POTION_SEND_TRY, BTN_2, data);
                break;
            }
        }
    }
    */
    
    isTouched = false;
}


void RankUp::EndScene()
{
    sound->playClick();
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    pBlackClose = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlackClose->setPosition(ccp(0, 0));
    pBlackClose->setAnchorPoint(ccp(0, 0));
    pBlackClose->setColor(ccc3(0, 0, 0));
    pBlackClose->setOpacity(0);
    this->addChild(pBlackClose, 1000);
    
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(1.0f), CCCallFuncND::create(this, callfuncND_selector(RankUp::EndSceneCallback), this), NULL);
    pBlackClose->runAction(action);
}

void RankUp::EndSceneCallback(CCNode* sender, void* pointer)
{
    RankUp* pThis = (RankUp*)pointer;
    
    // remove all CCNodes
    pThis->spriteClass->RemoveAllObjects();
    delete pThis->spriteClass;
    pThis->pBlackClose->removeFromParentAndCleanup(true);
    pThis->pBackground->removeFromParentAndCleanup(true);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/rankup.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/rankup.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/main_background.png");
    
    Common::ShowNextScene(this, "RankUp", "Ranking", true, 1);
}

