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
    
	return true;
}

void Splash::LogoLoadingCompleted()
{
    // logo
    m_pTitle = CCSprite::createWithSpriteFrameName("background/Title.png");
    m_pTitle->setPosition(ccp(winSize.width/2, 1350));
    this->addChild(m_pTitle, 5);
    
    
    mKakaoId = CCUserDefault::sharedUserDefault()->getIntegerForKey("kakaoId", -1);
    if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        mDeviceType = 2;
    else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        mDeviceType = 1;
    
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
        mKakaoId = 1000;
        CCUserDefault::sharedUserDefault()->setIntegerForKey("kakaoId", mKakaoId);
    }
    
    m_pStartBtn = CCSprite::createWithSpriteFrameName("button/btn_red.png");
    m_pStartBtn->setAnchorPoint(ccp(0, 0));
    m_pStartBtn->setPosition(ccp(319, 191));
    this->addChild(m_pStartBtn, 3);
    
    m_pStartLetter = CCSprite::createWithSpriteFrameName("letter/letter_gamestart.png");
    m_pStartLetter->setAnchorPoint(ccp(0.5, 0.5));
    m_pStartLetter->setPosition(ccp(319+446/2, 191+160/2+5));
    this->addChild(m_pStartLetter, 3);
    
    // 내 정보 class (extern) 미리 만들기
    myInfo = new MyInfo(mKakaoId, mDeviceType);
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
        m_pMsgLabel->setString("로딩중입니다......");
        
        // make url
        char temp[255];
        std::string url = "http://14.63.225.203/cogma/game/login.php?";
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
 
    m_pStartBtn->setOpacity(255);
    m_pStartLetter->setOpacity(255);
    isStarting = false;
}

void Splash::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    if (!res)
        return;
    
    int statusCode = res->getResponseCode();
    //char statusString[64] = {};
    //sprintf(statusString, "Http Status Code: %d, tag = %s", statusCode, res->getHttpRequest()->getTag());
    CCLog("response code : %d", statusCode);
    
    if (!res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
    {
        dumpData[i] = (*buffer)[i];
    }
    dumpData[buffer->size()] = NULL;
    CCLog("==================================");
    CCLog("%s", dumpData);
    CCLog("==================================");
    
    // save
    //CCUserDefault::sharedUserDefault()->setStringForKey("username", sUsername);
 
    // check login success
    //if (XmlParseLogin(dumpData, buffer->size()))
    XmlParseLogin(dumpData, buffer->size());
    //Common::ShowNextScene(this, "Splash", "Ranking", true);
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
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code == 0)
    {
        // login success
        
        bool todayFirst = nodeResult.child("today-first").text().as_bool();
        int userID = nodeResult.child("userID").text().as_int();
        if (todayFirst)
            CCLog("today first : true");
        else
            CCLog("today first : false");
        CCLog("user id = %d", userID);
        /*
        iAge = nodeResult.child("raise").attribute("age").as_int();
        iType = nodeResult.child("raise").attribute("type").as_int();
        iWeight = nodeResult.child("raise").attribute("weight").as_int();
        iMaxScore = nodeResult.child("puzzle").attribute("max-score").as_int();
        iRemainingHeartNum = nodeResult.child("puzzle").attribute("heart-num").as_int();
        iRemainingHeartTime = nodeResult.child("puzzle").attribute("heart-remain-time").as_int();
        */
        
        
        // request for data (friend list)
        char temp[50];
        std::string url = "http://14.63.225.203/cogma/game/get_friendslist.php?";
        sprintf(temp, "kakao_id=%d", mKakaoId);
        url += temp;
        CCLog("url = %s", url.c_str());
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedData));
        CCHttpClient::getInstance()->send(req);
        req->release();
        
        //return true;
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
    
    //return false;
}

void Splash::onHttpRequestCompletedData(CCNode *sender, void *data)
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
    //CCLog("==================================");
    //CCLog("%s", dumpData);
    //CCLog("==================================");
    
    // check login success
//    if (XMLParseData(dumpData, buffer->size()))
//        Common::ShowNextScene(this, "Splash", "Ranking", true);
    XMLParseData(dumpData, buffer->size());
}

bool Splash::XMLParseData(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return false;
    }
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code == 0)
    {
        profileCnt = 0;
        char tag[5];
        
        xml_object_range<xml_named_node_iterator> friends = nodeResult.child("friend-list").children("friend");
        for (xml_named_node_iterator it = friends.begin() ; it != friends.end() ; ++it)
        {
            int kakaoId;
            std::string nickname;
            std::string imageUrl;
            int potionMsgStatus;
            int weeklyHighScore;
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
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, weeklyHighScore, highScore, certificateType, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            
            // get profile image sprite from URL
            CCHttpRequest* req = new CCHttpRequest();
            CCLog("image url = %s", imageUrl.c_str());
            req->setUrl(imageUrl.c_str());
            req->setRequestType(CCHttpRequest::kHttpGet);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedProfile));
            sprintf(tag, "%d", (int)friendList.size()-1);
            req->setTag(tag);
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
        
        return true;
    }
    else
    {
        // failed msg
        CCLog("failed code = %d", code);
    }
    
    return false;
}

void Splash::onHttpRequestCompletedProfile(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        return;
    }
    
    profileCnt++;
    //CCLog("profile cnt : %d", profileCnt);
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    char dumpData[IMAGE_BUFFER_SIZE];
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData[i] = (*buffer)[i];
    dumpData[buffer->size()] = NULL;
    
    // make texture2D
    CCImage* img = new CCImage;
    img->initWithImageData(dumpData, buffer->size());
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithImage(img);
    
    // set CCSprite
    int index = atoi(res->getHttpRequest()->getTag());
    //CCLog("image index = %d", index);
    friendList[index]->SetSprite(texture);
    
    if (profileCnt == (int)friendList.size())
        Common::ShowNextScene(this, "Splash", "Ranking", true);
}





void Splash::EndScene()
{
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

