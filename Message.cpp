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
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    // init sprites & scrolls
    InitSprites();
    //MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    msgData.clear();
    // 네트워크로 메시지들을 받아온다.
    char temp[50];
    std::string url = "http://14.63.225.203/cogma/game/get_messagelist.php?";
    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
    url += temp;
    //CCLog("url = %s", url.c_str());
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
}

void Message::MakeScroll()
{
    int objSize = spriteClass->spriteObj.size();
    //msg_type.push_back(0); // potion receive
    //msg_type.push_back(1); // starcandy receive
    //msg_type.push_back(2); // 뽑기

    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));

    int numOfList = msgData.size();
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
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(44, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
        
        // content (fontList[2] = 나눔고딕볼드)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(msgData[i]->GetContent(), fontList[0], 36, ccp(0, 0), ccp(194, 45), ccc3(78,47,8), CCSize(400, 105), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", itemLayer, 3, 0) );
        
        // button (type마다 버튼이 다르다)
        switch (msgData[i]->GetType())
        {
            case 1: // 공지 (보기)
                break;
            case 2: // 별사탕 (별사탕아이콘 + 받기)
                break;
            case 3: // 토파즈 (토파즈아이콘 + 받기)
                break;
            case 4: // 포션 (포션아이콘 + 받기)
                sprintf(spriteName, "button/btn_green_mini.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(634, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName, ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
        }
        
        // button
        
        /*
        // button 안에 image1개, letter1개 넣는다.
        switch (msg_type[i])
        {
            case 0: // 포션
                sprintf(spriteName, "button/btn_receive_potion.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(667, 55), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 1: // 별사탕
                sprintf(spriteName, "button/btn_receive_starcandy.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(664, 63), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                sprintf(spriteName, "letter/letter_receive.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(725, 62), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
            case 2: // 뽑기
                sprintf(spriteName, "letter/letter_vote.png%d", i);
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
                            ccp(0, 0), ccp(676, 60), CCSize(0, 0), "", "Layer", itemLayer, 3, 0) );
                break;
        }
        */
        
        // dotted line
        sprintf(spriteName, "background/bg_dotted_line.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, spriteName,
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
    for (int i = objSize ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
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
    
    XmlParseMsg(dumpData, buffer->size());
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
    }
    else
    {
        CCLog("FAILED : code = %d", code);
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


