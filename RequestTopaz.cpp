#include "RequestTopaz.h"

CCScene* RequestTopaz::scene()
{
    CCScene* pScene = CCScene::create();
    RequestTopaz* pLayer = RequestTopaz::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void RequestTopaz::onEnter()
{
    CCLog("RequestTopaz : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(RequestTopaz::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void RequestTopaz::SceneCallback()
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}
void RequestTopaz::onExit()
{
    CCLog("RequestTopaz : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void RequestTopaz::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool RequestTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("RequestTopaz", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("RequestTopaz : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(RequestTopaz::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    

    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, 0));
    tLayer->setScale(0);
    this->addChild(tLayer, 1);
    
    scrollView = CCScrollView::create();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904+243+45-100 + (487-630)));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 630));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 3);
    
    this->schedule(schedule_selector(RequestTopaz::ProfileTimer), 1.0f);
    
    spriteClass = new SpriteClass();
    spriteClassScroll = new SpriteClass();
    InitSprites();
    MakeScroll();
    
    return true;
}

void RequestTopaz::Notification(CCObject* obj)
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
        CCLog("RequestTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // scroll 갱신
        RenewScroll();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("RequestTopaz 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        scrollView->setTouchEnabled(false);
    }
    /*
    else if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollView->setTouchEnabled(true);
        CCLog("RequestTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    */
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}


void RequestTopaz::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_pester.png",
                    ccp(0, 0), ccp(279, 1389+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_present_box.png",
                    ccp(0, 0), ccp(179, 1389+243+5), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "Layer", tLayer, 1) );
    
    // 설명 창 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",ccp(0, 0), ccp(110, 478), CCSize(857, 132), "", "Layer", tLayer, 2) );
    CCPoint p = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("요청한 친구에게는 24시간 후 다시 요청 가능합니다.", fontList[0], 36, ccp(0.5, 0.5), p, ccc3(78,47,8), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void RequestTopaz::MakeScroll()
{
    scrollContainer = CCLayer::create();
 
    int numOfList = friendList.size();
    int height = 0;
    char fname[50], fname2[50];
    for (int i = numOfList-1 ; i >= 0 ; i--)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        // 요청시간이 아직 남아있는 친구는 리스트에 보이지 않게 한다.
        if (friendList[i]->GetRemainRequestTopazTime() > 0)
            continue;
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, height*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        height++;
        
        
        // 프로필 이미지
        sprintf(fname, "background/bg_profile.png%d", i);
        ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
        if (friendList[i]->GetImageUrl() != "" && psp->IsLoadingDone())
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(35+5, 35+11), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 0.95f) );
            sprintf(fname, "background/bg_profile.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(35, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
        else
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(35, 35), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 1.0f, -888*(numOfList-i)) ); // tag = -888 * (i+1)
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(35, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255, -777*(numOfList-i)) ); // tag = -777 * (i+1)
        }
 
        // name (text)
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(196, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(fname, "button/btn_yellow_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(635, 34), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255, i*100) ); // 버튼에 친구 kakao id를 tag로 둔다.
        sprintf(fname2, "letter/letter_request2.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0), ccp(spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3) );
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
    
    // container 생성
    scrollContainer->setContentSize(CCSizeMake(862, height*166));
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, scrollView->minContainerOffset().y), false);
}

void RequestTopaz::RenewScroll()
{
    spriteClassScroll->RemoveAllObjects();
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    
    MakeScroll();
}

bool RequestTopaz::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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


void RequestTopaz::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void RequestTopaz::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
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

                std::string friendkakaoId = friendList[ spriteClassScroll->spriteObj[i]->sprite->getTag() / 100 ]->GetKakaoId();
                char temp[255];
                std::string param = "";
                sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
                param += temp;
                sprintf(temp, "friend_kakao_id=%s", friendkakaoId.c_str());
                param += temp;
                CCLog("param = %s", param.c_str());

                Network::HttpPost(param, URL_REQUEST_TOPAZ, this, httpresponse_selector(RequestTopaz::onHttpRequestCompleted), friendkakaoId.c_str());
                
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


