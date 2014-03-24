#include "Ranking.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

enum
{
	zBackground = 0,
	zGameObject = 1,
    zGameObjectSP = 2,
    zPieceConn = 3,
};

CCScene* Ranking::scene()
{
	CCScene* pScene = CCScene::create();
	Ranking* pLayer = Ranking::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Ranking::onEnter()
{
    CCLog("Ranking :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Ranking::onPause()
{
    CCLog("Ranking :: onPause");
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    //CCLayer::onEnter();
}
void Ranking::onExit()
{
    CCLog("Ranking :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Ranking::keyBackClicked()
{
    if (isKeyBackClicked)
        return;
    isKeyBackClicked = true;
    
    sound->playWarning();
    
    std::vector<int> nullData;
    Common::ShowPopup(this, "Ranking", "NoImage", false, POPUP_EXIT, BTN_2, nullData);
}

bool Ranking::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    srand(time(NULL));
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_1.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_2.plist");
    
    spriteClass = new SpriteClass();
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Ranking::Notification), "Ranking", NULL);
    
    
    /*
     - RGBA8888(kTexture2DPixelFormat_RGBA8888) : 최대 품질을 보장하며 32비트 픽셀 포맷이다. 단점은 다른 포맷들에 비해 메모리를 많이 소모한다는 점이다. 16비트 텍스처의 두 배나 되기 때문에 렌더링도 느리다. 이 픽셀 포맷이 적용된 이미지는 1024×1024 해상도에서 4MB, 2048×2048 해상도에서 16MB의 메모리를 사용한다. 다음에 설명할 RGB4444를 적용하면 이를 반으로 줄일 수 있다.
     - RGBA4444(kTexture2DPixelFormat_RGBA4444) : 16비트 픽셀 포맷으로 적당한 수준의 품질과 속도를 보장하며, 메모리도 그다지 많이 사용하지 않는다.
     - RGB5_A1( kTexture2DPixelFormat_RGB5A) : 역시 16비트 픽셀 포맷이며 RGB 채널에는 상대적으로 높은 품질을 보장하지만, 알파 채널에는 1비트만 할당했기 때문에 상세한 표현이 불가능하다. 평균적인 속도와 메모리 사용량을 갖는다.
     - RGB565(kTexture2DPixelFormat_RGB565) : 알파 채널을 지원하지 않지만 16비트 텍스체 중 가장 나은 RGB 품질을 갖는다.
     */
    /*
    CCTexture2D::setDefaultAlphaPixelFormat(kTexture2DPixelFormat_RGBA4444);
    // load images
    tBackground = CCTextureCache::sharedTextureCache()->addImage("images/ranking.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking.png");
    tBackground = NULL;
    */
    
    
    InitSprites();
    MakeScroll();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    
    bool opt0 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_0", true);
    bool opt1 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true);
    bool opt2 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_2", true);
    bool opt3 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_3", true);
    bool opt4 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_4", true);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_0", opt0);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_1", opt1);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_2", opt2);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_3", opt3);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_4", opt4);
    
    sound = new Sound();
    sound->PreLoadSound();
    if (opt1)
        sound->PlayBackgroundSound();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    //scrollViewLastPoint = ccp(0, 0);
    isOnceScrollViewTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
    isKeyBackClicked = false;
    
	return true;
}

void Ranking::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        //CCLog("noti 0 - ranking 터치 활성");
        //CCDirector::sharedDirector()->resume();
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
        isTouched = false;
        isKeyBackClicked = false;
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("noti 1 - 터치 비활성");
        //CCDirector::sharedDirector()->pause();
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
    }
}

