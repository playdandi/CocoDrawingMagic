#include "Splash.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

bool isStarting = false;


CCScene* Splash::scene()
{
	CCScene* pScene = CCScene::create();
    
	Splash* pLayer = Splash::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Splash::keyBackClicked()
{
    if (isKeyBackClicked)
        return;
    isKeyBackClicked = true;
    
    sound->playWarning();
    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Ranking", "NoImage", false, POPUP_EXIT, BTN_2, nullData);
}


bool Splash::init()
{
	if (CCLayer::init() == false)
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_1.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_2.plist");
    
    // 배경 액션
    m_pBackground = CCSprite::create("images/main_background.png", CCRectMake(0, 0, 1080, 1920));
    m_pBackground->setAnchorPoint(ccp(0, 0));
    m_pBackground->setPosition(ccp(0, 0));
    m_pBackground->setOpacity(0);
    this->addChild(m_pBackground, 0);
    CCActionInterval* action = CCSequence::create(CCFadeIn::create(0.5f),
                        CCCallFunc::create(this, callfunc_selector(Splash::LogoLoadingCompleted)), NULL);
    m_pBackground->runAction(action);

    this->setKeypadEnabled(true);
	this->setTouchEnabled(true);
    isStarting = false;
    
    httpStatus = 0;
    
	return true;
}

void Splash::LogoLoadingCompleted()
{
    // logo
    m_pTitle = CCSprite::createWithSpriteFrameName("background/Title.png");
    m_pTitle->setPosition(ccp(winSize.width/2, 1350+1000));
    this->addChild(m_pTitle, 5);
    CCActionInterval* action = CCMoveTo::create(0.5f, ccp(winSize.width/2, 1350));
    m_pTitle->runAction(CCEaseBounceOut::create(action));
    
    // 클라이언트에 들고 있던 kakao ID, device Type 불러오기
    mKakaoId = CCUserDefault::sharedUserDefault()->getIntegerForKey("kakaoId", -1);
    if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        mDeviceType = 2;
    else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        mDeviceType = 1;
    else
        mDeviceType = 3;
    
    // Label 생성
    m_pMsgLabel = CCLabelTTF::create("", fontList[2].c_str(), 40);
    m_pMsgLabel->setAnchorPoint(ccp(0.5, 0.5));
    m_pMsgLabel->setPosition(ccp(winSize.width/2, 400));
    m_pMsgLabel->setColor(ccc3(0, 0, 0));
    if (mKakaoId == -1)
        m_pMsgLabel->setString("처음이시군요!!!");
    else
        m_pMsgLabel->setString("다시 오셨군요! 환영합니다!");
    this->addChild(m_pMsgLabel, 5);
    
    // kakao id 세팅
    if (mKakaoId == -1)
    {
        // 처음이면 kakao platform에 동의하는 창으로 넘어가야 한다.
        mKakaoId = 1000;
        CCUserDefault::sharedUserDefault()->setIntegerForKey("kakaoId", mKakaoId);
    }
    
    // 버전 세팅
    iGameVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion", -1);
    iBinaryVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("binaryVersion", -1);
    
    // 시작 버튼
    m_pStartBtn = CCSprite::createWithSpriteFrameName("button/btn_blue.png");
    m_pStartBtn->setAnchorPoint(ccp(0, 0));
    m_pStartBtn->setPosition(ccp(319, 191));
    this->addChild(m_pStartBtn, 3);
    m_pStartLetter = CCSprite::createWithSpriteFrameName("letter/letter_gamestart.png");
    m_pStartLetter->setAnchorPoint(ccp(0.5, 0.5));
    m_pStartLetter->setPosition(ccp(319+446/2, 191+160/2+5));
    this->addChild(m_pStartLetter, 3);
}

/*
void Splash::keyboardWillShow(CCIMEKeyboardNotificationInfo &info)
{
    CCLog("keyboard show");
    m_pEditName->setString("");
}
void Splash::keyboardWillHide(CCIMEKeyboardNotificationInfo &info)
{
    CCLog("keyboard hide - %s", m_pEditName->getString());
}

void Splash::editBoxEditingDidBegin(CCEditBox* editBox)
{
    //CCLog("%p did begin", editBox);
}
void Splash::editBoxEditingDidEnd(CCEditBox* editBox)
{
    //CCLog("%p did end", editBox);
}

void Splash::editBoxReturn(CCEditBox* editBox)
{
    std::string newUsername;
    newUsername.assign(editBox->getText());
    sUsername = newUsername;
}
*/


void Splash::ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent)
{
    CCTouch* pTouch = (CCTouch*)pTouches->anyObject();
    CCPoint point = pTouch->getLocation();
    
    if (m_pStartBtn->boundingBox().containsPoint(point))
    {
        isStarting = true;
        m_pStartBtn->setOpacity(120);
        m_pStartLetter->setOpacity(120);
    }
    /*
    if (m_pEditName->boundingBox().containsPoint(point))
    {
        m_pEditName->attachWithIME();
    }
     */
}

