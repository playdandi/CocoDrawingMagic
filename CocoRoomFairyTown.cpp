#include "CocoRoomFairyTown.h"

CocoRoomFairyTown::~CocoRoomFairyTown()
{
    ////CCLog("CocoFairyTown 소멸자 실행");
}

CCScene* CocoRoomFairyTown::scene()
{    
    CCScene* pScene = CCScene::create();
    CocoRoomFairyTown* pLayer = CocoRoomFairyTown::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoomFairyTown::onEnter()
{
    //CCLog("CocoRoomFairyTown :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    isTouched = false;
    isScrolling = false;
    scrollViewTouch = false;
    
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
    
    // 네트워크로 메시지들을 받아온다.
    char temp[50];
    std::string params = "";
    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
    params += temp;
    
    //if (binaryVersion_current < 2000) // 1000번대 (기존 유저)
    //    Network::HttpPost(params, URL_PURCHASE_FAIRY_LIST, this, httpresponse_selector(CocoRoomFairyTown::onHttpRequestCompleted));
    //else // 2000번대
    Network::HttpPost(params, URL_PURCHASE_FAIRY_LIST_NEW, this, httpresponse_selector(CocoRoomFairyTown::onHttpRequestCompleted));
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(CocoRoomFairyTown::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void CocoRoomFairyTown::SceneCallback()
{
    
}
void CocoRoomFairyTown::onExit()
{
    //CCLog("CocoRoomFairyTown :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void CocoRoomFairyTown::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool CocoRoomFairyTown::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = true;
    isScrolling = true;
    scrollViewTouch = true;
    
    // make depth tree
    Depth::AddCurDepth("CocoRoomFairyTown", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("CocoRoomFairyTown : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(CocoRoomFairyTown::Notification), Depth::GetCurName(), NULL);
    
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
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-40));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setDelegate(this);
    scrollView->setTouchPriority(Depth::GetCurPriority()); // priority
    tLayer->addChild(scrollView, 3);
    
    InitSprites();

    fairyData.clear();

    return true;
}

void CocoRoomFairyTown::Notification(CCObject* obj)
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
        //CCLog("CocoRoomFairyTown : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("CocoRoomFairyTown : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 2)
    {
        // 스크롤 내용 갱신
        RenewScroll();
    }
    else if (param->intValue() == 3)
    {
        // 요정을 하나 구입해서, 요정의마을 리스트를 새로 갱신한다.
        
        // Loading 화면으로 MESSAGE request 넘기기
        Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
        
        // 네트워크로 메시지들을 받아온다.
        char temp[50];
        std::string params = "";
        sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
        params += temp;
        
        if (binaryVersion_current < 2000) // 1000번대 (기존 유저)
            Network::HttpPost(params, URL_PURCHASE_FAIRY_LIST, this, httpresponse_selector(CocoRoomFairyTown::onHttpRequestCompleted))
            ;
        else // 2000번대 이상
            Network::HttpPost(params, URL_PURCHASE_FAIRY_LIST_NEW, this, httpresponse_selector(CocoRoomFairyTown::onHttpRequestCompleted));
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
    else if (param->intValue() == 11) // MP 관련 보상
    {
        if (myInfo->GetRewardTopaz() > 0)
        {
            std::vector<int> nullData;
            Common::ShowPopup(this, "CocoRoom", "NoImage", false, MP_REWARD_100, BTN_1, nullData);
        }
        else if (myInfo->IsRewardPotion())
        {
            std::vector<int> nullData;
            Common::ShowPopup(this, "CocoRoom", "NoImage", false, MP_REWARD_50, BTN_1, nullData);
        }
    }
}


void CocoRoomFairyTown::InitSprites()
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
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_fairytown.png", ccp(0, 0), ccp(103, 1429), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1420), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void CocoRoomFairyTown::MakeScroll()
{
    spriteClassScroll = new SpriteClass();
    
    int numOfList = fairyData.size();
    char fname[50];
    
    // container
    scrollContainer = CCLayer::create();
    scrollContainer->setContentSize(CCSizeMake(870, ((int)(numOfList-1)/3 + 1)*(290+5)));
   
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(290, 290));
        itemLayer->setPosition(ccp(20+((290+5)*(i%3)), scrollContainer->getContentSize().height-(295)*(i/3+1) ) );
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        
        FairyInfo* fi = FairyInfo::GetObj(fairyData[i]->GetCommonId());
        
        // bg
        sprintf(fname, "background/bg_board_brown.png_a%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0), ccp(0, 0), CCSize(290, 290), "", "Layer", itemLayer, 3) );
        
        // tag에 fairy ID 혹은 이상한 값(터치방지용)을 넣어둔다.
        if (fairyData[i]->GetCommonId() != -1)
            spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite9->setTag(i);
        else
            spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite9->setTag(-1);
        
        // 이름
        sprintf(fname, "background/bg_dontknow_1.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(1, fname, ccp(0, 0), ccp(22, 25), CCSize(248, 58), "", "Layer", itemLayer, 100) );
        if (fairyData[i]->GetCommonId() != -1)
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(fi->GetName(), fontList[0], 36, ccp(0.5, 0), ccp(spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite9->getContentSize().width/2, 11), ccc3(255,255,255), fname, "1", NULL, 100) );
        
        if (fairyData[i]->GetCommonId() == -1)
        {
            sprintf(fname, "icon/icon_fairy_empty.png%d", i);
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0.5, 0.5), ccp(290/2, 290/2+17), CCSize(0, 0), "", "Layer", itemLayer, 90) );
            spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite->setScale(1.1f);
            continue;
        }
        
        // 그림
        CCLayer* picture = Fairy::GetFairy(fi->GetId());
        float scale = 290.0f / std::max(picture->getContentSize().width, picture->getContentSize().height);
        switch (fi->GetId())
        {
            case 0: picture->setScale(1.0f); break;
            case 1: picture->setScale(0.7f); break;
            case 2: picture->setScale(0.8f); break;
            case 3: picture->setScale(0.8f); break;
            case 4: picture->setScale(scale-0.15f); break;
            case 5: picture->setScale(scale-0.25f); break;
            case 6: picture->setScale(scale-0.20f); break;
            case 7: picture->setScale(scale-0.15f); break;
            case 8: picture->setScale(scale-0.15f); break;
            case 9: picture->setScale(scale-0.17f); break;
            case 10: picture->setScale(scale-0.22f); break;
            default: picture->setScale(scale); break;
        }
        picture->setAnchorPoint(ccp(0, 0));
        picture->setPosition(ccp(290/2, 290/2+30));
        itemLayer->addChild(picture, 10);

        // grade
        if (fi->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png%d", i);
        else if (fi->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png%d", i);
        else if (fi->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png%d", i);
        else if (fi->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(25, 219), CCSize(0, 0), "", "Layer", itemLayer, 90) );
        
        
        if (myInfo->GetActiveFairyId() == fi->GetId()) // 착용중 표시 (check)
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_fairy_select.png", ccp(0, 0), ccp(172, 173), CCSize(0, 0), "", "Layer", itemLayer, 90) );
        }
        else if (MyFairy::GetObj(fi->GetId()) != NULL) // 구입은 했고, 착용중이지 않은 것 표시
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_fairy_select_no.png", ccp(0, 0), ccp(172, 173), CCSize(0, 0), "", "Layer", itemLayer, 90) );
        }
        else
        {
            if (fairyData[i]->GetCostType() == 2) // 구입하지 않았고, 달성 조건을 채우지 못한 경우 (토파즈 구입)
            {
                spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png", ccp(1, 1), ccp(287, 287), CCSize(0, 0), "", "Layer", itemLayer, 90) );
            }
            // 구입하지 않았고, 달성 조건 채운 경우(별사탕 구입)는 아무것도 표시하지 않는다
        }
    }
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
        spriteClassScroll->AddChild(i);
    
    // container 설정 + offset
    scrollView->setContainer(scrollContainer);
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setContentOffset(ccp(0, (904-40)-scrollContainer->getContentSize().height ));
}

