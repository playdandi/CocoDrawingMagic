#include "CocoRoomTodayCandy.h"


CCScene* CocoRoomTodayCandy::scene()
{
    CCScene* pScene = CCScene::create();
    CocoRoomTodayCandy* pLayer = CocoRoomTodayCandy::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoomTodayCandy::onEnter()
{
    //CCLog("CocoRoomTodayCandy :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(CocoRoomTodayCandy::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void CocoRoomTodayCandy::SceneCallback()
{
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
}
void CocoRoomTodayCandy::onExit()
{
    //CCLog("CocoRoomTodayCandy :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CocoRoomTodayCandy::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    //sound->playClick();
    EndScene();
}


bool CocoRoomTodayCandy::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrollViewTouched = true;
    isScrolling = true;
    
    // make depth tree
    Depth::AddCurDepth("CocoRoomTodayCandy", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("CocoRoomTodayCandy : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoomTodayCandy::Notification), Depth::GetCurName(), NULL);
    
    // notification
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
    //scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+40));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    tLayer->addChild(scrollView, 3);
    
    this->schedule(schedule_selector(CocoRoomTodayCandy::ProfileTimer), 1.0f);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    spriteClassList = new SpriteClass();
    RefreshProfileList();
    
    return true;
}

void CocoRoomTodayCandy::Notification(CCObject* obj)
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
        //CCLog("CocoRoomTodayCandy : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("CocoRoomTodayCandy : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}

void CocoRoomTodayCandy::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();

    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                    ccp(0, 0), ccp(49, 458), CCSize(982, 1073), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_todaystarcandy_groupfriend_select.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    // bottom background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(929, 177), "", "Layer", tLayer, 1) );
    
    
    // bottom 5 profile 중에 내 프로필
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, MyInfo::GetProfile(), ccp(0, 0), ccp(100+5, 254+11), CCSize(0,0), "", "Layer", tLayer, 5, 0, 255, 0.95f) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png1", ccp(0, 0), ccp(100, 254), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // bottom button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                    ccp(0, 0), ccp(750, 262), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 1, 1) );
}

void CocoRoomTodayCandy::MakeScroll()
{
    int numOfList = friendList.size();
    
    // init select array
    selected.clear();
    for (int i = 0 ; i < numOfList ; i++)
        selected.push_back(false);
    
    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setPosition(ccp(77, 492+904+243));
    //scrollContainer->setContentSize(CCSizeMake(862, (numOfList-1)*166));
    
    int height = 0;
    char fname[50], fname2[50];
    for (int i = numOfList-1 ; i >= 0 ; i--)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        //itemLayer->setPosition(ccp(34, (numOfList-1-height)*166));
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
        sprintf(fname, "button/btn_blue_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(635, 34+3), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255) );
        sprintf(fname2, "letter/letter_select.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3, 1, 255) );
        
        /*if (friendList[i]->GetPotionMsgStatus() == 0)
        {
            ((CCSprite*)spriteClass->FindSpriteByName(fname))->setColor(ccc3(150,150,150));
            ((CCSprite*)spriteClass->FindSpriteByName(fname2))->setColor(ccc3(150,150,150));
        }*/
        
        sprintf(fname, "button/btn_skill_master.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(635, 34+6), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 0) );
        sprintf(fname2, "letter/letter_select_brown.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 27), CCSize(0, 0), fname, "0", NULL, 3, 1, 0) );
        
        SetSelectMode(i);
        
        // dotted line
        //if (i < numOfList-1)
        //{
            sprintf(fname, "background/bg_dotted_line.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        //}
    }
    
    // container 생성 + offset
    scrollContainer->setContentSize(CCSizeMake(862, height*166));
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, scrollView->minContainerOffset().y), false);
}

void CocoRoomTodayCandy::SetSelectMode(int idx)
{
    char name[50];
    // 오.별 리스트에 idx번째 친구가 들어있다면, '선택됨'으로 체크해 주어야 한다.
    for (int i = 0 ; i < todayCandyKakaoId.size() ; i++)
    {
        if (todayCandyKakaoId[i] == friendList[idx]->GetKakaoId())
        {
            sprintf(name, "button/btn_blue_mini.png%d", idx);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
            sprintf(name, "letter/letter_select.png%d", idx);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
            sprintf(name, "button/btn_skill_master.png%d", idx);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
            sprintf(name, "letter/letter_select_brown.png%d", idx);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
            selected[idx] = true;
            break;
        }
    }
}

void CocoRoomTodayCandy::RefreshTodayCandyList(int idx)
{
    char name[50];
    
    selected[idx] = !selected[idx];
    
    if (selected[idx])
    {
        sprintf(name, "button/btn_blue_mini.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        sprintf(name, "letter/letter_select.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        sprintf(name, "button/btn_skill_master.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        sprintf(name, "letter/letter_select_brown.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
    }
    else
    {
        sprintf(name, "button/btn_blue_mini.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        sprintf(name, "letter/letter_select.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        sprintf(name, "button/btn_skill_master.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        sprintf(name, "letter/letter_select_brown.png%d", idx);
        ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
    }
    
    RefreshProfileList();
}

void CocoRoomTodayCandy::RefreshProfileList()
{
    spriteClassList->RemoveAllObjects();

    char name[50];
    int cnt = 0;
    // 화면 아래에 있는 list 갱신
    for (int i = 0 ; i < selected.size() ; i++)
    {
        if (selected[i])
        {
            ProfileSprite* psp = ProfileSprite::GetObj(friendList[i]->GetImageUrl());
            if (friendList[i]->GetImageUrl() != "" && psp->IsLoadingDone())
            {
                spriteClassList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(230+cnt*130+5, 254+11), CCSize(0,0), "", "Layer", tLayer, 5, 0, 255, 0.95f) );
                sprintf(name, "background/bg_profile.png%d", cnt);
                spriteClassList->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(230+cnt*130, 254), CCSize(0, 0), "", "Layer", tLayer, 5) );
            }
            else
            {
                spriteClassList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(230+cnt*130, 254), CCSize(0,0), "", "Layer", tLayer, 5) );
            }

            cnt++;
        }
    }
    
    for (int i = 0 ; i < spriteClassList->spriteObj.size() ; i++)
        spriteClassList->AddChild(i);
}


bool CocoRoomTodayCandy::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                EndScene();
                break;
            }
        }
        else if(spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                // todayCandyKakaoId 리스트 갱신
                todayCandyKakaoId.clear();
                todayCandyKakaoId.push_back(myInfo->GetKakaoId()); // 본인 kakao id부터 넣자.
                
                for (int i = 0 ; i < selected.size() ; i++)
                {
                    if (selected[i])
                        todayCandyKakaoId.push_back( friendList[i]->GetKakaoId() );
                }
                for (int i = todayCandyKakaoId.size(); i < 5; i++)
                    todayCandyKakaoId.push_back("");
                
                // User Default 값 갱신
                char name[15];
                for (int i = 1 ; i < todayCandyKakaoId.size() ; i++)
                {
                    //CCLog("%s", todayCandyKakaoId[i].c_str());
                    sprintf(name, "todayCandy_%d", i);
                    CCUserDefault::sharedUserDefault()->setStringForKey(name, todayCandyKakaoId[i].c_str());
                }
                
                EndScene();
                break;
            }
        }
    }
    
    return true;
}