void Splash::ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent)
{
    
}

void Splash::ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent)
{
    CCTouch* pTouch = (CCTouch*)pTouches->anyObject();
    CCPoint point = pTouch->getLocation();
    
    if (isStarting && m_pStartBtn->boundingBox().containsPoint(point))
    {
        m_pMsgLabel->setString("게임 버전이 잘생겼는지 확인 중...");
        
        // 게임 버전 체크
        std::string url = "http://14.63.225.203/cogma/game/get_version.php?";
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
 
    m_pStartBtn->setOpacity(255);
    m_pStartLetter->setOpacity(255);
    isStarting = false;
}


void Splash::XmlParseVersion(char* data, int size)
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
        // 버전 정보를 받는다.
        //xml_node setting = nodeResult.child("setting");
        int gameVersion = nodeResult.child("game-version").text().as_int();
        int binaryVersion = nodeResult.child("binary-version").text().as_int();
        std::string balanceFileUrl = nodeResult.child("balance-file-url").text().as_string();
        // 나중에 마켓버전도 받자.
        
        if (binaryVersion != iBinaryVersion)
        {
            CCLog("바이너리 버전 다름");
            // 마켓/스토어에서 다시 앱을 받으라는 팝업창을 띄우자.
            
            iBinaryVersion = binaryVersion;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("binaryVersion", iBinaryVersion);
        }
        if (gameVersion != iGameVersion)
        {
            CCLog("게임 버전 다름");
            // 밸런스 파일 업데이트 !
            
            iGameVersion = gameVersion;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("gameVersion", iGameVersion);
        }
        
        // 로그인 시도
        m_pMsgLabel->setString("로그인 중...");
        char temp[255];
        std::string url = "http://14.63.225.203/cogma/game/login.php?";
        sprintf(temp, "game_version=%d&", iGameVersion);
        url += temp;
        sprintf(temp, "kakao_id=%d&", mKakaoId);
        url += temp;
        sprintf(temp, "push_token=TEST_PUSH_VALUE&");
        url += temp;
        sprintf(temp, "device_type=%d&", mDeviceType);
        url += temp;
        sprintf(temp, "nick_name=ijpark&");
        url += temp;
        sprintf(temp, "profile_image_url=http://14.63.225.203/resource/profile_img_ijpark.png");
        url += temp;
        
        CCLog("url = %s", url.c_str());
        
        // post request
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        // write data
        //char postData[25];
        //sprintf(postData, "user_name=%s", sUsername.c_str());
        //req->setRequestData(postData, strlen(postData));
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    else
    {
        CCLog("failed code = %d", code);
    }
}

void Splash::XmlParseLogin(char* data, int size)
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
        // 로그인 기본 정보를 받는다.
        xml_node setting = nodeResult.child("setting");
        bool kakaoMsg = (setting.attribute("kakao-message").as_int() == 1) ? true : false;
        bool pushNoti = (setting.attribute("push-notification").as_int() == 1) ? true : false;
        bool potionMsg = (setting.attribute("potion-message").as_int() == 1) ? true : false;
        int userId = nodeResult.child("userID").text().as_int();
        int msgCnt = nodeResult.child("message").attribute("count").as_int();
        
        // 내 정보 class (extern) 만들기
        myInfo = new MyInfo();
        myInfo->Init(mKakaoId, mDeviceType, userId, kakaoMsg, pushNoti, potionMsg, msgCnt);
        
        // 내 모든 정보 요청
        m_pMsgLabel->setString("나의 정보를 요청 중...");
        char temp[50];
        std::string url = "http://14.63.225.203/cogma/game/user_info.php?";
        sprintf(temp, "kakao_id=%d", mKakaoId);
        url += temp;
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}

