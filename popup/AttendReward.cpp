#include "AttendReward.h"


CCScene* AttendReward::scene()
{
    CCScene* pScene = CCScene::create();
    AttendReward* pLayer = AttendReward::create();
    pScene->addChild(pLayer);
	return pScene;
}

void AttendReward::onEnter()
{
    //CCLog("AttendReward :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(AttendReward::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void AttendReward::SceneCallback()
{
    isTouched = false;
}
void AttendReward::onExit()
{
    //CCLog("AttendReward :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void AttendReward::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool AttendReward::init()
{
	if (!CCLayer::init())
		return false;
    
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("AttendReward", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(AttendReward::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(tLayer, 1);
    tLayer->setScale(0);
    
    InitSprites();
    
    return true;
}

void AttendReward::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}


void AttendReward::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    int ofs = 100;
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640-ofs), CCSize(982, 623+ofs*2+30), "", "Layer", tLayer, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678-ofs), CCSize(929, 562+ofs*2+30), "", "Layer", tLayer, 0) );
    
    
    // 기본 팝업창 틀
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(982, 623), "", "Layer", tLayer, 0) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(929, 562), "", "Layer", tLayer, 0) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, winSize.height/2+970/2-225), CCSize(0, 0), "", "Layer", tLayer, 1) );
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, winSize.height/2+970/2-131), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    /*
    int ofs = 50;
    
    std::string week[7] = {"일요일", "토요일", "금요일", "목요일", "수요일", "화요일", "월요일"};
    std::string desc[7] = {"테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!", "테스트입니다. 이히히히히~힛! 으아아아!"};
    std::string reward[7] = {"icon/icon_item_clear.png", "icon/icon_potion.png", "icon/icon_topaz.png", "icon/icon_starcandy.png", "icon/icon_item_paint.png", "icon/icon_item_staff.png", "icon/icon_item_time.png"};
    std::string cnt[7] = {"x 3", "x 10", "x 5", "x 7,000", "x 5", "x 5", "x 3"};
    
    
    for (int i = 0 ; i < 7 ; i++)
    {
        CCLayer* pl = CCLayer::create();
        pl->setAnchorPoint(ccp(0, 0));
        pl->setPosition(ccp(X, Y + 156*i + 7*i));
        tLayer->addChild(pl, 10001);
        
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(0, 0), CCSize(100+156+400 + 100, 156), "", "Layer", pl, 10001) );
        
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(week[i], fontList[0], 48, ccp(0.5, 0.5), ccp(ofs+50, 78), ccc3(78,47,8), "", "Layer", pl, 10002) );
        
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow.png", ccp(0.5, 0.5), ccp(ofs+100+156/2, 78), CCSize(0,0), "", "Layer", pl, 10002) );
        
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, reward[i], ccp(0.5, 0.5), ccp(ofs+100+156/2, 78), CCSize(0,0), "", "Layer", pl, 10002) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cnt[i], fontList[0], 36, ccp(0.5, 0), ccp(ofs+100+156/2, 10), ccc3(255,255,255), "", "Layer", pl, 10003) );
        
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(desc[i], fontList[0], 36, ccp(0.5, 0.5), ccp(ofs+100+156+400/2, 78), ccc3(255,255,255), CCSize(370, 146), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", pl, 10003) );
    }
     
     std::vector<CCPoint> p;
     p.push_back(ccp(winSize.width/2-280, winSize.height/2+40));
     p.push_back(ccp(winSize.width/2-80, winSize.height/2+40));
     p.push_back(ccp(winSize.width/2+120, winSize.height/2+40));
     p.push_back(ccp(winSize.width/2-380, winSize.height/2-190));
     p.push_back(ccp(winSize.width/2-180, winSize.height/2-190));
     p.push_back(ccp(winSize.width/2+20, winSize.height/2-190));
     p.push_back(ccp(winSize.width/2+220, winSize.height/2-190));
    */
    
    
    int w = 180;
    int h = 280;

    std::vector<CCPoint> p;
    p.push_back(ccp(76+37+(w+45)*1, 678+50+h+35));
    p.push_back(ccp(76+37+(w+45)*2, 678+50+h+35));
    p.push_back(ccp(76+37+(w+45)*3, 678+50+h+35));
    p.push_back(ccp(76+37+(w+45)*0, 678+50));
    p.push_back(ccp(76+37+(w+45)*1, 678+50));
    p.push_back(ccp(76+37+(w+45)*2, 678+50));
    p.push_back(ccp(76+37+(w+45)*3, 678+50));
    
    std::string weekday[7] = {"월요일", "화요일", "수요일", "목요일", "금요일", "토요일", "일요일"};
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown_mini.png", ccp(0, 0), ccp(76+37, 678+50+h+35), CCSize(w, h), "", "Layer", tLayer, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_attendreward.png", ccp(0.5, 0.5), ccp(76+37+w/2, 678+50+h+45+h/2-8), CCSize(0,0), "", "Layer", tLayer, 10001) );
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_attendreward.png"))->setScale(1.1f);
    
    /*
    sp = CCScale9Sprite::createWithSpriteFrameName("background/bg_board_brown_mini.png");
    sp->setPosition(ccp(76+37, 678+50+h+45));
    sp->setContentSize(CCSize(w, h));
    
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(120-3, height-3 +ofs), CCSize(imgSize.width+6, imgSize.height+6), "", "Layer", tLayer, 3) );
    
    // mask sprite
    mask = CCSprite::createWithSpriteFrameName("letter/letter_attendreward.png");
        //CCScale9Sprite::createWithSpriteFrameName("letter/letter_attendreward.png");
    mask->setAnchorPoint(ccp(0, 0));
    mask->setPosition(ccp(76+37, 678+50+h+45));
    
    //mask->setContentSize(CCSize(imgSize.width, imgSize.height));
    
    clip = CCClippingNode::create();
    clip->setInverted(false);
    clip->setAlphaThreshold(0);
    clip->addChild(mask);
    
    node = CCNode::create();
    node->addChild(sp);
    clip->setStencil(node);
    tLayer->addChild(clip, 10005);
    */
    
    
    
    int lastRewardIdx = 0;
    for (int i = 0 ; i < (int)loginEvent.size(); i++)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown_mini.png", ccp(0, 0), p[i], CCSize(w, h), "", "Layer", tLayer, 10001) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_dontknow_1.png", ccp(0.5, 0), ccp(p[i].x+w/2, p[i].y+20), CCSize(w-20, 40), "", "Layer", tLayer, 10002) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow.png", ccp(0.5, 0.5), ccp(p[i].x+w/2, p[i].y+145), CCSize(0,0), "", "Layer", tLayer, 10002) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png", ccp(0.5, 0), ccp(p[i].x+w/2, p[i].y+220), CCSize(w-20, 50), "", "Layer", tLayer, 10002) );
        
        // 요일
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(weekday[i], fontList[0], 34, ccp(0.5, 0), ccp(p[i].x+w/2+2, p[i].y+230-2), ccc3(0,0,0), "", "Layer", tLayer, 10003) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(weekday[i], fontList[0], 34, ccp(0.5, 0), ccp(p[i].x+w/2, p[i].y+230), ccc3(255,255,255), "", "Layer", tLayer, 10003) );
        
        // 보상 타입에 따른 이미지
        char rewardName[40];
        switch (loginEvent[i]->GetType())
        {
            case 1: sprintf(rewardName, "icon/icon_starcandy.png"); break;
            case 2: sprintf(rewardName, "icon/icon_topaz.png"); break;
            case 3: sprintf(rewardName, "icon/icon_potion.png"); break;
            case 4: sprintf(rewardName, "icon/icon_item_clear.png"); break;
            case 5: sprintf(rewardName, "icon/icon_item_time.png"); break;
            case 6: sprintf(rewardName, "icon/icon_item_paint.png"); break;
            case 7: sprintf(rewardName, "icon/icon_item_staff.png"); break;
        }
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, rewardName, ccp(0.5, 0.5), ccp(p[i].x+w/2, p[i].y+145), CCSize(0,0), "", "Layer", tLayer, 10003) );
        if (loginEvent[i]->GetType() == 3) // 포션은 이미지 크기를 좀 키우자.
        {
            ((CCSprite*)spriteClass->FindSpriteByName(rewardName))->setScale(1.2f);
            ((CCSprite*)spriteClass->FindSpriteByName(rewardName))->setPosition(ccp(p[i].x+w/2+3, p[i].y+145+3));
        }
        
        // 보상 개수
        char rewardValue[15];
        sprintf(rewardValue, "x %s", Common::MakeComma(loginEvent[i]->GetValue()).c_str());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(rewardValue, fontList[0], 36, ccp(0.5, 0.5), ccp(p[i].x+w/2, p[i].y+41), ccc3(255,255,255), "", "Layer", tLayer, 10003) );
        
        // 보상 받은 여부
        if (loginEvent[i]->IsAchieved())
        {
            lastRewardIdx = i;
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_attendreward_done.png", ccp(0.5, 0), ccp(p[i].x+w/2, p[i].y+60), CCSize(0,0), "", "Layer", tLayer, 10004, 0, 255, 100001*(i+1)) ); // tag = 100001*(i+1)
            ((CCSprite*)spriteClass->FindSpriteByTag(100001*(i+1)))->setScale(0.9f);
        }
    }
    
    // 오늘 지급된 보상 설명
    char text[70];
    switch(loginEvent[lastRewardIdx]->GetType())
    {
        case 1: sprintf(text, "별사탕 %s개가 지급되었어요!", Common::MakeComma(loginEvent[lastRewardIdx]->GetValue()).c_str()); break;
        case 2: sprintf(text, "토파즈 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
        case 3: sprintf(text, "포션 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
        case 4: sprintf(text, "4색피스 아이템 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
        case 5: sprintf(text, "+5초시계 아이템 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
        case 6: sprintf(text, "색깔붓 아이템 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
        case 7: sprintf(text, "마법지팡이 아이템 %d개가 지급되었어요!", loginEvent[lastRewardIdx]->GetValue()); break;
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(text, fontList[0], 34, ccp(0, 0), ccp(76+37+10, 678-ofs+30+30), ccc3(78,47,8), "", "Layer", tLayer, 10003) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(722+25, 678-ofs+30-3), CCSize(0, 0), "", "Layer", tLayer, 10005) );
    CCPoint pp = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(pp.x, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 10006, 1) );
    
    // 버튼 젤리 움직임
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
    temp->runAction(CCRepeatForever::create(action));

    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}




bool AttendReward::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
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
    }
    
    return true;
}


void AttendReward::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void AttendReward::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            EndScene();
        }
    }
    
    isTouched = false;
}

void AttendReward::EndScene()
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



