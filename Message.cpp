#include "Message.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

CCScene* Message::scene()
{
    CCScene* pScene = CCScene::create();
    Message* pLayer = Message::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Message::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Message::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Message::keyBackClicked()
{
    EndScene();
}


bool Message::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Message::Notification), "Message", NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    // init sprites
    InitSprites();
    
    spriteClassScroll = new SpriteClass();
    
    httpStatus = 0;
    
    msgData.clear();
    // 네트워크로 메시지들을 받아온다.
    char temp[50];
    std::string url = "http://14.63.225.203/cogma/game/get_messagelist.php?";
    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
    url += temp;
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(this, httpresponse_selector(Message::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void Message::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        CCLog("Message : noti 활성");
        // 터치 활성
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
        isTouched = false;
        
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
        CCLog("Message : noti 비활성");
        // 터치 비활성
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
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
                    ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_message.png",
                    ccp(0, 0), ccp(409, 1389), CCSize(0, 0), "", "Message", this, 2, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Message", this, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Message", this, 1, 0) );

    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png",
            ccp(0, 0), ccp(319, 191), CCSize(929, 904), "", "Message", this, 1, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_potion_all_recieve.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 40), CCSize(0, 0), "button/btn_red.png", "0", NULL, 1, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void Message::MakeScroll()
{
    int numOfList = msgData.size();
    
    // 메시지가 0개면 스크롤뷰를 아예 생성하지 말 것.
    //if (numOfList == 0)
    //    return;

    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));

    char spriteName[35];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        layer.push_back(itemLayer);
        
        /*
          id : 메시지 인덱스
          type : 버튼출력 타입 (1-공지, 2-별사탕, 3-토파즈, 4-포션)
          content : 출력 메시지
          friend-profile-image-url : 선물한 친구의 프로필 사진 경로
          reward-count : 선물한 개수 (공지의 경우 내용 없음)
          notice-url : 공지의 경우 출력해야할 공지 이미지 주소 (공지 외 메시지의 경우 내용 없음)
        */
        
        // profile 영역 bg
        sprintf(spriteName, "background/bg_profile.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
        
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
        }
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    this->addChild(scrollView, 3);
    
    // add child
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
}

void Message::RenewScroll()
{
    // delete & init all scroll-related variables.
    spriteClassScroll->RemoveAllObjects();
    for (int i = 0 ; i < layer.size() ; i++)
        layer[i]->removeFromParentAndCleanup(true);
    layer.clear();
    scrollContainer->removeFromParentAndCleanup(true);
    scrollView->removeFromParentAndCleanup(true);
    
    // 다시 스크롤 생성
    MakeScroll();
}


void Message::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;

    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    switch (httpStatus)
    {
        case 0:
            XmlParseMsg(dumpData, buffer->size()); break;
        case 1:
            XmlParseMsgReceiveOne(dumpData, buffer->size()); break;
    }
}

void Message::XmlParseMsg(char* data, int size)
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
        int id, type;
        int rewardCount;
        std::string content, profileUrl, noticeUrl;
        std::string name;
        
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("message-list").children("message");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "type") type = ait->as_int();
                else if (name == "content") content = ait->as_string();
                else if (name == "friend-profile-image-url") profileUrl = ait->as_string();
                else if (name == "reward-count") rewardCount = ait->as_int();
                else if (name == "notice-url") noticeUrl = "";
            }
            msgData.push_back( new Msg(id, type, rewardCount, content, profileUrl, noticeUrl) );
        }
        
        // scroll을 생성 후 데이터 보여주기
        MakeScroll();
        
        // Notification : Ranking 화면에 데이터 갱신
        myInfo->SetMsgCnt((int)msgData.size());
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    }
    else
    {
        CCLog("FAILED : code = %d", code);
    }
}

void Message::XmlParseMsgReceiveOne(char* data, int size)
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
            case 1: break;
                //Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_NOTICE, BTN_1, data); break;
            case 2:
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_STARCANDY, BTN_1, data); break;
            case 3:
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_TOPAZ, BTN_1, data); break;
            case 4:
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_OK_POTION, BTN_1, data); break;
        }
    }
    else if (code == 10)
    {
        // 없는 메시지 (삭제하자)
        std::vector<int> nullData;
        Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_EMPTY, BTN_1, nullData);
    }
}