void Splash::XmlParseMyInfo(char *data, int size)
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
        // 내 모든 정보를 받는다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int staffLvNext = nodeResult.child("next-staff").attribute("staff-level").as_int();
        int mpNextCostStarcandy = nodeResult.child("next-staff").attribute("star-candy-cost-value").as_int();
        int mpNextCostTopaz= nodeResult.child("next-staff").attribute("topaz-cost-value").as_int();
        int staffNextPercent= nodeResult.child("next-staff").attribute("bonus-mp").as_int();
        
        int highScore = nodeResult.child("score").attribute("high-score").as_int();
        int weeklyHighScore = nodeResult.child("score").attribute("weekly-high-score").as_int();
        int certificateType = nodeResult.child("score").attribute("certificate-type").as_int();
        int remainWeeklyRankTime = nodeResult.child("score").attribute("remain-weekly-rank-time").as_int();
        
        int item1 = nodeResult.child("item").attribute("count-1").as_int();
        int item2 = nodeResult.child("item").attribute("count-2").as_int();
        int item3 = nodeResult.child("item").attribute("count-3").as_int();
        int item4 = nodeResult.child("item").attribute("count-4").as_int();
        int item5 = nodeResult.child("item").attribute("count-5").as_int();
        
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainPotionTime = nodeResult.child("potion").attribute("remain-time").as_int();
        
        int fire = nodeResult.child("properties").attribute("fire").as_int();
        int water = nodeResult.child("properties").attribute("water").as_int();
        int land = nodeResult.child("properties").attribute("land").as_int();
        int master = nodeResult.child("properties").attribute("master").as_int();
        
        myInfo->InitRestInfo(topaz, starcandy, mp, mpStaffPercent, mpFairy, staffLv, highScore, weeklyHighScore, certificateType, remainWeeklyRankTime, item1, item2, item3, item4, item5, potion, remainPotionTime, fire, water, land, master);
        myInfo->SetNextStaff(staffLvNext, mpNextCostStarcandy, mpNextCostTopaz, staffNextPercent);
        
        
        // 친구 리스트 정보를 받는다.
        m_pMsgLabel->setString("상품 정보를 불러오는 중...");
        char temp[50];
        std::string url = "http://14.63.225.203/cogma/game/get_pricelist.php?";
        sprintf(temp, "device_type=%d", myInfo->GetDeviceType());
        url += temp;
        CCLog("url = %s", url.c_str());
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}

void Splash::XmlParsePrice(char* data, int size)
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
        int id, count, price, bonus;
        
        // topaz 가격 정보를 받는다.
        xml_object_range<xml_named_node_iterator> topaz = nodeResult.child("price-topaz").children("topaz");
        for (xml_named_node_iterator it = topaz.begin() ; it != topaz.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "count") count = ait->as_int();
                else if (name == "price") price = ait->as_int();
                else if (name == "bonus-percentage") bonus = ait->as_int();
            }
            priceTopaz.push_back( new PriceTopaz(id, count, price, bonus) );
        }
        // starCandy 가격 정보를 받는다.
        xml_object_range<xml_named_node_iterator> sc = nodeResult.child("price-starcandy").children("starcandy");
        for (xml_named_node_iterator it = sc.begin() ; it != sc.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "count") count = ait->as_int();
                else if (name == "price") price = ait->as_int();
                else if (name == "bonus-percentage") bonus = ait->as_int();
            }
            priceStarCandy.push_back( new PriceStarCandy(id, count, price, bonus) );
        }

                    
        // 친구 리스트 정보를 받는다.
        m_pMsgLabel->setString("못생긴 친구들을 불러오는 중...");
        char temp[50];
        std::string url = "http://14.63.225.203/cogma/game/get_friendslist.php?";
        sprintf(temp, "kakao_id=%d", mKakaoId);
        url += temp;
        CCLog("url = %s", url.c_str());
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    else if (code == 10)
    {
        CCLog("잘못된 device type 값입니다.");
    }
}

