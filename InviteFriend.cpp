#include "InviteFriend.h"

static std::vector<std::string> kakaoIds;
static std::vector<int> remainTimes;
static int todayCnt, monthCnt, totalCnt;


CCScene* InviteFriend::scene()
{
    CCScene* pScene = CCScene::create();
    InviteFriend* pLayer = InviteFriend::create();
    pScene->addChild(pLayer);
	return pScene;
}

void InviteFriend::onEnter()
{
    CCLog("InviteFriend : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
    
    // 네트워크로 초대할 친구 리스트를 받아온다.
    httpStatus = 0;
    char temp[50];
    std::string params = "";
    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
    params += temp;
    
    Network::HttpPost(params, URL_INVITE_FRIEND_LIST, this, httpresponse_selector(InviteFriend::onHttpRequestCompleted));
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(InviteFriend::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void InviteFriend::SceneCallback()
{
}
void InviteFriend::onExit()
{
    CCLog("InviteFriend : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void InviteFriend::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool InviteFriend::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("InviteFriend", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("InviteFriend : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(InviteFriend::Notification), Depth::GetCurName(), NULL);
    
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
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 3);
    //this->addChild(scrollView, 3);
    
    spriteClass = new SpriteClass();
    spriteClassScroll = new SpriteClass();
    
    return true;
}

void InviteFriend::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("InviteFriend : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("InviteFriend : 터치 비활성");
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void InviteFriend::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // strap
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png", ccp(0, 0), ccp(14-ofs, 1343), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875-ofs, 1391), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_invite.png", ccp(0, 0), ccp(359-ofs, 1389), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png", ccp(0, 0), ccp(264-ofs, 1389), CCSize(0, 0), "", "Layer", tLayer, 2) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49-ofs, 147), CCSize(982, 1265), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(75-ofs, 492), CCSize(929, 904), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(98-ofs, 256-30), CCSize(244, 176+30), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(390-ofs, 256-30), CCSize(244, 176+30), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png4", ccp(0, 0), ccp(686-ofs, 256-30), CCSize(293, 176+30), "", "Layer", tLayer, 1) );
    
    // 친구초대 보상이벤트
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_friendevent.png", ccp(0, 0), ccp(98-ofs, 438), CCSize(0, 0), "", "Layer", tLayer, 2) );
    
    // invite 10,20,30
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png1", ccp(0, 0), ccp(228-ofs, 378), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png2", ccp(0, 0), ccp(519-ofs, 378), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png3", ccp(0, 0), ccp(817+49-ofs, 378), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_10.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png1"), CCSize(0, 0), "background/bg_dontknow_1.png1", "0", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_20.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png2"), CCSize(0, 0), "background/bg_dontknow_1.png2", "0", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_30.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png3"), CCSize(0, 0), "background/bg_dontknow_1.png3", "0", NULL, 2, 1) );
    
    // 그림들
    /*
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_10.png", ccp(0.5, 0), ccp(98+244/2, 226+5), CCSize(700, 30), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_20.png", ccp(0.5, 0), ccp(390+244/2, 226+5), CCSize(700, 30), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_30.png", ccp(0.5, 0), ccp(686+293/2, 226+5), CCSize(700, 30), "", "Layer", tLayer, 2) );
    */
    
    // progress bar 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_petlevel.png1", ccp(0, 0), ccp(96+10-ofs, 192-15), CCSize(700, 30), "", "Layer", tLayer, 2) );
    // bar
    float size = (float)totalCnt / (float)MAX_NUM_OF_INVITE_FRIEND;
    if (size > 1.0f) size = 1.0f;
    bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, size*(700-10), 30-12));
    bar->setPosition(ccp(96+10+5-ofs, 192+6-15));
    bar->setAnchorPoint(ccp(0, 0));
    bar->setColor(ccc3(255,255,255));
    tLayer->addChild(bar, 3);
    
    // 초대 인원 수 text
    char name[20];
    sprintf(name, "%d명 초대", totalCnt);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0, 0), ccp(812+10-ofs, 200-20), ccc3(255,255,255), "", "Layer", tLayer, 2, 0, 255, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