void Ranking::InitSprites()
{
    CCTexture2D::setDefaultAlphaPixelFormat(kTexture2DPixelFormat_RGBA4444);
    // load images
    CCTexture2D* tBackground = CCTextureCache::sharedTextureCache()->addImage("images/bg.png");
    CCSprite* temp = new CCSprite();
    temp->initWithTexture(tBackground, CCRectMake(0, 0, 500, 889));
    temp->setAnchorPoint(ccp(0, 0));
    temp->setPosition(ccp(0, 0));
    temp->setScaleX((float)1080/500);
    temp->setScaleY((float)1920/889);
    this->addChild(temp, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Ranking", this, 0) );
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15, 1656), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), ccp(848, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), ccp(904, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), ccp(959, 1611), CCSize(0, 0), "", "Ranking", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_rank.png", ccp(0, 0), ccp(118, 1920-441-88), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_rank_time.png", ccp(0, 0), ccp(423, 1391), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_sketchbook.png", ccp(0, 0), ccp(67, 1920-1569-96), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_sketchbook.png", ccp(0, 0), ccp(53, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png", ccp(0, 0), ccp(912, 1920-1582-86), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_addfriend.png", ccp(0, 0), ccp(886, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0, 0), ccp(319, 191), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gameready.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_setting.png", ccp(0, 0), ccp(750, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_msg.png", ccp(0, 0), ccp(750+90+35, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    CCSize msgSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_msg_setting.png", ccp(0, 0), ccp(msgSize.width-30, msgSize.height-30), CCSize(0, 0), "button/btn_msg.png", "0", NULL, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("88", fontList[0], 30, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_msg_setting.png"), ccc3(255,255,255), "background/bg_msg_setting.png", "0", NULL, 5) );
    
    // make potion
    char name[30], name2[30];
    int numOfPotion = 3;
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "Ranking", this, 4) );
        if (i < numOfPotion)
        {
            sprintf(name2, "icon/icon_potion.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4) );
        }
    }
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("88,888", fontList[0], 36, ccp(0, 0), ccp(105, 1686), ccc3(255,255,255), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("200,000", fontList[0], 36, ccp(0, 0), ccp(415, 1686), ccc3(255,255,255), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("5,200,000", fontList[0], 36, ccp(0, 0), ccp(795, 1686), ccc3(255,255,255), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("11:59", fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("23:50", fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1445), ccc3(152, 49, 64), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("23:50", fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1448), ccc3(255,255,255), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("남음", fontList[0], 30, ccp(0, 0), ccp(550, 1400), ccc3(212, 212, 212), "", "Ranking", this, 5) );
    
}

void Ranking::MakeScroll()
{
    // usernames (temporary)
    usernames.push_back("박일진");
    usernames.push_back("동해물과");
    usernames.push_back("정연준");
    usernames.push_back("Yhlas Beganov");
    usernames.push_back("Sera");
    usernames.push_back("AHRORO");
    usernames.push_back("TREVI");
    usernames.push_back("칸쵸");
    usernames.push_back("로드리게스");
    usernames.push_back("yo so sexy");
    usernames.push_back("큭");
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_NOTHING);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_X);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_X);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_SEND);
    
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    int numOfList = 11;
    
    char rankNum[3], name[40];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* profileLayer = CCLayer::create();
        profileLayer->setContentSize(CCSizeMake(862, 166));
        profileLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(profileLayer, 5);
        
        // my profile bg
        if (usernames[i] == "정연준")
        {
            CCLog("hi");
            spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_my_profile.png", ccp(0, 0), ccp(0, 15), CCSize(862, 166-10), "", "Layer", profileLayer, 3) );
        }
        
        // rank number
        if (i < 3) // 1~3위는 이미지 사용
        {
            CCPoint pos;
            if (i == 0) pos = ccp(0, 65);
            else pos = ccp(8, 77);
            char number[28];
            sprintf(number, "icon/icon_rank_number_%d.png", i+1);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, number, ccp(0, 0), pos, CCSize(0, 0), "", "Layer", profileLayer, 5) );
        }
        else // 4위부터는 일반 font 사용
        {
            sprintf(rankNum, "%d", i+1);
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(rankNum, fontList[0], 36, ccp(0, 0), ccp(32, 115), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
        }
        
        // profile image
        sprintf(name, "background/bg_profile.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
        
        // user name
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(usernames[i].c_str(), fontList[0], 48, ccp(0, 0), ccp(252, 110), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
        
        // user score
        CCLayer* scoreLayer = Common::MakeImageNumberLayer("14,083,569");
        scoreLayer->setPosition(ccp(282, 36));
        profileLayer->addChild(scoreLayer, 5);

        // potion state
        if (userPotionStates[i] != POTION_NOTHING)
        {
            char potionState[18];
            if (userPotionStates[i] == POTION_REMAIN)
                sprintf(potionState, "icon/icon_potion_remain.png");
            else if (userPotionStates[i] == POTION_SEND)
                sprintf(potionState, "icon/icon_potion_send.png");
            else
                sprintf(potionState, "icon/icon_potion_x.png");
            
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, potionState, ccp(0, 0), ccp(724, 24), CCSize(0, 0), "", "Layer", profileLayer, 5) );
        }
        
        // dotted line
        if (i < numOfList-1)
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", profileLayer, 5) );
        }
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    //scrollViewLastPoint = scrollView->getContentOffset();
    //scrollView->setBounceable(false);
    this->addChild(scrollView, 5);
}

