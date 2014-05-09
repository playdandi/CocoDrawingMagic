#include "Ranking.h"
#include "pugixml/pugixml.hpp"
#include "curl/curl.h"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

static int fromWhere;

enum
{
	zBackground = 0,
	zGameObject = 1,
    zGameObjectSP = 2,
    zPieceConn = 3,
};

CCScene* Ranking::scene(int from)
{
    fromWhere = from;
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
    
    if (fromWhere != -1)
        Common::ShowNextScene(this, "Ranking", "GameReady", false);
}
void Ranking::onPause()
{
    CCLog("Ranking :: onPause");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}
void Ranking::onExit()
{
    CCLog("Ranking :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
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
    
    // make depth tree
    Depth::AddCurDepth("Ranking");
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(0);
    CCLog("Ranking : touch prio = %d", this->getTouchPriority());
    
    srand(time(NULL));
    
    // 퍼즐게임이 끝나고 돌아온 경우 다시 spriteFrameCache를 불러와야 한다.
    if (fromWhere != -1)
    {
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_1.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/texture_2.plist");
    }
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/fairy.plist");
    
    spriteClass = new SpriteClass();
    spriteClassProperty = new SpriteClass();
    
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
    
    // 효과음 , 배경음은 클라이언트에 user data로 보관해야 한다. 기본 세팅을 위한 코드.
    bool opt0 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_0", true);
    bool opt1 = CCUserDefault::sharedUserDefault()->getBoolForKey("setting_option_1", true);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_0", opt0);
    CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_1", opt1);
    
    sound = new Sound();
    sound->PreLoadSound();
    if (opt1)
        sound->PlayBackgroundSound();
    
    isOnceScrollViewTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    isTouched = false;
    isKeyBackClicked = false;
    
    this->schedule(schedule_selector(Ranking::PotionTimer), 1.0f);
    
	return true;
}


void Ranking::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeyBackClicked = false;
        CCLog("Ranking : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
        
        // 포션 개수에 따른 포션 아이콘 업데이트
        char name[40];
        for (int i = 0 ; i < 5 ; i++)
        {
            sprintf(name, "icon/icon_potion.png%d", i);
            if (i < myInfo->GetPotion())
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
            else
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        }
        
        // message Count 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setString(Common::MakeComma(myInfo->GetMsgCnt()).c_str());
        if (myInfo->GetMsgCnt() == 0)
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(0);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(0);
        }
        else
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(255);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(255);
        }
        
        // 포션 보내기에 대한 정보 업데이트
        //for (int i = 0 ; i < friendList.size() ; i++)
        //    friendList[i]->SetPotionSprite();
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("Ranking 터치 비활성");
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 토파즈, 별사탕, MP, 포션남은시간 정보 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(1))->setString(Common::MakeComma(myInfo->GetTopaz()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(2))->setString(Common::MakeComma(myInfo->GetStarCandy()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(3))->setString(Common::MakeComma(myInfo->GetMPTotal()).c_str());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
        
        // 포션 개수에 따른 포션 아이콘 업데이트
        char name[40];
        for (int i = 0 ; i < 5 ; i++)
        {
            sprintf(name, "icon/icon_potion.png%d", i);
            if (i < myInfo->GetPotion())
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
            else
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(0);
        }
        
        // message Count 업데이트
        ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setString(Common::MakeComma(myInfo->GetMsgCnt()).c_str());
        if (myInfo->GetMsgCnt() == 0)
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(0);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(0);
        }
        else
        {
            ((CCLabelTTF*)spriteClass->FindLabelByTag(0))->setOpacity(255);
            ((CCSprite*)spriteClass->FindSpriteByName("background/bg_msg_setting.png"))->setOpacity(255);
        }
    }
    else if (param->intValue() == 3)
    {
        pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
        pBlack->setPosition(ccp(0, 0));
        pBlack->setAnchorPoint(ccp(0, 0));
        pBlack->setColor(ccc3(0, 0, 0));
        this->addChild(pBlack, 0);
    }
    else if (param->intValue() == 4)
    {
        EndScene();
    }
    else if (param->intValue() == 8)
    {
        // 스킬 속성 종류 갱신
        InitProperties();
    }
}