void CocoRoomFairyTown::RenewScroll()
{
    // delete & init all scroll-related variables.
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    spriteClassScroll = NULL;
    
    scrollContainer->removeAllChildren();
    scrollContainer->removeFromParentAndCleanup(true);
    scrollView->removeAllChildren();
    
    // 다시 스크롤 생성
    MakeScroll();
}

bool CocoRoomFairyTown::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    scrollViewTouch = false;
    
    CCPoint point = pTouch->getLocation();

    if (scrollView->boundingBox().containsPoint(point))
        scrollViewTouch = true;
    
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
    }
    
    return true;
}


void CocoRoomFairyTown::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void CocoRoomFairyTown::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClassScroll->spriteObj.size() ; i++)
    {
        if (spriteClassScroll->spriteObj[i]->name.substr(0, 31) == "background/bg_board_brown.png_a")
        {
            if (spriteClassScroll->spriteObj[i]->sprite9->getTag() == -1)
                continue;
            
            CCPoint p = spriteClassScroll->spriteObj[i]->sprite9->convertToNodeSpace(point);
            CCSize size = spriteClassScroll->spriteObj[i]->sprite9->getContentSize();
            if (scrollViewTouch && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClickboard();
                int idx = spriteClassScroll->spriteObj[i]->sprite9->getTag();
                // common-fairy-id를 popup창에 넘겨주는데, 구입 type이 토파즈면 음수로 바꿔서 넘겨준다.
                int cfi = fairyData[idx]->GetCommonId();
                if (fairyData[idx]->GetCostType() == 2)
                    cfi *= -1;
                Common::ShowNextScene(this, "CocoRoomFairyTown", "FairyOneInfo", false, cfi);
                break;
            }
        }
    }
    
    isTouched = false;
}

