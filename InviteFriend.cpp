#include "InviteFriend.h"
#include "Kakao/Plugins/KakaoNativeExtension.h"
#include <regex>

static std::vector<std::string> kakaoIds;
static std::vector<int> remainTimes;

CCScene* InviteFriend::scene()
{
    CCScene* pScene = CCScene::create();
    InviteFriend* pLayer = InviteFriend::create();
    pScene->addChild(pLayer);
	return pScene;
}

void InviteFriend::onEnter()
{
    //CCLog("InviteFriend : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.2f, ccp(0, 0)), CCScaleTo::create(0.2f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(InviteFriend::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void InviteFriend::SceneCallback()
{
    if (!isInviteListGathered)
    {
        // Loading 화면으로 MESSAGE request 넘기기
        Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
        
        // 네트워크로 초대할 친구 리스트를 받아온다.
        httpStatus = 0;
        char temp[50];
        std::string params = "";
        sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
        params += temp;
        
        Network::HttpPost(params, URL_INVITE_FRIEND_LIST, this, httpresponse_selector(InviteFriend::onHttpRequestCompleted));
    }
    else
    {
        // Loading 화면으로 MESSAGE request 넘기기
        Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
        
        // init sprite
        InitSprites();
        // scroll을 생성 후 데이터 보여주기
        MakeScroll();
        
        ((Loading*)Depth::GetCurPointer())->EndScene();
        
        // profile timer 시작
        this->schedule(schedule_selector(InviteFriend::ProfileTimer), 1.0f);
    }
}
void InviteFriend::onExit()
{
    //CCLog("InviteFriend : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void InviteFriend::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
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
    
    inviteIdx = -1;
    
    // make depth tree
    Depth::AddCurDepth("InviteFriend", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("InviteFriend : touch prio = %d", this->getTouchPriority());
    
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
        //CCLog("InviteFriend : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("InviteFriend : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        
        scrollView->setTouchEnabled(false);
    }
    else if (param->intValue() == 5)
    {
        Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
        
        // 카카오 api 호출 (초대메시지 템플릿 이용)
        std::string templateId = KAKAO_MSG_TEMPLATE_INVITEFRIEND;
        std::string executeUrl = "";
        char temp[100];
        sprintf(temp, "{\"sender_name\":\"%s\"}", MyInfo::GetName().c_str());
        std::string metaInfo = temp;
        //CCLog("metaInfo = %s", metaInfo.c_str());
        //CCLog("Touched idx = %d (name = %s)", inviteIdx, inviteList[inviteIdx]->nickname.c_str());
        
        KakaoNativeExtension::getInstance()->sendLinkMessage(std::bind(&InviteFriend::onSendLinkMessageComplete, this), std::bind(&InviteFriend::onSendLinkMessageErrorComplete, this, std::placeholders::_1, std::placeholders::_2), templateId, inviteList[inviteIdx]->userId, "", executeUrl, metaInfo);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
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
    // 보상 그림 3개
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_1.png", ccp(0, 0), ccp(98-ofs+45, 275), CCSize(0,0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_2.png", ccp(0, 0), ccp(390-ofs+45, 275), CCSize(0,0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_invitefriend_3.png", ccp(0, 0), ccp(686-ofs+65, 275), CCSize(0,0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("별사탕 15,000개", fontList[0], 25, ccp(0, 0), ccp(98-ofs+30, 243), ccc3(0,0,0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("토파즈 10개", fontList[0], 25, ccp(0, 0), ccp(390-ofs+55, 243), ccc3(0,0,0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("토파즈 25개", fontList[0], 25, ccp(0, 0), ccp(686-ofs+80, 243), ccc3(0,0,0), "", "Layer", tLayer, 1) );
    
    
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
/*
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
*/
void InviteFriend::MakeScroll()
{
    int numOfList = (int)inviteList.size();
    
    // make scroll
    scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    
    char name[50], name2[50], name3[50], name4[50];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* itemLayer = CCLayer::create();
        itemLayer->setContentSize(CCSizeMake(862, 166));
        itemLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(itemLayer, 2);
        spriteClassScroll->layers.push_back(itemLayer);
        
        // 프로필 이미지
        sprintf(name, "background/bg_profile.png%d", i);
        ProfileSprite* psp = ProfileSprite::GetObj(inviteList[i]->profileUrl);
        if (inviteList[i]->profileUrl != "" && psp->IsLoadingDone())
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(45+5, 35+11), CCSize(0,0), "", "Layer", itemLayer, 5, 0, 255, 0.95f) );
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255) );
        }
        else
        {
            spriteClassScroll->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0, 0), ccp(45, 35), CCSize(0,0), "", "Layer", itemLayer, 5, 0, 255, 1.0f, -888*(i+1)) ); // tag = -888 * (i+1)
            spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(45, 35), CCSize(0, 0), "", "Layer", itemLayer, 5, 0, 255, -777*(i+1)) ); // tag = -777 * (i+1)
        }
        inviteList[i]->profile = spriteClassScroll->spriteObj[spriteClassScroll->spriteObj.size()-1]->sprite;
        
        
        // nickname
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel(inviteList[i]->nickname, fontList[0], 48, ccp(0, 0), ccp(196, 118-10), ccc3(78,47,8), "", "Layer", itemLayer, 3) );
        
        // starcandy bg + starcandy + text(x 1000)
        sprintf(name, "background/bg_degree_desc.png1%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(269-60, 25), CCSize(223+160-5, 76), "", "Layer", itemLayer, 3) );
        sprintf(name2, "icon/icon_starcandy_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(13, 6), CCSize(0, 0), name, "1", NULL, 3, 1) );
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel("1,000", fontList[0], 36, ccp(0, 0), ccp(83, 19), ccc3(78,47,8), name, "1", NULL, 3, 1) );
        
        sprintf(name2, "icon/icon_potion.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(17+215, 10), CCSize(0, 0), name, "1", NULL, 3, 1) );
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setScale(0.8f);
        spriteClassScroll->spriteObj.push_back( SpriteObject::CreateLabel("1", fontList[0], 36, ccp(0, 0), ccp(83+215-3, 19), ccc3(78,47,8), name, "1", NULL, 3, 1) );
        
        
        
        //////// 친구초대 가능한 경우 ///////
        sprintf(name, "button/btn_blue_mini.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 0) );
        sprintf(name2, "letter/letter_invite.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(45, 25), CCSize(0, 0), name, "0", NULL, 3, 1, 0) );
        ////////////////////////////////
        
        //////// 친구초대 이미 한 경우 //////
        sprintf(name3, "button/btn_invite.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name3, ccp(0, 0), ccp(635, 34+10), CCSize(0, 0), "", "Layer", itemLayer, 3, 0, 0) );
        sprintf(name4, "letter/letter_invite_brown.png%d", i);
        spriteClassScroll->spriteObj.push_back( SpriteObject::Create(0, name4, ccp(0, 0), ccp(45, 25), CCSize(0, 0), name3, "0", NULL, 3, 1, 0) );
        ////////////////////////////////
        
        
        if (!inviteList[i]->wasInvited) // 친구초대 가능한 경우
        {
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name))->setOpacity(255);
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setOpacity(255);
            
            // 메시지 수신거부한 친구 or 지원되지 않는 디바이스 이용중인 친구는 어둡게 만들어서 터치 못하게 하자.
            if (inviteList[i]->messageBlocked || !inviteList[i]->supportedDevice)
            {
                ((CCSprite*)spriteClassScroll->FindSpriteByName(name))->setColor(ccc3(150,150,150));
                ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setColor(ccc3(150,150,150));
            }
        }
        else // 이미 초대한 경우
        {
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name3))->setOpacity(255);
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name4))->setOpacity(255);
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

