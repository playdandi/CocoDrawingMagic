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
    CCLog("CocoRoomTodayCandy :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void CocoRoomTodayCandy::onExit()
{
    CCLog("CocoRoomTodayCandy :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CocoRoomTodayCandy::keyBackClicked()
{
    EndScene();
}


bool CocoRoomTodayCandy::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("CocoRoomTodayCandy", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("CocoRoomTodayCandy : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoomTodayCandy::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // scrollView 생성
    scrollView = CCScrollView::create();
    //scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+40));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollView, 3);
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    spriteClassList = new SpriteClass();
    RefreshProfileList();
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
    
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
        scrollView->setTouchEnabled(true);
        CCLog("CocoRoomTodayCandy : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("CocoRoomTodayCandy : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
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
                    ccp(0, 0), ccp(49, 458), CCSize(982, 1073), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                    ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_todaystarcandy_groupfriend_select.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 1) );
    
    // bottom background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                    ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "CocoRoomTodayCandy", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                    ccp(0, 0), ccp(77, 228), CCSize(929, 177), "", "CocoRoomTodayCandy", this, 1) );
    
    
    // bottom 5 profile 중에 내 프로필
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, MyInfo::GetProfile(), ccp(0, 0), ccp(100+5, 254+11), CCSize(0,0), "", "CocoRoomTodayCandy", this, 5, 0, 255, 0.85f) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png1", ccp(0, 0), ccp(100, 254), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 5) );
    
    // bottom button
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                    ccp(0, 0), ccp(750, 262), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 1) );
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
        CCSprite* profile = ProfileSprite::GetProfile(friendList[i]->GetImageUrl());
        if (friendList[i]->GetImageUrl() != "")
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(50+5, 46+11), CCSize(0,0), "", "Layer", itemLayer, 3, 0, 255, 0.85f) );
            sprintf(fname, "background/bg_profile.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(50, 46), CCSize(0, 0), "", "Layer", itemLayer, 3) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(50, 46), CCSize(0,0), "", "Layer", itemLayer, 3) );
        }
    
        // name (text)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[i]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(196, 71), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // button
        sprintf(fname, "button/btn_blue_mini.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(635, 34+3), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 255) );
        sprintf(fname2, "letter/letter_select.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), fname, "0", NULL, 3, 1, 255) );
        
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
    //scrollView->setContainer(scrollContainer);
    //scrollView->setContentSize(scrollContainer->getContentSize());
    //scrollView->setContentOffset(ccp(0, 904-80-((numOfList-1)*166)), false);
    
    //scrollContainer->setContentSize(CCSizeMake(862, (numOfList-1)*166));
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
            CCSprite* profile = ProfileSprite::GetProfile(friendList[i]->GetImageUrl());
            if (friendList[i]->GetImageUrl() != "")
            {
                spriteClassList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(230+cnt*130+5, 254+11), CCSize(0,0), "", "CocoRoomTodayCandy", this, 5, 0, 255, 0.85f) );
                sprintf(name, "background/bg_profile.png%d", cnt);
                spriteClassList->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(230+cnt*130, 254), CCSize(0, 0), "", "CocoRoomTodayCandy", this, 5) );
            }
            else
            {
                spriteClassList->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(230+cnt*130, 254), CCSize(0,0), "", "CocoRoomTodayCandy", this, 5) );
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
                    todayCandyKakaoId.push_back(-1);
                
                // User Default 값 갱신
                char name[15];
                for (int i = 1 ; i < todayCandyKakaoId.size() ; i++)
                {
                    //CCLog("%d", todayCandyKakaoId[i]);
                    sprintf(name, "todayCandy_%d", i);
                    CCUserDefault::sharedUserDefault()->setIntegerForKey(name, todayCandyKakaoId[i]);
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
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassList->RemoveAllObjects();
    delete spriteClassList;
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    
    pBlack->removeFromParentAndCleanup(true);
    selected.clear();
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoomTodayCandy::EndSceneCallback()
{
}

