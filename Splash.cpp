#include "Splash.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

Splash::~Splash(void)
{
    CCLog("Splash 소멸자");
}

CCScene* Splash::scene()
{
	CCScene* pScene = CCScene::create();
    
	Splash* pLayer = Splash::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Splash::onEnter()
{
    CCLog("Splash :: onEnter");
    CCLayer::onEnter();
}
void Splash::onExit()
{
    CCLog("Splash :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}


void Splash::keyBackClicked()
{
    if (isLoading || isKeyBackClicked) // 백버튼 팝업창이 떠 있거나, 데이터 로딩중에는 못 누르게 함.
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
    
    // add depth
    Depth::AddCurDepth("Splash", this);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_1.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_2.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/skill.plist");
    
    // 배경 액션
    m_pBackground = CCSprite::create("images/main_background.png", CCRectMake(0, 0, 1080, 1920));
    m_pBackground->setAnchorPoint(ccp(0.5, 0.5));
    m_pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    m_pBackground->setScale(1.2f);
    m_pBackground->setOpacity(0);
    this->addChild(m_pBackground, 0);
    CCActionInterval* action = CCSequence::create(CCFadeIn::create(0.5f),
                        CCCallFunc::create(this, callfunc_selector(Splash::LogoLoadingCompleted)), NULL);
    m_pBackground->runAction(action);

    isStarting = false;
    isLoading = false;
    
    isInGame = false;
    isInGameTutorial = false;
    isRebooting = false;
    
    httpStatus = 0;
    
    m_pEditName = NULL;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t,
                                        "com/playDANDi/CocoMagic/CocoMagic",
                                        "GetRegistrationId",
                                        "()Ljava/lang/String;"))
    {
        // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
		jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID,t.methodID);
        
		const char* msg = t.env->GetStringUTFChars(str, 0);
        strcpy(regId, msg);
		CCLog("받은메시지 = %s", regId); // 여기서 전달받은 값이 출력됨

		t.env->ReleaseStringUTFChars(str, msg);
		// Release
		t.env->DeleteLocalRef(t.classID);
    }
#endif

	return true;
}

void Splash::LogoLoadingCompleted()
{
    // logo
    m_pTitle = CCSprite::createWithSpriteFrameName("background/Title.png");
    m_pTitle->setPosition(ccp(winSize.width/2, 1350+1000));
    this->addChild(m_pTitle, 5);

    m_pForKakao = CCSprite::createWithSpriteFrameName("letter/letter_forkakao.png");
    m_pForKakao->setAnchorPoint(ccp(0, 1));
    m_pForKakao->setScale(0.8f);
    m_pForKakao->setPosition(ccp(winSize.width/2+20, 130));
    m_pTitle->addChild(m_pForKakao, 6);
    
    CCActionInterval* action = CCSequence::create(CCEaseBounceOut::create(CCMoveTo::create(0.5f, ccp(winSize.width/2, 1350))), CCCallFuncND::create(this, callfuncND_selector(Splash::SoundCallback), NULL), CCDelayTime::create(0.0f), CCCallFunc::create(this, callfunc_selector(Splash::Button_Callback)), NULL);
    m_pTitle->runAction(action);
    
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
        //mKakaoId = 1020;
        //CCUserDefault::sharedUserDefault()->setIntegerForKey("kakaoId", mKakaoId);
        
        m_pEditName = CCTextFieldTTF::textFieldWithPlaceHolder("ID", CCSize(300, 100), kCCTextAlignmentCenter, fontList[0].c_str(), 72);
        m_pEditName->setColor(ccc3(0,0,0));
        m_pEditName->setPosition(ccp(319+446/2, 191+160/2+5+200));
        m_pEditName->setAnchorPoint(ccp(0.5,0.5));
        this->addChild(m_pEditName);
    }
    
    // 버전 세팅
    iGameVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion", -1);
    iBinaryVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("binaryVersion", -1);
}

void Splash::SoundCallback(CCNode* sender, void* p)
{
    // sound
    //sound->PlayVoice(VOICE_TITLE);
    
    
}