void Splash::XmlParseFriends(char* data, int size)
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
        profileCnt = 0;
        
        int kakaoId;
        std::string nickname;
        std::string imageUrl;
        int potionMsgStatus;
        int weeklyHighScore;
        int scoreUpdateTime;
        int remainPotionTime;
        int highScore;
        int certificateType;
        int fire;
        int water;
        int land;
        int master;
        int fairyId;
        int fairyLevel;
        int skillId;
        int skillLevel;
        
        xml_object_range<xml_named_node_iterator> friends = nodeResult.child("friend-list").children("friend");
        for (xml_named_node_iterator it = friends.begin() ; it != friends.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "kakao-id") kakaoId = ait->as_int();
                else if (name == "nick-name") nickname = ait->as_string();
                else if (name == "profile-image-url") imageUrl = ait->as_string();
                else if (name == "potion-message-receive") potionMsgStatus = ait->as_int();
                else if (name == "remain-potion-send-time") remainPotionTime = ait->as_int();
                else if (name == "high-score") highScore = ait->as_int();
                else if (name == "weekly-high-score") weeklyHighScore = ait->as_int();
                else if (name == "score-update-time") scoreUpdateTime = ait->as_int();
                else if (name == "certificate-type") certificateType = ait->as_int();
                else if (name == "properties-fire") fire = ait->as_int();
                else if (name == "properties-water") water = ait->as_int();
                else if (name == "properties-land") land = ait->as_int();
                else if (name == "properties-master") master = ait->as_int();
                else if (name == "fairy-id") fairyId = ait->as_int();
                else if (name == "fairy-level") fairyLevel = ait->as_int();
                else if (name == "skill-id") skillId = ait->as_int();
                else if (name == "skill-level") skillLevel = ait->as_int();
            }
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, weeklyHighScore, highScore, scoreUpdateTime, certificateType, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            // potion image 처리
            friendList[(int)friendList.size()-1]->SetPotionSprite();
            
            // profile이 없으면 미리 NOIMAGE sprite를 만든다.
            if (imageUrl == "")
            {
                profileCnt++;
                friendList[(int)friendList.size()-1]->SetSprite();
            }
        }
        
        // sort by { max[weeklyScore], min[scoreUpdateTime] }
        DataProcess::SortFriendListByScore();
        
        // get image by url (다 받으면 Ranking으로 넘어간다)
        m_pMsgLabel->setString("친구의 못생긴 얼굴 지적하는 중...");
        char tag[5];
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            CCLog("sorted : %d", friendList[i]->GetKakaoId());
            if (friendList[i]->GetImageUrl() != "")
            {
                // get profile image sprite from URL
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(friendList[i]->GetImageUrl().c_str());
                req->setRequestType(CCHttpRequest::kHttpGet);
                req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
                sprintf(tag, "%d", i);
                req->setTag(tag);
                CCHttpClient::getInstance()->send(req);
                req->release();
            }
        }
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
}


void Splash::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    //int statusCode = res->getResponseCode();
    //char statusString[64] = {};
    //sprintf(statusString, "Http Status Code: %d, tag = %s", statusCode, res->getHttpRequest()->getTag());
    //CCLog("response code : %d", statusCode);
    
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
    
    // parse xml data
    httpStatus++;
    switch (httpStatus-1)
    {
        case HTTP_VERSION:
            XmlParseVersion(dumpData, buffer->size()); break;
        case HTTP_LOGIN:
            XmlParseLogin(dumpData, buffer->size()); break;
        case HTTP_MYINFO:
            XmlParseMyInfo(dumpData, buffer->size()); break;
        case HTTP_PRICE:
            XmlParsePrice(dumpData, buffer->size()); break;
        case HTTP_FRIENDS:
            XmlParseFriends(dumpData, buffer->size()); break;
        default:
            profileCnt++;
            // make texture2D
            CCImage* img = new CCImage;
            img->initWithImageData(dumpData, buffer->size());
            CCTexture2D* texture = new CCTexture2D();
            texture->initWithImage(img);
            
            // set CCSprite
            int index = atoi(res->getHttpRequest()->getTag());
            friendList[index]->SetSprite(texture);
            
            //CCLog("callback : %d", profileCnt);
            
            if (profileCnt == (int)friendList.size())
                Common::ShowNextScene(this, "Splash", "Ranking", true);
            break;
    }
}


void Splash::EndScene()
{
    CCLog("Splash :: EndScene");
    //this->removeAllChildren();
    //CCScene* nextScene = RaisingLayer::scene();
//	CCDirector::sharedDirector()->setDepthTest(true);
    //CCScene* transition = CCTransitionFade::create(1.0f, nextScene);
	//CCDirector::sharedDirector()->replaceScene(transition);
    
    m_pBackground->removeFromParentAndCleanup(true);
    m_pTitle->removeFromParentAndCleanup(true);
    m_pMsgLabel->removeFromParentAndCleanup(true);
    m_pStartBtn->removeFromParentAndCleanup(true);
    m_pStartLetter->removeFromParentAndCleanup(true);
}

