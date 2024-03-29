#include <regex>
#include "Splash.h"
#include "pugixml/pugixml.hpp"
#include "Kakao/Plugins/KakaoNativeExtension.h"
#include "Kakao/Common/KakaoLocalUser.h"
#include "Kakao/Common/KakaoFriends.h"
#include "support/zip_support/unzip.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "bridge.h"
#endif

using namespace pugi;
using namespace cocos2d;

static std::vector<std::string> addedList;
static std::vector<std::string> deletedList;
static bool isFriendListChecked;
static bool isJoinNeeded;
static std::string fKakaoId;


Splash::~Splash(void)
{
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
    CCLayer::onEnter();
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    CCActionInterval* action = CCSequence::create( CCFadeIn::create(0.5f), CCDelayTime::create(1.5f), CCCallFuncND::create(this, callfuncND_selector(Splash::Callback_Logo_KakaoGame), this), NULL );
    CCActionInterval* action2 = CCSequence::create( CCFadeIn::create(0.5f), CCDelayTime::create(1.5f), NULL );
    this->runAction(action2);
    m_pBackground->runAction(action);
    #endif
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    CCActionInterval* action = CCSequence::create( CCDelayTime::create(1.3f), CCCallFuncND::create(this, callfuncND_selector(Splash::Callback_Logo_playDANDi), this), NULL );
    m_pBackground->runAction(action);
    #endif
}
void Splash::onExit()
{
    //CCLog("Splash :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}


void Splash::keyBackClicked()
{
    if (isLoading || isKeybackTouched) // 백버튼 팝업창이 떠 있거나, 데이터 로딩중에는 못 누르게 함.
        return;
    isKeybackTouched = true;
    
    sound->playWarning();
    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Splash", "NoImage", false, POPUP_EXIT, BTN_2, nullData);
}


void Splash::onInitComplete()
{
    //CCLog("onInitComplete");
    //CCLog("token : %s", CCUserDefault::sharedUserDefault()->getStringForKey("refresh_token").c_str());
    KakaoNativeExtension::getInstance()->auth(std::bind(&Splash::onAuthComplete, this, std::placeholders::_1), std::bind(&Splash::onAuthErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
}
void Splash::onInitErrorComplete(const char* status, const char* error)
{
    //CCLog("onInitErrorComplete : %s , %s", status, error);
}

void Splash::onAuthComplete(bool result)
{
    needToLoginForKakao = !result;
    
    if (result && isJoinNeeded && !stopHere) // join을 해야 함.
    {
        stopHere = true; // 처음 로그인할 때 auth가 두 번 호출되어서 이 flag로 막도록 조치함.
        m_pMsgLabel->setString("잠시만 기다려주세요.");
        KakaoNativeExtension::getInstance()->localUser(std::bind(&Splash::onLocalUserComplete, this), std::bind(&Splash::onLocalUserErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    }
    waitTouch = false;
}
void Splash::onAuthErrorComplete(char const* status, char const* error)
{
    //CCLog("onAuthErrorComplete : %s, %s", status, error);
    waitTouch = false;
}

void Splash::onTokenComplete()
{
    KakaoNativeExtension::getInstance()->auth(std::bind(&Splash::onAuthComplete, this, std::placeholders::_1), std::bind(&Splash::onAuthErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    //CCLog("access_token : %s", CCUserDefault::sharedUserDefault()->getStringForKey("access_token").c_str());
}
void Splash::onTokenErrorComplete(char const* status, char const* error)
{
    //CCMessageBox(error, "onTokenErrorComplete");
    //CCLog("onTokenErrorComplete : %s, %s", status, error);
}

void Splash::onLoginComplete()
{
    //CCLog("onLoginComplete");
    isJoinNeeded = true;
    m_pKakaoBtn->removeFromParentAndCleanup(true);
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    m_pGuestBtn->removeFromParentAndCleanup(true);
    #endif
    
    KakaoNativeExtension::getInstance()->auth(std::bind(&Splash::onAuthComplete, this, std::placeholders::_1), std::bind(&Splash::onAuthErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
}
void Splash::onLoginErrorComplete(char const* status, char const* error)
{
    //CCLog("onLoginErrorComplete : %s, %s", status, error);
    waitTouch = false;
}

void Splash::onLocalUserComplete()
{
    //CCLog("onLocalUserComplete");
    isKeybackTouched = true;
    
    // 그 다음 친구목록 불러온다.
    KakaoNativeExtension::getInstance()->friends(std::bind(&Splash::onFriendsComplete, this), std::bind(&Splash::onFriendsErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
}
void Splash::onLocalUserErrorComplete(char const* status, char const* error)
{
    //CCMessageBox(error, "onLocalUserErrorComplete");
    //CCLog("onLocalUserErrorComplete : %s, %s", status, error);
    isTouched = false;
    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Splash", "NoImage", false, NEED_TO_REBOOT, BTN_1, nullData);
}

void Splash::onFriendsComplete()
{
    //CCLog("onFriendsComplete");
    
    //CCLog("친구 수 : %d", KakaoFriends::getInstance()->appFriends->count());
    //CCLog("X 친구 수 : %d", KakaoFriends::getInstance()->friends->count());
    
    isTouched = false;
    isLoading = true;
    
    StartProtocol();
}
void Splash::onFriendsErrorComplete(char const* status, char const* error)
{
    //CCMessageBox(error, "onFriendsErrorComplete");
    //CCLog("onFriendsErrorComplete : %s, %s", status, error);
    isTouched = false;
    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Splash", "NoImage", false, NEED_TO_REBOOT, BTN_1, nullData);
}

void Splash::StartProtocol()
{
    // mt.php를 호출한다. 존재하지 않으면 그냥 넘어가고,
    // 존재한다면 그 안의 <message>...</message> 내용을 불러와서 '서버점검중' 팝업을 띄우고 진행하지 못하게 한다.
    m_pMsgLabel->setString("서버 점검 테스트 중...");
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(URL_MT);
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setTag("8888888");
    req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
    CCHttpClient::getInstance()->send(req);
    req->release();
}

bool Splash::init()
{
	if (!CCLayerColor::initWithColor(ccc4(255, 235, 0, 255)))
		return false;
    this->setOpacity(0);
    
    // for kakao
    if (!isRebooting)
    {
        KakaoNativeExtension::getInstance()->init(std::bind(&Splash::onInitComplete, this), std::bind(&Splash::onInitErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
        KakaoNativeExtension::getInstance()->tokenListener(std::bind(&Splash::onTokenComplete, this), std::bind(&Splash::onTokenErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    }
    
    // add depth
    Depth::AddCurDepth("Splash", this);
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Splash::Notification), "Splash", NULL);
    
    // 기본적으로 필요한 이미지 리소스
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/popup.plist");
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    vs = CCDirector::sharedDirector()->getVisibleSize();
    vo = CCDirector::sharedDirector()->getVisibleOrigin();
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    float logo_h = vs.height / (6.7f) * 3.0f;
    float size_h = vs.height / 6.7f;
    m_pBackground = CCSprite::create("images/splash/kakao/splash_logo.png");
    m_pBackground->setAnchorPoint(ccp(0.5, 0));
    m_pBackground->setPosition(ccp(winSize.width/2, vo.y+logo_h));
    float origin_h = m_pBackground->getContentSize().height;
    m_pBackground->setScale(size_h / origin_h);
    m_pBackground->setOpacity(0);
    this->addChild(m_pBackground, 1);
    #endif
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    this->setOpacity(255);
    this->setColor(ccc3(255,255,255));
    m_pBackground = CCSprite::create("images/splash/logo_playdandi_text.png");
    m_pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(m_pBackground, 0);
    m_pBackground2 = CCSprite::create("images/splash/logo_playdandi_copyright.png");
    m_pBackground2->setAnchorPoint(ccp(0.5, 0));
    m_pBackground2->setPosition(ccp(winSize.width/2, vo.y+50));
    this->addChild(m_pBackground2, 0);
    #endif 
    
    
    savedMyPotionTime = -1;
    
    isKakaoLoading = false;
    isStarting = false;
    isLoading = false;
    isInGame = false;
    isInGameTutorial = false;
    isRebooting = false;
    isFriendListChecked = false;
    isJoinNeeded = false;
    isTouched = false;
    waitTouch = false;
    stopHere = false;
    m_pKakaoBtn = NULL;
    
    isKeybackTouched = true; // 약관동의가 뜨는 상황이면 그 때 false로 바꾸자.
    httpStatus = 0;
    
    isInviteListGathered = false; // 친구초대 리스트에 쓰일 변수
    
    sound = new Sound();
    
	return true;
}

void Splash::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 게스트로그인 확인 버튼 누른 직후
        
        isGuestLogin = true;
        //CCLog("guest kakao id = %s", CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "").c_str());
        
        if (CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "") == "")
            isJoinNeeded = true;
        else
            isJoinNeeded = false;
        isLoading = true;
        
        m_pKakaoBtn->removeFromParentAndCleanup(true);
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        m_pGuestBtn->removeFromParentAndCleanup(true);
        #endif
        
        StartProtocol();
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        if (needToLoginForKakao)
        {
            isTouched = false;
            isKeybackTouched = false;
        }
        if (m_pKakaoBtn != NULL && needToLoginForKakao)
            m_pKakaoBtn->setColor(ccc3(255,255,255));
    }
}



void Splash::Callback_Logo_KakaoGame(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
    sender = NULL;
    
    this->setColor(ccc3(255,255,255));
    m_pBackground = CCSprite::create("images/splash/logo_playdandi_text.png");
    m_pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(m_pBackground, 0);
    m_pBackground2 = CCSprite::create("images/splash/logo_playdandi_copyright.png");
    m_pBackground2->setAnchorPoint(ccp(0.5, 0));
    m_pBackground2->setPosition(ccp(winSize.width/2, vo.y+50));
    this->addChild(m_pBackground2, 0);

    CCActionInterval* action = CCSequence::create( CCDelayTime::create(1.3f), CCCallFuncND::create(this, callfuncND_selector(Splash::Callback_Logo_playDANDi), this), NULL );
    m_pBackground->runAction(action);
}

void Splash::Callback_Logo_playDANDi(CCNode* sender, void* p)
{
    sender->removeFromParentAndCleanup(true);
    sender = NULL;
    m_pBackground2->removeFromParentAndCleanup(true);
    this->setColor(ccc3(0,0,0));
    
    // 배경 액션
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    m_pBackground = CCSprite::create("images/splash/main_background.png", CCRectMake(0, 0, 1080, 1920));
    m_pBackground->setPosition(ccp(winSize.width/2, winSize.height/2));
    m_pBackground->setScale(1.2f);
    m_pBackground->setOpacity(0);
    this->addChild(m_pBackground, 0);
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    
    CCActionInterval* action = CCSequence::create(CCFadeIn::create(1.0f),CCCallFunc::create(this, callfunc_selector(Splash::LogoLoadingCompleted)), NULL);
    m_pBackground->runAction(action);
}

void Splash::LogoLoadingCompleted()
{
    // logo
    m_pTitle = CCSprite::create("images/splash/Title.png"); //CCSprite::createWithSpriteFrameName("background/Title.png");
    m_pTitle->setPosition(ccp(winSize.width/2, 1350+1000));
    this->addChild(m_pTitle, 5);

    m_pForKakao = CCSprite::create("images/splash/letter_forkakao.png"); //CCSprite::createWithSpriteFrameName("letter/letter_forkakao.png");
    m_pForKakao->setAnchorPoint(ccp(0, 1));
    m_pForKakao->setScale(0.8f);
    m_pForKakao->setPosition(ccp(winSize.width/2+20, 130));
    m_pTitle->addChild(m_pForKakao, 6);
    
    CCActionInterval* action = CCSequence::create(CCEaseBounceOut::create(CCMoveTo::create(0.5f, ccp(winSize.width/2, 1350))), CCCallFuncND::create(this, callfuncND_selector(Splash::SoundCallback), NULL), CCDelayTime::create(0.7f), CCCallFunc::create(this, callfunc_selector(Splash::Button_Callback)), NULL);
    m_pTitle->runAction(action);
    
    // 클라이언트에 들고 있던 kakao ID, device Type 불러오기
    if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        mDeviceType = 2;
    else if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        mDeviceType = 1;
    else
        mDeviceType = 3;
    
    // Label 생성
    m_pMsgLabel = CCLabelTTF::create("", fontList[0].c_str(), 40);
    m_pMsgLabel->setAnchorPoint(ccp(0.5, 0.5));
    m_pMsgLabel->setPosition(ccp(winSize.width/2, 400));
    m_pMsgLabel->setColor(ccc3(0, 0, 0));
    this->addChild(m_pMsgLabel, 5);
  
    // 버전 세팅
    /*
    iGameVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion", -1);
    iBinaryVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("binaryVersion", 2000);
    */
    
    iGameVersion = CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion", -1);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t,
                                       "com/playDANDi/CocoMagic/CocoMagic",
                                       "GetBinaryVersion",
                                       "()I"))
    {
        binaryVersion_current = (jint)t.env->CallStaticObjectMethod(t.classID,t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
#endif
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    std::string ver = GetBundleVersion();
    int a = atoi( ver.substr(0, ver.find(".")).c_str() );
    ver = ver.substr(ver.find(".")+1);
    int b = atoi( ver.substr(0, ver.find(".")).c_str() );
    ver = ver.substr(ver.find(".")+1);
    int c = atoi( ver.c_str() );
    binaryVersion_current = a*1000 + b*100 + c;
#endif
}

void Splash::SoundCallback(CCNode* sender, void* p)
{
    // sound
    //sound->PlayVoice(VOICE_TITLE);
}

void Splash::Button_Callback()
{
    // 터치 활성화
    isTouched = true;
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    if (needToLoginForKakao)
    {
        ShowClause();
        isKeybackTouched = false;
    }
    else
    {
        m_pMsgLabel->setString("카카오 관련 정보를 불러오는 중...");
        KakaoNativeExtension::getInstance()->localUser(std::bind(&Splash::onLocalUserComplete, this), std::bind(&Splash::onLocalUserErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Splash::ShowClause()
{
    isChecked1 = isChecked2 = false;
    int ofs = 185;
    
    term1 = CCSprite::create("images/splash/term_private.png");
    term1->setAnchorPoint(ccp(0, 0));
    term1->setPosition(ccp(100+ofs, 191+50+50));
    term1->setScale(0.9f);
    this->addChild(term1, 10);
    
    term2 = CCSprite::create("images/splash/term_service.png");
    term2->setAnchorPoint(ccp(0, 0));
    term2->setScale(0.9f);
    term2->setPosition(ccp(100+ofs, 191+50+150));
    this->addChild(term2, 10);
    
    agreeBtn1 = CCSprite::create("images/splash/btn_clause_agree.png");
    agreeBtn1->setAnchorPoint(ccp(1, 0));
    agreeBtn1->setScale( 80 / agreeBtn1->getContentSize().width );
    agreeBtn1->setContentSize(CCSize(80, 80));
    agreeBtn1->setColor(ccc3(120,120,120));
    agreeBtn1->setPosition(ccp(90+ofs, 191+50+50-15));
    this->addChild(agreeBtn1, 10);
    
    agreeBtn2 = CCSprite::create("images/splash/btn_clause_agree.png");
    agreeBtn2->setAnchorPoint(ccp(1, 0));
    agreeBtn2->setScale( 80 / agreeBtn2->getContentSize().width );
    agreeBtn2->setContentSize(CCSize(80, 80));
    agreeBtn2->setColor(ccc3(120,120,120));
    agreeBtn2->setPosition(ccp(90+ofs, 191+50+150-15));
    this->addChild(agreeBtn2, 10);
    
    check1 = CCSprite::create("images/splash/icon_check.png");
    check1->setPosition(ccp(90-agreeBtn1->getContentSize().width/2+ofs-30, 191+50+50+20));
    check1->setOpacity(0);
    this->addChild(check1, 11);
    
    check2 = CCSprite::create("images/splash/icon_check.png");
    check2->setPosition(ccp(90-agreeBtn2->getContentSize().width/2+ofs-30, 191+50+150+20));
    check2->setOpacity(0);
    this->addChild(check2, 11);
    
    isTouched = false;
}

void Splash::ShowKakaoBtn()
{
    m_pKakaoBtn = CCSprite::create("images/splash/kakao/kakao_login.png");
    m_pKakaoBtn->setPosition(ccp(winSize.width/2, 191+50));
    m_pKakaoBtn->setScale(4.0f/3.0f);
    this->addChild(m_pKakaoBtn, 3);
    
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    m_pGuestBtn = CCLabelTTF::create("게스트 로그인 >", fontList[0].c_str(), 42);
    m_pGuestBtn->setColor(ccc3(0,0,0));
    m_pGuestBtn->setAnchorPoint(ccp(1, 0));
    m_pGuestBtn->setPosition(ccp(winSize.width/2+400, 191+50 +50+40));
    /*
    m_pGuestBtn = CCSprite::createWithSpriteFrameName("button/btn_guestlogin.png");
    m_pGuestBtn->setPosition(ccp(winSize.width/2, 191+50 +100 +50));
    */
    this->addChild(m_pGuestBtn, 3);
    #endif
    
    isTouched = false;
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
    if(rsa == NULL)
    {
        //CCLog("res null");
        exit(0);
        printf( "Failed to create RSA");
    }
    
    return rsa;
}


bool Splash::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCLog("%d %d %d", isTouched, waitTouch, isLoading);
    //CCLog("needtologin = %d", needToLoginForKakao);
    if (waitTouch)
        return false;
    if (isLoading)
        return true;
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    
    // 개인정보취급방침 or 이용약관 클릭 시 브라우저 실행
    if (!(isChecked1 && isChecked2))
    {
        if (term1->boundingBox().containsPoint(point))
        {
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            JniMethodInfo minfo;
            if(JniHelper::getStaticMethodInfo(minfo,
                                              "com/playDANDi/CocoMagic/CocoMagic",
                                              "openURL",
                                              "(Ljava/lang/String;)V"))
            {
                jstring StringArg1 = minfo.env->NewStringUTF("private");
                minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, StringArg1);
                minfo.env->DeleteLocalRef(StringArg1);
                minfo.env->DeleteLocalRef(minfo.classID);
            }
            #endif
            
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCApplication::sharedApplication()->openURL(URL_TERM_PRIVATE);
            isTouched = false;
            #endif
            return true;
        }
        else if (term2->boundingBox().containsPoint(point))
        {
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            JniMethodInfo minfo;
            if(JniHelper::getStaticMethodInfo(minfo,
                                              "com/playDANDi/CocoMagic/CocoMagic",
                                              "openURL",
                                              "(Ljava/lang/String;)V"))
            {
                jstring StringArg1 = minfo.env->NewStringUTF("service");
                minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, StringArg1);
                minfo.env->DeleteLocalRef(StringArg1);
                minfo.env->DeleteLocalRef(minfo.classID);
            }
            #endif
            
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCApplication::sharedApplication()->openURL(URL_TERM_SERVICE);
            isTouched = false;
            #endif
            return true;
        }
    }
    
    // 카카오 로그인이 안 되어 있는 상황
    if (needToLoginForKakao)
    {
        if (!(isChecked1 && isChecked2)) // 약관동의가 아직 체크되지 않았을 떄
        {
            if (agreeBtn1->boundingBox().containsPoint(point))
            {
                if (isChecked1) check1->setOpacity(0);
                else check1->setOpacity(255);
                isChecked1 = !isChecked1;
            }
            if (agreeBtn2->boundingBox().containsPoint(point))
            {
                if (isChecked2) check2->setOpacity(0);
                else check2->setOpacity(255);
                isChecked2 = !isChecked2;
            }
            if (isChecked1 && isChecked2) // 약관 동의 2개가 모두 체크되어, 카카오 버튼이 나타나도록 한다.
            {
                term1->removeFromParentAndCleanup(true);
                term2->removeFromParentAndCleanup(true);
                agreeBtn1->removeFromParentAndCleanup(true);
                agreeBtn2->removeFromParentAndCleanup(true);
                check1->removeFromParentAndCleanup(true);
                check2->removeFromParentAndCleanup(true);
                
                ShowKakaoBtn();
            }
        }
        else // 약관동의는 모두 끝났고, 카카오버튼 + (opt.)게스트로그인버튼 이 화면에 나타나 있는 상황.
        {
            if (m_pKakaoBtn->boundingBox().containsPoint(point)) // kakao 로그인하기
            {
                m_pKakaoBtn->setColor(ccc3(170,170,170));
                rect = m_pKakaoBtn->boundingBox();
                kind = BTN_MENU_KAKAOBTN;
                return true;
            }
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            else if (m_pGuestBtn->boundingBox().containsPoint(point))
            {
                std::vector<int> nullData;
                Common::ShowPopup(this, "Splash", "NoImage", false, GUEST_LOGIN, BTN_1, nullData);
            }
            #endif
        }
    }
    
    return true;
}

void Splash::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    
}

void Splash::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched || isLoading)
        return;

    CCPoint point = pTouch->getLocation();
    
    if (kind == BTN_MENU_KAKAOBTN)
    {
        m_pKakaoBtn->setColor(ccc3(255,255,255));
        if (rect.containsPoint(point)) // 카카오 로그인하기
        {
            isKakaoLoading = true;
            //waitTouch = true;
            isTouched = false;
            KakaoNativeExtension::getInstance()->login(std::bind(&Splash::onLoginComplete, this), std::bind(&Splash::onLoginErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
        }
    }
    
    isTouched = false;
}

void Splash::XmlParseVersion(xml_document *xmlDoc)
{
    //CCLog("xml parse : version");
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
        balanceFileUrl = nodeResult.child("balance-file-url").text().as_string();
        //int binaryVersion = nodeResult.child("binary-version").text().as_int();
        //CCLog("게임 버전 = %d", gameVersion);
        
        // 마켓 버전
        int binaryVersion_latest;
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        int binaryVersion_android = nodeResult.child("market-version").child("android").text().as_int();
        binaryVersion_latest = binaryVersion_android;
        
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "com/playDANDi/CocoMagic/CocoMagic",
                                           "GetBinaryVersion",
                                           "()I"))
        {
            binaryVersion_current = (jint)t.env->CallStaticObjectMethod(t.classID,t.methodID);
            t.env->DeleteLocalRef(t.classID);
        }
#endif
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        int binaryVersion_ios = nodeResult.child("market-version").child("ios").text().as_int();
        binaryVersion_latest = binaryVersion_ios;
        
        std::string ver = GetBundleVersion();
        int a = atoi( ver.substr(0, ver.find(".")).c_str() );
        ver = ver.substr(ver.find(".")+1);
        int b = atoi( ver.substr(0, ver.find(".")).c_str() );
        ver = ver.substr(ver.find(".")+1);
        int c = atoi( ver.c_str() );
        binaryVersion_current = a*1000 + b*100 + c;
#endif
        
        CCLog("binary = %d , %d", binaryVersion_current, binaryVersion_latest);
        CCLog("game = %d , %d", gameVersion, iGameVersion);
        
        // rsa 만들기 (초기 1회만 만들면 됨)
        rsa = createRSA((unsigned char*)(basicKey.c_str()), 1);
        
        
        // 안드로이드 설정에 따라 자동업데이트를 한 경우, 앱 버전이 서버설정 버전보다 클 수도 있다.
        // 따라서 앱 버전이 서버설정 버전보다 작을 때 업데이트를 하도록 유도하자.
        if (binaryVersion_current < binaryVersion_latest)
        {
            std::vector<int> nullData;
            Common::ShowPopup(this, "Splash", "NoImage", false, NEED_TO_UPDATE, BTN_1, nullData);
            return;
        }
        
        if (gameVersion != iGameVersion)
        {
            // 리소스 파일 정보 가져오기
            m_pMsgLabel->setString("못생긴 리소스 설득 중...");
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl(balanceFileUrl.c_str());
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
            req->setTag("9999998");
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
    //CCLog("리소스 다운로드 시간 : %ld", time(0) - resourcedownloadtime);
    
    // UserDefault에 바뀐 gameVersion 저장.
    iGameVersion = gameVersion;
    CCUserDefault::sharedUserDefault()->setIntegerForKey("gameVersion", iGameVersion);
    
    
    // 필요한 이미지 리소스 미리 로드
    Common::AddSpriteFramesWithFile("texture_1");
    Common::AddSpriteFramesWithFile("texture_2");
    Common::AddSpriteFramesWithFile("skill");
    
    // 회원가입이 필요한 경우
    if (isJoinNeeded)
    {
        m_pMsgLabel->setString("마법의 세계에 들어가는 중...");
        
        //CCLog("isGuestLogin = %d", isGuestLogin);
        //CCLog("id = %s", CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "").c_str());
        if (isGuestLogin && CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "") == "")
        {
            // 게스트로그인 처음하는 경우, 게스트로그인용 join 프로토콜을 호출한다. (join_guest.php)
            httpStatus = HTTP_JOIN_GUEST;
            
            std::string param = "";
            Network::HttpPost(param, URL_JOIN_GUEST, this, httpresponse_selector(Splash::onHttpRequestCompleted), "", "", true);
        }
        else
        {
            // 카카오 로그인을 했을 경우 회원가입을 해야 한다. (join.php)
            httpStatus = HTTP_JOIN;
            
            char temp[255];
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", KakaoLocalUser::getInstance()->userId.c_str());
            param += temp;
            sprintf(temp, "kakao_user_type=1");
            param += temp;
            Network::HttpPost(param, URL_JOIN, this, httpresponse_selector(Splash::onHttpRequestCompleted), "", "", true);
        }
    }
    
    // 로그인 바로 시작하는 경우
    else
    {
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
            //CCLog("받은메시지 = %s", regId); // 여기서 전달받은 값이 출력됨
            
            t.env->ReleaseStringUTFChars(str, msg);
            // Release
            t.env->DeleteLocalRef(t.classID);
        }
        #endif
        
        httpStatus = HTTP_LOGIN;

        m_pMsgLabel->setString("로그인 중...");
        
        // required parameter values
        char temp[255];
        std::string param = "";
        //sprintf(temp, "game_version=%d&", iGameVersion);
        sprintf(temp, "game_version=1061&");
        param += temp;
        
        // 카카오톡을 탈퇴한 경우 => 닉네임/프로필을 공란으로 만든다.
        if (!isGuestLogin && KakaoLocalUser::getInstance()->hashedTalkUserId == "")
        {
            KakaoLocalUser::getInstance()->nickName = "";
            KakaoLocalUser::getInstance()->profileImageUrl = "";
        }
        KakaoLocalUser::getInstance()->nickName = Common::SubstrNickname(KakaoLocalUser::getInstance()->nickName);
        
        sprintf(temp, "device_type=%d&", mDeviceType);
        param += temp;
        sprintf(temp, "access_token=%s&", CCUserDefault::sharedUserDefault()->getStringForKey("access_token", "").c_str());
        param += temp;
        if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) // 안드로이드는 기기 고유 id를,
            sprintf(temp, "push_token=%s&", regId);
        else                                           // iOS는 아이폰에서 기기 고유 id를 받아넣는다.
            sprintf(temp, "push_token=%s&", CCUserDefault::sharedUserDefault()->getStringForKey("device_token").c_str());
        param += temp;
        
        if (isGuestLogin)
        {
            sprintf(temp, "kakao_id=%s&", CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "").c_str());
            param += temp;
            sprintf(temp, "profile_image_url=&"); // 프로필이미지 공백
            param += temp;
            sprintf(temp, "nick_name=GUEST"); // 닉네임 GUEST
            param += temp;
        }
        else
        {
            sprintf(temp, "kakao_id=%s&", KakaoLocalUser::getInstance()->userId.c_str());
            param += temp;
            sprintf(temp, "profile_image_url=%s&", KakaoLocalUser::getInstance()->profileImageUrl.c_str());
            param += temp;
            sprintf(temp, "nick_name=%s", KakaoLocalUser::getInstance()->nickName.c_str());
            param += temp;
        }
        
        Network::HttpPost(param, URL_LOGIN, this, httpresponse_selector(Splash::onHttpRequestCompleted), "", "", true);
    }
}


void Splash::XMLParseGameData()
{
    std::string pdiurl = CCUserDefault::sharedUserDefault()->getStringForKey("pdiUrl");
    Network::replaceAll(pdiurl, "/", "@");
    std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + pdiurl;
    //CCLog("filepath = %s", filepath.c_str());
    
    // read file
    unsigned long iSize = 0;
    unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(filepath.c_str(), "rb", &iSize);
    
    // base64_decode
    std::string base64EncodedStr = "";
    for (int i = 0 ; i < iSize ; i++)
        base64EncodedStr.push_back(pBuffer[i]);
    std::string obfuscatedStr = Common::base64_decode(base64EncodedStr);
    
    // deObfuscation
    int obfKey = 36 - 10;
    int keyLen = obfuscationKey[obfKey].size();
    std::string decodedStr = "";
    for (int i = 0 ; i < obfuscatedStr.size() ; i++) // xor operation
        decodedStr += obfuscatedStr[i] ^ obfuscationKey[obfKey][i%keyLen];
    
    ////CCLog("%s", decodedStr.c_str());
    
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(decodedStr.c_str(), (int)decodedStr.size());
    
    base64EncodedStr.clear();
    decodedStr.clear();
    
    if (!result)
    {
        //CCLog("error description: %s", result.description());
        //CCLog("error offset: %d", result.offset);
        return;
    }
    
    // variables
    int id;
    int count, price, price_KRW, price_USD, bonus;
    int level, bonusMPPercent, bonusMPPlus, cost_starcandy, cost_topaz;
    int costType, cost;
    int category, type;
    int grade;
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
            else if (name == "nMagicStaffBonusMPPercent") bonusMPPercent = ait->as_int();
            else if (name == "nMagicStaffBonusMPPlus") bonusMPPlus = ait->as_int();
            else if (name == "nStarCandyCostValue") cost_starcandy = ait->as_int();
            else if (name == "nTopazCostValue") cost_topaz = ait->as_int();
        }
        magicStaffBuildupInfo.push_back( new MagicStaffBuildUpInfo(level, bonusMPPercent, bonusMPPlus, cost_starcandy, cost_topaz) );
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
    
    // fairy info
    int refVal;
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
            else if (name == "nRefValue") refVal = ait->as_int();
            //else if (name == "nPrerequisiteID") pid = ait->as_int();
        }
        fairyInfo.push_back( new FairyInfo(id, type, grade, cost_starcandy, cost_topaz, refVal) );
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
            //else if (name == "nRequireMP") mp = ait->as_int();
            //else if (name == "nRequireStaffLevel") staffLv = ait->as_int();
            else if (name == "nRequireSkillID") skillId = ait->as_int();
            else if (name == "nRequireSkillLevel") skillLv = ait->as_int();
            else if (name == "bIsActive") isActive = ait->as_int();
        }
        mp = 0;
        staffLv = 0;
        skillInfo.push_back( new SkillInfo(id, skillName, type, maxLevel, mp, staffLv, skillId, skillLv, isActive) );
    }
    
    // skill_require_mp_define info
    int skillCnt, skillRequireMP, discountOne, discountTwo, topazVal;
    its = nodeResult.child("skill_require_mp_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nSkillCount") skillCnt = ait->as_int();
            else if (name == "nRequireMP") skillRequireMP = ait->as_int();
            else if (name == "nTopazCostValue") topazVal = ait->as_int();
            else if (name == "nDiscountPercentOneType") discountOne = ait->as_int();
            else if (name == "nDiscountPercentTwoType") discountTwo = ait->as_int();
        }
        skillBuildupMPInfo.push_back( new SkillBuildupMPInfo(skillCnt, skillRequireMP, discountOne, discountTwo, topazVal) );
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
    
    // item cost define
    int cost_item, item_type;
    its = nodeResult.child("game_item_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nItemID") id = ait->as_int();
            else if (name == "bCostType") item_type = ait->as_int(); // 1(별사탕), 2(토파즈)
            else if (name == "nCostValue") cost_item = ait->as_int();
        }
        
        itemType[id-1] = item_type;
        itemCost[id-1] = cost_item;
    }
    
    // tip 내용
    std::string script;
    its = nodeResult.child("loading_script_define_client").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nScriptID") id = ait->as_int();
            else if (name == "bCategory") category = ait->as_int(); // 1(개소리), 2(게임팁)
            else if (name == "charScript") script = ait->as_string();
        }
        tipContent.push_back( new TipContent(id, category, script) );
    }
    
    // 프로필 문구 내용
    std::string title;
    int propertyType;
    its = nodeResult.child("name_title_define_client").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nIndex") id = ait->as_int();
            //else if (name == "bCertificateType") certificateType = ait->as_int(); // -1(공통), 0(무학위), 1(학사), 2(석사), 3(박사)
            else if (name == "bPropertieseType") propertyType = ait->as_int(); // -1(공통), 0(무속성), 1(물), 2(불), 3(땅)
            else if (name == "charNameTitle") title = ait->as_string();
        }
        profileTitle.push_back( new ProfileTitle(id, -1, propertyType, title) );
    }
    
    // ingame_item cost define (인게임 내 출현하는 아이템 목록)
    its = nodeResult.child("ingame_item_define").children("Data");
    for (it = its.begin() ; it != its.end() ; ++it)
    {
        for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
        {
            std::string name = ait->name();
            if (name == "nItemID") id = ait->as_int();
            else if (name == "bCostType") item_type = ait->as_int(); // 1(별사탕), 2(토파즈)
            else if (name == "nCostValue") cost_item = ait->as_int();
        }
        inGameItem.push_back( new InGameItem(id, item_type, cost_item) );
    }
}