RSA* Splash::createRSA(unsigned char * key, int pub)
{
    RSA* rsa = NULL;
    BIO* keybio;
    keybio = BIO_new_mem_buf(key, -1);
    
    if (keybio == NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
    
    /*
    if(pub)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }
    */
    if(rsa == NULL)
    {
        CCLog("res null");
        exit(0);
        printf( "Failed to create RSA");
    }
    
    return rsa;
}


void Splash::Button_Callback()
{
    // 시작 버튼
    m_pStartBtn = CCSprite::createWithSpriteFrameName("button/btn_red.png");
    m_pStartBtn->setAnchorPoint(ccp(0, 0));
    m_pStartBtn->setPosition(ccp(319, 191));
    this->addChild(m_pStartBtn, 3);
    m_pStartLetter = CCSprite::createWithSpriteFrameName("letter/letter_startgame.png");
    m_pStartLetter->setAnchorPoint(ccp(0.5, 0.5));
    m_pStartLetter->setPosition(ccp(319+446/2, 191+160/2+5));
    this->addChild(m_pStartLetter, 3);
    
    // 터치 활성화
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    this->setKeypadEnabled(true);
	this->setTouchEnabled(true);
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    
   // char* publicKey = "-----BEGIN PUBLIC KEY-----\nMDwwDQYJKoZIhvcNAQEBBQADKwAwKAIhAKP5Lsq9tblK50ghhot8gT3xc8tlae71\nUUpRjkB2aNyvAgMBAAE=\n-----END PUBLIC KEY-----";
    
    //BIO* keyBIO = NULL;
    //RSA* rsa = NULL;
    
    /*
    BIO* bp = BIO_new_mem_buf(publicKey, -1);
    if (bp == NULL)
    {
        CCLog("bio  error ");
        exit(1);
    }
    
    rsa = PEM_read_bio_RSA_PUBKEY(bp, &rsa, NULL, NULL);
    
    // pem파일 public key 추출
    //rsa = PEM_read_bio_RSA_PUBKEY(keyBIO, NULL, NULL, NULL);
    
    // 키를 로드 하는데 에러 발생
    if(rsa == NULL) {
        CCLog("로드 불가능");
        BIO_printf(errBIO, "키를 로드 할 수 없습니다.");
        ERR_print_errors(errBIO);
        exit(1);
    }
    */
    
     /*
    unsigned char plainText[4098] = "Hello this";
    unsigned char  encrypted[4098]={};
    unsigned char decrypted[4098]={};
    
      int result = RSA_public_encrypt(data_len,data,encrypted,rsa,RSA_PKCS1_PADDING);
    int len = 11;
    int encrypted_length= public_encrypt(plainText,len, (unsigned char*)publicKey,encrypted);
    if(encrypted_length == -1)
    {
        //printLastError("Public Encrypt failed ");
        CCLog("ak...");
        exit(0);
    }
     */
}




void Splash::keyboardWillShow(CCIMEKeyboardNotificationInfo &info)
{
    //CCLog("keyboard show");
}
void Splash::keyboardWillHide(CCIMEKeyboardNotificationInfo &info)
{
    //CCLog("keyboard hide");
}

/*
void Splash::editBoxTextChanged(CCEditBox* editBox, const std::string& text)
{
    CCLog("changing...");
}
void Splash::editBoxEditingDidBegin(CCEditBox* editBox)
{
    CCLog("%p did begin", editBox);
}
void Splash::editBoxEditingDidEnd(CCEditBox* editBox)
{
    CCLog("%p did end", editBox);
}

void Splash::editBoxReturn(CCEditBox* editBox)
{
    int kakao_id = atoi(editBox->getText());
    CCLog("editBoxReturn = %d", kakao_id);
}
*/

bool Splash::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isLoading)
        return true;
    
    CCPoint point = pTouch->getLocation();

    if (m_pStartBtn->boundingBox().containsPoint(point))
    {
        isStarting = true;
        m_pStartBtn->setColor(ccc3(170,170,170));
        m_pStartLetter->setColor(ccc3(170,170,170));
    }
    
    if (mKakaoId == -1 && m_pEditName->boundingBox().containsPoint(point))
        m_pEditName->attachWithIME();
    
    return true;
}

void Splash::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    
}

void Splash::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isLoading)
        return;
    
    CCPoint point = pTouch->getLocation();
    
    if (isStarting && m_pStartBtn->boundingBox().containsPoint(point))
    {
        isLoading = true;
        
        // kakao id save (처음 로그인 때만)
        if (mKakaoId == -1)
        {
            mKakaoId = atoi(m_pEditName->getString());
            CCUserDefault::sharedUserDefault()->setIntegerForKey("kakaoId", mKakaoId);
            m_pEditName->setOpacity(0);
        }
        
        m_pMsgLabel->setString("게임 버전이 잘생겼는지 확인 중...");
        
        // 게임 버전 체크
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(URL_VERSION);
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
        CCHttpClient::getInstance()->send(req);
        req->release();
        
        m_pStartBtn->setOpacity(0);
        m_pStartLetter->setOpacity(0);
    }
    else
    {
        m_pStartBtn->setColor(ccc3(255,255,255));
        m_pStartLetter->setColor(ccc3(255,255,255));
        isStarting = false;
    }
}


