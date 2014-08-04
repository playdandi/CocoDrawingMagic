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
    //CCLog("GetDegree :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void GetDegree::onExit()
{
    //CCLog("GetDegree :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void GetDegree::keyBackClicked()
{
}


bool GetDegree::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    isLoadingDone = false;
    
    // make depth tree
    Depth::AddCurDepth("GetDegree", this);
    
    this->setTouchEnabled(true);
    //this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("GetDegree : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(GetDegree::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    sound->PauseBackgroundSound();
    
    InitSprites();
    
    return true;
}

void GetDegree::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("GetDegree : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("GetDegree : 터치 비활성");
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

void GetDegree::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    this->addChild(pBlack, 0);
    
    // background
    pBackground = CCSprite::create("images/main_background.png");
    pBackground->setScale(1.5f);
    pBackground->setAnchorPoint(ccp(0.5, 1));
    //pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    pBackground->setPosition(ccp(winSize.width/2, winSize.height));
    pBackground->setOpacity(0);
    this->addChild(pBackground, 0);
    
    int h = pBackground->getContentSize().height;
    CCSize vs = CCDirector::sharedDirector()->getVisibleSize();
    int dy = std::max((int)(h-vs.height)/2, 100);
    ////CCLog("%d %d", h, (int)vs.height);
    dy = h-vs.height;
    
    CCActionInterval* action = CCSequence::create( CCSpawn::create( CCFadeIn::create(2.0f), CCMoveBy::create(2.0f, ccp(0, dy)), CCSequence::create(CCDelayTime::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(GetDegree::SoundCallback), this), NULL) , NULL ), CCCallFuncND::create(this, callfuncND_selector(GetDegree::SpriteCallback), this), NULL );
    pBackground->runAction(action);
}

void GetDegree::SoundCallback(CCNode* sender, void *p)
{
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true))
        sound->ResumeBackgroundSound();
}
void GetDegree::SpriteCallback(CCNode* sender, void* p)
{
    ((GetDegree*)p)->InitDegree();
}

void GetDegree::InitDegree()
{
    spriteClass = new SpriteClass();
    
    // 학위 배경, 메달
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "bg_degree.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(0, 0), "", "GetDegree", this, 0, 0, 0, 0) ); // tag = 0
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon_medal.png", ccp(0.5, 0.5), ccp(winSize.width/2, 1420), CCSize(0, 0), "", "GetDegree", this, 0, 0, 0, 1) ); // tag = 1
    
    // The University of Magic
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("The University of Magic", fontList[3], 135, ccp(0.5,0.5), ccp(winSize.width/2, 1220), ccc3(78,47,8), "", "GetDegree", this, 5) );
    
    // 닉네임
    std::string nickname;
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            nickname = friendList[i]->GetNickname();
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(nickname, fontList[0], 56, ccp(0.5,0.5), ccp(winSize.width/2, 1050), ccc3(78,47,8), "", "GetDegree", this, 5, 0, 0, 2) ); // tag = 2
    
    // 학위 명칭
    std::string message = "";
    if (certificateType == 1) message = "Bachelor of Magic";
    else if (certificateType == 2) message = "Master of Magic";
    else if (certificateType == 3) message = "Doctor of Magic";
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(message, fontList[0], 64, ccp(0.5,0.5), ccp(winSize.width/2, 900), ccc3(78,47,8), "", "GetDegree", this, 5, 0, 0, 3) ); // tag = 3
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0.5, 0.5), ccp(winSize.width/2, 275), CCSize(0, 0), "", "GetDegree", this, 5, 0, 0, 10) );
    CCPoint p = spriteClass->FindParentCenterPos("button/btn_red.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm.png", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_red.png", "0", NULL, 5, 1, 0, 11) );
    
    // 확인 버튼 젤리 움직임
    CCActionInterval* action2
    = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"))->runAction(CCRepeatForever::create(action2));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action2->copy()));
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // action (fade-in)
    callbackCnt = 0;
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(2.0f), CCCallFuncND::create(this, callfuncND_selector(GetDegree::DegreeCallback), this), NULL );
    ((CCSprite*)spriteClass->FindSpriteByTag(0))->runAction(action);
    ((CCSprite*)spriteClass->FindSpriteByTag(1))->runAction((CCActionInterval*)action->copy());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->runAction((CCActionInterval*)action->copy());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->runAction((CCActionInterval*)action->copy());
}

void GetDegree::DegreeCallback(CCNode* sender, void* p)
{
    GetDegree* pThis = (GetDegree*)p;
    pThis->callbackCnt++;

    if (pThis->callbackCnt >= 4)
    {
        // 확인 버튼 나타나기
        ((CCSprite*)spriteClass->FindSpriteByTag(10))->setOpacity(255);
        ((CCSprite*)spriteClass->FindSpriteByTag(11))->setOpacity(255);
        
        // touch enabled
        pThis->isTouched = false;
        pThis->isLoadingDone = true;
    }
}


bool GetDegree::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched || !isLoadingDone)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
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


void GetDegree::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void GetDegree::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isLoadingDone)
        return;
    
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

void GetDegree::EndScene()
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
    pBackground->removeFromParentAndCleanup(true);
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/rank.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/rank.png");
    
    this->removeFromParentAndCleanup(true);
}