void Splash::WriteResFile(char* data, int size)
{
    std::string pdiurl = CCUserDefault::sharedUserDefault()->getStringForKey("pdiUrl");
    Network::replaceAll(pdiurl, "/", "@");
    std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + pdiurl;
    //CCLog("filepath = %s", filepath.c_str());
    
    FILE* ptr_fp;
    if ((ptr_fp = fopen(filepath.c_str(), "wb")) == NULL)
    {
        exit(1);
    }
    
    size_t realSize = strlen(data);
    size_t fwriteSize = fwrite(data, 1, realSize, ptr_fp);
    if (fwriteSize != realSize)
    {
        exit(1);
    }
    fclose(ptr_fp);
}

void Splash::WriteResFileTexture(char* data, int size)
{
    std::string rfn = resourceFilename[numOfDownloadedFiles];
    rfn = rfn.substr(rfn.find("/")+1);
    Network::replaceAll(rfn, "/", "@");
    
    std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + rfn;
    //CCLog("filepath = %s", filepath.c_str());
    
    FILE* ptr_fp;
    if ((ptr_fp = fopen(filepath.c_str(), "wb")) == NULL)
    {
        CCLog("FILE OPEN ERROR !");
        exit(1);
    }
    
    size_t realSize = size;
    size_t fwriteSize = fwrite(data, 1, realSize, ptr_fp);
    //CCLog("%d %d", (int)realSize, (int)fwriteSize);
    if (fwriteSize != realSize)
    {
        CCLog("FILE WRITE ERROR !");
        exit(1);
    }
    fclose(ptr_fp);
}