void Splash::XmlParseVersion(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 버전 정보를 받는다.
        gameVersion = nodeResult.child("game-version").text().as_int();
        int binaryVersion = nodeResult.child("binary-version").text().as_int();
        
        CCLog("바이너리 버전 = %d", binaryVersion);
        CCLog("게임 버전 = %d", gameVersion);
        std::string balanceFileUrl = nodeResult.child("balance-file-url").text().as_string();
        // 나중에 마켓버전도 받자.
        
        if (iBinaryVersion == -1)
        {
            // 최초 실행했을 때
            iBinaryVersion = binaryVersion;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("binaryVersion", iBinaryVersion);
        }
        else if (binaryVersion != iBinaryVersion)
        {
            CCLog("바이너리 버전 다름");
            // 마켓/스토어에서 다시 앱을 받으라는 팝업창을 띄우자.
            
            iBinaryVersion = binaryVersion;
            CCUserDefault::sharedUserDefault()->setIntegerForKey("binaryVersion", iBinaryVersion);
        }
        
        
        if (gameVersion != iGameVersion)
        {
            CCLog("게임 버전 다름");
            m_pMsgLabel->setString("못생긴 리소스 설득 중...");
            // 밸런스 파일 업데이트 !
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(balanceFileUrl.c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
            req->setTag("999");
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
        else // 최신 버전임
        {
            // 새 리소스 XML parsing
            m_pMsgLabel->setString("못생긴 리소스 배치 중...");
            XMLParseGameData();
            
            // 로그인 시도
            TryLogin();
        }
    }
}

void Splash::TryLogin()
{
    m_pMsgLabel->setString("로그인 중...");
    
    // required parameter values
    char temp[255];
    std::string param = "";
    sprintf(temp, "game_version=%d&", iGameVersion);
    param += temp;
    sprintf(temp, "kakao_id=%d&", mKakaoId);
    param += temp;
    if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // 안드로이드는 기기 고유 id를,
        sprintf(temp, "push_token=%s&", regId);
    else                                            // iPhone은 아이폰에서 기기 고유 id를 받아넣는다.
        sprintf(temp, "push_token=TEST_PUSH_VALUE&");
    param += temp;
    sprintf(temp, "device_type=%d&", mDeviceType);
    param += temp;
    if (mKakaoId == 1000) sprintf(temp, "nick_name=ijpark&");
    else if (mKakaoId == 1001) sprintf(temp, "nick_name=yjjung&");
    else if (mKakaoId == 1002) sprintf(temp, "nick_name=jwmoon&");
    else if (mKakaoId == 1020) sprintf(temp, "nick_name=카카오테스트&");
    param += temp;
    if (mKakaoId == 1000) sprintf(temp, "profile_image_url=http://14.63.212.106/resource/profile_img_ijpark.png");
    else if (mKakaoId == 1001) sprintf(temp, "profile_image_url=http://14.63.212.106/resource/profile_img_yjjung.png");
    else if (mKakaoId == 1002) sprintf(temp, "profile_image_url=http://14.63.212.106/resource/profile_img_jwmoon.png");
    else if (mKakaoId == 1020) sprintf(temp, "profile_image_url=http://14.63.212.106/resource/profile_img_kakao.png");
    param += temp;
    
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(URL_LOGIN);
    req->setRequestData(param.c_str(), param.size());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
}

void Splash::XMLParseGameData()
{
    //std::string filepath = CCFileUtils::sharedFileUtils()->fullPathForFilename("gamedata.xml");
    std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + "gamedata.xml";
    CCLog("filepath = %s", filepath.c_str());
    
    // read file
    unsigned long iSize = 0;
    unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(filepath.c_str(), "rb", &iSize);
    
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(pBuffer, iSize);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    // variables
    int id;
    int count, price, price_KRW, price_USD, bonus;
    int level, bonusMP, cost_starcandy, cost_topaz;
    int costType, cost;
    int category, type, value1, value2;
    int grade, pid;
    int ability, refId, ability2;
    std::string skillName;
    int maxLevel, mp, staffLv, skillId, skillLv, isActive;
    int maxExp, prob;
    xml_named_node_iterator it;
    
    xml_node nodeResult = xmlDoc.child("client-data");
    
    // price topaz
    xml_object_range<xml_named_node_iterator> its = nodeResult.child("price_topaz_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "bIndex") id = ait->as_int();
            else if (name == "nCount") count = ait->as_int();
            else if (name == "nPriceKRW") price_KRW = ait->as_int();
            else if (name == "nPriceUSD") price_USD = ait->as_int();
            else if (name == "nBonusPercentage") bonus = ait->as_int();
        }
        priceTopaz.push_back( new PriceTopaz(id, count, price_KRW, price_USD, bonus) );
    }
    
    // price starcandy
    its = nodeResult.child("price_starcandy_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "bIndex") id = ait->as_int();
            else if (name == "nCount") count = ait->as_int();
            else if (name == "nPriceTopaz") price = ait->as_int();
            else if (name == "nBonusPercentage") bonus = ait->as_int();
        }
        priceStarCandy.push_back( new PriceStarCandy(id, count, price, bonus) );
    }
    
    // magicstaff buildup info
    its = nodeResult.child("magic_staff_define_client").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nMagicStaffLevel") level = ait->as_int();
            else if (name == "nMagicStaffBonusMP") bonusMP = ait->as_int();
            else if (name == "nStarCandyCostValue") cost_starcandy = ait->as_int();
            else if (name == "nTopazCostValue") cost_topaz = ait->as_int();
        }
        magicStaffBuildupInfo.push_back( new MagicStaffBuildUpInfo(level, bonusMP, cost_starcandy, cost_topaz) );
    }
    DataProcess::SortMagicStaffBuildUpInfo(); // level 오름차순 정렬
    
    // skill slot info
    its = nodeResult.child("skill_slot_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "bSlotID") id = ait->as_int();
            else if (name == "bCostType") costType = ait->as_int();
            else if (name == "nCostValue") cost = ait->as_int();
        }
        skillSlotInfo.push_back( new SkillSlotInfo(id, costType, cost) );
    }
    
    /*
    // prerequisite info
    its = nodeResult.child("prerequisite_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nPrerequisiteID") id = ait->as_int();
            else if (name == "bCategory") category = ait->as_int();
            else if (name == "nType") type = ait->as_int();
            else if (name == "nValue1") value1 = ait->as_int();
            else if (name == "nValue2") value2 = ait->as_int();
        }
        prerequisiteInfo.push_back( new PrerequisiteInfo(id, category, type, value1, value2) );
    }
    */
    
    // fairy info
    its = nodeResult.child("fairy_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nFairyID") id = ait->as_int();
            else if (name == "nFairyType") type = ait->as_int();
            else if (name == "nFairyGrade") grade = ait->as_int();
            else if (name == "nStarCandyCostValue") cost_starcandy = ait->as_int();
            else if (name == "nTopazCostValue") cost_topaz = ait->as_int();
            else if (name == "nPrerequisiteID") pid = ait->as_int();
        }
        fairyInfo.push_back( new FairyInfo(id, type, grade, cost_starcandy, cost_topaz, pid) );
    }
    
    // fairy buildup info
    its = nodeResult.child("fairy_detail_define_client").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nFairyID") id = ait->as_int();
            else if (name == "nFairyLevel") level = ait->as_int();
            else if (name == "nFairyEffectValue") ability = ait->as_int();
            else if (name == "nFairyEffectRefID") refId = ait->as_int();
            else if (name == "nStarCandyCostValue") cost_starcandy = ait->as_int();
            else if (name == "nTopazCostValue") cost_topaz = ait->as_int();
        }
        fairyBuildUpInfo.push_back( new FairyBuildUpInfo(id, level, ability, refId, cost_starcandy, cost_topaz) );
    }
    
    // skill info
    its = nodeResult.child("skill_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nSkillID") id = ait->as_int();
            else if (name == "charSkillName") skillName = ait->as_string();
            else if (name == "bSkillType") type = ait->as_int();
            else if (name == "nSkillMaxLevel") maxLevel = ait->as_int();
            else if (name == "nRequireMP") mp = ait->as_int();
            else if (name == "nRequireStaffLevel") staffLv = ait->as_int();
            else if (name == "nRequireSkillID") skillId = ait->as_int();
            else if (name == "nRequireSkillLevel") skillLv = ait->as_int();
            else if (name == "bIsActive") isActive = ait->as_int();
        }
        skillInfo.push_back( new SkillInfo(id, skillName, type, maxLevel, mp, staffLv, skillId, skillLv, isActive) );
    }
    
    // skill buildup info
    its = nodeResult.child("skill_detail_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nSkillID") id = ait->as_int();
            else if (name == "charSkillName") skillName = ait->as_string();
            else if (name == "nSkillLevel") level = ait->as_int();
            else if (name == "nSkillMaxExp") maxExp = ait->as_int();
            else if (name == "nSkillEffectValue") ability = ait->as_int();
            else if (name == "nSkillEffectValue2") ability2 = ait->as_int();
            else if (name == "nSkillEffectProbability") prob = ait->as_int();
            else if (name == "nStarCandyCostValue") cost_starcandy = ait->as_int();
        }
        skillBuildUpInfo.push_back( new SkillBuildUpInfo(id, skillName, level, maxExp, ability, ability2, prob, cost_starcandy) );
    }
    
    // skill property buy info
    its = nodeResult.child("skill_properties_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "bSkillPropertiesID") id = ait->as_int();
            else if (name == "nCostValue") cost_topaz = ait->as_int();
        }
        
        skillPropertyInfo.push_back( new SkillPropertyInfo(id, cost_topaz) );
    }
}