void InviteFriend::ProfileTimer(float f)
{
    // 프로필 사진 왼쪽 위 지점과 스크롤뷰 위치를 비교한다.
    // 음수가 되면, 아래에 있던 프로필이 스크롤뷰에 보이기 시작했다는 의미 -> 프로필 로딩 시작.
    CCPoint p;
    float h;
    for (int i = 0 ; i < inviteList.size() ; i++)
    {
        ProfileSprite* psp = ProfileSprite::GetObj(inviteList[i]->profileUrl);
        if (psp->IsLoadingStarted() || psp->IsLoadingDone())
            continue;
        
        p = inviteList[i]->profile->convertToNodeSpace(scrollView->getPosition());
        h = inviteList[i]->profile->getContentSize().height;
        
        if (p.y-h < 0)
        {
            psp->SetLoadingStarted(true);
            
            char tag[6];
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(psp->GetProfileUrl().c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(InviteFriend::onHttpRequestCompletedNoEncrypt));
            sprintf(tag, "%d", i);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
    }
}
void InviteFriend::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
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
    int index = atoi(res->getHttpRequest()->getTag());
    for (int i = 0 ; i < profiles.size() ; i++)
    {
        if (profiles[i]->GetProfileUrl() == inviteList[index]->profileUrl)
        {
            profiles[i]->SetSprite(texture);
            profiles[i]->SetLoadingDone(true);
            // 화면에 보이는 스프라이트 교체
            if (spriteClassScroll == NULL)
                return;
            spriteClassScroll->ChangeSprite(-888*(index+1), profiles[i]->GetProfile());
            ((CCSprite*)spriteClassScroll->FindSpriteByTag(-777*(index+1)))->setOpacity(255);
            break;
        }
    }
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
                inviteIdx = atoi(spriteClassScroll->spriteObj[i]->name.substr(24).c_str());
                CCLog("Touched idx = %d (name = %s)", inviteIdx, inviteList[inviteIdx]->nickname.c_str());
                
                // 메시지 수신거부한 친구 , 지원되지 않는 디바이스 사용중인 친구는 터치 못하게 막아두자.
                if (inviteList[inviteIdx]->messageBlocked || !inviteList[inviteIdx]->supportedDevice)
                    break;
                if (inviteList[inviteIdx]->wasInvited)
                    break;
                
                if (totalCnt >= 30) // 최대 초대 수를 넘은 경우
                {
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_NO_MORE, BTN_1, nullData);
                    break;
                }
                
                sound->playClick();
                std::vector<int> data;
                data.push_back(inviteIdx);
                Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_TRY, BTN_2, data);
                break;
            }
        }
    }
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
}