bool Message::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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


void Message::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
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
                
                httpStatus = 1;
                // 메시지에 대한 처리 서버 통신
                char temp[50];
                std::string url = "http://14.63.225.203/cogma/game/receive_message_one.php?";
                sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                url += temp;
                sprintf(temp, "message_id=%d", msgData[httpMsgIdx]->GetId());
                url += temp;
                CCLog("url : %s", url.c_str());
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(url.c_str());
                req->setRequestType(CCHttpRequest::kHttpPost);
                req->setResponseCallback(this, httpresponse_selector(Message::onHttpRequestCompleted));
                CCHttpClient::getInstance()->send(req);
                req->release();
                break;
            }
        }
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                if (msgData.size() == 0)
                    continue;
                
                sound->playClick();
                
                httpMsgIdx = -1; // 메시지 리스트 갱신 방지
                
                std::vector<int> nullData;
                Common::ShowPopup(this, "Message", "NoImage", false, MESSAGE_ALL_TRY, BTN_2, nullData);
                break;
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
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    ReleaseAll();
    
    this->removeFromParentAndCleanup(true);
}

void Message::EndSceneCallback()
{
}

void Message::ReleaseAll()
{
    pBlack->autorelease();
    //pBlack->release();
    //pBlack->removeFromParentAndCleanup(true);
    //CCLog("pBlack : %d", pBlack->retainCount());
    
    /*
    // spriteClass release
    int maxPriority = -1;
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        maxPriority = std::max(maxPriority, spriteClass->spriteObj[i]->priority);
    for (int priority = maxPriority ; priority >= 0 ; priority--)
    {
        for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        {
            if (spriteClass->spriteObj[i]->priority == priority)
            {
                //CCLog("%d : %d", i, spriteClass->spriteObj[i]->type);
                if (spriteClass->spriteObj[i]->type == 0)
                {
                    //spriteClass->spriteObj[i]->sprite->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->sprite->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->sprite->autorelease();
                    //spriteClass->spriteObj[i]->sprite = NULL;
                }
                else if (spriteClass->spriteObj[i]->type == 1)
                {
                    //spriteClass->spriteObj[i]->sprite9->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->sprite9->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->sprite9->autorelease();
                    //spriteClass->spriteObj[i]->sprite9 = NULL;
                }
                else
                {
                    //spriteClass->spriteObj[i]->label->removeAllChildrenWithCleanup(true);
                    //spriteClass->spriteObj[i]->label->removeFromParentAndCleanup(true);
                    //spriteClass->spriteObj[i]->label->autorelease();
                    //spriteClass->spriteObj[i]->label = NULL;
                }
            }
        }
    }
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->type == 0)
            CCLog("sprite : %d", spriteClass->spriteObj[i]->sprite->retainCount());
        else if (spriteClass->spriteObj[i]->type == 1)
            CCLog("sprite : %d", spriteClass->spriteObj[i]->sprite9->retainCount());
        else
            CCLog("sprite : %d", spriteClass->spriteObj[i]->label->retainCount());
    }
    */
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        delete spriteClass->spriteObj[i];
    spriteClass->spriteObj.clear();
    delete spriteClass;
    
    // layers
    for (int i = 0 ; i < layer.size() ; i++)
    {
        layer[i]->removeAllChildren();
        //layer[i]->removeFromParentAndCleanup(true);
        //CCLog("layer %d : %d", i, layer[i]->retainCount());
        //layer[i]->release();
        //layer[i]->autorelease();
    }
    layer.clear();
    
    // scrollview
    scrollContainer->removeAllChildren();
    //scrollContainer->removeFromParentAndCleanup(true);
    //scrollContainer->autorelease();
    scrollView->removeAllChildren();
    //scrollView->removeFromParentAndCleanup(true);
    //scrollView->autorelease();

    
    this->removeAllChildren();
}