void RequestTopaz::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void RequestTopaz::scrollViewDidZoom(CCScrollView* view)
{
}


void RequestTopaz::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->unschedule(schedule_selector(RequestTopaz::ProfileTimer));
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    spriteClass = NULL;
    spriteClassScroll = NULL;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}


void RequestTopaz::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    std::string friendKakaoId = res->getHttpRequest()->getTag();
    XmlParseResult(&xmlDoc, friendKakaoId);
}

void RequestTopaz::XmlParseResult(xml_document *xmlDoc, std::string friendKakaoId)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 친구가 아님
        {
            for (int i = 0 ; i < friendList.size() ; i++)
            {
                if (friendList[i]->GetKakaoId() == friendKakaoId)
                {
                    friendList[i]->SetPotionMsgStatus(0);
                    friendList[i]->SetRemainPotionTime(0);
                    friendList[i]->SetRemainRequestTopazTime(0);
                    friendList[i]->SetPotionSprite();
                }
            }
            Common::ShowPopup(this, "RequestTopaz", "NoImage", false, REQUEST_TOPAZ_NO_FRIEND, BTN_1, nullData);
        }
        else if (code == 11) // 토파즈 요청 보내고 아직 24시간 지나지 않음
            Common::ShowPopup(this, "RequestTopaz", "NoImage", false, REQUEST_TOPAZ_EARLY, BTN_1, nullData);
        else
            Common::ShowPopup(this, "RequestTopaz", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // remain request potion time 갱신 (60*60*24)
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == friendKakaoId)
                friendList[i]->SetRemainRequestTopazTime(60*60*24);
        }
        
        std::vector<int> nullData;
        Common::ShowPopup(this, "RequestTopaz", "NoImage", false, REQUEST_TOPAZ_OK, BTN_1, nullData);
    }
}



void RequestTopaz::ProfileTimer(float f)
{
    // 프로필 사진 왼쪽 위 지점과 스크롤뷰 위치를 비교한다.
    // 음수가 되면, 아래에 있던 프로필이 스크롤뷰에 보이기 시작했다는 의미 -> 프로필 로딩 시작.
    CCPoint p;
    float h;
    int numOfList = friendList.size();
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
        if (psp->IsLoadingStarted() || psp->IsLoadingDone())
            continue;
        
        if (spriteClassScroll == NULL)
            return;
        p = ((CCSprite*)spriteClassScroll->FindSpriteByTag(-888*(numOfList-i)))->convertToNodeSpace(scrollView->getPosition());
        h = friendList[i]->GetProfile()->getContentSize().height;
        
        if (p.y - h < 0)
        {
            psp->SetLoadingStarted(true);
            
            char tag[6];
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(psp->GetProfileUrl().c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(RequestTopaz::onHttpRequestCompletedNoEncrypt));
            sprintf(tag, "%d", i);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
    }
}
void RequestTopaz::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[110*110*2];
    
    // 프로필 사진 받아오기 실패
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
    
    // make texture2D
    CCImage* img = new CCImage;
    img->initWithImageData(dumpData, (int)buffer->size());
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
    
    // set CCSprite (profile 모음 리스트에 갱신)
    int numOfList = friendList.size();
    int index = atoi(res->getHttpRequest()->getTag());
    
    ProfileSprite* psp = ProfileSprite::GetObj(friendList[index]->GetImageUrl());
    psp->SetSprite(texture);
    psp->SetLoadingDone(true);
    
    // 화면에 보이는 스프라이트 교체
    if (spriteClassScroll == NULL)
        return;
    spriteClassScroll->ChangeSprite(-888*(numOfList-index), psp->GetProfile());
    ((CCSprite*)spriteClassScroll->FindSpriteByTag(-777*(numOfList-index)))->setOpacity(255);
}