/*void Ranking::touchDownAction(CCObject* sender, CCControlEvent controlEvent)
{
    CCLog("touch down action!");
}*/

bool Ranking::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("Ranking : (%d , %d)", (int)point.x, (int)point.y);
    
    // scrollview touch check
    if (scrollView->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "GameReady", false);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false, 0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false, 0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_topinfo_plus.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClickboard();
                Common::ShowNextScene(this, "Ranking", "BuyPotion", false, 0);
            }
        }
        
        else if (spriteClass->spriteObj[i]->name == "button/btn_msg.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Message", false);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_setting.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Setting", false);
            }
        }
        
        else if (spriteClass->spriteObj[i]->name == "button/btn_sketchbook.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Sketchbook", false, 0);
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_addfriend.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "InviteFriend", false);
            }
        }
    }
    
    return true;
}


void Ranking::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    //scrollViewLastPoint = scrollView->getContentOffset();
}

void Ranking::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    CCPoint p;
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 25) == "background/bg_profile.png")
        {
            p = spriteClass->spriteObj[i]->sprite->convertToNodeSpace(point);
            // convertToNodeSpace 설명
            // ex) sprite1->convertToNodeSpace(sprite2->getPosition());
            // sprite1 좌측하단부터 sprite2의 anchorPoint까지의 거리
            CCSize size = spriteClass->spriteObj[i]->sprite->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
            {
                sound->playClick();
                Common::ShowNextScene(this, "Ranking", "Profile", false);
            }
        }
    }
    
    //scrollViewLastPoint = scrollView->getContentOffset();
    isOnceScrollViewTouched = true;
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
    CCLog("Ranking : touch ended");
    
    //scrollView->setBounceable(false);
    //scrollView->setBounceable(true);
    //scrollView->i
    /*
    //scrollView->setClippingToBounds(false);
    //scrollView->setBounceable(false);
    scrollView->setAccelerometerEnabled(true);
    CCLog("%d" ,scrollView->isAccelerometerEnabled());
    CCLog("%d" ,scrollView->isBounceable());
    CCLog("%d" ,scrollView->isClippingToBounds());
    */
//    CCLog("%d" ,scrollView->isDragging());
    //CCLog("%d", scrollView->isAccelerometerEnabled());
    //scrollView->setAccelerometerEnabled(true);
    //scrollView->setAccelerometerInterval(0.5);
}


void Ranking::scrollViewDidScroll(CCScrollView* view)
{
    //view->stopAllActions();
    /*
    if (isOnceScrollViewTouched && !scrollView->isDragging())
    {
        CCLog("%f , %f", scrollViewLastPoint.x, scrollViewLastPoint.y);
        scrollView->setContentOffset(scrollViewLastPoint);
        //return;
    }
    else {
     */
    //CCLog("dragging : %d" ,);
    //if (scrollView->isDragging())
    //{
    //CCPoint p = scrollView->getContentOffset();
    //CCLog("offset : %d , %d", (int)p.x, (int)p.y);
    //CCLog("%d , %d, %d", scrollView->isRunning(), scrollView->isTouchMoved(), scrollView->isDragging());
    isScrolling = true;
    //}
}

void Ranking::scrollViewDidZoom(CCScrollView* view)
{
}