void Splash::WriteResFile(char* data, int size)
{
    std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + "gamedata.xml";
    CCLog("filepath = %s", filepath.c_str());
    
    FILE* ptr_fp;
    if ((ptr_fp = fopen(filepath.c_str(), "wb")) == NULL)
    {
        CCLog("FILE OPEN ERROR !");
        exit(1);
    }
    
    size_t realSize = strlen(data);
    size_t fwriteSize = fwrite(data, 1, realSize, ptr_fp);
    if (fwriteSize != realSize)
    {
        CCLog("FILE WRITE ERROR !");
        exit(1);
    }
    fclose(ptr_fp);
    CCLog("FILE WRITE done~");
    
    // UserDefault에 바뀐 gameVersion 저장.
    iGameVersion = gameVersion;
    CCUserDefault::sharedUserDefault()->setIntegerForKey("gameVersion", iGameVersion);
    
    // 새 리소스 XML parsing
    m_pMsgLabel->setString("못생긴 리소스 배치 중...");
    XMLParseGameData();
    
    // 로그인 시도
    TryLogin();
}

void Splash::XmlParseLogin(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 11) // 블록당한 유저
            Common::ShowPopup(this, "Splash", "NoImage", false, YOU_WERE_BLOCKED, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
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
        myInfo->Init(mKakaoId, mDeviceType, userId, kakaoMsg, pushNoti, potionMsg, msgCnt, sessionId);
        
        // get GCM key
        gcmKey = nodeResult.child("gcm-key").text().as_string();
        
        // get public-key for RSA
        publicKey = nodeResult.child("public-key").text().as_string();
        publicKeyIndex = nodeResult.child("public-key-index").text().as_int();
        CCLog("gcm key = %s", gcmKey.c_str());
        CCLog("public key = %s", publicKey.c_str());
        CCLog("public key idx = %d", publicKeyIndex);
        
        // rsa 만들기 (초기 1회만 만들면 됨)
        //rsa = createRSA((unsigned char*)(publicKey[myInfo->GetKeyValue()-10].c_str()), 1);
        rsa = createRSA((unsigned char*)(publicKey.c_str()), 1);
    
        
        // 내 모든 정보 요청
        m_pMsgLabel->setString("나의 정보를 요청 중...");
        
        // required parameter values
        char temp[255];
        std::string param = "";
        sprintf(temp, "kakao_id=%d", mKakaoId);
        param += temp;
        
        Network::HttpPost(param, URL_USERINFO, this, httpresponse_selector(Splash::onHttpRequestCompleted));
    }
}

