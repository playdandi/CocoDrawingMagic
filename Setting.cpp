#include "Setting.h"

CCScene* Setting::scene()
{
    CCScene* pScene = CCScene::create();
    Setting* pLayer = Setting::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Setting::onEnter()
{
    //CCLog("Setting : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(Setting::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void Setting::SceneCallback()
{
    isTouched = false;
}
void Setting::onExit()
{
    //CCLog("Setting : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Setting::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    EndScene();
}


bool Setting::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("Setting", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("Setting : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Setting::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, 0));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);
    
    spriteClass = new SpriteClass();
    InitSprites();
    
    // 팝업창을 나갈 때 서버 저장용 예비 변수들
    //kakaoMsgReserved = myInfo->GetKakaoMsg();
    pushNotiReserved = myInfo->GetPushNotification();
    potionMsgReserved = myInfo->GetPotionMsg();
    
    selectedBtn = -1;
    
    code = -1;
    
    kind = -1;
    
    return true;
}

void Setting::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        if (code != 0) // 성공적으로 네트워크가 마무리되면, 터치를 활성화시키지 않는다. (어차피 끄니까)
        {
            // 터치 활성
            CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
            this->setTouchPriority(Depth::GetCurPriority());
            isTouched = false;
            isKeybackTouched = false;
            //CCLog("Setting : 터치 활성 (Priority = %d)", this->getTouchPriority());
        }
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("Setting : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("Setting : 터치 비활성");
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
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_tutorial.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_coupon.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_service.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_maker.png"))->setColor(ccc3(255,255,255));
        }
    }
}

void Setting::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_setting.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Layer", tLayer, 2) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 147), CCSize(982, 1265), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492+270), CCSize(929, 904-270), "", "Layer", tLayer, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", // ver
                    ccp(0, 0), ccp(630, 192), CCSize(373, 97), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", // id
                    ccp(0, 0), ccp(77, 326), CCSize(926, 97), "", "Layer", tLayer, 1) );

    char temp[50];
    // text (version, kakaoID)
    int version1 = binaryVersion_current / 1000;
    int version2 = (binaryVersion_current / 100) % 10;
    int version3 = binaryVersion_current % 100;
    sprintf(temp, "게임버전 : %d.%d.%d", version1, version2, version3);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(temp, fontList[0], 36, ccp(0, 0), ccp(630+30, 192+30), ccc3(78,47,8), "", "Layer", tLayer, 4) );
    sprintf(temp, "카카오회원번호 : %s", myInfo->GetKakaoId().c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(temp, fontList[0], 36, ccp(0, 0), ccp(107, 356), ccc3(78,47,8), "", "Layer", tLayer, 4) );
    
    int offset = 30;

    // 버튼 : 공식카페
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_cafe.png", ccp(0, 0), ccp(96, 633-offset), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    // 버튼 : 만든 사람들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png1",
                    ccp(0, 0), ccp(737, 633-offset), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_maker.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 36), CCSize(0, 0), "button/btn_purple_mini.png1", "0", NULL, 1) );

    // 버튼 : 쿠폰등록
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png2",
                ccp(0, 0), ccp(96, 492-offset), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_coupon.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png2", "0", NULL, 1) );
    
    // 버튼 : 튜토리얼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png3",
                ccp(0, 0), ccp(423, 492-offset), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tutorial.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png3", "0", NULL, 1) );

    // 버튼 : ?
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_purple_mini.png4",
                ccp(0, 0), ccp(737, 492-offset), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_service.png",
                ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->
                getContentSize().width/2, 32), CCSize(0, 0), "button/btn_purple_mini.png4", "0", NULL, 1) );
    
    // 버튼 : logout
    m_pLogoutBtn = CCSprite::create("images/kakao/kakao_logout.png");
    m_pLogoutBtn->setAnchorPoint(ccp(0, 0));
    m_pLogoutBtn->setPosition(ccp(82, 192));
    tLayer->addChild(m_pLogoutBtn, 5);
    
    // 버튼 : id복사
    float w = m_pLogoutBtn->getContentSize().width;
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("탈퇴하기", fontList[0], 36, ccp(0, 0), ccp(82+w+15, 192+5), ccc3(78,47,8), "", "Layer", tLayer, 1, 0, 255, -100) );

    // text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("효과음", fontList[0], 48, ccp(0, 0), ccp(162, 1277-offset), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("배경음", fontList[0], 48, ccp(0, 0), ccp(162, 1168-offset), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("푸시알림", fontList[0], 48, ccp(0, 0), ccp(162, 1057-offset), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("카톡메시지수신", fontList[0], 48, ccp(0, 0), ccp(162, 942-offset), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("(포션,자랑하기,오늘의별사탕)", fontList[0], 32, ccp(0, 0), ccp(162, 829+50-offset), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("포션,그룹메시지", fontList[0], 48, ccp(0, 0), ccp(162, 829), ccc3(78,47,8), "", "Layer", tLayer, 2) );
    
    
    // on-off button
    char name[50], name2[50], key[20];
    bool status;
    CCPoint pos;
    for (int i = 0; i < 4; i++)
    {
        sprintf(name, "background/bg_degree_desc.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(627, 1261-i*112-offset), CCSize(300, 82), "", "Layer", tLayer, 2) );
        sprintf(name2, "letter/letter_on.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(38, 18), CCSize(0, 0), name, "1", NULL, 2) );
        sprintf(name2, "letter/letter_off.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(168, 18), CCSize(0, 0), name, "1", NULL, 2) );
      
        if (i < 2) // 클라이언트에서 들고오는 정보 (효과음, 배경음)
        {
            sprintf(key, "setting_option_%d", i);
            status = CCUserDefault::sharedUserDefault()->getBoolForKey(key);
        }
        else // 서버에서 들고오는 정보 (카카오, 푸시)
        {
            if (i == 2) status = myInfo->GetPushNotification();
            else if (i == 3) status = (myInfo->GetPotionMsg() && !myInfo->IsMessageBlocked());
        }
        if (status)
            pos = ccp(627, 1261-i*112-offset);
        else
            pos = ccp(627+150, 1261-i*112-offset);
        sprintf(name2, "button/btn_option.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), pos, CCSize(0, 0), "", "Layer", tLayer, 3) );
    }
    standardBtnPos = ccp(627, 0);
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool Setting::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // 카카오메시지, 푸시메시지, 포션수신 중 하나라도 바뀐 게 있다면 서버에 전송.
                if (pushNotiReserved != myInfo->GetPushNotification() || potionMsgReserved != myInfo->GetPotionMsg())
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
                    
                    char temp[50];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                    param += temp;
                    sprintf(temp, "push_notification=%d&", pushNotiReserved);
                    param += temp;
                    sprintf(temp, "potion_message=%d", potionMsgReserved);
                    param += temp;
                  
                    Network::HttpPost(param, URL_SETTING, this, httpresponse_selector(Setting::onHttpRequestCompleted));
                }
                else
                {
                    // 바뀐 게 없으면 그냥 끈다.
                    EndScene();
                }
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 21) == "button/btn_option.png") // on-off's
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                // 카카오톡에서 메시지 수신 차단한 경우, 게임 내에서 바꿀 수 없다. (안내메시지를 띄우자)
                int number = atoi(spriteClass->spriteObj[i]->name.substr(21).c_str());
                if (number == 3 && myInfo->IsMessageBlocked())
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "Setting", "NoImage", false, KAKAO_MSG_BLOCKED, BTN_1, nullData);
                    return true;
                }
                
                selectedSprite = spriteClass->spriteObj[i]->sprite;
                selectedBtn = spriteClass->spriteObj[i]->name[spriteClass->spriteObj[i]->name.size()-1] - '0';
                selectedPos = spriteClass->spriteObj[i]->sprite->getPosition();
                standardBtnPos.y = selectedPos.y;
                selectedTouchPos = point;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_cafe.png") // 공식카페
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CAFE;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_purple_mini.png1") // 만든사람들 버튼
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_maker.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CREDIT;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_purple_mini.png2") // 쿠폰등록 버튼
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_coupon.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_COUPON;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_purple_mini.png3") // 튜토리얼 버튼
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                //sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_tutorial.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_TUTORIAL;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_purple_mini.png4") // 고객센터 버튼
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_service.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_SERVICE;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->type == 2 && spriteClass->spriteObj[i]->label->getTag() == -100)
        {
            if (spriteClass->spriteObj[i]->label->boundingBox().containsPoint(point))
            {
                sound->playClick();
                std::vector<int> nullData;
                Common::ShowPopup(this, "Setting", "NoImage", false, KAKAO_UNREGISTER, BTN_2, nullData);
                return true;
            }
        }
    }
    
    if (m_pLogoutBtn->boundingBox().containsPoint(point)) // 로그아웃
    {
        sound->playClick();
        m_pLogoutBtn->setColor(ccc3(170,170,170));
        rect = m_pLogoutBtn->boundingBox();
        kind = BTN_MENU_LOGOUT;
        idx = 999;
        return true;
    }
    
    return true;
}

