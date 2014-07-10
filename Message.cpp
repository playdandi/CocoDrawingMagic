#include "Message.h"

CCScene* Message::scene()
{
    CCScene* pScene = CCScene::create();
    Message* pLayer = Message::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Message::onEnter()
{
    CCLog("Message : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, "Message", "Loading", false, LOADING_MESSAGE);
    
    httpStatus = 0;
    msgData.clear();
    // 네트워크로 메시지들을 받아온다.
    char temp[50];
    std::string params = "";
    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
    params += temp;
    
    Network::HttpPost(params, URL_MESSAGE_LIST, this, httpresponse_selector(Message::onHttpRequestCompleted));
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(Message::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void Message::SceneCallback()
{
}
void Message::onExit()
{
    CCLog("Message : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Message::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool Message::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("Message", this);
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("Message : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Message::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, 0));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);
    
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 3);
    
    InitSprites();
    
    spriteClassScroll = new SpriteClass();
    scrollContainer = NULL;

    return true;
}

void Message::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("Message : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("Message : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 메시지 data 갱신 (전체 포션 받기를 했을 때는 행하지 않는다)
        if (httpMsgIdx != -1)
        {
            for (int i = 0 ; i < msgData.size() ; i++)
            {
                if (i == httpMsgIdx)
                    delete msgData[i];
                if (i > httpMsgIdx)
                    msgData[i-1] = msgData[i];
            }
            msgData.pop_back();
        }
        
        // scroll 갱신
        RenewScroll();
        
        // Notification : Ranking 화면에 데이터 갱신
        myInfo->SetMsgCnt((int)msgData.size());
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("Message : 터치 비활성");
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        // scroll 갱신
        RenewScroll();
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        if (idx > -1)
        {
            ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"))->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_potion_all_recieve.png"))->setColor(ccc3(255,255,255));
        }
    }
}

void Message::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
     
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png",
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Layer", tLayer, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Layer", tLayer, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_message.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Layer", tLayer, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Layer", tLayer, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Layer", tLayer, 1, 0) );

    // 모두받기 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png",
            ccp(0, 0), ccp(319, 191), CCSize(929, 904), "", "Layer", tLayer, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_potion_all_recieve.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 40), CCSize(0, 0), "button/btn_red.png", "0", NULL, 1, 1) );
    // 버튼 젤리 움직임
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
    temp->runAction(CCRepeatForever::create(action));
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void Message::MakeScroll()
{
    int numOfList = msgData.size();

    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));

    char spriteName[35], spriteName2[35];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        
        /*
          id : 메시지 인덱스
          type : 버튼출력 타입 (1-공지, 2-별사탕, 3-토파즈, 4-포션, 5-포션요청)
          content : 출력 메시지
          friend-profile-image-url : 선물한 친구의 프로필 사진 경로
          reward-count : 선물한 개수 (공지의 경우 내용 없음)
          notice-url : 공지의 경우 출력해야할 공지 이미지 주소 (공지 외 메시지의 경우 내용 없음)
        */
        
        // 프로필 이미지
        bool flag = false;
        for (int j = 0 ; j < profiles.size() ; j++)
        {
            if (msgData[i]->GetProfileUrl() == profiles[j]->GetProfileUrl())
            {
                flag = true;
                if (msgData[i]->GetProfileUrl() != "") // 프로필 사진이 있으면
                {
                    spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profiles[j]->GetProfile(), ccp(0, 0), ccp(44+5, 35+11), CCSize(0,0), "", "Layer", itemLayer, 2, 0, 255, 0.85f) );
                    sprintf(spriteName, "background/bg_profile.png%d", i);
                    spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0, 0), "", "Layer", itemLayer, 2) );
                }
                else // 없으면
                {
                    spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profiles[j]->GetProfile(), ccp(0, 0), ccp(44, 35), CCSize(0,0), "", "Layer", itemLayer, 2) );
                }
            }
        }
        if (!flag)
        {
            if (msgData[i]->GetProfileUrl() == "COCO_IMG") // 공지/시스템/이벤트
            {
                sprintf(spriteName, "background/bg_profile_coco.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0,0), "", "Layer", itemLayer, 2) );
            }
            else if (msgData[i]->GetProfileUrl() == "PET_IMG_MEDAL") // 오.별 본인당첨
            {
                sprintf(spriteName, "background/bg_profile_fairy.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0,0), "", "Layer", itemLayer, 2) );
                int w = ((CCSprite*)spriteClassScroll->FindSpriteByName(spriteName))->getContentSize().width;
                sprintf(spriteName2, "icon/icon_medal_mini.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName2, ccp(1, 0), ccp(w, 0), CCSize(0,0), spriteName, "0", NULL, 2, 1) );
            }
            else if (msgData[i]->GetProfileUrl() == "PET_IMG_NOMEDAL") // 오.별 참가상
            {
                sprintf(spriteName, "background/bg_profile_fairy.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0,0), "", "Layer", itemLayer, 2) );
            }
        }
        
    
        // content (fontList[2] = 나눔고딕볼드)
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabelArea(msgData[i]->GetContent(), fontList[0], 36, ccp(0, 0), ccp(194, 45), ccc3(78,47,8), CCSize(400, 105), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", itemLayer, 3, 0) );
        
        
        // button (type마다 버튼이 다르다)
        sprintf(spriteName, "button/btn_green_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(634, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
        switch (msgData[i]->GetType())
        {
            case 1: // 공지 (보기)
                break;
            case 2: // 별사탕 (별사탕아이콘 + 받기)
                sprintf(spriteName, "button/btn_receive_starcandy.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(664, 63), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 3: // 토파즈 (토파즈아이콘 + 받기)
                sprintf(spriteName, "button/btn_receive_topaz.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(667, 60), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 4: // 포션 (포션아이콘 + 받기)
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 5: // 포션요청 (포션아이콘 + 받기)
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_present.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 6: // 토파즈 (토파즈아이콘 + 받기)
                sprintf(spriteName, "button/btn_receive_topaz.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(667, 60), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_present.png%d", i);
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
        }
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
    }
    
    // container 세팅
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    
    // add child
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
}

void Message::RenewScroll()
{
    // delete & init all scroll-related variables.
    spriteClassScroll->RemoveAllObjects();
    scrollContainer->removeAllChildren();
    scrollContainer->removeFromParentAndCleanup(true);
    scrollView->removeAllChildren();
    
    // 다시 스크롤 생성
    MakeScroll();
}


bool Message::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
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
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_potion_all_recieve.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
            }
        }
    }
    
    return true;
}


void Message::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void Message::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    CCPoint p;
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
    {
        if (spriteClassScroll->spriteObj[i]->name.substr(0, 25) == "button/btn_green_mini.png")
        {
            p = spriteClassScroll->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassScroll->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                httpMsgIdx = atoi(spriteClassScroll->spriteObj[i]->name.substr(25).c_str());
                
                // 공지는 팝업창 띄우는 거라 나중에 구현하자...
                if (msgData[httpMsgIdx]->GetType() == 1)
                    continue;
                
                sound->playClick();
                
                // Loading 화면으로 MESSAGE request 넘기기
                Common::ShowNextScene(this, "Message", "Loading", false, LOADING_MESSAGE);
                
                httpStatus = 1;
                // 메시지에 대한 처리 서버 통신
                char temp[50];
                std::string param = "";
                sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                param += temp;
                sprintf(temp, "message_id=%d", msgData[httpMsgIdx]->GetId());
                param += temp;
                
                Network::HttpPost(param, URL_MESSAGE_ONE, this, httpresponse_selector(Message::onHttpRequestCompleted));
                
                // 포션 요청에 대한 메시지이므로 send_potion protocol을 호출한다. 결과는 전혀 신경쓰지 않는다.
                if (msgData[httpMsgIdx]->GetType() == 5 &&
                    Friend::GetRemainPotionTime(msgData[httpMsgIdx]->GetFriendKakaoId()) <= 0)
                {
                    std::string param = "";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    param += temp;
                    sprintf(temp, "friend_kakao_id=%d", msgData[httpMsgIdx]->GetFriendKakaoId());
                    param += temp;
                    
                    Network::HttpPost(param, URL_SEND_POTION, this, httpresponse_selector(Message::onHttpRequestCompleted));
                }
                
                break;
            }
        }
    }
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_potion_all_recieve.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            if (msgData.size() > 0)
            {
                httpMsgIdx = -1; // 메시지 리스트 갱신 방지
                std::vector<int> nullData;
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_ALL_TRY, BTN_2, nullData);
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void Message::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Message::scrollViewDidZoom(CCScrollView* view)
{
}


void Message::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    if (scrollContainer != NULL)
        scrollContainer->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void Message::EndSceneCallback()
{
}



void Message::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    //((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    switch (httpStatus)
    {
        case 0: XmlParseMsg(&xmlDoc); break;
        case 1: XmlParseMsgReceiveOne(&xmlDoc); break;
        //case 2: ParseProfileImage(dumpData, bufferSize, atoi(res->getHttpRequest()->getTag())); break;
    }
}
void Message::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
{
    // Loading 창 끄기
    //((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[BUFFER_SIZE];
    
    // 프로필 사진 or resource.xml 받아올 때
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    
    ParseProfileImage(dumpData, (int)buffer->size(), atoi(res->getHttpRequest()->getTag()));
}