void Splash::XmlParseMyInfo(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 내 모든 정보를 받는다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int practiceUserSkillId = nodeResult.child("coco").attribute("practice-user-skill-id").as_int();
        
        int highScore = nodeResult.child("score").attribute("high-score").as_int();
        int weeklyHighScore = nodeResult.child("score").attribute("weekly-high-score").as_int();
        int certificateType = nodeResult.child("score").attribute("certificate-type").as_int();
        int remainWeeklyRankTime = nodeResult.child("score").attribute("remain-weekly-rank-time").as_int();
        int lastWeeklyHighScore = nodeResult.child("score").attribute("last-weekly-high-score").as_int();
        int isWeeklyRankReward = nodeResult.child("score").attribute("is-weekly-rank-reward").as_int();
        
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
        
        myInfo->InitRestInfo(topaz, starcandy, mp, mpStaffPercent, mpFairy, staffLv, highScore, weeklyHighScore, lastWeeklyHighScore, isWeeklyRankReward, certificateType, remainWeeklyRankTime, item1, item2, item3, item4, item5, potion, remainPotionTime, fire, water, land, master);
        
        int profileSkillId = nodeResult.child("profile-skill").attribute("id").as_int();
        int profileSkillLv = nodeResult.child("profile-skill").attribute("level").as_int();
        myInfo->SetProfileSkill(profileSkillId, profileSkillLv);
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-slot").children("slot");
        int id, csi, usi;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
            }
            myInfo->AddSkillSlot(id, csi, usi);
        }
        
        its = nodeResult.child("fairy-list").children("fairy");
        int cfi, ufi, level, isUse;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-fairy-id") cfi = ait->as_int();
                else if (name == "user-fairy-id") ufi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "is-use") isUse = ait->as_int();
            }
            myInfo->AddFairy(cfi, ufi, level, isUse);
        }
        
        its = nodeResult.child("skill-list").children("skill");
        int exp;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "exp") exp = ait->as_int();
            }
            myInfo->AddSkill(csi, usi, level, exp);
            
            // 현재 연습 중인 스킬 id 가져오기
            if (usi == practiceUserSkillId)
            {
                myInfo->SetPracticeSkill(csi, level);
            }
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        // 오늘의 별사탕 관련 정보
        int todayCandyType = nodeResult.child("today-starcandy-info").attribute("type").as_int();
        int todayCandyValueChoice = nodeResult.child("today-starcandy-info").attribute("value-choice").as_int();
        int todayCandyValueMiss = nodeResult.child("today-starcandy-info").attribute("value-miss").as_int();
        int istodayCandyUsed = nodeResult.child("today-starcandy-info").attribute("today-use").as_int();
        myInfo->SetTodayCandy(todayCandyType, todayCandyValueChoice, todayCandyValueMiss, istodayCandyUsed);
        
        char temp[50];
        std::string param;
        if (isWeeklyRankReward == 0 && lastWeeklyHighScore != -1)
        {
            // 저번주 주간랭킹 결과 불러온다.
            m_pMsgLabel->setString("지난 주 상이 있는지 힐끔 바라보는 중...");
            
            sprintf(temp, "kakao_id=%d", mKakaoId);
            param += temp;
            
            Network::HttpPost(param, URL_WEEKLYRANK, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
        else
        {
            // 친구 리스트 정보를 받는다.
            m_pMsgLabel->setString("못생긴 친구들을 불러오는 중...");
            
            sprintf(temp, "kakao_id=%d", mKakaoId);
            param += temp;
            
            httpStatus++;
            Network::HttpPost(param, URL_FRIENDLIST, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
    }
}

void Splash::XmlParseRewardWeeklyRank(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10 || code == 11)
        {
            // code 10 : 지난 주 게임 전혀 하지 않음.
            // code 11 : 지난 주 점수 업데이트가 되어있지 않음.
            Common::ShowPopup(this, "Splash", "NoImage", false, NEED_TO_REBOOT, BTN_1, nullData);
        }
        else if (code == 12)
            CCLog("Splash : 이미 보상을 받은 경우");
        else
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        myRank = nodeResult.child("my-rank").attribute("rank").as_int();
        myLastWeekHighScore = nodeResult.child("my-rank").attribute("last-week-high-score").as_int();
        rewardType = nodeResult.child("my-rank").attribute("reward-type").as_int();
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("friend-rank-list").children("friend");
        std::string nickname, profileUrl;
        int rank, lastWeekHighScore, isFriend;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "nick-name") nickname = ait->as_string();
                else if (name == "profile-url") profileUrl = ait->as_string();
                else if (name == "rank") rank = ait->as_int();
                else if (name == "last-week-high-score") lastWeekHighScore = ait->as_int();
                else if (name == "is-friend") isFriend = ait->as_int();
            }
            
            // nickname 너무 길면 자르자.
            nickname = SubstrNickname(nickname);
            
            lastWeeklyRank.push_back( new LastWeeklyRank(nickname, profileUrl, rank, lastWeekHighScore, isFriend) );
            if (ProfileSprite::GetProfile(profileUrl) == NULL) // 프로필 sprite에 모은다.
                profiles.push_back( new ProfileSprite(profileUrl) );
        }
        

        // 친구 리스트 정보를 받는다.
        m_pMsgLabel->setString("못생긴 친구들을 불러오는 중...");
        
        char temp[50];
        std::string param;
        sprintf(temp, "kakao_id=%d", mKakaoId);
        param += temp;

        Network::HttpPost(param, URL_FRIENDLIST, this, httpresponse_selector(Splash::onHttpRequestCompleted));
    }
}