void Ranking::InitProperties()
{
    spriteClassProperty->RemoveAllObjects();
    
    CCPoint pos = ccp(848, 1611);
    if (myInfo->IsFire())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png1", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png1"), CCSize(0, 0), "background/bg_property.png1", "0", NULL, 5, 1) );
        pos = ccp(904, 1611);
    }
    if (myInfo->IsWater())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png2", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png2"), CCSize(0, 0), "background/bg_property.png2", "0", NULL, 5, 1) );
        pos = ccp(959, 1611);
    }
    if (myInfo->IsLand())
    {
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property.png3", ccp(0, 0), pos, CCSize(0, 0), "", "Ranking", this, 5) );
        spriteClassProperty->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClassProperty->FindParentCenterPos("background/bg_property.png3"), CCSize(0, 0), "background/bg_property.png3", "0", NULL, 5, 1) );
        pos = ccp(959, 1611);
    }
    
    for (int i = 0 ; i < spriteClassProperty->spriteObj.size() ; i++)
        spriteClassProperty->AddChild(i);
}

void Ranking::InitSprites()
{
    // background
    pBackground = CCSprite::create("images/main_background.png");
    pBackground->setAnchorPoint(ccp(0, 0));
    pBackground->setPosition(ccp(0, 0));
    this->addChild(pBackground, 0);
    
    CCLog("=========================================================");
    CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
    CCLog("=========================================================");
    
    char name[30], name2[30];
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png1", ccp(0, 0), ccp(80, 1666), CCSize(230, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png2", ccp(0, 0), ccp(390, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_topinfo.png3", ccp(0, 0), ccp(765, 1666), CCSize(290, 75), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0, 0), ccp(15+10, 1656), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0, 0), ccp(317, 1660), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png", ccp(0, 0), ccp(696, 1669), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png1", ccp(0, 0), ccp(80+230-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png2", ccp(0, 0), ccp(390+290-55, 1679), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0.5, 0), ccp((80+230+80)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0.5, 0), ccp((390+290+390)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0.5, 0), ccp((765+765+290)/2, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 3) );
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 458), CCSize(982, 954), "", "Ranking", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(75, 492), CCSize(929, 904), "", "Ranking", this, 0) );
    
    // property 문양 (mp 바로 밑에)
    InitProperties();
    
    // 메인 지붕
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png1", ccp(0, 0), ccp(10, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    CCSize roofSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_roof.png2", ccp(0, 0), ccp(10+roofSize.width, 1433), CCSize(0, 0), "", "Ranking", this, 1) );
    spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->setFlipX(true);
    
    // 플러스 아이콘들
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_potion_time.png", ccp(0, 0), ccp(506, 1493), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_topinfo_plus.png3", ccp(0, 0), ccp(645, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 스트랩
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_red.png", ccp(0, 0), ccp(14, 1343), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_rank.png", ccp(0, 0), ccp(118, 1920-441-88), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_rank_time.png", ccp(0, 0), ccp(423, 1391), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 스케치북 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_sketchbook.png", ccp(0, 0), ccp(67, 1920-1569-96), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_sketchbook.png", ccp(0, 0), ccp(53, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );

    // 친구추가 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_addfriend.png", ccp(0, 0), ccp(912, 1920-1582-86), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_invitefriend.png", ccp(0, 0), ccp(886, 1920-1674-44), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 게임준비 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red.png", ccp(0, 0), ccp(319, 191), CCSize(0, 0), "", "Ranking", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_gameready.png", ccp(0.5, 0.5), ccp(319+446/2, 191+160/2+5), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 세팅 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_setting.png", ccp(0, 0), ccp(750, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    // 메시지함 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_msg.png", ccp(0, 0), ccp(750+90+35, 1498), CCSize(0, 0), "", "Ranking", this, 5) );
    
    // 메시지 빨간배경 + 숫자
    int opacity = 255;
    if (myInfo->GetMsgCnt() == 0)
        opacity = 0;
    CCSize msgSize = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize();
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_msg_setting.png", ccp(0, 0), ccp(msgSize.width-30, msgSize.height-30), CCSize(0, 0), "button/btn_msg.png", "0", NULL, 5, 1, opacity) );
    sprintf(name, "%d", myInfo->GetMsgCnt());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 30, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_msg_setting.png"), ccc3(255,255,255), "background/bg_msg_setting.png", "0", NULL, 5, 2, opacity, 0) );
    
    // make potion
    for (int i = 0; i < 5; i++)
    {
        sprintf(name, "icon/icon_potion_empty.png%d", i);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(89+83*i, 1480), CCSize(0, 0), "", "Ranking", this, 4) );
        
        sprintf(name2, "icon/icon_potion.png%d", i);
        if (i < myInfo->GetPotion())
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 255) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 4, 1, 0) );
    }

    /*
    // topaz
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetTopaz()), fontList[0], 36, ccp(0, 0), ccp(110, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 1) );
    // starcandy
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetStarCandy()), fontList[0], 36, ccp(0, 0), ccp(420, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 2) );
    // magic-point
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(myInfo->GetMPTotal()), fontList[0], 36, ccp(0, 0), ccp(800, 1686), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 3) );
     */
    
    // potion-remain-time
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainPotionTime(), fontList[0], 36, ccp(0, 0), ccp(530, 1508), ccc3(255,255,255), "", "Ranking", this, 5, 0, 255, 4) );
    
    // 주간랭킹 남은시간
    if (myInfo->GetRemainWeeklyRankTimeInt() < 86400) // '시간' 단어가 들어갈 경우
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 36, ccp(0.5, 0.5), ccp(498, 1450), ccc3(152, 49, 64), "", "Ranking", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 36, ccp(0.5, 0.5), ccp(498, 1453), ccc3(255,255,255), "", "Ranking", this, 5) );
    }
    else // '일' 단어가 들어갈 경우
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1440), ccc3(152, 49, 64), "", "Ranking", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(myInfo->GetRemainWeeklyRankTime(), fontList[0], 48, ccp(0.5, 0.5), ccp(498, 1443), ccc3(255,255,255), "", "Ranking", this, 5) );
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("남음", fontList[0], 30, ccp(0, 0), ccp(550, 1400), ccc3(212, 212, 212), "", "Ranking", this, 5) );
}