bool compareInvite(InviteList *a, InviteList *b)
{
    int c = a->nickname.compare(b->nickname);
    
    if (a->wasInvited && b->wasInvited)
        return (c < 0);
    else if (a->wasInvited)
        return false;
    else if (b->wasInvited)
        return true;
    else
        return (c < 0);
}

void InviteFriend::MakeScroll()
{
    for (int i = 0 ; i < inviteList.size() ; i++)
        delete inviteList[i];
    inviteList.clear();
    
    int numOfList = KakaoFriends::getInstance()->friends->count();
    CCLog("numOfList = %d", numOfList);
    CCArray* keys = KakaoFriends::getInstance()->friends->allKeys();
    
    bool wasInvited;
    for (int j = 0 ; j < numOfList ; j++)
    {
        CCString* k = (CCString*)keys->objectAtIndex(j);
        std::string userId = k->getCString();
        CCLog("user id = %s", userId.c_str());
        KakaoFriends::Friends* f = (KakaoFriends::Friends*)KakaoFriends::getInstance()->friends->objectForKey(userId.c_str());
        
        // 이미 초대되어있는지 서버에서 받아온 정보와 비교한다.
        wasInvited = false;
        for (int i = 0 ; i < kakaoIds.size() ; i++)
        {
            if (userId == kakaoIds[i] && remainTimes[i] > 0)
            {
                wasInvited = true;
                break;
            }
        }

        inviteList.push_back( new InviteList(userId, f->nickname, f->profileImageUrl, f->hashedTalkUserId, f->messageBlocked, f->supportedDevice, wasInvited) );
    }
    // 정렬 : 닉네임 순 (단, 초대된 사람은 무조건 뒤로 보낸다)
    std::sort(inviteList.begin(), inviteList.end(), compareInvite);
    
    
    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    
    char name[50], name2[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        
        
        // profile bg
        sprintf(name, "background/bg_profile_noimage.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        
        // nickname
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(inviteList[i]->nickname, fontList[0], 48, ccp(0, 0), ccp(196, 118-10), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // starcandy bg + starcandy + text(x 1000)
        sprintf(name, "background/bg_degree_desc.png1%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(269-60, 25), CCSize(223+160-5, 76), "", "Layer", itemLayer, 3) );
        sprintf(name2, "icon/icon_starcandy_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(13, 6), CCSize(0, 0), name, "1", NULL, 3, 1) );
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel("1,000", fontList[0], 36, ccp(0, 0), ccp(83, 19), ccc3(78,47,8), name, "1", NULL, 3, 1) );
        
        // potion bg + potion + text(x 1)
        //sprintf(name, "background/bg_degree_desc.png2%d", i);
        //spriteClassScroll->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(269-60+230, 25), CCSize(223-70, 76), "", "Layer", itemLayer, 3) );
        sprintf(name2, "icon/icon_potion.png%d", i);
        //spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(17, 7), CCSize(0, 0), name, "1", NULL, 3, 1) );
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(17+215, 10), CCSize(0, 0), name, "1", NULL, 3, 1) );
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setScale(0.8f);
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel("1", fontList[0], 36, ccp(0, 0), ccp(83+215-3, 19), ccc3(78,47,8), name, "1", NULL, 3, 1) );
        //spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel("x 1", fontList[0], 36, ccp(0, 0), ccp(83, 19), ccc3(78,47,8), name, "1", NULL, 3, 1) );
        
        // button
        if (!inviteList[i]->wasInvited) // 초대 가능한 경우
        {
            sprintf(name, "button/btn_blue_mini.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
            sprintf(name2, "letter/letter_invite.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(45, 25), CCSize(0, 0), name, "0", NULL, 3, 1) );
        }
        else // 이미 초대한 경우
        {
            sprintf(name, "button/btn_invite.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
            sprintf(name2, "letter/letter_invite_brown.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(45, 25), CCSize(0, 0), name, "0", NULL, 3, 1) );
        }
        
        // dotted line
        sprintf(name, "background/bg_dotted_line.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(0, 10), CCSize(0, 0), "", "Layer", itemLayer, 3) );
    }
    
    // container 설정
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
}


bool InviteFriend::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
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


void InviteFriend::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void InviteFriend::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    CCPoint p;
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
    {
        if (spriteClassScroll->spriteObj[i]->name.substr(0, 24) == "button/btn_blue_mini.png")
        {
            p = spriteClassScroll->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassScroll->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                int idx = atoi(spriteClassScroll->spriteObj[i]->name.substr(24, 25).c_str());
                
                // Loading 화면으로 MESSAGE request 넘기기
                Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
                
                // 친구를 초대한다.
                httpStatus = 1;
                char temp[50];
                std::string param = "";
                sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                param += temp;
                //sprintf(temp, "friend_kakao_id=%d", kakaoIds[idx]);
                //param += temp;
                
                // tag
                sprintf(temp, "%d", idx);
                
                Network::HttpPost(param, URL_INVITE_FRIEND, this, httpresponse_selector(InviteFriend::onHttpRequestCompleted), temp);

                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}

void InviteFriend::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void InviteFriend::scrollViewDidZoom(CCScrollView* view)
{
}


void InviteFriend::EndScene()
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
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    bar->removeFromParentAndCleanup(true);
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void InviteFriend::RenewData()
{
    // bar
    float size = (float)totalCnt / (float)MAX_NUM_OF_INVITE_FRIEND;
    if (size > 1.0f) size = 1.0f;
    bar->removeFromParentAndCleanup(true);
    bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, size*(700-10), 48-12));
    bar->setPosition(ccp(96+5, 192+6));
    bar->setAnchorPoint(ccp(0, 0));
    bar->setColor(ccc3(255,255,255));
    this->addChild(bar, 3);
    
    char name[20];
    sprintf(name, "%d명 초대", totalCnt);
    ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(name);
    
    spriteClassScroll->RemoveAllObjects();
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    MakeScroll();
}


void InviteFriend::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    //CCLog("http status = %d", httpStatus);
    switch (httpStatus)
    {
        case 0: XmlParseList(&xmlDoc); break;
        case 1: XmlParseInviteFriend(&xmlDoc, atoi(res->getHttpRequest()->getTag())); break;
    }
}

void InviteFriend::XmlParseList(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else
            Common::ShowPopup(this, "InviteFriend", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        kakaoIds.clear();
        remainTimes.clear();
        std::string kakaoId;
        int remainTime;
        std::string name;
        
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("invite-friend-list").children("friend");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "kakao-id") kakaoId = ait->as_string();
                else if (name == "remain-time") remainTime = ait->as_int();
            }
            kakaoIds.push_back(kakaoId);
            remainTimes.push_back(remainTime);
        }
        
        // today, month, total count
        todayCnt = nodeResult.child("count").attribute("today").as_int();
        monthCnt = nodeResult.child("count").attribute("month").as_int();
        totalCnt = nodeResult.child("count").attribute("total-invite").as_int();
        
        // init sprite
        InitSprites();
        // scroll을 생성 후 데이터 보여주기
        MakeScroll();
    }
}