void Splash::XmlParseLogin(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        if (code == 2) // login 시도했는데 user_id가 없음. 다시 join으로 돌아간다.
        {
            isJoinNeeded = true;
            TryLogin();
            return;
        }
        
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 11) // 블록당한 유저
            Common::ShowPopup(this, "Splash", "NoImage", false, YOU_WERE_BLOCKED, BTN_1, nullData);
        else if (code == 12) // 카카오 토큰값 인증 실패
            Common::ShowPopup(this, "Splash", "NoImage", false, KAKAO_TOKEN_ERROR, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        // 로그인 기본 정보를 받는다.
        xml_node setting = nodeResult.child("setting");
        //bool kakaoMsg = (setting.attribute("kakao-message").as_int() == 1) ? true : false;
        bool kakaoMsg = false; // deprecated
        bool pushNoti = (setting.attribute("push-notification").as_int() == 1) ? true : false;
        bool potionMsg = (setting.attribute("potion-message").as_int() == 1) ? true : false;
        int userId = nodeResult.child("userID").text().as_int();
        int msgCnt = nodeResult.child("message").attribute("count").as_int();
        
        bool couponViewFlag = (nodeResult.child("ios-coupon-view").text().as_int() == 1) ? true : false;
        
        int todayFirst = nodeResult.child("today-first").text().as_int();
        if (todayFirst == 1)
        {
            int type, value, achieve;
            std::string week[7] = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
            for (int i = 0 ; i < 7 ; i++)
            {
                type = nodeResult.child("attendance").child(week[i].c_str()).attribute("reward-type").as_int();
                value = nodeResult.child("attendance").child(week[i].c_str()).attribute("reward-value").as_int();
                achieve = nodeResult.child("attendance").child(week[i].c_str()).attribute("achieve").as_int();
                loginEvent.push_back( new LoginEvent(type, value, achieve) );
            }
        }
        
        // 내 정보 class (extern) 만들기
        std::string myKakaoId = KakaoLocalUser::getInstance()->userId;
        if (isGuestLogin)
            myKakaoId = CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "");
        myInfo = new MyInfo();
        myInfo->Init(myKakaoId, mDeviceType, userId, kakaoMsg, pushNoti, potionMsg, msgCnt, sessionId, todayFirst, couponViewFlag);
        
        // get GCM key
        gcmKey = nodeResult.child("gcm-key").text().as_string();
        
        // get public-key for RSA
        publicKey = nodeResult.child("public-key").text().as_string();
        publicKeyIndex = nodeResult.child("public-key-index").text().as_int();
        
        // rsa 만들기 (초기 1회만 만들면 됨)
        rsa = createRSA((unsigned char*)(publicKey.c_str()), 1);
        
        
        // 공지사항 요청
        m_pMsgLabel->setString("공지사항 불러오는 중...");
        std::string param = "";
        Network::HttpPost(param, URL_NOTICE, this, httpresponse_selector(Splash::onHttpRequestCompleted));
    }
}


