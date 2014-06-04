#include "InviteFriend.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

static std::vector<int> kakaoIds;
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
    EndScene();
}


bool InviteFriend::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
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
    
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 3);
    
    spriteClass = new SpriteClass();
    spriteClassScroll = new SpriteClass();
    
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
    
    // 네트워크로 초대할 친구 리스트를 받아온다.
    httpStatus = 0;
    char temp[50];
    std::string url = "http://14.63.225.203/cogma/game/friend_invite_list.php?";
    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
    url += temp;
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(this, httpresponse_selector(InviteFriend::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
    CCLog("url = %s", url.c_str());
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void InviteFriend::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("InviteFriend : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("InviteFriend : 터치 비활성");
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
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png", ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_invite.png", ccp(0, 0), ccp(359, 1389), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png", ccp(0, 0), ccp(264, 1389), CCSize(0, 0), "", "InviteFriend", this, 2) );
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 147), CCSize(982, 1265), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(98, 256-30), CCSize(244, 176+30), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(390, 256-30), CCSize(244, 176+30), "", "InviteFriend", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png4", ccp(0, 0), ccp(686, 256-30), CCSize(293, 176+30), "", "InviteFriend", this, 1) );
    
    // 친구초대 보상이벤트
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_friendevent.png", ccp(0, 0), ccp(98, 438), CCSize(0, 0), "", "InviteFriend", this, 2) );
    
    // invite 10,20,30
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png1", ccp(0, 0), ccp(228, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png2", ccp(0, 0), ccp(519, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dontknow_1.png3", ccp(0, 0), ccp(817+49, 378), CCSize(0, 0), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_10.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png1"), CCSize(0, 0), "background/bg_dontknow_1.png1", "0", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_20.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png2"), CCSize(0, 0), "background/bg_dontknow_1.png2", "0", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invite_30.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_dontknow_1.png3"), CCSize(0, 0), "background/bg_dontknow_1.png3", "0", NULL, 2, 1) );
    
    // 그림들
    /*
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_10.png", ccp(0.5, 0), ccp(98+244/2, 226+5), CCSize(700, 30), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_20.png", ccp(0.5, 0), ccp(390+244/2, 226+5), CCSize(700, 30), "", "InviteFriend", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_30.png", ccp(0.5, 0), ccp(686+293/2, 226+5), CCSize(700, 30), "", "InviteFriend", this, 2) );
    */
    
    // progress bar 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_petlevel.png1", ccp(0, 0), ccp(96+10, 192-15), CCSize(700, 30), "", "InviteFriend", this, 2) );
    // bar
    float size = (float)totalCnt / (float)MAX_NUM_OF_INVITE_FRIEND;
    if (size > 1.0f) size = 1.0f;
    bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, size*(700-10), 30-12));
    bar->setPosition(ccp(96+10+5, 192+6-15));
    bar->setAnchorPoint(ccp(0, 0));
    bar->setColor(ccc3(255,255,255));
    this->addChild(bar, 3);
    
    // 초대 인원 수 text
    char name[20];
    sprintf(name, "%d명 초대", totalCnt);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0, 0), ccp(812+10, 200-20), ccc3(255,255,255), "", "InviteFriend", this, 2, 0, 255, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void InviteFriend::MakeScroll()
{
    int numOfList = kakaoIds.size();
    
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
        
        // name
        sprintf(name, "%d번 친구", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0, 0), ccp(196, 118-10), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
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
        if (remainTimes[i] == 0) // 초대 가능한 경우
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
                std::string url = "http://14.63.225.203/cogma/game/friend_invite.php?";
                sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                url += temp;
                sprintf(temp, "friend_kakao_id=%d", kakaoIds[idx]);
                url += temp;
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(url.c_str());
                req->setRequestType(CCHttpRequest::kHttpPost);
                req->setResponseCallback(this, httpresponse_selector(InviteFriend::onHttpRequestCompleted));
                CCHttpClient::getInstance()->send(req);
                req->release();
                sprintf(temp, "%d", idx);
                req->setTag(temp);
                CCLog("url = %s", url.c_str());
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
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }

    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    switch (httpStatus)
    {
        case 0: XmlParseList(dumpData, buffer->size()); break;
        case 1: XmlParseInviteFriend(dumpData, buffer->size(), atoi(res->getHttpRequest()->getTag())); break;
    }
}

void InviteFriend::XmlParseList(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        kakaoIds.clear();
        remainTimes.clear();
        int kakaoId, remainTime;
        std::string name;
        
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("invite-friend-list").children("friend");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "kakao-id") kakaoId = ait->as_int();
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
    else
    {
        CCLog("FAILED : code = %d", code);
    }
}

void InviteFriend::XmlParseInviteFriend(char* data, int size, int idx)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
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
    else
    {
        std::vector<int> nullData;
        if (code == 10) Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_MONTH_OVER_30, BTN_1, nullData);
        else if (code == 11) Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_DAY_OVER_20, BTN_1, nullData);
        else if (code == 12) Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_ALREADY_DID, BTN_1, nullData);
        else Common::ShowPopup(this, "InviteFriend", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
}
