#include "RequestPotion.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

CCScene* RequestPotion::scene()
{
    CCScene* pScene = CCScene::create();
    RequestPotion* pLayer = RequestPotion::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void RequestPotion::onEnter()
{
    CCLog("RequestPotion : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void RequestPotion::onExit()
{
    CCLog("RequestPotion : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void RequestPotion::keyBackClicked()
{
    EndScene();
}


bool RequestPotion::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("RequestPotion", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("RequestPotion : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RequestPotion::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    

    winSize = CCDirector::sharedDirector()->getWinSize();
    
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904+243+45-100 + (487-630)));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 630));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 3);

    spriteClass = new SpriteClass();
    spriteClassScroll = new SpriteClass();
    InitSprites();
    MakeScroll();
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void RequestPotion::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("RequestPotion : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // scroll 갱신
        RenewScroll();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("RequestPotion 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("RequestPotion : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}

void RequestPotion::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_pester.png",
                    ccp(0, 0), ccp(279, 1389+243), CCSize(0, 0), "", "RequestPotion", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion_big.png",
                    ccp(0, 0), ccp(192, 1389+243), CCSize(0, 0), "", "RequestTopaz", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "RequestPotion", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "RequestPotion", this, 1) );
    
    // 설명 창 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",ccp(0, 0), ccp(110, 478), CCSize(857, 132), "", "RequestPotion", this, 2) );
    CCPoint p = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("요청한 친구에게는 24시간 후 다시 요청 가능합니다.", fontList[0], 36, ccp(0.5, 0.5), p, ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void RequestPotion::MakeScroll()
{
    scrollContainer = CCLayer::create();

    int numOfList = friendList.size();
    int height = 0;
    char fname[50], fname2[50];

    for (int i = numOfList-1 ; i >= 0 ; i--)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        // 수신거부한 친구 or 요청시간이 아직 남아있는 친구도 모두 리스트에 보이지 않게 한다.
        if (friendList[i]->GetPotionMsgStatus() == 0 || friendList[i]->GetRemainRequestPotionTime() > 0)
            continue;
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, height*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        height++;
        
        // 프로필 이미지
        CCSprite* profile = ProfileSprite::GetProfile(friendList[i]->GetImageUrl());
        if (friendList[i]->GetImageUrl() != "")
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(35+5, 35+11), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 0.85f) );
            sprintf(fname, "background/bg_profile.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(35, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
        else
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(35, 35), CCSize(0,0), "", "Layer", itemLayer, 3) );
        }
        
        // name (text)
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(196-10, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(fname, "button/btn_yellow_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(635-20, 34), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255, friendList[i]->GetKakaoId()) ); // 버튼에 친구 kakao id를 tag로 둔다.
        sprintf(fname2, "letter/letter_request2.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0), ccp(spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3, 1) );
        // dotted line
        //if (i < numOfList-1)
        //{
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        //}
    }
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
    
    // container 생성
    scrollContainer->setContentSize(CCSizeMake(862, height*166));
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, scrollView->minContainerOffset().y), false);
}

void RequestPotion::RenewScroll()
{
    spriteClassScroll->RemoveAllObjects();
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    
    MakeScroll();
}

bool RequestPotion::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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


void RequestPotion::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void RequestPotion::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
    {
        if (spriteClassScroll->spriteObj[i]->name.substr(0, 26) == "button/btn_yellow_mini.png")
        {
            CCPoint p = spriteClassScroll->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClassScroll->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                
                // Loading 화면으로 MESSAGE request 넘기기
                Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
                
                int friendkakaoId = spriteClassScroll->spriteObj[i]->sprite->getTag();

                char temp[255];
                std::string param = "";
                sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                param += temp;
                sprintf(temp, "friend_kakao_id=%d", friendkakaoId);
                param += temp;
                
                // tag
                sprintf(temp, "%d", friendkakaoId);
                
                Network::HttpPost(param, URL_REQUEST_POTION, this, httpresponse_selector(RequestPotion::onHttpRequestCompleted), temp);

                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void RequestPotion::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void RequestPotion::scrollViewDidZoom(CCScrollView* view)
{
}


void RequestPotion::EndScene()
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
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void RequestPotion::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[BUFFER_SIZE];
    int bufferSize = Network::GetHttpResponseData(res, dumpData);
    
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    /*
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
    */
    
    int friendKakaoId = atoi(res->getHttpRequest()->getTag());
    XmlParseResult(dumpData, bufferSize, friendKakaoId);
}

void RequestPotion::XmlParseResult(char* data, int size, int friendKakaoId)
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
    std::vector<int> nullData;
    
    if (code == 0)
    {
        // remain request potion time 갱신 (60*60*24)
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == friendKakaoId)
                friendList[i]->SetRemainRequestPotionTime(60*60*24);
        }
        Common::ShowPopup(this, "RequestPotion", "NoImage", false, REQUEST_POTION_OK, BTN_1, nullData);
    }
    else if (code == 10)
    {
        // 수신거부상태 갱신
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == friendKakaoId)
            {
                friendList[i]->SetPotionMsgStatus(0);
                friendList[i]->SetPotionSprite();
            }
        }
        Common::ShowPopup(this, "RequestPotion", "NoImage", false, REQUEST_POTION_REJECT, BTN_1, nullData);
    }
    else if (code == 12)
    {
        // 포션 받고 아지 24시간 지나지 않음
        Common::ShowPopup(this, "RequestPotion", "NoImage", false, REQUEST_POTION_EARLY, BTN_1, nullData);
    }
    else
    {
        Common::ShowPopup(this, "RequestPotion", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
        CCLog("RequestPotion : failed code = %d", code);
    }
}



