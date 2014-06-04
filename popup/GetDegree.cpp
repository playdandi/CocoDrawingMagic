#include "GetDegree.h"

CCScene* GetDegree::scene()
{
    CCScene* pScene = CCScene::create();
    GetDegree* pLayer = GetDegree::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void GetDegree::onEnter()
{
    CCLog("GetDegree :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void GetDegree::onExit()
{
    CCLog("GetDegree :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void GetDegree::keyBackClicked()
{
    EndScene();
}


bool GetDegree::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("GetDegree", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("GetDegree : touch prio = %d", this->getTouchPriority());
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
}

void GetDegree::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}

void GetDegree::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // 학위 배경, 메달
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_degree.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(0, 0), "", "GetDegree", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon_medal.png", ccp(0.5, 0.5), ccp(winSize.width/2, 1420), CCSize(0, 0), "", "GetDegree", this, 0) );
    
    // The University of Magic
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("The University of Magic", fontList[3], 135, ccp(0.5,0.5), ccp(winSize.width/2, 1220), ccc3(78,47,8), "", "GetDegree", this, 5) );
    
    // 닉네임
    std::string nickname;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            nickname = friendList[i]->GetNickname();
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(nickname, fontList[0], 56, ccp(0.5,0.5), ccp(winSize.width/2, 1050), ccc3(78,47,8), "", "GetDegree", this, 5) );
    
    // 학위 명칭
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("이 시대 최고의 지성, 마법박사!", fontList[0], 64, ccp(0.5,0.5), ccp(winSize.width/2, 900), ccc3(78,47,8), "", "GetDegree", this, 5) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0.5, 0.5), ccp(winSize.width/2, 275), CCSize(0, 0), "", "GetDegree", this, 5) );
    CCPoint p = spriteClass->FindParentCenterPos("button/btn_red.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm.png", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_red.png", "0", NULL, 5, 1) );
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool GetDegree::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
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


void GetDegree::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void GetDegree::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void GetDegree::EndScene()
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
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/rank.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/rank.png");
    
    this->removeFromParentAndCleanup(true);
}