void CocoRoomFairyTown::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void CocoRoomFairyTown::scrollViewDidZoom(CCScrollView* view)
{
}


void CocoRoomFairyTown::EndScene()
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

    // remove all data
    for (int i = 0 ; i < fairyData.size() ; i++)
        delete fairyData[i];
    fairyData.clear();
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassScroll->RemoveAllObjects();
    delete spriteClassScroll;
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParentAndCleanup(true);
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void CocoRoomFairyTown::EndSceneCallback()
{
}



void CocoRoomFairyTown::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParseFairyList(&xmlDoc);
}

bool compareFairy(FairyEach* f1, FairyEach* f2)
{
    if (f1->GetCommonId() == -1)
        return false;
    else if (f2->GetCommonId() == -1)
        return false;
    
    FairyInfo* fi1 = FairyInfo::GetObj(f1->GetCommonId());
    FairyInfo* fi2 = FairyInfo::GetObj(f2->GetCommonId());
    
    return fi1->GetCostTopaz() > fi2->GetCostTopaz();
}
void CocoRoomFairyTown::XmlParseFairyList(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "CocoRoomFairyTown", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // init
        for (int i = 0 ; i < fairyData.size() ; i++)
            delete fairyData[i];
        fairyData.clear();
        
        int cfi, costType, costValue;
        std::string name;
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("purchase-fairy-list").children("fairy");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "common-fairy-id") cfi = ait->as_int();
                else if (name == "cost-type") costType = ait->as_int();
                else if (name == "cost-value") costValue = ait->as_int();
            }
            // cost-type -->  1: 별사탕 , 2: 토파즈
            fairyData.push_back( new FairyEach(cfi, costType, costValue) );
        }
        
        for (int i = 0 ; i < 2 ; i++)
            fairyData.push_back( new FairyEach(-1, -1, -1) );
        
        std::sort(fairyData.begin(), fairyData.end(), compareFairy);
        
        // 스크롤에 정보 표시
        MakeScroll();
    }
}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

FairyEach::FairyEach(int cfi, int costType, int costValue)
{
    this->commonFairyId = cfi;
    this->costType = costType;
    this->costValue = costValue;
}
int FairyEach::GetCommonId()
{
    return commonFairyId;
}
int FairyEach::GetCostType()
{
    return costType;
}
int FairyEach::GetCostValue()
{
    return costValue;
}