void Splash::XmlParseNotice(xml_document *xmlDoc)
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
        int id, platform;
        std::string title, message, link, packageName;
        // 악성 앱 패키지 리스트 받는다.
        xml_named_node_iterator it;
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("app-block-list").children("app");
        for (it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "name") packageName = ait->as_string();
            }
            
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            JniMethodInfo t;
            if (JniHelper::getStaticMethodInfo(t,
                                               "com/playDANDi/CocoMagic/CocoMagic",
                                               "CheckFuckingApp",
                                               "(Ljava/lang/String;)I"))
            {
                // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
                int result = t.env->CallStaticIntMethod(t.classID, t.methodID,
                                            t.env->NewStringUTF(packageName.c_str())
                                            );
                // Release
                t.env->DeleteLocalRef(t.classID);
                
                if (result != 0)
                {
                    //CCLog("악성 패키지 감지! (%s)", packageName.c_str());
                    std::vector<int> nullData;
                    Common::ShowPopup(this, "Splash", "NoImage", false, FUCKING_APP_DETECTED, BTN_1, nullData);
                    return;
                }
            }
            #endif
        }
        
        // 루팅 폰 검사
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "com/playDANDi/CocoMagic/CocoMagic",
                                           "IsRootingPhone",
                                           "()Z"))
        {
            // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
            bool result = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
            // Release
            t.env->DeleteLocalRef(t.classID);
            
            if (result)
            {
                std::vector<int> nullData;
                Common::ShowPopup(this, "Splash", "NoImage", false, ROOTING_DETECTED, BTN_1, nullData);
                return;
            }
        }
        #endif
        
        // 공지사항 리스트를 받는다.
        int onetime;
        its = nodeResult.child("notice-list").children("notice");
        for (it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "platform") platform = ait->as_int(); // 0 : 모두 , 1 : 안드로이드 , 2 : iOS
                else if (name == "title") title = ait->as_string();
                else if (name == "message") message = ait->as_string();
                else if (name == "link") link = ait->as_string();
                else if (name == "one-time") onetime = ait->as_int();
            }
            if ( (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID && platform != 2) || // 플랫폼에 맞는 공지사항만 들고온다.
                 (CC_TARGET_PLATFORM == CC_PLATFORM_IOS && platform != 1) )
            {
                noticeList.push_back( new NoticeList(id, platform, title, message, link, onetime) );
            }
        }
        
        // 내 모든 정보 요청
        m_pMsgLabel->setString("나의 정보를 요청 중...");
        char temp[255];
        std::string param = "";
        if (isGuestLogin)
            sprintf(temp, "kakao_id=%s", CCUserDefault::sharedUserDefault()->getStringForKey("guest_kakao_id", "").c_str());
        else
            sprintf(temp, "kakao_id=%s", KakaoLocalUser::getInstance()->userId.c_str());
        param += temp;
         
        Network::HttpPost(param, URL_USERINFO, this, httpresponse_selector(Splash::onHttpRequestCompleted));
    }
}