void Splash::XmlParseFriends(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        int kakaoId;
        std::string nickname;
        std::string imageUrl;
        int potionMsgStatus;
        int weeklyHighScore;
        int scoreUpdateTime;
        int remainPotionTime;
        int remainRequestPotionTime;
        int remainRequestTopazTime;
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
                else if (name == "remain-request-potion-send-time") remainRequestPotionTime = ait->as_int();
                else if (name == "remain-request-topaz-send-time") remainRequestTopazTime = ait->as_int();
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
            
            // nickname 너무 길면 자르자.
            nickname = SubstrNickname(nickname);
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, remainRequestPotionTime, remainRequestTopazTime, weeklyHighScore, highScore, scoreUpdateTime, certificateType, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            // potion image 처리
            friendList[(int)friendList.size()-1]->SetPotionSprite();
            
            if (ProfileSprite::GetProfile(imageUrl) == NULL) // 프로필 sprite에 모은다.
                profiles.push_back( new ProfileSprite(imageUrl) );
        }
        // sort by { max[weeklyScore], min[scoreUpdateTime] }
        DataProcess::SortFriendListByScore();
        
        
        // get image by url (다 받으면 Ranking으로 넘어간다)
        profileCnt = 0;
        m_pMsgLabel->setString("친구의 못생긴 얼굴 지적하는 중...");
        char tag[5];
        for (int i = 0 ; i < profiles.size() ; i++)
        {
            if (profiles[i]->GetProfileUrl() == "")
            {
                profiles[i]->SetSpriteNoImage();
                profileCnt++;
            }
        }
        for (int i = 0 ; i < profiles.size() ; i++)
        {
            if (profiles[i]->GetProfileUrl() != "")
            {
                // get profile image sprite from URL
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl(profiles[i]->GetProfileUrl().c_str());
                req->setRequestType(CCHttpRequest::kHttpPost);
                req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
                sprintf(tag, "%d", i);
                req->setTag(tag);
                CCHttpClient::getInstance()->send(req);
                req->release();
            }
        }
    }
}