void Ranking::MakeScroll()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    
    int numOfList = friendList.size();
    //CCLog("number of friends = %d", numOfList);
    
    char rankNum[3], name[40], score[12];
    for (int i = 0 ; i < numOfList ; i++)
    {
        //CCLog("friend # %d", i);
        CCLayer* profileLayer = CCLayer::create();
        profileLayer->setContentSize(CCSizeMake(862, 166));
        profileLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(profileLayer, 5);
        profileLayers.push_back(profileLayer);
        
        // my profile bg
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
        {
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
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(rankNum, fontList[0], 48, ccp(0, 0), ccp(32, 115), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
        }
        
        // profile image
        if (friendList[i]->GetImageUrl() != "")
        {
            friendList[i]->GetProfile()->setScale(0.85f);
            friendList[i]->GetProfile()->setPosition(ccp(102+5, 36+10));
        }
        else
        {
            friendList[i]->GetProfile()->setPosition(ccp(102, 36));
        }
        friendList[i]->GetProfile()->setAnchorPoint(ccp(0, 0));
        profileLayer->addChild(friendList[i]->GetProfile(), 5);
        
        if (friendList[i]->GetImageUrl() != "")
        {
            sprintf(name, "background/bg_profile.png%d", i);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
        }
        
        // user name
        friendList[i]->GetNicknameLabel()->setAnchorPoint(ccp(0, 0));
        friendList[i]->GetNicknameLabel()->setPosition(ccp(252, 110));
        friendList[i]->GetNicknameLabel()->setColor(ccc3(78,47,8));
        profileLayer->addChild(friendList[i]->GetNicknameLabel(), 5);
        
        // user score
        sprintf(score, "%d", friendList[i]->GetWeeklyHighScore());
        CCLayer* scoreLayer = Common::MakeImageNumberLayer(score, 0);
        scoreLayer->setPosition(ccp(282, 36));
        profileLayer->addChild(scoreLayer, 5);

        // potion
        if (friendList[i]->GetPotionSprite() != NULL)
            profileLayer->addChild(friendList[i]->GetPotionSprite(), 5);
        
        // potion timer (안 보여도 그냥 포지션만 잡아 놓는다)
        friendList[i]->GetPotionLabelMin()->setPosition(ccp(765, 80));
        profileLayer->addChild(friendList[i]->GetPotionLabelMin(), 6);
        friendList[i]->GetPotionLabelMin()->setString("60:");
        friendList[i]->GetPotionLabelSec()->setPosition(ccp(773, 52));
        profileLayer->addChild(friendList[i]->GetPotionLabelSec(), 6);
        friendList[i]->GetPotionLabelSec()->setString("00");
        if (friendList[i]->GetRemainPotionTime() == 0 || friendList[i]->GetPotionMsgStatus() == 0)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(0);
            friendList[i]->GetPotionLabelSec()->setOpacity(0);
        }
        
        // dotted line
        //if (i < numOfList-1)
        //{
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", profileLayer, 5) );
        //}
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

void Ranking::PotionTimer(float f)
{
    char time[5];
    int remainTime, potion;
    
    potion = myInfo->GetPotion();
    if (potion < 5) // 5개 이상 되면 할 필요없음.
    {
        // 나의 포션 정보 갱신
        remainTime = myInfo->GetRemainPotionTimeNumber();
        if (remainTime == 0)
        {
            remainTime = 720;
            potion++;
            
            char name[25];
            sprintf(name, "icon/icon_potion.png%d", potion-1);
            ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
        }
        myInfo->SetPotion(potion, remainTime-1);
        ((CCLabelTTF*)spriteClass->FindLabelByTag(4))->setString(myInfo->GetRemainPotionTime().c_str());
    }
    
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        remainTime = friendList[i]->GetRemainPotionTime();
        if (remainTime > 0)
            friendList[i]->SetRemainPotionTime(remainTime-1);
        // 시간을 나타내야 하는 포션은 타이머를 보여준다.
        if (remainTime-1 > 0 && friendList[i]->GetPotionMsgStatus() == 1)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(255);
            friendList[i]->GetPotionLabelSec()->setOpacity(255);
            
            if ((remainTime-1)/60 < 10)
                sprintf(time, "0%d:", (remainTime-1)/60);
            else
                sprintf(time, "%d:", (remainTime-1)/60);
            friendList[i]->GetPotionLabelMin()->setString(time);
            
            if ((remainTime-1)%60 < 10)
                sprintf(time, "0%d", (remainTime-1)%60);
            else
                sprintf(time, "%d", (remainTime-1)%60);
            friendList[i]->GetPotionLabelSec()->setString(time);
        }
        
        // 이제 막 00:00가 되었다면 potion sprite를 바꾼다.
        if (remainTime-1 == 0)
        {
            friendList[i]->GetPotionLabelMin()->setOpacity(0);
            friendList[i]->GetPotionLabelSec()->setOpacity(0);
            friendList[i]->SetPotionSprite();
        }
    }
}



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
                //Common::ShowNextScene(this, "Ranking", "GameReady", false, -1, priority-1);
                Common::ShowNextScene(this, "Ranking", "GameReady", false, -1);
                break;
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
                //Common::ShowNextScene(this, "Ranking", "Sketchbook", false, 0, priority-1);
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
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        p = friendList[i]->GetProfile()->convertToNodeSpace(point);
        // convertToNodeSpace 설명
        // ex) sprite1->convertToNodeSpace(sprite2->getPosition());
        // sprite1 좌측하단부터 sprite2의 anchorPoint까지의 거리
        CCSize size = friendList[i]->GetProfile()->getContentSize();
        if (isScrollViewTouched && !isScrolling &&
            (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height)
        {
            sound->playClick();
            Common::ShowNextScene(this, "Ranking", "Profile", false, i);
        }
        
        if (friendList[i]->GetPotionSprite() != NULL)
        {
            // 포션 보낼 수 있는 상태 & 남은시간 = 0 일 때만 가능하다.
            p = friendList[i]->GetPotionSprite()->convertToNodeSpace(point);
            CCSize size = friendList[i]->GetPotionSprite()->getContentSize();
            if (isScrollViewTouched && !isScrolling &&
                (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= size.width && (int)p.y <= size.height &&
                friendList[i]->GetPotionMsgStatus() == 1 && friendList[i]->GetRemainPotionTime() == 0)
            {
                sound->playClick();
                std::vector<int> data;
                data.push_back(i);
                Common::ShowPopup(this, "Ranking", "NoImage", false, POTION_SEND_TRY, BTN_2, data);
                break;
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


void Ranking::EndScene()
{
    CCLog("Ranking : EndScene()");
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->unschedule(schedule_selector(Ranking::PotionTimer));
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Ranking");
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassProperty->RemoveAllObjects();
    delete spriteClassProperty;
    
    // itemLayer
    for (int i = 0 ; i < profileLayers.size() ; i++)
    {
        profileLayers[i]->removeAllChildren();
        //CCLog("ii : %d", profileLayers[i]->retainCount());
    }
    profileLayers.clear();
    
    // background layer
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    //CCLog("pBlack %d", pBlack->retainCount());
    //CCLog("pBackground %d", pBackground->retainCount());
    
    scrollView->getContainer()->removeAllChildren();
    scrollView->removeAllChildren();
    scrollView->removeFromParent();
    //CCLog("%d", scrollView->retainCount());
    
    //this->removeFromParentAndCleanup(true);
    
    sound->StopBackgroundSound();
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/texture_1.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/texture_2.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/texture_1.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/texture_2.png");
    
    Common::ShowNextScene(this, "Ranking", "Loading", true);
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