void Splash::XmlParseMyInfo(xml_document *xmlDoc)
{
    //CCLog("xml parse : myinfo");
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
        int staffFailPoint = nodeResult.child("coco").attribute("magic-staff-fail-point").as_int();
        
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
        
        int fireByTopaz = nodeResult.child("properties").attribute("fire-purchase-topaz").as_int();
        int waterByTopaz = nodeResult.child("properties").attribute("water-purchase-topaz").as_int();
        int landByTopaz = nodeResult.child("properties").attribute("land-purchase-topaz").as_int();
        
        myInfo->InitRestInfo(topaz, starcandy, mp, mpStaffPercent, mpFairy, staffLv, staffFailPoint, highScore, weeklyHighScore, lastWeeklyHighScore, isWeeklyRankReward, certificateType, remainWeeklyRankTime, item1, item2, item3, item4, item5, potion, remainPotionTime, fire, water, land, master, fireByTopaz, waterByTopaz, landByTopaz);
        
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
        int cfi, ufi, level, isUse, failPoint;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-fairy-id") cfi = ait->as_int();
                else if (name == "user-fairy-id") ufi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "is-use") isUse = ait->as_int();
                else if (name == "fairy-fail-point") failPoint = ait->as_int();
            }
            myInfo->AddFairy(cfi, ufi, level, isUse, failPoint);
        }
        
        its = nodeResult.child("skill-list").children("skill");
        int exp, learntime;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "common-skill-id") csi = ait->as_int();
                else if (name == "user-skill-id") usi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "exp") exp = ait->as_int();
                else if (name == "learn-time") learntime = ait->as_int();
            }
            myInfo->AddSkill(csi, usi, level, exp, learntime);
            
            // 현재 연습 중인 스킬 id 가져오기
            if (usi == practiceUserSkillId)
            {
                myInfo->SetPracticeSkill(csi, level);
            }
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        // 오늘의 별사탕 관련 정보
        int todayCandyTypeChoice = nodeResult.child("today-starcandy-info").attribute("type-choice").as_int();
        int todayCandyValueChoice = nodeResult.child("today-starcandy-info").attribute("value-choice").as_int();
        int todayCandyTypeMiss = nodeResult.child("today-starcandy-info").attribute("type-miss").as_int();
        int todayCandyValueMiss = nodeResult.child("today-starcandy-info").attribute("value-miss").as_int();
        int istodayCandyUsed = nodeResult.child("today-starcandy-info").attribute("today-use").as_int();
        myInfo->SetTodayCandy(todayCandyTypeChoice, todayCandyValueChoice, todayCandyTypeMiss, todayCandyValueMiss, istodayCandyUsed);
        
        
        // 선물하기에서 실패한 경우에 대비한 친구kakaoId 들고오기
        TryGetNonConsumed();
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
        else if (code == 12) // 이미 보상을 받은 경우
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    
    else if (code == 0)
    {
        int cnt = KakaoFriends::getInstance()->appFriends->count();
        CCArray* keys = KakaoFriends::getInstance()->appFriends->allKeys();
        std::string id;
        
        myRank = nodeResult.child("my-rank").attribute("rank").as_int();
        myLastWeekHighScore = nodeResult.child("my-rank").attribute("last-week-high-score").as_int();
        rewardType = nodeResult.child("my-rank").attribute("reward-type").as_int();
        certificateType = nodeResult.child("my-rank").attribute("certificate-type").as_int();
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("friend-rank-list").children("friend");
        std::string nickname, profileUrl, kakaoId;
        int rank, lastWeekHighScore, isFriend;
        for (xml_named_node_iterator it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "nick-name") nickname = ait->as_string();
                else if (name == "kakao-id") kakaoId = ait->as_string();
                else if (name == "profile-url") profileUrl = ait->as_string();
                else if (name == "rank") rank = ait->as_int();
                else if (name == "last-week-high-score") lastWeekHighScore = ait->as_int();
                else if (name == "is-friend") isFriend = ait->as_int();
            }
            
            // nickname 너무 길면 자르자.
            nickname = Common::SubstrNickname(nickname);
            
            // 친구리스트에 있다면 닉네임과 프로필url을 바꿔준다.
            for (int j = 0 ; j < cnt ; j++)
            {
                id = ((CCString*)keys->objectAtIndex(j))->getCString();
                if (kakaoId == id)
                {
                    KakaoFriends::Friends* f = (KakaoFriends::Friends*)KakaoFriends::getInstance()->appFriends->objectForKey(id.c_str());
                    nickname = f->nickname;
                    profileUrl = f->profileImageUrl;
                    break;
                }
            }
            
            lastWeeklyRank.push_back( new LastWeeklyRank(nickname, profileUrl, rank, lastWeekHighScore, isFriend) );
            if (ProfileSprite::GetProfile(profileUrl) == NULL) // 프로필 sprite에 모은다.
                profiles.push_back( new ProfileSprite(profileUrl, true) );
        }

        // 친구 리스트 정보를 받는다.
        m_pMsgLabel->setString("못생긴 친구들을 불러오는 중...");
        
        char temp[50];
        std::string param;
        sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
        param += temp;

        Network::HttpPost(param, URL_FRIENDLIST, this, httpresponse_selector(Splash::onHttpRequestCompleted));
    }
}

void Splash::XmlParseFriends(xml_document *xmlDoc)
{
    //CCLog("xml parse : friends");
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
        // 초기화 (다시 받아올 수 있으므로)
        for (int i = 0 ; i < friendList.size() ; i++)
            delete friendList[i];
        friendList.clear();
        
        std::string kakaoId;
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
        int profileTitleId;
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
                if (name == "kakao-id") kakaoId = ait->as_string();
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
                else if (name == "name-title-id") profileTitleId = ait->as_int();
                else if (name == "properties-fire") fire = ait->as_int();
                else if (name == "properties-water") water = ait->as_int();
                else if (name == "properties-land") land = ait->as_int();
                else if (name == "properties-master") master = ait->as_int();
                else if (name == "fairy-id") fairyId = ait->as_int();
                else if (name == "fairy-level") fairyLevel = ait->as_int();
                else if (name == "skill-id") skillId = ait->as_int();
                else if (name == "skill-level") skillLevel = ait->as_int();
            }
            
            // 본인이 카카오톡을 탈퇴한 경우, 내 정보를 제외하고 무시해야 한다. (친구가 없어야 함)
            if (!isGuestLogin && myInfo->GetHashedTalkUserId() == "")
                if (myInfo->GetKakaoId() != kakaoId)
                    continue;
            
            nickname = Common::SubstrNickname(nickname);
            
            friendList.push_back( new Friend(kakaoId, nickname, imageUrl, potionMsgStatus, remainPotionTime, remainRequestPotionTime, remainRequestTopazTime, weeklyHighScore, highScore, scoreUpdateTime, certificateType, profileTitleId, fire, water, land, master, fairyId, fairyLevel, skillId, skillLevel) );
            // potion image 처리
            friendList[(int)friendList.size()-1]->SetPotionSprite();
        }
        // sort by { max[weeklyScore], min[scoreUpdateTime] }
        DataProcess::SortFriendListByScore();
        
        
        // 추가 또는 삭제할 친구가 있는지 검사한다. (처음 한번만)
        if (!isGuestLogin && !isFriendListChecked)
            CheckFriendList();
        
        if (addedList.size() > 0) // 추가할 친구가 있으면 프로토콜 실행. 게스트로그인 시 무시하자.
        {
            char temp[255];
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            for (int i = 0 ; i < addedList.size() ; i++)
            {
                sprintf(temp, "friend_kakao_id_list[%d]=%s&", i, addedList[i].c_str());
                param += temp;
            }
            param = param.substr(0, param.size()-1);
            
            addedList.clear();
            httpStatus--;
            Network::HttpPost(param, URL_FRIENDADD, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
        else if (deletedList.size() > 0) // 삭제할 친구가 있으면 프로토콜 실행. 게스트로그인 시 무시하자.
        {
            char temp[255];
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            for (int i = 0 ; i < deletedList.size() ; i++)
            {
                sprintf(temp, "friend_kakao_id_list[%d]=%s&", i, deletedList[i].c_str());
                param += temp;
            }
            param = param.substr(0, param.size()-1);
            
            deletedList.clear();
            httpStatus--;
            Network::HttpPost(param, URL_FRIENDDEL, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
        else
        {
            // 프로필 문구를 갱신하기 위해 서버로 업데이트
            Common::UpdateProfileTitle();
            
            if (!isGuestLogin)
            {
                int cnt = KakaoFriends::getInstance()->appFriends->count();
                CCArray* keys = KakaoFriends::getInstance()->appFriends->allKeys();
                std::string id;
                
                // 카카오 친구리스트에서 필요한 변수들을 집어넣는다.
                for (int i = 0 ; i < friendList.size() ; i++)
                {
                    for (int j = 0 ; j < cnt ; j++)
                    {
                        id = ((CCString*)keys->objectAtIndex(j))->getCString();
                        if (friendList[i]->GetKakaoId() == id)
                        {
                            KakaoFriends::Friends* f = (KakaoFriends::Friends*)KakaoFriends::getInstance()->appFriends->objectForKey(id.c_str());
                            
                            // 친구가 카카오톡을 탈퇴했다면, 프로필 사진과 닉네임을 보여주지 않는다.
                            if (f->hashedTalkUserId == "")
                            {
                                f->profileImageUrl = "";
                                f->nickname = "";
                            }
                            else
                                f->nickname = Common::SubstrNickname(f->nickname);
                            
                            friendList[i]->SetKakaoVariables( f->nickname, f->profileImageUrl, f->hashedTalkUserId, f->messageBlocked, f->supportedDevice );
                            break;
                        }
                    }
                }
            }

            // 본인과 상위 5명 (스크롤뷰에 보이는 정도) : 미리 프로필 사진을 받아두도록 하자. (flag 'preload' = true)
            bool flag;
            for (int i = 0 ; i < friendList.size() ; i++)
            {
                if (ProfileSprite::GetProfile(friendList[i]->GetImageUrl()) == NULL)
                {
                    flag = (i < 5 || friendList[i]->GetKakaoId() == myInfo->GetKakaoId());
                    if (myInfo->GetHashedTalkUserId() == "")
                        flag = false;
                    profiles.push_back( new ProfileSprite(friendList[i]->GetImageUrl(), flag) );
                }
            }
            if (ProfileSprite::GetProfile("") == NULL) // 빈 프로필도 모은다.
                profiles.push_back( new ProfileSprite("", false) );

            
            // preload 해야하는 것들 (주간랭킹결과 3명, 랭킹 처음 5명 정도) -> 프로필 로딩하자.
            // 단, 당연하게도 profile url이 공백이면 로딩할 필요가 없다.
            char tag[6];
            profileCnt = 0;
            for (int i = 0 ; i < profiles.size() ; i++)
            {
                std::string url = profiles[i]->GetProfileUrl();
                if (profiles[i]->IsPreload() && url != "" && ((int)url.size() >= 4 && url.substr(0, 4) == "http"))
                    profileCnt++;
            }
            //CCLog("프로필 로딩해야 할 수 : %d", profileCnt);
            for (int i = 0 ; i < profiles.size() ; i++)
            {
                std::string url = profiles[i]->GetProfileUrl();
                if (profiles[i]->IsPreload() && url != "" && ((int)url.size() >= 4 && url.substr(0, 4) == "http"))
                {
                    //CCLog("친구 로딩 후 프로필 로딩 시작...(%d)", i);
                    profiles[i]->SetLoadingStarted(true);
                    
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
                    sprintf(tag, "%d", i);
                    req->setTag(tag);
                    CCHttpClient::getInstance()->send(req);
                    req->release();
                }
            }

            if (profileCnt == 0) // preload할 것이 없으면 바로 시작.
            {
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                GetNonConsumedItems();
                #endif
                
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                LastActionStart();
                #endif
            }
        }
    }
}

void Splash::CheckFriendList()
{
    if (myInfo->GetHashedTalkUserId() == "")
    {
        isFriendListChecked = true;
        return;
    }
    
    bool flag;
    int cnt = KakaoFriends::getInstance()->appFriends->count();
    //CCLog("COUNT = %d", cnt);
    CCArray* keys = KakaoFriends::getInstance()->appFriends->allKeys();
    
    // 새로 추가된 친구가 있는지 찾는다.
    for (int j = 0 ; j < cnt ; j++)
    {
        flag = false;
        CCString* k = (CCString*)keys->objectAtIndex(j);
        std::string userId = k->getCString();
        //CCLog("user id = %s", userId.c_str());
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (myInfo->GetKakaoId() == friendList[i]->GetKakaoId())
                continue;
            if (userId == friendList[i]->GetKakaoId())
            {
                flag = true;
                break;
            }
        }
        if (!flag)
            addedList.push_back(userId);
    }
    
    // 사라진 친구가 있는지 찾는다.
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (myInfo->GetKakaoId() == friendList[i]->GetKakaoId())
            continue;
        
        flag = false;
        for (int j = 0 ; j < cnt ; j++)
        {
            CCString* k = (CCString*)keys->objectAtIndex(j);
            std::string userId = k->getCString();
            if (userId == friendList[i]->GetKakaoId())
            {
                flag = true;
                break;
            }
        }
        if (!flag)
            deletedList.push_back(friendList[i]->GetKakaoId());
    }
    
    isFriendListChecked = true;
}