void Splash::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    /*
    // LOGIN 프로토콜 응답의 경우, response header에 있는 set-cookie의 session 값을 들고온다.
    if (httpStatus == HTTP_LOGIN)
    {
        std::string ss = "";
        std::vector<char> *buffer2 = res->getResponseHeader();
        for (unsigned int i = 0 ; i < buffer2->size() ; i++)
            ss += (*buffer2)[i];
        int pos = ss.find("Set-Cookie:");
        ss = ss.substr(pos+12, ss.size()-1);
        ss = ss.substr( ss.find("PS=")+3, ss.find("; path=")-3 );
        sessionId = ss;
        CCLog("session ID = %s", sessionId.c_str());
    }
    */

    // parse xml data
    httpStatus++;
    switch (httpStatus-1)
    {
        case HTTP_VERSION:
            XmlParseVersion(&xmlDoc); break;
        case HTTP_LOGIN:
            XmlParseLogin(&xmlDoc); break;
        case HTTP_MYINFO:
            XmlParseMyInfo(&xmlDoc); break;
        case HTTP_REWARDWEELYRANK:
            XmlParseRewardWeeklyRank(&xmlDoc); break;
        case HTTP_FRIENDS:
            XmlParseFriends(&xmlDoc); break;
    }
}

void Splash::onHttpRequestCompletedNoEncrypt(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;
    char dumpData[BUFFER_SIZE];
    
    // 프로필 사진 or resource.xml 받아올 때
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

    
    // gameVersion 변경으로 resource XML 파일 받았을 경우
    if (atoi(res->getHttpRequest()->getTag()) == 999)
    {
        WriteResFile(dumpData, (int)buffer->size());
    }
    // 프로필 이미지 받아오기
    else
    {
        // make texture2D
        CCImage* img = new CCImage;
        img->initWithImageData(dumpData, (int)buffer->size());
        CCTexture2D* texture = new CCTexture2D();
        texture->initWithImage(img);
        
        // set CCSprite
        int index = atoi(res->getHttpRequest()->getTag());
        profiles[index]->SetSprite(texture);
        
        profileCnt++;
        if (profileCnt == (int)profiles.size())
        {
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            GetNonConsumedItems();
            //LastActionStart();
            #endif
            
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            // 1) 로고랑 글자를 없앤다.
            // 2) 배경화면 축소하면서 Ranking 시작.
            LastActionStart();
            #endif
        }
    }
}