void Setting::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    if (selectedBtn != -1)
    {
        CCPoint point = pTouch->getLocation();
        CCSize size = selectedSprite->getContentSize();
    
        int x = (int)selectedPos.x + ((int)point.x-(int)selectedTouchPos.x);
        if (x < (int)standardBtnPos.x) x = (int)standardBtnPos.x;
        if (x > (int)standardBtnPos.x+(int)size.width) x = (int)standardBtnPos.x+(int)size.width;
    
        selectedSprite->setPosition(ccp((int)x, (int)selectedPos.y));
    }
}

void Setting::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    if (selectedBtn != -1)
    {
        char name[20];
        sprintf(name, "setting_option_%d", selectedBtn);
        
        CCPoint pos = selectedSprite->getPosition();
        CCSize size = selectedSprite->getContentSize();
        
        if ((int)pos.x <= (int)standardBtnPos.x + (int)size.width/2)
        {
            // On에 가까우면 on위치로 정확히 옮긴다.
            selectedSprite->setPosition(standardBtnPos);
            
            // 효과음 버튼은 on->on 상황일 때 한번만 소리내자.
            bool playSound = !CCUserDefault::sharedUserDefault()->getBoolForKey(name);
            
            if (selectedBtn == 0)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
                sound->SetEffectVolume();
                if (playSound) sound->playClick();
            }
            else if (selectedBtn == 1)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, true);
                sound->PlayBackgroundSound();
            }
            else if (selectedBtn == 2)
                pushNotiReserved = true;
            else if (selectedBtn == 3)
                potionMsgReserved = true;
        }
        else
        {
            // Off에 가까우면 off위치로 정확히 옮긴다.
            selectedSprite->setPosition(ccp((int)standardBtnPos.x+(int)size.width,
                                            (int)standardBtnPos.y));
            
            if (selectedBtn == 0)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
                sound->SetEffectVolume();
            }
            else if (selectedBtn == 1)
            {
                CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
                sound->StopBackgroundSound();
            }
            else if (selectedBtn == 2)
                pushNotiReserved = false;
            else if (selectedBtn == 3)
                potionMsgReserved = false;
        }
    }
    
    selectedBtn = -1;
    
    if (idx > -1)
    {
        if (idx == 999)
            m_pLogoutBtn->setColor(ccc3(255,255,255));
        //else if (idx == 888)
        //    cafe->setColor(ccc3(255,255,255));
        else
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_tutorial.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_coupon.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_service.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_maker.png"))->setColor(ccc3(255,255,255));
        }
    }
    if (rect.containsPoint(point))
    {
        std::vector<int> nullData;
        switch (kind)
        {
            case BTN_MENU_TUTORIAL:
                menuInSetting = 0;
                EndScene();
                break;
            case BTN_MENU_SERVICE:
                Common::ShowPopup(this, "Setting", "NoImage", false, SERVICE, BTN_1, nullData);
                break;
            case BTN_MENU_COUPON:
                Common::ShowNextScene(this, "Setting", "Coupon", false);
                break;
            case BTN_MENU_LOGOUT:
                if (pushNotiReserved != myInfo->GetPushNotification() || potionMsgReserved != myInfo->GetPotionMsg())
                {
                    // Loading 화면으로 MESSAGE request 넘기기
                    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
                    
                    char temp[50];
                    std::string param = "";
                    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                    param += temp;
                    sprintf(temp, "push_notification=%d&", pushNotiReserved);
                    param += temp;
                    sprintf(temp, "potion_message=%d", potionMsgReserved);
                    param += temp;
                    
                    Network::HttpPost(param, URL_SETTING, this, httpresponse_selector(Setting::onHttpRequestCompleted));
                }
                else
                    Common::ShowPopup(this, "Setting", "NoImage", false, KAKAO_LOGOUT, BTN_2, nullData);
                break;
            case BTN_MENU_CREDIT:
                Common::ShowPopup(this, "Setting", "NoImage", false, CREDIT, BTN_1, nullData);
                break;
            case BTN_MENU_CAFE:
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                JniMethodInfo t;
                if (JniHelper::getStaticMethodInfo(t,
                                                   "com/playDANDi/CocoMagic/CocoMagic",
                                                   "OpenNoticeURL",
                                                   "(Ljava/lang/String;)V"))
                {
                    // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
                    t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(URL_PUBLIC_CAFE));
                    // Release
                    t.env->DeleteLocalRef(t.classID);
                }
                #endif
                break;
        }
    }
}

void Setting::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParseResult(&xmlDoc);
}

void Setting::XmlParseResult(xml_document *xmlDoc)
{
    // Loading 창 끄기 (EndScene이 바로 뒤에 실행된다면 여기서 해야 함 [code 확인 때문에])
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "Setting", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        myInfo->SetSettingVariables(kakaoMsgReserved, pushNotiReserved, potionMsgReserved);
        
        if (kind == BTN_MENU_LOGOUT)
        {
            std::vector<int> nullData;
            Common::ShowPopup(this, "Setting", "NoImage", false, KAKAO_LOGOUT, BTN_2, nullData);
        }
        else
            EndScene();
    }
}


void Setting::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    m_pLogoutBtn->removeFromParentAndCleanup(true);
    //cafe->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
    
    // 특정 메뉴를 누른 경우
    switch (menuInSetting)
    {
        case 0: // 튜토리얼 시작
            param = CCString::create("3");
            CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
            param = CCString::create("4");
            CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
            break;
    }
}