void Message::XmlParseMsg(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        // Loading 창 끄기
        ((Loading*)Depth::GetCurPointer())->EndScene();
        
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "Message", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        int id, type;
        int rewardCount, friendKakaoId;
        std::string content, profileUrl, noticeUrl;
        std::string name;
        
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("message-list").children("message");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            friendKakaoId = -1;
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "type") type = ait->as_int();
            }
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "content") content = ait->as_string();
                else if (name == "friend-profile-image-url") profileUrl = ait->as_string();
                else if (name == "reward-count") rewardCount = ait->as_int();
                else if (name == "notice-url") noticeUrl = "";
                else if (type == 5 && name == "friend-kakao-id") friendKakaoId = ait->as_int();
            }
            msgData.push_back( new Msg(id, type, rewardCount, content, profileUrl, noticeUrl, friendKakaoId) );
        }
        
        httpStatus = 2;
        // 새로 받아야 할 프로필 이미지가 있는지 검사.
        newProfileCnt = profiles.size();
        char tag[5];
        bool flag = true;
        for (int i = 0 ; i < msgData.size() ; i++)
        {
            if (msgData[i]->GetProfileUrl() == "COCO_IMG" ||
                msgData[i]->GetProfileUrl() == "PET_IMG_MEDAL" ||
                msgData[i]->GetProfileUrl() == "PET_IMG_NOMEDAL")
            {
                continue;
            }
            else if (ProfileSprite::GetProfile(msgData[i]->GetProfileUrl()) == NULL)
            {
                flag = false;
                profiles.push_back( new ProfileSprite(msgData[i]->GetProfileUrl()) );
                
                // get profile image sprite from URL
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(msgData[i]->GetProfileUrl().c_str());
                req->setRequestType(CCHttpRequest::kHttpGet);
                req->setResponseCallback(this, httpresponse_selector(Message::onHttpRequestCompletedNoEncrypt));
                sprintf(tag, "%d", (int)profiles.size()-1);
                req->setTag(tag);
                CCHttpClient::getInstance()->send(req);
                req->release();
            }
        }

        // 새로 받을 프로필 이미지가 없다면, 바로 화면에 나타낸다.
        if (flag)
        {
            // Loading 창 끄기
            ((Loading*)Depth::GetCurPointer())->EndScene();
            
            MakeScroll();
        }
        
        // Notification : Ranking 화면에 데이터 갱신
        myInfo->SetMsgCnt((int)msgData.size());
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }
}