void Splash::XmlParseServerCheck(void* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        return;
    }
    
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 문제없으니 그대로 넘겨라. (계속 진행)
        {
            // 게임 버전 체크
            m_pMsgLabel->setString("게임 버전이 잘생겼는지 확인 중...");
            CCHttpRequest* req = new CCHttpRequest();
            //if (binaryVersion_current < 2000) // 1000번대 (기존 유저)
            //    req->setUrl(URL_VERSION);
            //else // 2000번대 이상
            req->setUrl(URL_VERSION_NEW);
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompleted));
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
        else
            Common::ShowPopup(this, "Splash", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    else
    {
        serverCheckMsg = nodeResult.child("message").text().as_string();
        std::vector<int> nullData;
        Common::ShowPopup(this, "Splash", "NoImage", false, SERVER_CHECK, BTN_1, nullData);
    }
}


void Splash::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCHttpResponse* res = (CCHttpResponse*) data;

    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);

    // join.php에서 넘어온 경우,  ///바로 get_version을 시작하도록 한다.
    if (httpStatus == HTTP_JOIN || httpStatus == HTTP_JOIN_GUEST)
    {
        isJoinNeeded = false;
        if (httpStatus == HTTP_JOIN_GUEST) // join_guest.php 직후라면, 새로운 게스트Id를 받아와야 함.
            XmlParseJoinGuest(&xmlDoc);

        TryLogin();
        return;
    }
    
    httpStatus++;
    switch (httpStatus-1)
    {
        case HTTP_VERSION:
            XmlParseVersion(&xmlDoc); break;
        case HTTP_NONCONSUMED_GET_FRIEND_ID:
            XmlParseGetFriendKakaoId(&xmlDoc); break;
        case HTTP_LOGIN:
            XmlParseLogin(&xmlDoc); break;
        case HTTP_NOTICE:
            XmlParseNotice(&xmlDoc); break;
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
    char* dumpData;
    
    std::string tag = res->getHttpRequest()->getTag();
    
    //CCLog("%s", res->getHttpRequest()->getUrl());
    //CCLog("%d", res->getResponseCode());
    
    // mt.php (제일 처음) 호출 결과
    if (atoi(res->getHttpRequest()->getTag()) == 8888888)
    {
        // dump data
        std::vector<char> *buffer = res->getResponseData();
        dumpData = reinterpret_cast<char*>(buffer->data());

        XmlParseServerCheck(dumpData, (int)buffer->size());
        return;
    }
    
    // 프로필 사진 or resource.xml 받아올 때
    if (!res || !res->isSucceed())
    {
        if ( !(atoi(res->getHttpRequest()->getTag()) == 9999998) && !(atoi(res->getHttpRequest()->getTag()) == 9999999) )
        {
            profileCnt--;
            //CCLog("cnt = %d // 프로필 로딩 done", profileCnt);
            if (profileCnt <= 0)
            {
                //CCLog("프로필 로딩 끝!");
                /*
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                 m_pMsgLabel->setString("새로운 아이템을 쳐다보는 중...");
                 httpStatus = HTTP_NONCONSUMED_GET_FRIEND_ID;
                 
                 char temp[255];
                 std::string param = "";
                 sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
                 param += temp;
                 
                 Network::HttpPost(param, URL_NONCONSUMED_GETFRIENDID, this, httpresponse_selector(Splash::onHttpRequestCompleted));
                #endif
                */
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                GetNonConsumedItems();
                #endif
                
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                // 1) 로고랑 글자를 없앤다.
                // 2) 배경화면 축소하면서 Ranking 시작.
                LastActionStart();
                #endif
            }
            return;
        }
        else if (atoi(res->getHttpRequest()->getTag()) == 9999998 || atoi(res->getHttpRequest()->getTag()) == 9999999)
            return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    dumpData = reinterpret_cast<char*>(buffer->data());
    
    
    // update_list.xml 파일 받았을 경우
    if (atoi(res->getHttpRequest()->getTag()) == 9999998)
    {
        // 리소스 파일 정보 분석
        XmlParseResourceList(dumpData, (int)buffer->size());
    }
    
    // 일반 리소스 파일 받았을 경우
    else if (atoi(tag.substr(0, 7).c_str()) == 1231234)
    {
        // md5 hash 검사
        unsigned char md5_res[16];
        MD5_Init(&md5);
        MD5_Update(&md5, dumpData, (int)buffer->size());
        MD5_Final(md5_res, &md5);
        
        char mdString[33];
        for (int i = 0 ; i < 16 ; i++)
            sprintf(&mdString[i*2], "%02x", (unsigned int)md5_res[i]);
        //CCLog("mdString = %s", mdString);
        
        // checksum, filesize 검사
        int index = atoi(tag.substr(tag.find("_")+1).c_str());
        
        // 체크섬, 파일크기, status code 중 하나라도 비정상적이면 다시 다운로드 받는다.
        if (!res || !res->isSucceed() || res->getResponseCode() != 200 ||
            strcmp(mdString, resourceChecksum[index].c_str()) != 0 ||
            (int)buffer->size() != resourceFilesize[index])
        {
            //CCLog("사이즈 : %d , %d", (int)buffer->size(), resourceFilesize[index]);
            //CCLog("체크섬 : %s , %s", mdString, resourceChecksum[index].c_str());
            // 리소스 파일 다시 다운로드
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl( res->getHttpRequest()->getUrl() );
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
            req->setTag(res->getHttpRequest()->getTag());
            CCHttpClient::getInstance()->send(req);
            req->release();
            return;
        }
        
        // xor operation with a key
        int keyLen = basicKey.size();
        std::string decryptedKey = "";
        for (int i = 0 ; i < (int)buffer->size() ; i++)
        {
            decryptedKey += dumpData[i] ^ basicKey[i%keyLen];
        }
        
        // 리소스 파일 해석 및 배치
        WriteResFileTexture((char*)decryptedKey.c_str(), (int)buffer->size());  //dumpData, (int)buffer->size());
        
        
        // 그 다음 리소스 파일 다운로드
        numOfDownloadedFiles++;
        char text[50];
        sprintf(text, "리소스 다운로드 중... (%d/%d)", numOfDownloadedFiles, numOfResourceFiles);
        m_pMsgLabel->setString(text);
        
        if (numOfDownloadedFiles >= (int)resourceFilename.size())
        {
            if (pdiUrl != "") // 새로운 balance.pdi 다운로드
            {
                CCHttpRequest* req = new CCHttpRequest();
                req->setUrl( (prevUrl+pdiUrl).c_str() );
                req->setRequestType(CCHttpRequest::kHttpPost);
                req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
                req->setTag("9999999");
                CCHttpClient::getInstance()->send(req);
                req->release();
            }
            else // balance.pdi를 새로 받을 필요가 없다면, 클라에 있던 기존 것으로 data parsing
            {
                // 리소스 XML parsing
                m_pMsgLabel->setString("못생긴 리소스 배치 중...");
                XMLParseGameData();
                
                TryLogin();
            }
        }
        //StartDownloadImageResources();
    }
    
    // balance.pdi 파일 받았을 경우
    else if (atoi(res->getHttpRequest()->getTag()) == 9999999)
    {
        // 클라에 파일 저장 후, 해석한다.
        WriteResFile(dumpData, (int)buffer->size());
        
        // 새 리소스 XML parsing
        m_pMsgLabel->setString("못생긴 리소스 배치 중...");
        XMLParseGameData();

        // 로그인 시도
        TryLogin();
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
        profiles[index]->SetLoadingDone(true);
        
        profileCnt--;
        if (profileCnt <= 0)
        {
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            GetNonConsumedItems();
            #endif
            
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            LastActionStart();
            #endif
        }
    }
}

int versionCompare(int a, int b)
{
    return a > b;
}

void Splash::XmlParseResourceList(char* data, int size)
{
    // 'update_list.pdi' 파일을 해석한다.
    
    std::string obfuscatedStr = data;
    // replacing '-' to '+'
    Network::replaceAll(obfuscatedStr, "-", "+");
    // base64 decode
    obfuscatedStr = Common::base64_decode(data);
    
    // deObfuscation
    int obfKey = 36 - 10;
    int keyLen = obfuscationKey[obfKey].size();
    std::string decodedStr = "";
    for (int i = 0 ; i < obfuscatedStr.size() ; i++) // xor operation
        decodedStr += obfuscatedStr[i] ^ obfuscationKey[obfKey][i%keyLen];

    //CCLog("%s", decodedStr.c_str());
    
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(decodedStr.c_str(), (int)decodedStr.size());
    
    obfuscatedStr.clear();
    decodedStr.clear();
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return;
    }
    
    
    // 기본 url 구하기
    prevUrl = balanceFileUrl;
    Network::replaceAll(prevUrl, "update_list.pdi", "");
    
    // 받지 않은 모든 버전에 대해 리소스 순차 검사
    std::string filename, checksum;
    int fsize;
    xml_named_node_iterator it;
    xml_node nodeResult = xmlDoc.child("update-list");
    
    // xml 파일에 형성되어 있는 버전을 모두 뽑아내서 내림차순으로 정렬한다. (최신버전이 먼저 오도록)
    std::vector<int> versions;
    xml_node_iterator ite;
    for (ite = nodeResult.begin() ; ite != nodeResult.end() ; ++ite)
    {
        std::string ver = ite->name();
        int versionNumber = atoi( ver.substr(ver.find("v")+1).c_str() );
        versions.push_back(versionNumber);
    }
    std::sort(versions.begin(), versions.end(), versionCompare);
    
    // 방식 : 최신버전 V부터 V-1, V-2, ..., K+1 (K는 이전까지의 최신버전) 순서대로 다운받아야 할 리소스 파일명을 검사한다. 중복되는 파일명은 제외한다.
    char version[7];
    char fullfilename[100];
    for (int v = 0 ; v < (int)versions.size() ; v++)
    {
        if (versions[v] <= iGameVersion)
            break;
        
        sprintf(version, "v%d", versions[v]);
        xml_object_range<xml_named_node_iterator> its = nodeResult.child(version).children("file");
        for (it = its.begin() ; it != its.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                std::string name = ait->name();
                if (name == "name") filename = ait->as_string();
                else if (name == "checksum") checksum = ait->as_string();
                else if (name == "size") fsize = ait->as_int();
            }
            
            // 이미 똑같은 파일명이 있으면 무시한다.
            int i;
            for (i = 0 ; i < (int)resourceFilename.size() ; i++)
            {
                if (resourceFilename[i].substr(resourceFilename[i].find("/")+1) == filename.substr(filename.find("/")+1))
                    break;
            }
            if (i < (int)resourceFilename.size())
                continue;

            
            if (filename.substr(filename.find(".")+1) == "pdi") // 가장 최근 balance.pdi의 주소를 저장한다.
            {
                if (pdiUrl == "")
                {
                    sprintf(fullfilename, "%d/%s", versions[v], filename.c_str());
                    pdiUrl = fullfilename;
                    CCUserDefault::sharedUserDefault()->setStringForKey("pdiUrl", pdiUrl);
                    //CCLog("pdi = %s", pdiUrl.c_str());
                }
            }
            else
            {
                sprintf(fullfilename, "%d/%s", versions[v], filename.c_str());
                resourceFilename.push_back(fullfilename);
                resourceChecksum.push_back(checksum);
                resourceFilesize.push_back(fsize);
                //CCLog("filename = %s", filename.c_str());
            }
        }
    }
    numOfResourceFiles = (int)resourceFilename.size();
    
    // 각 이미지 리소스 + pdi 파일 다운로드
    StartDownloadImageResources();
}