void CocoRoomTodayCandy::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void CocoRoomTodayCandy::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
    
    CCPoint point = pTouch->getLocation();
    
    char fname[50];
    for (int i = 0 ; i < friendList.size(); i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            continue;
        //if (friendList[i]->GetPotionMsgStatus() == 0)
        //    continue;
        
        sprintf(fname, "button/btn_blue_mini.png%d", i); // 이름은 이걸로 하는데, 사실 선택됨/선택안됨 sprite 모두 같은 위치라 그냥 이거 하나로 쓰자.
        CCSprite* temp = (CCSprite*)spriteClass->FindSpriteByName(fname);
        CCPoint p = temp->convertToNodeSpace(point);
        
        CCSize size = spriteClass->GetContentSizeByName(fname);
        if (isScrollViewTouched && !isScrolling &&
            (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
        {
            // 이미 4명 모두 선택되어 있다면 클릭을 못하게 한다.
            int cnt = 0;
            for (int j = 0 ; j < selected.size() ; j++)
            {
                if (selected[j])
                    cnt++;
            }
            
            if (selected[i] || (!selected[i] && cnt < 4))
            {
                sound->playClick();
                RefreshTodayCandyList(i);
                break;
            }
        }
    }
}

void CocoRoomTodayCandy::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void CocoRoomTodayCandy::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoomTodayCandy::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->unschedule(schedule_selector(CocoRoomTodayCandy::ProfileTimer));
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassList->RemoveAllObjects();
    delete spriteClassList;
    spriteClass = NULL;
    spriteClassList = NULL;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    selected.clear();
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void CocoRoomTodayCandy::ProfileTimer(float f)
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
            req->setResponseCallback(this, httpresponse_selector(CocoRoomTodayCandy::onHttpRequestCompletedNoEncrypt));
            sprintf(tag, "%d", i);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
    }
}
void CocoRoomTodayCandy::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
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