static int stat;

void InviteFriend::onSendLinkMessageComplete()
{
    //CCLog("onSendLinkMessageComplete");
    stat = 0;
    SendToServer();
}
void InviteFriend::onSendLinkMessageErrorComplete(char const *status, char const *error)
{
    //CCMessageBox(error, "onSendLinkMessageErrorComplete");
    //CCLog("onSendLinkMessageErrorComplete : %s, %s", status, error);
    stat = atoi(status);
    
    if (stat != -31)
        ((Loading*)Depth::GetCurPointer())->EndScene();
    
    std::vector<int> nullData;
    if (stat == -32) // 초대메시지 1일 쿼터 초과
    {
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_MAX_PER_DAY, BTN_1, nullData);
    }
    else if (stat == -14) // 그 유저가 지원되지 않는 device 이용중임.
    {
        inviteList[inviteIdx]->supportedDevice = false;
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_NOT_SUPPORTED_DEVICE, BTN_1, nullData);
    }
    else if (stat == -17 || stat == -16) // 메시지 수신거부 사용자임. (앱 설치하지 않은 친구(-17) / 앱 설치한 친구(-16))
    {
        inviteList[inviteIdx]->messageBlocked = true;
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_BLOCKED, BTN_1, nullData);
    }
    else if (stat == -11) // 탈퇴한 사용자
    {
        inviteList[inviteIdx]->messageBlocked = true;
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_UNREGISTERED, BTN_1, nullData);
    }
    //if (stat != 31)
    
    // 필요한 상황에서만 스크롤뷰 데이터 갱신을 한다.
    switch (stat)
    {
        case -17:
        case -16:
        case -14:
        case -11:
            RenewData();
            break;
    }

    // 동일 대상자(30일 이내)에게 한 번 더 보낼 경우에만 friend_invite.php를 호출한다.
    if (stat == -31)
        SendToServer();
}
void InviteFriend::SendToServer()
{
    // 친구를 초대한다.
    httpStatus = 1;
    char temp[50];
    std::string param = "";
    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
    param += temp;
    sprintf(temp, "friend_kakao_id=%s", inviteList[inviteIdx]->userId.c_str());
    param += temp;
    
    Network::HttpPost(param, URL_INVITE_FRIEND, this, httpresponse_selector(InviteFriend::onHttpRequestCompleted));
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
    
    this->unschedule(schedule_selector(InviteFriend::ProfileTimer));
    
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
    spriteClass = NULL;
    spriteClassScroll = NULL;
    
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
    bar = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, size*(700-10), 30-12));
    bar->setPosition(ccp(96+10+5-ofs, 192+6-15));
    bar->setAnchorPoint(ccp(0, 0));
    bar->setColor(ccc3(255,255,255));
    tLayer->addChild(bar, 3);

    char name[20];
    sprintf(name, "%d명 초대", totalCnt);
    ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(name);
    
    // 정렬 : 닉네임 순 (단, 초대된 사람은 무조건 뒤로 보낸다)
    //std::sort(inviteList.begin(), inviteList.end(), compareInvite);
    
    /*
    // 스크롤뷰 갱신
    spriteClassScroll->RemoveAllObjects();
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    MakeScroll();
     */

    char name1[50], name2[50], name3[50], name4[50];
    sprintf(name1, "button/btn_blue_mini.png%d", inviteIdx);
    sprintf(name2, "letter/letter_invite.png%d", inviteIdx);
    sprintf(name3, "button/btn_invite.png%d", inviteIdx);
    sprintf(name4, "letter/letter_invite_brown.png%d", inviteIdx);
    
    if (!inviteList[inviteIdx]->wasInvited) // 친구초대 가능한 경우
    {
        CCLog("sss : %s", name3);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name1))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name3))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name4))->setOpacity(0);