void Message::ParseProfileImage(char* data, int size, int idx)
{
    // make texture2D
    CCImage* img = new CCImage;
    img->initWithImageData(data, size);
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
    
    // set CCSprite
    profiles[idx]->SetSprite(texture);
    
    // 새 프로필까지 다 받으면 화면에 나타낸다.
    newProfileCnt++;
    if (newProfileCnt == (int)profiles.size())
    {
        // Loading 창 끄기
        ((Loading*)Depth::GetCurPointer())->EndScene();
        
        MakeScroll();
    }
}

void Message::XmlParseMsgReceiveOne(xml_document *xmlDoc)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 없는 메시지 (삭제하자)
        {
            std::vector<int> nullData;
            Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_EMPTY, BTN_1, nullData);
        }
        else
            Common::ShowPopup(this, "Message", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetMoney(topaz, starcandy);
        myInfo->SetPotion(potion, remainTime);
        
        std::vector<int> data;
        data.push_back(msgData[httpMsgIdx]->GetRewardCount());
        switch (msgData[httpMsgIdx]->GetType())
        {
            case 1: break; // 공지
                //Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_NOTICE, BTN_1, data); break;
            case 2: // 별사탕 받기
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_STARCANDY, BTN_1, data); break;
            case 3: // 토파즈 받기
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_TOPAZ, BTN_1, data); break;
            case 4: // 포션 받기
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_POTION, BTN_1, data); break;
            case 5: // 포션 요청에 응하기
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_POTION_REQUEST, BTN_1, data);
                if (Friend::GetRemainPotionTime(msgData[httpMsgIdx]->GetFriendKakaoId()) <= 0)
                {
                    Friend* f = Friend::GetObj(msgData[httpMsgIdx]->GetFriendKakaoId());
                    f->SetRemainPotionTime(3600);
                    f->SetPotionSprite();
                }
                break;
            case 6: // 토파즈 요청에 응하기
                Common::ShowNextScene(this, "Message", "BuyTopaz", false); break;
        }
    }
}


