#include "SendTopaz.h"

static int priceTopazIdx;
static std::string friendKakaoId;
static int topazId;

CCScene* SendTopaz::scene(int idx)
{
    priceTopazIdx = idx;
    friendKakaoId = "";
    topazId = -1;
    
    CCScene* pScene = CCScene::create();
    SendTopaz* pLayer = SendTopaz::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void SendTopaz::onEnter()
{
    //CCLog("SendTopaz : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(SendTopaz::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void SendTopaz::SceneCallback()
{
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}
void SendTopaz::onExit()
{
    //CCLog("SendTopaz : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void SendTopaz::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool SendTopaz::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrolling = true;
    isScrollViewTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("SendTopaz", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("SendTopaz : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SendTopaz::Notification), Depth::GetCurName(), NULL);
    
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
    scrollView->setViewSize(CCSizeMake(929, 904+243+45-100));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492-45+40));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 3);
    
    this->schedule(schedule_selector(SendTopaz::ProfileTimer), 1.0f);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    return true;
}

void SendTopaz::Notification(CCObject* obj)
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
        //CCLog("SendTopaz : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("SendTopaz 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        // '토파즈 선물하기'에서 선물할 때 (팝업창에서 돌아왔음)
        EndScene();
        
        char p[40];
        sprintf(p, "%s/%d", friendKakaoId.c_str(), topazId-1);
        CCString* param = CCString::create(p);
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}

void SendTopaz::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_green.png",
                    ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
                    ccp(0, 0), ccp(875, 1391+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_present.png",
                    ccp(0, 0), ccp(359, 1389+243), CCSize(0, 0), "", "Layer", tLayer, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 458-45), CCSize(982, 954+243+45), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(75, 492-45), CCSize(929, 904+243+45), "", "Layer", tLayer, 1) );
}

void SendTopaz::MakeScroll()
{
    int numOfList = friendList.size();
    
    scrollContainer = CCLayer::create();
    
    int height = 0;
    char fname[50], fname2[50];
    for (int i = numOfList-1 ; i >= 0 ; i--)
    {
        // 본인은 리스트 제외
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        // 수신 차단한 친구도 제외
        //if (friendList[i]->IsMessageBlocked())
        //    continue;
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, height*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClass->layers.push_back(itemLayer);
        height++;
        
        
        // 프로필 이미지
        sprintf(fname, "background/bg_profile.png%d", i);
        ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
        if (friendList[i]->GetImageUrl() != "" && psp->IsLoadingDone())
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(35+5, 35+11), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 0.95f) );
            sprintf(fname, "background/bg_profile.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(35, 35), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(35, 35), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 1.0f, -888*(numOfList-i)) ); // tag = -888 * (i+1)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(35, 35), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255, -777*(numOfList-i)) ); // tag = -777 * (i+1)
        }
        
        // name (text)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(196, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(fname, "button/btn_yellow_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(635, 34), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        sprintf(fname2, "button/btn_present.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2,
                        ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3, 1) );
        // dotted line
        if (i < numOfList-1)
        {
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname,
                        ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
    }
    
    // container 설정
    scrollContainer->setContentSize(CCSizeMake(862, height*166));
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    //scrollView->setContentOffset(ccp(0, 904+243+45-100-(height*166)), false);
    scrollView->setContentOffset(ccp(0, scrollView->minContainerOffset().y), false);
}


bool SendTopaz::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    return true;
}


void SendTopaz::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void SendTopaz::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
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
        else if (spriteClass->spriteObj[i]->name.substr(0, 26) == "button/btn_yellow_mini.png")
        {
            CCPoint p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            CCSize size = spriteClass->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                
                int number = atoi(spriteClass->spriteObj[i]->name.substr(26).c_str());
                std::vector<int> data;
                data.push_back(number);
                data.push_back(priceTopazIdx);
                
                friendKakaoId = friendList[number]->GetKakaoId();
                topazId = priceTopaz[priceTopazIdx]->GetId();
                
                Common::ShowPopup(this, "SendTopaz", "NoImage", false, SEND_TOPAZ_TRY, BTN_2, data);
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}

void SendTopaz::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void SendTopaz::scrollViewDidZoom(CCScrollView* view)
{
}


void SendTopaz::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->unschedule(schedule_selector(SendTopaz::ProfileTimer));
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClass = NULL;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void SendTopaz::ProfileTimer(float f)
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
        
        if (spriteClass == NULL)
            return;
        p = ((CCSprite*)spriteClass->FindSpriteByTag(-888*(numOfList-i)))->convertToNodeSpace(scrollView->getPosition());
        h = friendList[i]->GetProfile()->getContentSize().height;
        
        if (p.y - h < 0)
        {
            psp->SetLoadingStarted(true);
            
            char tag[6];
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(psp->GetProfileUrl().c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(SendTopaz::onHttpRequestCompletedNoEncrypt));
            sprintf(tag, "%d", i);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
    }
}
void SendTopaz::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[110*110*2];
    
    // 프로필 사진 받아오기 실패
    if (!res || !res->isSucceed())
    {
        //CCLog("res failed. error buffer: %s", res->getErrorBuffer());
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
    if (spriteClass == NULL)
        return;
    spriteClass->ChangeSprite(-888*(numOfList-index), psp->GetProfile());
    ((CCSprite*)spriteClass->FindSpriteByTag(-777*(numOfList-index)))->setOpacity(255);
}