void Splash::StartDownloadImageResources() // 필요한 모든 이미지 리소스 다운받기
{
    // 1) balance.xml 파일에서 리스트를 모두 받아온다.
    // 2) 클라에 파일이 정상적으로 존재하는지 검사한다. 비정상적인 파일의 개수를 센다.
    // 3) 비정상적인 파일들을 일시에 다운로드 시작한다.
    // 4) 완료된 파일은 압축을 풀어서 저장한다. (.zip)
    // 5) 다운로드 완료 개수를 세고, 모두 완료되면 다음 상태인 TryLogin()으로 이동한다.
    // * 만약 다운은 완료되었는데 그게 비정상적이라면? 어떻게 해야 하는가?
    
    char text[50];
    sprintf(text, "리소스 다운로드 중... (%d/%d)", numOfDownloadedFiles, numOfResourceFiles);
    m_pMsgLabel->setString(text);
    

    char temp[12];
    for (int i = 0 ; i < (int)resourceFilename.size() ; i++)
    {
        // 각 리소스 파일 동시 다운로드 (balance.pdi 제외!)
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl( (prevUrl + resourceFilename[i]).c_str() );
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
        sprintf(temp, "1231234_%d", i);
        req->setTag(temp);
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    
    // 새로운 리소스 이미지 파일을 받을 게 전혀 없는 경우 : 바로 balance.pdi 검사로 넘어감.
    if ((int)resourceFilename.size() == 0)
    {
        for (int i = 0 ; i < (int)resourceFilename.size() ; i++)
            resourceFilename[i].clear();
        resourceFilename.clear();
        
        if (pdiUrl != "") // 새로운 balance.pdi 다운로드
        {
            CCHttpRequest* req = new CCHttpRequest();
            req->setUrl( (prevUrl+pdiUrl).c_str() );
            req->setRequestType(CCHttpRequest::kHttpPost);
            req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
            req->setTag("9999999");
            CCHttpClient::getInstance()->send(req);
            req->release();
        }
        else // balance.pdi를 새로 받을 필요가 없다면, 클라에 있던 기존 것으로 data parsing
        {
            // 리소스 XML parsing
            m_pMsgLabel->setString("못생긴 리소스 배치 중...");
            XMLParseGameData();
            
            TryLogin();
        }
    }
    
    
    /*
    if (numOfDownloadedFiles+1 >= numOfResourceFiles) // pdi를 제외한 파일이 없는 경우
    {
        for (int i = 0 ; i < (int)resourceFilename.size() ; i++)
            resourceFilename[i].clear();
        resourceFilename.clear();
        
        // balance.pdi 다운로드
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl( (prevUrl+pdiUrl).c_str() );
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
        req->setTag("9999999");
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
    else // pdi를 제외한 리소스 파일이 1개 이상 있는 경우
    {
        // 각 리소스 파일 다운로드 (순서대로)
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl( (prevUrl+resourceFilename[numOfDownloadedFiles]).c_str() );
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(Splash::onHttpRequestCompletedNoEncrypt));
        req->setTag("1231234");
        CCHttpClient::getInstance()->send(req);
        req->release();
    }
     */
}

void Splash::TryGetNonConsumed()
{
    //m_pMsgLabel->setString("새로운 아이템을 쳐다보는 중...");
    httpStatus = HTTP_NONCONSUMED_GET_FRIEND_ID;
    
    char temp[255];
    std::string param = "";
    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
    param += temp;
    
    Network::HttpPost(param, URL_NONCONSUMED_GETFRIENDID, this, httpresponse_selector(Splash::onHttpRequestCompleted));
}

void Splash::GetNonConsumedItems()
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t,
                                       "com/playDANDi/CocoMagic/CocoMagic",
                                       "StartIAB",
                                       "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
        t.env->CallStaticVoidMethod(t.classID, t.methodID,
                                    0,
                                    -1, // topazid
                                    t.env->NewStringUTF(myInfo->GetKakaoId().c_str()), // kakaoid
                                    t.env->NewStringUTF(fKakaoId.c_str()), // friendkakaoid
                                    t.env->NewStringUTF(""), // productid
                                    t.env->NewStringUTF(""), // payload
                                    t.env->NewStringUTF(gcmKey.c_str())
                                    );
        // Release
        t.env->DeleteLocalRef(t.classID);
    }
    // 이 부분이 끝나면, 자바 단에서 LastActionStart() 호출함.
    #endif
}

void Splash::XmlParseJoinGuest(xml_document *xmlDoc)
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
    else
    {
        // 새로 받아온 게스트로그인용 아이디를 클라이언트에 저장.
        std::string guestId = nodeResult.child("guest-id").text().as_string();
        CCUserDefault::sharedUserDefault()->setStringForKey("guest_kakao_id", guestId);
    }
}