void InviteFriend::XmlParseInviteFriend(xml_document *xmlDoc, int idx)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10)
            Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_MONTH_OVER_30, BTN_1, nullData);
        else if (code == 11)
            Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_DAY_OVER_20, BTN_1, nullData);
        else if (code == 12)
            Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_ALREADY_DID, BTN_1, nullData);
        else
            Common::ShowPopup(this, "InviteFriend", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        // 포션 갱신
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, potionRemainTime);
        // 정보 갱신
        todayCnt = nodeResult.child("friend-invite").attribute("today-count").as_int();
        monthCnt = nodeResult.child("friend-invite").attribute("month-count").as_int();
        totalCnt = nodeResult.child("friend-invite").attribute("total-count").as_int();
        int reward = nodeResult.child("friend-invite").attribute("special-reward").as_int();
        
        // 리스트 데이터 갱신
        remainTimes[idx] = 9999999;
        
        // scroll을 생성 후 데이터 보여주기
        RenewData();
        
        // Ranking scene에 데이터 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
        
        // 결과 내용 popup 띄우기
        std::vector<int> data;
        data.push_back(reward);
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_OK, BTN_1, data);
    }
}

InviteList::InviteList(std::string userid, std::string name, std::string purl, std::string htuid, bool msgblocked, bool supporteddevice, bool wi)
{
    this->userId = userid;
    this->nickname = name;
    this->profileUrl = purl;
    this->hashedTalkUserId = htuid;
    this->messageBlocked = msgblocked;
    this->supportedDevice = supporteddevice;
    this->wasInvited = wi;
}