void Splash::GetNonConsumedItems()
{
    m_pMsgLabel->setString("새로운 아이템을 쳐다보는 중...");
    httpStatus = HTTP_NONCONSUMEDITEMS;

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t,
                                       "com/playDANDi/CocoMagic/CocoMagic",
                                       "StartIAB",
                                       "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        char myKakaoId[20];
        sprintf(myKakaoId, "%d", myInfo->GetKakaoId());
        
        // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
        t.env->CallStaticVoidMethod(t.classID, t.methodID,
                                    0,
                                    -1, // topazid
                                    t.env->NewStringUTF(myKakaoId), // kakaoid
                                    t.env->NewStringUTF(""), // friendkakaoid
                                    t.env->NewStringUTF(""), // productid
                                    t.env->NewStringUTF(""), // payload
                                    t.env->NewStringUTF(gcmKey.c_str())
                                    );
        // Release
        t.env->DeleteLocalRef(t.classID);
    }
    #endif
}
void Splash::GetTodayCandyFriend()
{
    m_pMsgLabel->setString("오늘의 별사탕 그룹 구성 중...");
    char name[15];
    int kakaoId;
    bool flag;
    
    // 내 kakao id부터 넣자.
    todayCandyKakaoId.clear();
    todayCandyKakaoId.push_back(myInfo->GetKakaoId());
    
    for (int i = 1 ; i < 5; i++)
    {
        sprintf(name, "todayCandy_%d", i);
        kakaoId = CCUserDefault::sharedUserDefault()->getIntegerForKey(name, -1);
        flag = false;
        for (int j = 0 ; j < friendList.size() ; j++)
        {
            if (friendList[j]->GetKakaoId() == kakaoId)
            {
                todayCandyKakaoId.push_back(kakaoId);
                flag = true;
                break;
            }
        }
        if (!flag)
            todayCandyKakaoId.push_back(-1);
        
        CCUserDefault::sharedUserDefault()->setIntegerForKey(name, todayCandyKakaoId[todayCandyKakaoId.size()-1]);
    }
}

void Splash::LastActionStart()
{
    Splash* pThis = (Splash*)Depth::GetCurPointer();

    // 오늘의 별사탕 친구 목록 받아오기
    pThis->GetTodayCandyFriend();

    // 이제 시작
    CCActionInterval* action = CCSequence::create(CCEaseBounceIn::create( CCMoveBy::create(0.5f, ccp(0, 900)) ),
                                               //   CCDelayTime::create(0.5f),
                        CCCallFuncND::create(pThis, callfuncND_selector(Splash::LastActionCallback), NULL), NULL);
    pThis->m_pTitle->runAction(action);

    pThis->m_pMsgLabel->setOpacity(0);
}

void Splash::LastActionCallback(CCNode* sender, void* data)
{
    CCActionInterval* action = CCSequence::create(CCScaleTo::create(1.0f, 1.0f),
                        CCCallFuncND::create(this, callfuncND_selector(Splash::LastActionCallback2), NULL), NULL);
    m_pBackground->runAction(CCEaseIn::create(action, 1.0f));
}

void Splash::LastActionCallback2(CCNode* sender, void *data)
{
    EndScene();
    Common::ShowNextScene(this, "Splash", "Ranking", true);
}

std::string Splash::SubstrNickname(std::string nickname)
{
    if (nickname.size() > 20)
    {
        //CCLog("%s", nickname.c_str());
        int i;
        for (i = 0 ; i < nickname.size() ; i++)
        {
            if (i >= 20)
                break;
            if (isascii(nickname[i]) != 1)
                i += 2;
        }
        nickname = nickname.substr(0, i);
        nickname += "...";
    }
    return nickname;
}

void Splash::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    m_pBackground->removeFromParentAndCleanup(true);
    m_pForKakao->removeFromParentAndCleanup(true);
    m_pTitle->removeFromParentAndCleanup(true);
    m_pMsgLabel->removeFromParentAndCleanup(true);
    m_pStartBtn->removeFromParentAndCleanup(true);
    m_pStartLetter->removeFromParentAndCleanup(true);
    
    if (m_pEditName != NULL)
        m_pEditName->removeFromParentAndCleanup(true);
}