/*
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(+1)))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(222*(inviteIdx+1)))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(333*(inviteIdx+1)))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(444*(inviteIdx+1)))->setOpacity(0);
        */
        // 메시지 수신거부한 친구 or 지원되지 않는 디바이스 이용중인 친구는 어둡게 만들어서 터치 못하게 하자.
        if (inviteList[inviteIdx]->messageBlocked || !inviteList[inviteIdx]->supportedDevice)
        {
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name1))->setColor(ccc3(150,150,150));
            ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setColor(ccc3(150,150,150));
/*
            ((CCSprite*)spriteClassScroll->FindSpriteByTag(111*(inviteIdx+1)))
            ((CCSprite*)spriteClassScroll->FindSpriteByTag(222*(inviteIdx+1)))->setColor(ccc3(150,150,150));
 */
        }
    }
    else // 이미 초대한 경우
    {
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name1))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name2))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name3))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByName(name4))->setOpacity(255);
        /*
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(111*(inviteIdx+1)))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(222*(inviteIdx+1)))->setOpacity(0);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(333*(inviteIdx+1)))->setOpacity(255);
        ((CCSprite*)spriteClassScroll->FindSpriteByTag(444*(inviteIdx+1)))->setOpacity(255);
         */
    }
}


void InviteFriend::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    switch (httpStatus)
    {
        case 0: XmlParseList(&xmlDoc); break;
        case 1: XmlParseInviteFriend(&xmlDoc); break;
    }
}

void InviteFriend::XmlParseList(xml_document *xmlDoc)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
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
        
        // 초대리스트 초기화
        InitInviteList();
        
        // init sprite
        InitSprites();
        // scroll을 생성 후 데이터 보여주기
        MakeScroll();
        
        // profile timer 시작
        this->schedule(schedule_selector(InviteFriend::ProfileTimer), 1.0f);
    }
}

void InviteFriend::InitInviteList()
{
    for (int i = 0 ; i < inviteList.size() ; i++)
        delete inviteList[i];
    inviteList.clear();
    
    int numOfList = KakaoFriends::getInstance()->friends->count();
    CCArray* keys = KakaoFriends::getInstance()->friends->allKeys();
    
    bool wasInvited;
    for (int j = 0 ; j < numOfList ; j++)
    {
        CCString* k = (CCString*)keys->objectAtIndex(j);
        std::string userId = k->getCString();
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
        
        // 닉네임 특수문자 처리
        f->nickname = Common::SubstrNickname(f->nickname);
        
        inviteList.push_back( new InviteList(userId, f->nickname, f->profileImageUrl, f->hashedTalkUserId, f->messageBlocked, f->supportedDevice, wasInvited) );
        
        if (ProfileSprite::GetProfile(f->profileImageUrl) == NULL) // 프로필 sprite에 모은다.
            profiles.push_back( new ProfileSprite(f->profileImageUrl, false) );
    }
    // 정렬 : 닉네임 순 (단, 초대된 사람은 무조건 뒤로 보낸다)
    //std::sort(inviteList.begin(), inviteList.end(), compareInvite);
    
    isInviteListGathered = true;
}


void InviteFriend::XmlParseInviteFriend(xml_document *xmlDoc)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    int reward = 0;
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
        reward = nodeResult.child("friend-invite").attribute("special-reward").as_int();
        
        // 리스트 데이터 갱신
        //remainTimes[idx] = 9999999;
        
        inviteList[inviteIdx]->wasInvited = true;
        
        // Ranking OR CocoRoom 화면에 데이터 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    }
    
    // 에러 코드에 대한 처리
    std::vector<int> nullData;
    if (code == 11) // 최대 제한 30명을 넘은 경우
    {
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_NO_MORE, BTN_1, nullData);
    }
    else if (stat == -31 || code == 12) // 동일 대상자에 30일에 한번만 가능
    {
        inviteList[inviteIdx]->wasInvited = true;
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_ALREADY_DID, BTN_1, nullData);
    }
    else if (stat == 0) // 성공
    {
        inviteList[inviteIdx]->wasInvited = true;
        std::vector<int> data;
        data.push_back(reward);
        Common::ShowPopup(this, "InviteFriend", "NoImage", false, INVITE_FRIEND_OK, BTN_1, data);
    }
    
    //CCLog("xml parse (inviteIdx) = %d", inviteIdx);
    
    
    // 스크롤뷰 내용 갱신
    if (stat == -31 || stat == 0 || code == 12)
    {
        RenewData();
    }
}