void Splash::XmlParseGetFriendKakaoId(xml_document *xmlDoc)
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
    else
    {
        fKakaoId = nodeResult.child("friend").attribute("kakao-id").as_string();
        Network::replaceAll(fKakaoId, "NULL", "");
        //if (fKakaoId == "NULL")
        //    fKakaoId = "";
        
        // iOS 결제를 위한 초기화 시작
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        TransactionStart();
        #endif
        
        
        char temp[50];
        std::string param;
        if (!isGuestLogin && myInfo->IsWeeklyRankReward() == 0 && myInfo->GetLastWeeklyHighScore() != -1) // 게스트로그인 시 무시함.
        {
            // 저번주 주간랭킹 결과 불러온다.
            m_pMsgLabel->setString("지난 주 상이 있는지 힐끔 바라보는 중...");
            
            sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
            param += temp;
            
            Network::HttpPost(param, URL_WEEKLYRANK, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
        else
        {
            // 친구 리스트 정보를 받는다.
            m_pMsgLabel->setString("못생긴 친구들을 불러오는 중...");
            
            sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
            param += temp;
            
            httpStatus++;
            Network::HttpPost(param, URL_FRIENDLIST, this, httpresponse_selector(Splash::onHttpRequestCompleted));
        }
    }
}


void Splash::GetTodayCandyFriend()
{
    //m_pMsgLabel->setString("오늘의 별사탕 그룹 구성 중...");
    //todayCandyKakaoId.push_back(myInfo->GetKakaoId());
    //for (int i = 1 ; i < 5 ; i++)
    //    todayCandyKakaoId.push_back("");
    
    m_pMsgLabel->setString("오늘의 별사탕 그룹 구성 중...");
    char name[15];
    std::string kakaoId;
    bool flag;
    
    // 내 kakao id부터 넣자.
    todayCandyKakaoId.clear();
    todayCandyKakaoId.push_back(myInfo->GetKakaoId());
    
    if (!myInfo->IsTodayCandyUsed())
    {
        for (int i = 1 ; i < 5 ; i++)
            todayCandyKakaoId.push_back("");
    }
    else
    {
        for (int i = 1 ; i < 5; i++)
        {
            sprintf(name, "todayCandy_%d", i);
            kakaoId = CCUserDefault::sharedUserDefault()->getStringForKey(name, "");
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
                todayCandyKakaoId.push_back("");
            
            CCUserDefault::sharedUserDefault()->setStringForKey(name, todayCandyKakaoId[todayCandyKakaoId.size()-1]);
        }
    }
}

void Splash::LastActionStart()
{
    //CCLog("last action start");
    Splash* pThis = (Splash*)Depth::GetCurPointer();

    // 오늘의 별사탕 친구 목록 받아오기
    pThis->GetTodayCandyFriend();

    // 이제 시작
    CCActionInterval* action = CCSequence::create(CCEaseBounceIn::create( CCMoveBy::create(0.5f, ccp(0, 900)) ),
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
    delete sound;
    EndScene();
    Common::ShowNextScene(this, "Splash", "Ranking", true);
    //CCLog("loaded RANKING scene");
}

void Splash::EndScene()
{
    //CCLog("Splash : EndScene");
    // release depth tree
    Depth::RemoveCurDepth();
    
    m_pBackground->removeFromParentAndCleanup(true);
    m_pForKakao->removeFromParentAndCleanup(true);
    m_pTitle->removeFromParentAndCleanup(true);
    m_pMsgLabel->removeFromParentAndCleanup(true);
    
    CCTextureCache::sharedTextureCache()->removeAllTextures();
}




/*
 void Splash::WriteResFileZip(char* data, int size)
 {
 std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + "1.zip";
 CCLog("filepath = %s", filepath.c_str());
 
 FILE* ptr_fp;
 if ((ptr_fp = fopen(filepath.c_str(), "wb")) == NULL)
 {
 CCLog("FILE OPEN ERROR !");
 exit(1);
 }
 size_t realSize = size;
 size_t fwriteSize = fwrite(data, 1, realSize, ptr_fp);
 //CCLog("%d %d", (int)realSize, (int)fwriteSize);
 if (fwriteSize != realSize)
 {
 CCLog("FILE WRITE ERROR !");
 exit(1);
 }
 fclose(ptr_fp);
 
 UncompressZip("1.zip");
 }
 */

/*
#define READ_SIZE 1000000
#define dir_delimter '/'

void Splash::UncompressZip(std::string filename)
{
    // dont forget #include "support/zip_support/unzip.h"
    
    std::string pathName =  CCFileUtils::sharedFileUtils()->getWritablePath() + filename;
    
    unzFile zipfile = unzOpen(pathName.c_str());
    
    
    if ( zipfile == NULL )
    {
        printf( "%s: not found" ,pathName.c_str());
        return;
    }
    
    // Get info about the zip file
    unz_global_info global_info;
    if ( unzGetGlobalInfo( zipfile, &global_info ) != UNZ_OK )
    {
        printf( "could not read file global infon" );
        unzClose( zipfile );
        return;
    }
    
    // Loop to extract all files
    uLong i;
    for ( i = 0; i < global_info.number_entry; ++i )
    {
        // Get info about current file.
        unz_file_info file_info;
        char filename[ 100 ];
        if ( unzGetCurrentFileInfo(
                                   zipfile,
                                   &file_info,
                                   filename,
                                   100,
                                   NULL, 0, NULL, 0 ) != UNZ_OK )
        {
            CCLog( "could not read file infon" );
            unzClose( zipfile );
            return;
        }
        
        // Buffer to hold data read from the zip file.

        char read_buffer[ READ_SIZE ];
        
        std::string str(filename);
        
        //if(str.find(".png") != string::npos || str.find(".gif") != string::npos){
        
        CCLog("debug extracting file %s",filename );
        // Check if this entry is a directory or file.
        const size_t filename_length = strlen( filename );
        if ( filename[ filename_length-1 ] == dir_delimter )
        {
            // Entry is a directory, so create it.
            printf( "dir : %s", filename );
        }
        else
        {
            // Entry is a file, so extract it.
            CCLog( "file : %s", filename );
            if ( unzOpenCurrentFile( zipfile ) != UNZ_OK )
            {
                CCLog( "could not open file" );
                unzClose( zipfile );
                return ;
            }
            
            // Open a file to write out the data.
            std::string fname = filename;
            Network::replaceAll(fname, "/", "@"); // '/' -> '@'로 변형 (폴더 저장 방식을 피하기 위해)
            std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + fname;
            
            CCLog("%s", filepath.c_str());
            FILE *out = fopen( filepath.c_str(), "wb" );
            if ( out == NULL )
            {
                CCLog( "could not open destination file" );
                //unzCloseCurrentFile( zipfile );
                //unzClose( zipfile );
                //return;
            }
            else
            {
                int error = UNZ_OK;
                do
                {
                    error = unzReadCurrentFile( zipfile, read_buffer, READ_SIZE );
                    if ( error < 0 )
                    {
                        printf( "error %dn", error );
                        unzCloseCurrentFile( zipfile );
                        unzClose( zipfile );
                        return;
                    }
                    
                    // Write data to file.
                    if ( error > 0 )
                    {
                        int dbuf = fwrite( read_buffer, error, 1, out ); // You should check return of fwrite...
                        CCLog("debug bytes written %d %s", dbuf, fname.c_str());
                    }
                } while ( error > 0 );
                
                fclose( out );
            }
        }
        
        unzCloseCurrentFile( zipfile );
        
        
        // Go the the next entry listed in the zip file.
        if ( ( i+1 ) < global_info.number_entry )
        {
            if ( unzGoToNextFile( zipfile ) != UNZ_OK )
            {
                printf( "cound not read next file" );
                unzClose( zipfile );
                return;
            }
        }
        
    }
    
    unzClose( zipfile );
}
*/


/*
void Splash::Compress(unsigned char* data, int size)
{
    unsigned char* outbuf;
    unsigned long OUTBUFSIZE;
    
    //Upon entry, destLen is the total size of the destination buffer,
    //which must be at least 0.1% larger than sourceLen plus 12 bytes.
    OUTBUFSIZE = (unsigned long)1.001*(size+12) + 1;
    OUTBUFSIZE += 3000;
    outbuf = new unsigned char[OUTBUFSIZE];
    
    //#define Z_NO_COMPRESSION         0
    //#define Z_BEST_SPEED             1
    //#define Z_BEST_COMPRESSION       9
    //#define Z_DEFAULT_COMPRESSION  (-1)
    int err = compress2(outbuf, &OUTBUFSIZE, data, size, Z_BEST_COMPRESSION);
    

    // Z_OK             if success,
    // Z_MEM_ERROR      if there was not enough memory,
    // Z_BUF_ERROR      if there was not enough room in the output buffer
    // Z_STREAM_ERROR   if the level parameter is invalid
    if (err == Z_OK)
    {
        // original -> compress -> (base64) -> rsa -> base64(+ -> -)
        
        // 1) compress done
        CCLog("compress : success (size = %ld)", OUTBUFSIZE);
        
        
        // 2) base64 encode
        std::string base64 = Common::base64_encode(outbuf, OUTBUFSIZE);
        CCLog("1st base64 size = %d", (int)base64.size());
        
        // 3) rsa + base64 encode + (+ -> -)
        std::string encoded = Network::Encrypt_a(base64, (int)base64.size());
        CCLog("final length = %d", (int)encoded.size());
        
        
        std::string rfn = resourceFilename[numOfDownloadedFiles];
        rfn = rfn.substr(rfn.find("/")+1);
        Network::replaceAll(rfn, "/", "@");
        
        std::string filepath = CCFileUtils::sharedFileUtils()->getWritablePath() + rfn + "2";
        CCLog("filepath = %s", filepath.c_str());
        
        FILE* ptr_fp;
        if ((ptr_fp = fopen(filepath.c_str(), "wb")) == NULL)
        {
            CCLog("FILE OPEN ERROR !");
            exit(1);
        }
        
        size_t realSize = (int)encoded.size();
        size_t fwriteSize = fwrite(encoded.c_str(), 1, realSize, ptr_fp);
        //CCLog("%d %d", (int)realSize, (int)fwriteSize);
        if (fwriteSize != realSize)
        {
            CCLog("FILE WRITE ERROR !");
            exit(1);
        }
        fclose(ptr_fp);
        
        

        char decryptedData[3000000];
        int dSize = Network::DeObfuscation(encoded, decryptedData);
        std::string decoded = Common::base64_decode(decryptedData);
        
        // uncompress
        unsigned char* recbuf;
        unsigned long RECBUFSIZE = size + 5;
        recbuf = new unsigned char[RECBUFSIZE];

        err = uncompress(recbuf, &RECBUFSIZE, (unsigned char*)decoded.c_str(), (int)decoded.size());
        if (err == Z_OK)
        {
            CCLog("UNcompress : success (size = %ld)", RECBUFSIZE);
        }
        else
        {
            CCLog("UNcompress : ERROR (code = %d)", err);
        }
    }
    else
    {
        CCLog("compress : ERROR (code = %d)", err);
    }
    
    
    delete [] outbuf;
    //delete [] inbuf;
    
}
*/
