#include "NoImage.h"
#include "BuyPotion.h"
#include "SketchDetail.h"
#include "Kakao/Plugins/KakaoNativeExtension.h"

static int type;
static int btn;
static std::vector<int> d;
static int fromWhere;

static int newSkillType;

static std::string sendLinkType; // kakao msg 보낼 때 타입

CCScene* NoImage::scene(int popupType, int btnType, std::vector<int> data, int etc, std::vector<std::string> sdata)
{
    // data
    type = popupType;
    btn = btnType;
    d = data;
    
    fromWhere = etc;
    
    CCScene* pScene = CCScene::create();
    NoImage* pLayer = NoImage::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void NoImage::onEnter()
{
    //CCLog("NoImage : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(NoImage::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void NoImage::SceneCallback()
{
    isEnded = false;
    isTouched = false;
    isTouchDone = false;
    isKeybackTouched = false;
}
void NoImage::onExit()
{
    //CCLog("NoImage : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void NoImage::keyBackClicked()
{
    // 백버튼을 하면 안되는 팝업 종류들
    if (type == NEED_TO_REBOOT || type == NEED_TO_UPDATE || type == ERROR_IN_APP_BILLING || type == YOU_WERE_BLOCKED ||
        type == SERVER_CHECK || type == FUCKING_APP_DETECTED || type == ROOTING_DETECTED)
        return;
    
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    
    HandlingTouch(TOUCH_CANCEL);
}

void NoImage::onLogoutComplete()
{
    //CCLog("onLogoutComplete");

    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
void NoImage::onLogoutErrorComplete(char const* status, char const* error)
{
    //CCMessageBox(error, "onLogoutErrorComplete");
    //CCLog("onLogoutErrorComplete : %s, %s", status, error);
}

void NoImage::onUnregisterComplete()
{
    ////CCLog("onUnregisterComplete");
    KakaoNativeExtension::getInstance()->auth(std::bind(&NoImage::onAuthComplete, this, std::placeholders::_1), std::bind(&NoImage::onAuthErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
}
void NoImage::onUnregisterErrorComplete(char const* status, char const* error)
{
    //CCMessageBox(error, "onUnregisterErrorComplete");
    //CCLog("onUnregisterErrorComplete : %s, %s", status, error);
}

void NoImage::onAuthComplete(bool result)
{
    //CCLog("onAuthComplete : result (%d)", result);

    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
void NoImage::onAuthErrorComplete(char const* status, char const* error)
{
    //CCLog("onAuthErrorComplete : %s, %s", status, error);

    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void NoImage::onSendLinkMessageComplete()
{
    //CCLog("onSendLinkMessageComplete");
}
void NoImage::onSendLinkMessageErrorComplete(char const *status, char const *error)
{
    //CCMessageBox(error, "onSendLinkMessageErrorComplete");
    //CCLog("onSendLinkMessageErrorComplete : %s, %s", status, error);
}




bool NoImage::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isEnded = true;
    isTouched = true;
    isTouchDone = true;
    isKeybackTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("NoImage", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("NoImage : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(NoImage::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(tLayer, 1);
    tLayer->setScale(0);
    
    InitSprites();
    
    return true;
}

void NoImage::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;

    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("NoImage : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("NoImage : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        if (idx > -1)
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
        }
    }
}

void NoImage::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    int offset = 0;
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640+offset), CCSize(982, 623-offset*2), "", "Layer", tLayer, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678+offset), CCSize(929, 562-offset*2), "", "Layer", tLayer, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039-offset), CCSize(0, 0), "", "Layer", tLayer, 10001) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132-offset), CCSize(0, 0), "", "Layer", tLayer, 10001) );
    
    
    bool hasImage = false;
    if (type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == UPGRADE_STAFF_OK ||
        type == BUY_TOPAZ_TRY || type == BUY_TOPAZ_OK || type == NEED_TO_BUY_TOPAZ ||
        type == NEED_TO_BUY_STARCANDY || type == BUY_STARCANDY_OK || type == BUY_STARCANDY_TRY ||
        type == NEED_TO_BUY_POTION || type == BUYPOTION_1 || type == BUYPOTION_OK ||
        type == UPGRADE_FAIRY_OK || type == UPGRADE_FAIRY_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_TOPAZ_TRY ||
        type == BUY_PROPERTY_TRY || type == BUY_PROPERTY_OK ||
        type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY || type == BUY_SKILLSLOT_OK ||
        type == TODAYCANDY_RESULT_LOSE || type == TODAYCANDY_RESULT_WIN || type == RANKUP_BOAST ||
        type == SELECT_PROPERTY_TRY || type == SELECT_PROPERTY_OK ||
        type == PURCHASE_SKILL_BY_TOPAZ_TRY)
    {
        hasImage = true;
        
        int off = 0;
        CCPoint p;
        std::string e;
        char txt[50];
        switch (type)
        {
            case UPGRADE_STAFF_BY_TOPAZ_TRY:
            case UPGRADE_STAFF_BY_STARCANDY_TRY:
            case UPGRADE_STAFF_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/magicstaff.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->setScale(0.85f);
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->setRotation(20);
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.0f, -10), CCRotateBy::create(2.0f, 20), CCRotateBy::create(1.0f, -10), NULL)));
                //if (type != UPGRADE_STAFF_OK)
                if (type == UPGRADE_STAFF_BY_STARCANDY_TRY)
                {
                    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question_mini.png", ccp(0, 0), ccp(903, 710+115), CCSize(0, 0), "", "Layer", tLayer, 10001) );
                    sprintf(txt, "강화포인트 : (%d/100)", myInfo->GetStaffFailPoint());
                    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(txt, fontList[0], 30, ccp(1, 0), ccp(903-10, 710+115), ccc3(78,47,8), "", "Layer", tLayer, 10001) );
                }
                break;
            case NEED_TO_BUY_TOPAZ:
            case BUY_TOPAZ_TRY:
            case BUY_TOPAZ_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_topaz.png"))->setScale(1.5f);
                break;
            case NEED_TO_BUY_STARCANDY:
            case BUY_STARCANDY_TRY:
            case BUY_STARCANDY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_starcandy.png"))->setScale(1.5f);
                break;
            case NEED_TO_BUY_POTION:
            case BUYPOTION_1:
            case BUYPOTION_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_potion.png"))->setScale(2.0f);
                break;
            case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            case UPGRADE_FAIRY_BY_TOPAZ_TRY:
            case UPGRADE_FAIRY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_drug.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_drug.png"))->setScale(1.0f);
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_drug.png"))->setRotation(20);
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_drug.png"))->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.0f, -10), CCRotateBy::create(2.0f, 20), CCRotateBy::create(1.0f, -10), NULL)));
                if (type == UPGRADE_FAIRY_BY_STARCANDY_TRY)
                {
                    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question_mini.png", ccp(0, 0), ccp(903, 710+115), CCSize(0, 0), "", "Layer", tLayer, 10001) );
                    for (int i = 0 ; i < myInfo->GetFairyList().size() ; i++)
                    {
                        if (myInfo->GetFairyList()[i]->GetId() == myInfo->GetActiveFairyId())
                        {
                            sprintf(txt, "강화포인트 : (%d/100)", myInfo->GetFairyList()[i]->GetFailPoint());
                            break;
                        }
                    }
                    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(txt, fontList[0], 30, ccp(1, 0), ccp(903-10, 710+115), ccc3(78,47,8), "", "Layer", tLayer, 10001) );
                }
                break;
            case BUY_PROPERTY_TRY:
            case BUY_PROPERTY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_room.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("image/coco_room.png"))->setScale(0.62f);
                break;
            case BUY_SKILLSLOT_BY_STARCANDY_TRY:
            case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            case BUY_SKILLSLOT_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                ((CCSprite*)spriteClass->FindSpriteByName("background/bg_skill_yellow.png"))->setScale(1.3f);
                p = spriteClass->FindParentCenterPos("background/bg_skill_yellow.png");
                
                if (d[0] >= 10)
                    off = 15;
                spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(Common::MakeComma(d[0]), fontList[0], 48, ccp(1, 0.5), ccp(p.x+3+off, p.y), ccc3(78,47,8), "background/bg_skill_yellow.png", "0", NULL, 10002) );
                if (d[0]%10 == 1) e = "st";
                else if (d[0]%10 == 2) e = "nd";
                else if (d[0]%10 == 3) e = "rd";
                else if (d[0]%10 == 0 || d[0]%10 >= 4) e = "th";
                spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(e, fontList[0], 24, ccp(0, 0), ccp(p.x+2+off, p.y-23), ccc3(78,47,8), "background/bg_skill_yellow.png", "0", NULL, 10002) );
                break;
            case TODAYCANDY_RESULT_WIN:
            case TODAYCANDY_RESULT_LOSE:
            case RANKUP_BOAST:
                ProfileSprite* psp;
                psp = ProfileSprite::GetObj(friendList[d[0]]->GetImageUrl());
                if (friendList[d[0]]->GetImageUrl() != "" && psp->IsLoadingDone())
                {
                    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002, 0, 255, 1.65f*0.95f) ); // 1.5675f
                    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "Layer", tLayer, 10003) );
                    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_profile.png"))->setScale(1.65f);
                }
                else
                {
                    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, psp->GetProfile(), ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "Layer", tLayer, 10002, 0, 255, 1.65f) );
                }
                break;
            case SELECT_PROPERTY_TRY:
            case SELECT_PROPERTY_OK:
                char temp[40];
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0.5, 0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "Layer", tLayer, 10002) );
                p = spriteClass->FindParentCenterPos("background/bg_skill_brown.png");
                if (d[0] == 1) sprintf(temp, "icon/icon_property_fire.png");
                else if (d[0] == 2) sprintf(temp, "icon/icon_property_water.png");
                else if (d[0] == 3) sprintf(temp, "icon/icon_property_land.png");
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, temp, ccp(0.5, 0.5), p, CCSize(0, 0), "background/bg_skill_brown.png", "0", NULL, 10002, 1) );
                break;
            case PURCHASE_SKILL_BY_TOPAZ_TRY:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "Layer", tLayer, 10005) );
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "Layer", tLayer, 10005) );
                break;
        }
    }
    
    
    // text (각 팝업 상황에 맞는 문구를 만들어 출력한다)
    char text[250];
    std::string title;
    int fontSize = 40;
    switch (type)
    {
        case YOU_WERE_BLOCKED:
            title = "오류";
            sprintf(text, "블록된 계정입니다. 문의사항은 help@playdandi.com 으로 이메일을 보내주세요."); break;
        case FUCKING_APP_DETECTED:
            title = "악성 앱 감지";
            sprintf(text, "악성 앱이 감지되었습니다. 제거한 뒤 다시 실행해 주세요."); break;
        case ROOTING_DETECTED:
            title = "루팅 폰 감지";
            sprintf(text, "사용하시는 폰이 루팅되어 있습니다. 루팅을 해제한 뒤 다시 실행해 주세요."); break;
        case SERVER_CHECK:
            title = "서버 점검중";
            sprintf(text, "%s", serverCheckMsg.c_str()); break;
        case KAKAO_TOKEN_ERROR:
            title = "오류";
            sprintf(text, "카카오 아이디 인증을 실패하였습니다."); break;
        case WILL_BE_UPDATED:
            title = "앱 업데이트";
            sprintf(text, "업데이트 예정입니다."); break;
        case NEED_TO_UPDATE:
            title = "앱 업데이트";
            sprintf(text, "업데이트된 버전이 출시되었습니다.\n마켓으로 이동합니다."); break;
        case NEED_TO_REBOOT:
            title = "다시 시작하기";
            sprintf(text, "세션 종료 혹은 업데이트로 인해\n게임을 재부팅합니다."); break;
        case NEED_TO_BUY_POTION:
            title = "포션 부족";
            sprintf(text, "포션이 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case NEED_TO_BUY_TOPAZ:
            title = "토파즈 부족";
            sprintf(text, "토파즈가 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case NEED_TO_BUY_STARCANDY:
            title = "별사탕 부족";
            sprintf(text, "별사탕이 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case NETWORK_FAIL:
            title = "오류";
            sprintf(text, "[오류] 다시 시도해 주세요."); break;
        case ERROR_IN_APP_BILLING:
            title = "결제 오류";
            sprintf(text, "결제 도중 문제가 발생하여 게임을 재시작합니다.\n재시작 후 결제한 상품이 없으면 고객센터로 연락주세요."); break;
        
        case BUY_TOPAZ_TRY:
            title = "토파즈 구매하기";
            sprintf(text, "토파즈 %d개를 구매하시겠습니까?", priceTopaz[d[0]]->GetCount()); break;
        case BUY_TOPAZ_OK:
            title = "토파즈 구매하기";
            sprintf(text, "토파즈를 성공적으로 구매하였습니다."); break;
        case BUY_STARCANDY_TRY:
            title = "별사탕 구매하기";
            sprintf(text, "토파즈를 사용하여 별사탕 %d개를 구매하시겠습니까?", priceStarCandy[d[0]]->GetCount()); break;
        case BUY_STARCANDY_OK:
            title = "별사탕 구매하기";
            sprintf(text, "별사탕을 성공적으로 구매하였습니다."); break;
        case POPUP_EXIT:
            title = "종료하기";
            sprintf(text, "코코가 그리는 마법을 종료하시겠습니까?"); break;
        case BUYPOTION_1:
            title = "포션 구매하기";
            sprintf(text, "토파즈 5개를 사용하여 포션 5개를 구매하시겠습니까?"); break;
        case BUYPOTION_OK:
            title = "포션 구매하기";
            sprintf(text, "포션 5개를 구매하였습니다."); break;
        case POTION_SEND_TRY:
            title = "포션 보내기";
            sprintf(text, "%s님에게 포션을 1개 보내시겠습니까?\n(메시지 수신을 차단한 친구에게는 카카오톡 메시지가 전달되지 않아요)", friendList[d[0]]->GetNickname().c_str()); break;
        case POTION_SEND_OK:
            title = "포션 보내기";
            if (friendList[d[0]]->IsMessageBlocked())
                sprintf(text, "성공적으로 포션을 보냈습니다.\n(카카오톡 메시지는 보내지 않았습니다)");
            else
                sprintf(text, "성공적으로 포션을 보냈습니다.");
            break;
        case POTION_SEND_REJECT:
            title = "포션 보내기";
            sprintf(text, "수신 거부 상태인 친구입니다."); break;
        case POTION_SEND_NO_FRIEND:
            title = "포션 보내기";
            sprintf(text, "친구가 아닙니다."); break;
        case POTION_SEND_EARLY:
            title = "포션 보내기";
            sprintf(text, "포션을 보낸지 1시간이 경과하지 않았습니다."); break;
        case MESSAGE_OK_STARCANDY:
            title = "메시지함";
            sprintf(text, "별사탕 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_OK_TOPAZ:
            title = "메시지함";
            sprintf(text, "토파즈 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_OK_POTION:
            title = "메시지함";
            sprintf(text, "포션 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_OK_POTION_REQUEST:
            title = "메시지함";
            sprintf(text, "요청한 포션을 보내고, 포션을 1개 받았습니다."); break;
        case MESSAGE_EMPTY:
            title = "메시지함";
            sprintf(text, "삭제된 메시지입니다."); break;
        case MESSAGE_ALL_TRY:
            title = "메시지함";
            sprintf(text, "포션을 모두 받으시겠습니까?"); break;
        case MESSAGE_ALL_OK:
            title = "메시지함";
            sprintf(text, "모든 포션을 받았습니다."); break;
        case SEND_TOPAZ_TRY:
            title = "토파즈 선물하기";
            sprintf(text, "%s님에게 토파즈 %d개를 선물하시겠습니까?", friendList[d[0]]->GetNickname().c_str(), priceTopaz[d[1]]->GetCount()); break;
        case SEND_TOPAZ_OK:
            title = "토파즈 선물하기";
            sprintf(text, "토파즈를 선물하였습니다."); break;
        case SEND_TOPAZ_OK_NOKAKAOMSG:
            title = "토파즈 선물하기";
            sprintf(text, "토파즈를 선물하였습니다.\n(카카오톡 메시지는 보내지 않았습니다)"); break;
        case SEND_TOPAZ_FAIL:
            title = "토파즈 선물하기";
            sprintf(text, "선물을 보내지 못하였습니다. 다시 시도해 주세요."); break;
            
        case UPGRADE_STAFF_BY_TOPAZ_TRY:
            fontSize = 36;
            title = "지팡이 강화하기";
            sprintf(text, "지팡이를 강화하시겠습니까?\nLv %d > Lv %d (성공확률 100%%)\n1. MP 100 증가\n2.총 MP의 1%%만큼 추가 증가", myInfo->GetStaffLv(), myInfo->GetStaffLv()+1);
            break;
        case UPGRADE_STAFF_BY_STARCANDY_TRY:
            fontSize = 36;
            title = "지팡이 강화하기";
            sprintf(text, "지팡이를 강화하시겠습니까?\nLv %d > Lv %d (성공확률 1%%)\n1. MP 100 증가\n2.총 MP의 1%%만큼 추가 증가", myInfo->GetStaffLv(), myInfo->GetStaffLv()+1);
            break;
        case UPGRADE_STAFF_OK:
            sound->playLvUpSuccess();
            title = "지팡이 강화하기";
            sprintf(text, "지팡이의 MP가 증가했어요!"); break;
        case UPGRADE_STAFF_FAIL:
            title = "지팡이 강화하기";
            sound->playLvUpFail();
            sprintf(text, "실패는 성공의 어머니입니다.\n강화포인트가 1 증가했어요!"); break;
        case UPGRADE_STAFF_INSUFFICIENT_MP:
            title = "지팡이 강화하기";
            sprintf(text, "MP가 200이상이 되어야 강화를 할 수 있어요."); break;
        case UPGRADE_STAFF_FULL_LEVEL:
            title = "지팡이 강화하기";
            sprintf(text, "이미 최고 레벨입니다."); break;
        
        case UPGRADE_FAIRY_BY_TOPAZ_TRY:
            title = "요정 강화하기";
            int fid, flv;
            fid = myInfo->GetActiveFairyId();
            flv = myInfo->GetActiveFairyLevel();
            sprintf(text, "요정의 능력을\n강화하시겠습니까?\nMP +%d > +%d\n(강화확률 100%%)", FairyBuildUpInfo::GetTotalMP(fid, flv), FairyBuildUpInfo::GetTotalMP(fid, flv+1)); break;
        case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            title = "요정 강화하기";
            fid = myInfo->GetActiveFairyId();
            flv = myInfo->GetActiveFairyLevel();
            sprintf(text, "요정의 능력을\n강화하시겠습니까?\nMP +%d > +%d", FairyBuildUpInfo::GetTotalMP(fid, flv), FairyBuildUpInfo::GetTotalMP(fid, flv+1)); break;
        case UPGRADE_FAIRY_OK:
            title = "요정 강화하기";
            sound->playLvUpSuccess();
            sprintf(text, "요정이 힘을 내어\nMP가 증가했어요!"); break;
        case UPGRADE_FAIRY_FAIL:
            title = "요정 강화하기";
            sound->playLvUpFail();
            sprintf(text, "실패는 성공의 어머니입니다.\n강화포인트가 5 증가했어요!"); break;
        case UPGRADE_FAIRY_FULL_LEVEL:
            title = "요정 강화하기";
            sprintf(text, "이미 최고 레벨입니다."); break;
            
        case BUY_FAIRY_BY_STARCANDY_TRY:
            title = "요정 구매하기";
            sprintf(text, "'%s' 요정을 별사탕 %d개로 구매하시겠습니까?", fairyInfo[d[0]]->GetName().c_str(), fairyInfo[d[0]]->GetCostStarCandy()); break;
        case BUY_FAIRY_BY_TOPAZ_TRY:
            title = "요정 구매하기";
            sprintf(text, "'%s' 요정을 토파즈 %d개로 구매하시겠습니까?", fairyInfo[d[0]]->GetName().c_str(), fairyInfo[d[0]]->GetCostTopaz()); break;
        case BUY_FAIRY_OK:
            title = "요정 구매하기";
            sprintf(text, "요정을 성공적으로 구매하였습니다!"); break;
        case BUY_FAIRY_FAIL:
            title = "요정 구매하기";
            sprintf(text, "요정을 구매하지 못하였습니다. 다시 시도해 주세요."); break;
            
        case UPGRADE_SKILL_OK:
            title = "마법 강화하기";
            sprintf(text, "축하해요! '%s'의 레벨이 %d가 되었어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str(), d[1]); break;
        case UPGRADE_SKILL_FAIL:
            title = "마법 강화하기";
            sprintf(text, "마법을 강화하는데 문제가 있어요. 다시 시도해 주세요."); break;
        case PURCHASE_SKILL_OK:
            title = "마법 배우기";
            sprintf(text, "축하해요! '%s' 마법을 새로 배웠어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str()); break;
        case PURCHASE_SKILL_FAIL:
            title = "마법 배우기";
            sprintf(text, "마법을 배우는데 문제가 있어요. 다시 시도해 주세요."); break;
        case BUY_SKILLSLOT_BY_STARCANDY_TRY:
        case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            title = "슬롯 구매하기";
            sprintf(text, "%d번째 슬롯을 구매하시겠습니까?", d[0]); break;
        case BUY_SKILLSLOT_OK:
            title = "슬롯 구매하기";
            sprintf(text, "슬롯을 성공적으로 구매하였습니다."); break;
        case BUY_SKILLSLOT_FAIL:
            title = "슬롯 구매하기";
            sprintf(text, "슬롯 구매를 실패하였습니다. 다시 시도해 주세요."); break;
        case BUY_SKILLSLOT_FULL:
            title = "슬롯 구매하기";
            sprintf(text, "이미 모든 슬롯을 구매하였습니다."); break;
        case BUY_PROPERTY_TRY:
            title = "마법 속성 열기";
            switch (d[0])
            {
                case 1: sprintf(text, "'불'속성 마법을 배우게 됩니다. '불'속성을 습득하시겠습니까?\n(속성을 토파즈로 구매하면\n요구 MP가 %d%% 감소합니다)", d[2]); break;
                case 2: sprintf(text, "'물'속성 마법을 배우게 됩니다. '물'속성을 습득하시겠습니까?\n(속성을 토파즈로 구매하면\n요구 MP가 %d%% 감소합니다)", d[2]); break;
                case 3: sprintf(text, "'땅'속성 마법을 배우게 됩니다. '땅'속성을 습득하시겠습니까?\n(속성을 토파즈로 구매하면\n요구 MP가 %d%% 감소합니다)", d[2]); break;
                //case 4: sprintf(text, "'궁극'속성 마법을 배우게 됩니다. '궁극'속성을 습득하시겠습니까?"); break;
            }
            break;
        case BUY_PROPERTY_FAIL:
            title = "마법 속성 열기";
            sprintf(text, "속성 습득을 실패하였습니다. 다시 시도해 주세요."); break;
        case BUY_PROPERTY_OK:
            title = "마법 속성 열기";
            sprintf(text, "새로운 속성을 습득하였습니다. 더욱 풍부해진 마법들을 새로 익혀 보세요!"); break;
        
        case INVITE_FRIEND_TRY:
            title = "친구 초대하기";
            sprintf(text, "%s님을 초대하시겠습니까?\n(카카오톡 메시지를 보냅니다)", inviteList[d[0]]->nickname.c_str()); break;
        case INVITE_FRIEND_OK:
            title = "친구 초대하기";
            sprintf(text, "친구를 초대하였습니다. 별사탕 1,000개와 포션 1개를 지급해 드렸습니다."); break;
        case INVITE_FRIEND_NO_MORE:
            title = "친구 초대하기";
            sprintf(text, "더 이상 초대할 수 없습니다.\n(최대 30명)"); break;
        case INVITE_FRIEND_MAX_PER_DAY:
            title = "친구 초대하기";
            sprintf(text, "하루에 보낼 수 있는 초대장을 모두 사용하였습니다."); break;
        case INVITE_FRIEND_BLOCKED:
            title = "친구 초대하기";
            sprintf(text, "메시지 수신 거부중인 친구입니다."); break;
        case INVITE_FRIEND_NOT_SUPPORTED_DEVICE:
            title = "친구 초대하기";
            sprintf(text, "지원되지 않는 디바이스를 사용중인 친구입니다."); break;
        case INVITE_FRIEND_UNREGISTERED: // 이 메시지가 들어올 수 있나?
            title = "친구 초대하기";
            sprintf(text, "탈퇴한 친구입니다."); break;
        case INVITE_FRIEND_MONTH_OVER_30:
            title = "친구 초대하기";
            sprintf(text, "한달 최대 30명까지만 초대 가능합니다. 다음 달에 새로운 친구를 불러 보아요!"); break;
        case INVITE_FRIEND_DAY_OVER_20:
            title = "친구 초대하기";
            sprintf(text, "하루 최대 20명까지만 초대 가능합니다. 내일 새로운 친구를 불러 보아요!"); break;
        case INVITE_FRIEND_ALREADY_DID:
            title = "친구 초대하기";
            sprintf(text, "이미 초대장을 보낸 친구입니다."); break;
        case INVITE_FRIEND_10:
            title = "친구 초대하기";
            sprintf(text, "친구를 10명 초대하여 별사탕 15,000개가 지급되었습니다."); break;
        case INVITE_FRIEND_20:
            title = "친구 초대하기";
            sprintf(text, "친구를 20명 초대하여 토파즈 10개가 지급되었습니다."); break;
        case INVITE_FRIEND_30:
            title = "친구 초대하기";
            sprintf(text, "친구를 30명 초대하여 토파즈 25개가 지급되었습니다."); break;
            
        case REQUEST_POTION_OK:
            title = "포션 조르기";
            sprintf(text, "포션을 성공적으로 요청하였습니다."); break;
        case REQUEST_POTION_REJECT:
            title = "포션 조르기";
            sprintf(text, "수신거부 상태인 친구입니다."); break;
        case REQUEST_POTION_EARLY:
        case REQUEST_TOPAZ_EARLY:
            title = "포션 조르기";
            sprintf(text, "요청한 친구에게는 24시간 후 다시 요청 가능합니다."); break;
            
        case REQUEST_TOPAZ_OK:
            title = "토파즈 조르기";
            sprintf(text, "토파즈를 성공적으로 요청하였습니다."); break;
        case REQUEST_TOPAZ_NO_FRIEND:
            title = "토파즈 조르기";
            sprintf(text, "친구가 아닙니다."); break;
            
        case TODAYCANDY_RESULT_WIN:
            sound->playLvUpSuccess();
            char present[15];
            switch (myInfo->GetTodayCandyTypeChoice())
            {
                case 1: sprintf(present, "별사탕"); break;
                case 2: sprintf(present, "토파즈"); break;
                case 3: sprintf(present, "포션"); break;
            }
            title = present;
            title += " 당첨!";
            fontSize = 32;
            sprintf(text, "축하합니다!\n%s %d개에 당첨되셨어요.\n(메시지함으로 전송되었습니다)", present, myInfo->GetTodayCandyValueChoice());
            break;
        case TODAYCANDY_RESULT_LOSE:
            sound->playLvUpSuccess();
            switch (myInfo->GetTodayCandyTypeChoice())
            {
                case 1: sprintf(present, "별사탕"); break;
                case 2: sprintf(present, "토파즈"); break;
                case 3: sprintf(present, "포션"); break;
            }
            title = present;
            title += " 당첨!";
            fontSize = 32;
            //if (friendList[d[0]]->GetPotionMsgStatus() > 0 || !friendList[d[0]]->IsMessageBlocked())
                sprintf(text, "%s님께서 %s %d개에 당첨되어 보상을 받았어요.\n카카오톡 메시지를 보냅니다.\n(수신을 차단한 친구에게는 카카오톡 메시지가 발송되지 않습니다.", friendList[d[0]]->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice());
            //else
            //    sprintf(text, "%s님께서 %s %d개에 당첨되어 보상을 받았어요.", friendList[d[0]]->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice());
            //sprintf(text, "축하합니다!\n%s님께서 %s %d개에 당첨되셨어요.\n%s님께 %s %d개를 보냅니다.\n(카카오톡 메시지가 전송됩니다)", Friend::GetObj(d[0])->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice(), Friend::GetObj(d[0])->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice());
            break;
        case TODAYCANDY_ALREADY_DONE:
            title = "오늘의 별사탕";
            sprintf(text, "오늘은 이미 완료한 상태입니다."); break;
        case TODAYCANDY_NOTENOUGH_FRIENDS:
            title = "오늘의 별사탕";
            sprintf(text, "친구가 4명 필요해요.\n친구를 초대해 보아요."); break;
        case PRACTICE_SKILL_FULL_EXP:
            title = "스킬 연습 완료";
            sprintf(text, "'%s'\n스킬의 연습이 완료되었습니다.\n스케치북에서 LEVEL UP 하세요!", SkillInfo::GetSkillInfo(myInfo->GetPracticeSkillId())->GetName().c_str());
            break;
        
        case SELECT_PROPERTY_TRY:
            title = "속성 선택하기";
            if (d[0] == 1) sprintf(text, "불 속성을 선택하시겠습니까?\n(한 번 선택하면 바꿀 수 없습니다.");
            else if (d[0] == 2) sprintf(text, "물 속성을 선택하시겠습니까?\n(한 번 선택하면 바꿀 수 없습니다.");
            else if (d[0] == 3) sprintf(text, "땅 속성을 선택하시겠습니까?\n(한 번 선택하면 바꿀 수 없습니다.");
            break;
        case SELECT_PROPERTY_OK:
            title = "속성 선택하기";
            if (d[0] == 1) sprintf(text, "불 속성을 새로 배웠습니다.");
            else if (d[0] == 2) sprintf(text, "물 속성을 새로 배웠습니다.");
            else if (d[0] == 3) sprintf(text, "땅 속성을 새로 배웠습니다.");
            break;
        case GET_DEGREE:
            title = "학위 취득";
            sprintf(text, "축하합니다. 학위를 받았어요!\n학위수여식으로 이동합니다.");
            break;
        case COUPON_OK:
            title = "쿠폰 등록하기";
            // type { 1 : 별사탕, 2 : 토파즈 , 3 : 포션 }
            if (d[0] == 1) sprintf(text, "쿠폰이 성공적으로 등록되었습니다.\n별사탕 %d개가 지급되었어요!", d[1]);
            else if (d[0] == 2) sprintf(text, "쿠폰이 성공적으로 등록되었습니다.\n토파즈 %d개가 지급되었어요!", d[1]);
            else if (d[0] == 3) sprintf(text, "쿠폰이 성공적으로 등록되었습니다.\n포션 %d개가 지급되었어요!", d[1]);
            break;
        case COUPON_ALREADY_USED:
            title = "쿠폰 등록하기";
            sprintf(text, "이미 사용된 쿠폰입니다.\n다른 쿠폰을 이용해 주세요.");
            break;
        case COUPON_EXPIRED:
            title = "쿠폰 등록하기";
            sprintf(text, "유효기간이 지난 쿠폰입니다.\n다른 쿠폰을 이용해 주세요.");
            break;
        case COUPON_NOT_EXIST:
            title = "쿠폰 등록하기";
            sprintf(text, "존재하지 않는 쿠폰입니다.\n다른 쿠폰을 이용해 주세요.");
            break;
        case COUPON_SAME_TYPE:
            title = "쿠폰 등록하기";
            sprintf(text, "같은 이벤트의 쿠폰이 이미 등록되었습니다.");
            break;
        case BUY_PROPERTY_FREE_MSG:
            title = "속성 선택하기";
            sprintf(text, "축하합니다! 이제 다른 속성을 배울 수 있어요.");
            break;
        case KAKAO_LOGOUT:
            title = "카카오 계정 로그아웃";
            sprintf(text, "카카오 계정을 로그아웃하시겠습니까?\n(로그아웃하면 게임을 종료합니다)");
            break;
        case KAKAO_UNREGISTER:
            title = "카카오 게임 탈퇴";
            sprintf(text, "카카오 게임을 탈퇴하시겠습니까?\n모든 게임 데이터가 삭제됩니다.\n(회원탈퇴하면 게임을 종료합니다)");
            break;
        case RANKUP_BOAST:
            title = "자랑하기";
            sprintf(text, "%s님에게 자랑해요!\n(카카오톡 메시지를 보내요)", friendList[d[0]]->GetNickname().c_str()); break;
        case RANKUP_BOAST_REJECTED:
            title = "자랑하기";
            sprintf(text, "메시지 수신을 거부한 친구입니다."); break;
        case SERVICE:
            title = "고객센터 이메일";
            sprintf(text, "help@playdandi.com\n으로 이메일을 보내주세요.");
            break;
        case MP_REWARD_50:
            title = "MP 달성 보상";
            sprintf(text, "축하해요! %d MP 달성!\n(포션이 모두 채워졌어요)", myInfo->GetMPTotal() - (myInfo->GetMPTotal()%100 - 50*(myInfo->GetMPTotal()%100 >= 50)));
            break;
        case MP_REWARD_100:
            title = "MP 달성 보상";
            sprintf(text, "축하해요! %d MP 달성!\n(포션이 모두 채워지고, %d 토파즈가 지급되었어요)", myInfo->GetMPTotal()-myInfo->GetMPTotal()%100, myInfo->GetRewardTopaz());
            break;
        case POTION_REWARD:
            title = "초보 유저 보상";
            sprintf(text, "초보 유저를 위한 선물!\n포션을 가득 채웠어요!");
            break;
        case POSSIBLE_BUY_FAIRY:
            title = "요정 구입 가능";
            sprintf(text, "'%s' 요정을 이제 별사탕으로 구입할 수 있어요!", fairyInfo[d[0]]->GetName().c_str());
            break;
        case KAKAOTALK_UNKNOWN:
            title = "카카오톡 탈퇴 상태";
            sprintf(text, "카카오계정과 카카오톡 계정의 연결이 끊긴 상태입니다. 카카오톡에서 계정 연동을 하세요.");
            break;
        case KAKAO_MSG_BLOCKED:
            title = "카카오톡 수신거부";
            sprintf(text, "카카오톡에서 먼저 수신설정을 허용해야 합니다.");
            break;
        case CREDIT:
            title = "만든 사람들";
            sprintf(text, "문재웅 박일진 정연준\n(주)플레이단디");
            break;
        case TUTORIAL_START:
            title = "튜토리얼 시작";
            sprintf(text, "튜토리얼을 시작합니다.");
            break;
        case PURCHASE_SKILL_BY_TOPAZ_TRY:
            title = "즉시 마법 구매하기";
            sprintf(text, "%d번째 스킬인\n\"%s\"을(를)\n구매하시겠습니까?", d[2], SkillInfo::GetSkillInfo(d[0])->GetName().c_str());
            break;
        case POPUP_NOTICE:
            title = noticeList[d[0]]->title;
            sprintf(text, "%s", noticeList[d[0]]->message.c_str());
            
            if (noticeList[d[0]]->oneTime != 1) // 1회성 공지의 경우 '24시간 보지않기' 문구를 표시하지 않는다.
            {
                spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("24시간 동안 보지 않습니다.", fontList[0], 32, ccp(0, 0), ccp(76+40+80+20, 711+20), ccc3(78,47,8), "", "Layer", tLayer, 10005) );
            
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_clause_agree2.png", ccp(0, 0), ccp(76+40, 711), CCSize(0,0), "", "Layer", tLayer, 10005) );
                float w = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_clause_agree2.png"))->getContentSize().width;
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_clause_agree2.png"))->setScale(80 / w);
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_clause_agree2.png"))->setContentSize(CCSize(80, 80));
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_clause_agree2.png"))->setColor(ccc3(120,120,120));
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png", ccp(0.5, 0.5), ccp(76+40+40, 711+40), CCSize(0,0), "", "Layer", tLayer, 10005, 0, 0) );
            }
            break;
        case HINT_BUY_PROPERTY:
            title = "속성 배우기";
            if (d[0] == 1) sprintf(text, "물 속성 마법을 다 배웠습니다.\n모든 물 마법을 5레벨 이상 달성하면\n다른 속성을 배울 수 있습니다.");
            else if (d[0] == 2) sprintf(text, "불 속성 마법을 다 배웠습니다.\n모든 불 마법을 5레벨 이상 달성하면\n다른 속성을 배울 수 있습니다.");
            else if (d[0] == 3) sprintf(text, "땅 속성 마법을 다 배웠습니다.\n모든 땅 마법을 5레벨 이상 달성하면\n다른 속성을 배울 수 있습니다.");
            break;
    }
    
    // 내용 문장
    if (!hasImage)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 40, ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), ccc3(78,47,8), CCSize(829-20, 250), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 10005) );
    else
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], fontSize, ccp(0, 0.5), ccp(126+254+50, winSize.height/2+5), ccc3(78,47,8), CCSize(525, 260), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 10005) );
    
    // 팝업창 타이틀 (종료할 때만 띄우지 않는다)
    if (type != POPUP_EXIT)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(126+5, 678+562-offset - 35), CCSize(759-126+52, 90), "", "Layer", tLayer, 10001) );
        CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 10002, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 10002, 1) );
    }
    
    
    // 가격표
    if (type == BUY_TOPAZ_TRY) // 유일하게 '별사탕'/'토파즈' 아이콘이 없는 경우임
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0, 0), ccp(493, 723+offset), CCSize(201, 77), "", "Layer", tLayer, 10005) );
        CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png2");
        
        std::string val = "";
        if (myInfo->GetDeviceType() == 1) // ANDROID
            val = "₩ " + Common::MakeComma(d[1]);
        else if (myInfo->GetDeviceType() == 2) // iPHONE
            val = "$ " + Common::MakeComma(d[1]/100) + "." + Common::MakeComma(d[1]%100);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 40, ccp(0.5, 0.5), ccp(pos.x+2, pos.y-2), ccc3(0,0,0), "background/bg_degree_desc.png2", "1", NULL, 10005, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 40, ccp(0.5, 0.5), pos, ccc3(255,255,255), "background/bg_degree_desc.png2", "1", NULL, 10005, 1) );
    }
    else if (type == BUY_STARCANDY_TRY || type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_TOPAZ_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_PROPERTY_TRY || type == BUYPOTION_1 || type == UPGRADE_FAIRY_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_TOPAZ_TRY || type == PURCHASE_SKILL_BY_TOPAZ_TRY)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(1, 0), ccp(493+201, 723+offset), CCSize(201+30, 77), "", "Layer", tLayer, 10005) );
        
        if (type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_STARCANDY_TRY)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(513-25, 730+offset), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(513-25, 730+offset), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        
        char cost[10];
        sprintf(cost, "%s", Common::MakeComma(d[1]).c_str());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 40, ccp(0, 0), ccp(580+2-25, 743+offset-2), ccc3(0,0,0), "", "Layer", tLayer, 10005) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 40, ccp(0, 0), ccp(580-25, 743+offset), ccc3(255,255,255), "", "Layer", tLayer, 10005) );
    }
    
    
    // 취소 버튼이 필요할 경우 넣는다.
    if (btn == BTN_2)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(0, 0), ccp(126, 711+offset), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 10005, 1) );
    }
    
    
    if (type == TODAYCANDY_NOTENOUGH_FRIENDS)
    {
        // 오.별 사람수 모자란 팝업창에서만 버튼이 다르다.
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(717+5-200, 711-7+offset), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        CCPoint pos = spriteClass->FindParentCenterPos("button/btn_blue.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_friendinvite.png", ccp(0.5,0.5), ccp(pos.x, pos.y+7), CCSize(0, 0), "button/btn_blue.png", "0", NULL, 10005, 1) );
    }
    else
    {
        // 확인 버튼
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717+5, 711+offset), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        CCPoint p = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(717+5+p.x, 711+offset+24), CCSize(0, 0), "", "Layer", tLayer, 10005) );
        
        // 버튼 젤리 움직임
        CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"));
        CCSize t = temp->getContentSize();
        temp->setAnchorPoint(ccp(0.5, 0.5));
        temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
        CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
        temp->runAction(CCRepeatForever::create(action));
    }
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool NoImage::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)// || isTouchDone)
        return false;
    isTouched = true;
    //isTouchDone = true;
    
    CCPoint point = pTouch->getLocation();
    
    // 강화 힌트 있으면 안 보이게 한다.
    if (balloon != NULL && ball != NULL)
    {
        balloon->setOpacity(0);
        ball->setOpacity(0);
    }
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                HandlingTouch(TOUCH_CANCEL);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_blue.png")
        {
            // 오.별에서 친구 부족하다는 팝업창에서 '친구 초대하기' 버튼 눌렀을 때
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                if (myInfo->GetHashedTalkUserId() == "") // 카카오톡 탈퇴한 경우 친구초대 못함.
                    ReplaceScene("NoImage", KAKAOTALK_UNKNOWN, BTN_1);
                else
                {
                    EndScene();
                    Common::ShowNextScene(Depth::GetCurPointer(), "CocoRoom", "InviteFriend", false);
                }
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(170,170,170));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_clause_agree2.png") // 공지사항 24시간 안보기 체크 유/무
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                int alpha = ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_check.png"))->getOpacity();
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_check.png"))->setOpacity(255-alpha);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_question_mini.png") // 강화포인트 설명 물음표
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                ShowHintOfUpgrade();
                return true;
            }
        }
    }
    
    isTouchDone = false;
    return true;
}

void NoImage::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void NoImage::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    if (!isTouched)
        return;

    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            HandlingTouch(TOUCH_CONFIRM);
        }
    }
    
    isTouched = false;
}

void NoImage::HandlingTouch(int touchType)
{
    char temp[255];
    if (btn == BTN_1 || touchType == TOUCH_CANCEL) // 팝업창에서 '확인' 버튼 하나만 있는 경우 , 혹은 'x' 누른 경우
    {
        if (type == YOU_WERE_BLOCKED || type == FUCKING_APP_DETECTED || type == ROOTING_DETECTED || type == SERVER_CHECK)
        {
            Exit();
        }
        else if (type == POPUP_NOTICE && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            if (noticeList[d[0]]->link != "")
            {
                // 공지사항 팝업창 링크 연결
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                JniMethodInfo t;
                if (JniHelper::getStaticMethodInfo(t,
                                                   "com/playDANDi/CocoMagic/CocoMagic",
                                                   "OpenNoticeURL",
                                                   "(Ljava/lang/String;)V"))
                {
                    // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
                    t.env->CallStaticVoidMethod(t.classID, t.methodID, t.env->NewStringUTF(noticeList[d[0]]->link.c_str()));
                    // Release
                    t.env->DeleteLocalRef(t.classID);
                }
                #endif
                
                #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                EndScene();
                #endif
            }
            else
                EndScene();
        }
        else if (type == NEED_TO_UPDATE)
        {
            EndScene();
            
            // 앱 업데이트 (마켓으로 이동)
            #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            JniMethodInfo t;
            if (JniHelper::getStaticMethodInfo(t,
                                               "com/playDANDi/CocoMagic/CocoMagic",
                                               "GoToPlayStore",
                                               "()V"))
            {
                // 함수 호출할 때 Object값을 리턴하는 함수로 받아야함!!!!
                t.env->CallStaticVoidMethod(t.classID, t.methodID);
                // Release
                t.env->DeleteLocalRef(t.classID);
            }
            #endif
            CCDirector::sharedDirector()->end();
        }
        else if (type == NEED_TO_REBOOT || type == ERROR_IN_APP_BILLING)
        {
            Common::RebootSystem(this); // 재부팅.
        }
        else if (type == INVITE_FRIEND_OK && d[0] > 0) // 친구초대 성공 후, 보상(10/20/30)달성했을 경우
        {
            if (d[0] == 1) ReplaceScene("NoImage", INVITE_FRIEND_10, BTN_1);
            else if (d[0] == 2) ReplaceScene("NoImage", INVITE_FRIEND_20, BTN_1);
            else if (d[0] == 3) ReplaceScene("NoImage", INVITE_FRIEND_30, BTN_1);
        }
        else if (type == BUY_FAIRY_OK)
        {
            EndScene();
            // 요정 구매 성공 후에는 'FairyOneInfo' 창도 같이 끄자.
            CCString* param = CCString::create("2");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("FairyOneInfo", param);
        }
        else if (type == PRACTICE_SKILL_FULL_EXP && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            // 스케치북으로 이동
            EndScene();
            Common::ShowNextScene(Depth::GetCurPointer(), "GameReady", "Sketchbook", false, 0);
        }
        else if (type == POSSIBLE_BUY_FAIRY && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            // 요정의 방으로 이동
            EndScene();
            Common::ShowNextScene(Depth::GetCurPointer(), "GameReady", "CocoRoom", false, 2);
        }
        else if (type == SELECT_PROPERTY_OK)
        {
            EndScene();
            CCString* param;
            if (d[1] == 1) // 인게임 튜토리얼 시작
                param = CCString::create("3");
            else // 스케치북 튜토리얼 시작
                param = CCString::create("2");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("SelectProperty", param);
        }
        else if (type == GET_DEGREE)
        {
            // 학위수여 화면으로 이동
            EndScene();
            Common::ShowNextScene(Depth::GetCurPointer(), Depth::GetCurName(), "GetDegree", false);
        }
        else if (type == COUPON_OK)
        {
            EndScene();
            // 쿠폰 입력창도 종료시킨다.
            CCString* param = CCString::create("2");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Coupon", param);
        }
        else if (type == UPGRADE_SKILL_OK)
        {
            EndScene();
            // 스케치북에서, 한 속성의 7개 스킬이 모두 5레벨 이상인지 확인한다.
            CCString* param = CCString::create("4");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        }
        else if (type == BUY_PROPERTY_FREE_MSG && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            EndScene();
            // 속성선택창을 연다.
            CCString* param = CCString::create("5");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        }
        else if (type == TODAYCANDY_RESULT_LOSE && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            if (!friendList[d[0]]->IsMessageBlocked() && friendList[d[0]]->GetPotionMsgStatus() > 0)
            {
                // 카카오 api 호출 (초대메시지 템플릿 이용)
                sendLinkType = type;
                std::string templateId = KAKAO_MSG_TEMPLATE_TODAYCANDY;
                std::string executeUrl = "";
                char temp[128];
                sprintf(temp, "{\"sender_name\":\"%s\"}", MyInfo::GetName().c_str());
                std::string metaInfo = temp;
                ////CCLog("metaInfo = %s", metaInfo.c_str());
                KakaoNativeExtension::getInstance()->sendLinkMessage(std::bind(&NoImage::onSendLinkMessageComplete, this), std::bind(&NoImage::onSendLinkMessageErrorComplete, this, std::placeholders::_1, std::placeholders::_2), templateId, friendList[d[0]]->GetKakaoId(), "", executeUrl, metaInfo);
            }
            EndScene();
        }
        else if (type == RANKUP_BOAST && touchType == TOUCH_CONFIRM) // only '확인'버튼
        {
            // 친구의 카톡메시지 수신여부 받아오기 (현재는 '자랑하기' 할 때만 쓰인다)
            std::string url = URL_GET_POTION_STATUS;
            std::string param = "";
            sprintf(temp, "friend_kakao_id=%s", friendList[d[0]]->GetKakaoId().c_str());
            param += temp;
            
            HttpRequest(url, param);
        }
        else if (type == RANKUP_BOAST_REJECTED)
        {
            // 자랑하기 다시 못하게 하기
            CCString* param = CCString::create("5");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("RankUp", param);
            
            EndScene();
        }
        else if (type == MP_REWARD_50 || type == MP_REWARD_100)
        {
            myInfo->SetReward(0, 0); // 보상 초기화
            EndScene();
        }
        else if (type == PURCHASE_SKILL_OK)
        {
            // 한 속성의 스킬을 다 배운 순간 (속성의 마지막 스킬 구매 후) -> 안내팝업창 띄우기
            EndScene();
            CCString* param = CCString::create("6");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        }
        else // 그 외, 그냥 끈다.
        {
            EndScene();
        }
    }
    
    // 아래부터는 무조건 '확인'/'취소' 버튼 2개 있는 팝업창에서 '확인'을 누른 경우다.
    
    else if (type == POPUP_EXIT)
    {
        Exit();
    }
    else if (type == NEED_TO_BUY_POTION)
    {
        CCNode* parent = this->getParent();
        EndScene();
        Common::ShowNextScene(parent, Depth::GetCurName(), "BuyPotion", false, 3); // curName == 결국 부모
    }
    else if (type == NEED_TO_BUY_TOPAZ)
    {
        CCNode* parent = this->getParent();
        EndScene();
        Common::ShowNextScene(parent, Depth::GetCurName(), "BuyTopaz", false, 3); // curName == 결국 부모
    }
    else if (type == NEED_TO_BUY_STARCANDY)
    {
        CCNode* parent = this->getParent();
        EndScene();
        Common::ShowNextScene(parent, Depth::GetCurName(), "BuyStarCandy", false, 3); // curName == 결국 부모
    }
    else if (type == KAKAO_LOGOUT)
    {
        // 카카오 로그아웃 후 게임종료
        KakaoNativeExtension::getInstance()->logout(std::bind(&NoImage::onLogoutComplete, this), std::bind(&NoImage::onLogoutErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    }
    else if (type == KAKAO_UNREGISTER)
    {
        std::string url = URL_QUIT;
        std::string param = "";
        sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
        param += temp;
        
        HttpRequest(url, param);
    }
    else if (type == BUY_TOPAZ_TRY)
    {
        // 토파즈 구입하기. (미결제 버전) -> (현재 iOS에만 fake로 이용됨)
        std::string url = URL_BUY_TOPAZ_TEST;
        std::string param = "";
        sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
        param += temp;
        sprintf(temp, "topaz_id=%d", priceTopaz[d[0]]->GetId());
        param += temp;
        
        HttpRequest(url, param);
    }
    else if (type == BUY_STARCANDY_TRY)
    {
        if (myInfo->GetTopaz() < priceStarCandy[d[0]]->GetPrice()) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
        {
            std::string url = URL_PURCHASE_STARCANDY;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "starcandy_id=%d", priceStarCandy[d[0]]->GetId());
            param += temp;
            
            HttpRequest(url, param);
        }
    }
    else if (type == BUYPOTION_1)
    {
        if (myInfo->GetTopaz() < 5) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
        {
            // 포션 구매 프로토콜을 요청한다.
            std::string url = URL_PURCHASE_POTION;
            sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
            std::string param = "";
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == POTION_SEND_TRY)
    {
        // 포션 보내기 (랭킹 화면에서)
        std::string url = URL_SEND_POTION;
        std::string param = "";
        sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
        param += temp;
        sprintf(temp, "friend_kakao_id=%s", friendList[d[0]]->GetKakaoId().c_str());
        param += temp;
        HttpRequest(url, param);
    }
    else if (type == MESSAGE_ALL_TRY)
    {
        // 포션 모두 받기 프로토콜 요청.
        std::string url = URL_MESSAGE_ALL;
        std::string param = "";
        sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
        param += temp;
        HttpRequest(url, param);
    }
    else if (type == SEND_TOPAZ_TRY)
    {
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        EndScene();
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
        #endif
        
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        // 토파즈 선물하기. (미결제 버전) -> 현재 iOS에만 fake 이용됨.
        std::string url = URL_SEND_TOPAZ_TEST;
        std::string param = "";
        sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
        param += temp;
        sprintf(temp, "friend_kakao_id=%s&", friendList[d[0]]->GetKakaoId().c_str());
        param += temp;
        sprintf(temp, "topaz_id=%d", priceTopaz[d[1]]->GetId());
        param += temp;
        HttpRequest(url, param);
        #endif
    }
    else if (type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY)
    {
        // 토파즈vs별사탕 구분 및 가격
        int costType = (type == UPGRADE_STAFF_BY_TOPAZ_TRY) ? 2 : 1;
        int cost = magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_Topaz();
        if (costType == 1) // 일반강화는 별사탕
            cost = magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_StarCandy();
        
        // 잔액 부족
        if (costType == 2 && myInfo->GetTopaz() < cost) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else if (costType == 1 && myInfo->GetStarCandy() < cost) // 별사탕 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        else
        {
            // 지팡이 강화 (by 별사탕, by 토파즈 모두 통용됨)
            std::string url = URL_UPGRADE_STAFF;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "cost_type=%d", costType);
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == UPGRADE_FAIRY_BY_TOPAZ_TRY || type == UPGRADE_FAIRY_BY_STARCANDY_TRY)
    {
        // 토파즈vs별사탕 구분 및 가격
        int costType = (type == UPGRADE_FAIRY_BY_TOPAZ_TRY) ? 2 : 1;
        int cost = d[1];
        
        // 잔액 부족
        if (costType == 2 && myInfo->GetTopaz() < cost) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else if (costType == 1 && myInfo->GetStarCandy() < cost) // 별사탕 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        else
        {
            // 요정 강화 (by 별사탕, by 토파즈 모두 통용됨)
            std::string url = URL_UPGRADE_FAIRY;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "user_fairy_id=%d&",  myInfo->GetActiveFairyUserId());
            param += temp;
            sprintf(temp, "cost_type=%d", costType);
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == BUY_FAIRY_BY_TOPAZ_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY)
    {
        FairyInfo* fi = fairyInfo[d[0]];
        
        // 토파즈vs별사탕 구분 및 가격
        int costType = (type == BUY_FAIRY_BY_TOPAZ_TRY) ? 2 : 1;
        int cost = fi->GetCostTopaz();
        if (costType == 1) // 별사탕으로 산다면,
            cost = fi->GetCostStarCandy();
        
        // 잔액 부족
        if (costType == 2 && myInfo->GetTopaz() < cost)
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else if (costType == 1 && myInfo->GetStarCandy() < cost)
            ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        else
        {
            // 요정 구입 (by 별사탕, by 토파즈 모두 통용됨)
            std::string url = URL_PURCHASE_FAIRY;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "fairy_id=%d&", fi->GetId());
            param += temp;
            sprintf(temp, "cost_type=%d", costType);
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY)
    {
        if (type == BUY_SKILLSLOT_BY_STARCANDY_TRY && myInfo->GetStarCandy() < d[1]) // 별사탕 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        else if (type == BUY_SKILLSLOT_BY_TOPAZ_TRY && myInfo->GetTopaz() < d[1]) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
        {
            // 스킬 슬롯 구매
            char temp[255];
            std::string url = URL_UPGRADE_SKILLSLOT;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "slot_id=%d", d[0]);
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == BUY_PROPERTY_TRY || type == SELECT_PROPERTY_TRY)
    {
        if (type == BUY_PROPERTY_TRY && myInfo->GetTopaz() < d[1]) // 토파즈 구매 창으로 이동
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
        {
            // 스킬 새 속성 열기
            char temp[255];
            std::string url = URL_PURCHASE_SKILL_PROPERTY;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            // 서버랑 클라이언트랑 불/물 숫자가 서로 반대여서 부득이하게 아래처럼 판별하도록 한다.
            if (d[0] == 1) newSkillType = 2;
            else if (d[0] == 2) newSkillType = 1;
            else newSkillType = d[0];
            sprintf(temp, "skill_type=%d", newSkillType);
            param += temp;
            HttpRequest(url, param);
        }
    }
    else if (type == INVITE_FRIEND_TRY)
    {
        EndScene();
        CCString* param = CCString::create("5");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    else if (type == PURCHASE_SKILL_BY_TOPAZ_TRY)
    {
        // 스킬 즉시 구매하기 (토파즈로)
        if (myInfo->GetTopaz() < d[1])
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
        {
            char temp[255];
            std::string url = URL_PURCHASE_SKILL;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "skill_id=%d&", d[0]);
            param += temp;
            sprintf(temp, "cost_type=2"); // 1 : 별사탕, 2 : 토파즈
            param += temp;
            HttpRequest(url, param);
        }
    }
}


void NoImage::ShowHintOfUpgrade()
{
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    balloon = NULL;
    ball = NULL;
    
    balloon = CCScale9Sprite::create("images/tutorial_balloon2.png");
    balloon->setContentSize(CCSize(600, 140+120));
    balloon->setAnchorPoint(ccp(1, 0));
    balloon->setPosition(ccp(896+100, 572+55+250));
    this->addChild(balloon, 10007);
    ball = CCLabelTTF::create("강화에 실패할 경우\n강화포인트를 획득해요!\n강화포인트를 모두 모으면\n강화를 100% 성공할 수 있어요.", fontList[0].c_str(), 36);
    ball->setPosition(ccp(600/2, (140+120)/2+30));
    ball->setColor(ccc3(255,255,255));
    balloon->addChild(ball, 10008);
    
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL );
    balloon->runAction( CCRepeatForever::create(action) );
}


void NoImage::ReplaceScene(std::string to, int type, int btnType)
{
    isEnded = true;
    isTouched = true;
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // 강화 힌트 있으면 지운다.
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    ball = NULL;
    balloon = NULL;
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    CCNode* parent = this->getParent();
    this->removeFromParentAndCleanup(true);
    Common::ShowPopup(parent, "NoImage", to, false, type, btnType, d, fromWhere);
}

void NoImage::EndScene()
{
    isEnded = true;
    isTouched = true;
    
    Depth::DumpDepth();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);

    // 강화 힌트 있으면 지운다.
    if (balloon != NULL && ball != NULL)
    {
        ball->removeFromParentAndCleanup(true);
        balloon->removeFromParentAndCleanup(true);
    }
    ball = NULL;
    balloon = NULL;
    
    // 공지사항 팝업창일 때 24시간 보여주기 체크
    if (type == POPUP_NOTICE)
    {
        int alpha = -1;
        if (noticeList[d[0]]->oneTime != 1)
            alpha = ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_check.png"))->getOpacity();
        if (alpha == 255 || noticeList[d[0]]->oneTime == 1) // 체크를 했거나, 1회성 공지의 경우
        {
            //CCLog("24시간동안 안 보여줌!");
            char s[20];
            sprintf(s, "noticelist_%d", noticeList[d[0]]->id);
            CCUserDefault::sharedUserDefault()->setIntegerForKey(s, time(0));
        }
    }
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
    
    
    if (Depth::GetCurNameString() == "GameReady" || Depth::GetCurNameString() == "Ranking")
    {
        // 보상 관련 (GameReady)  or  다른 팝업창 관련 (Ranking)
        param = CCString::create("11");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
}

void NoImage::EndSceneCallback()
{
}

void NoImage::Exit()
{
    CCDirector::sharedDirector()->end();
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
    #endif
}


/////////////////////////////////////////////////////////////////////////////////////

void NoImage::HttpRequest(std::string url, std::string param)
{
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);

    Network::HttpPost(param, url, this, httpresponse_selector(NoImage::onHttpRequestCompleted));
}

void NoImage::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);

    // parse xml data
    switch (type)
    {
        case BUY_TOPAZ_TRY:
            XmlParseBuyTopaz(&xmlDoc); break;
        case BUY_STARCANDY_TRY:
            XmlParseBuyStarCandy(&xmlDoc); break;
        case BUYPOTION_1:
            XmlParseBuyPotion(&xmlDoc); break;
        case POTION_SEND_TRY:
            XmlParseSendPotion(&xmlDoc); break;
        case MESSAGE_ALL_TRY:
            XmlParseMsg(&xmlDoc); break;
        case SEND_TOPAZ_TRY:
            XmlParseSendTopaz(&xmlDoc); break;
        case UPGRADE_STAFF_BY_TOPAZ_TRY:
        case UPGRADE_STAFF_BY_STARCANDY_TRY:
            XmlParseUpgradeStaff(&xmlDoc); break;
        case UPGRADE_FAIRY_BY_TOPAZ_TRY:
        case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            XmlParseUpgradeFairy(&xmlDoc); break;
        case BUY_FAIRY_BY_TOPAZ_TRY:
        case BUY_FAIRY_BY_STARCANDY_TRY:
            XmlParseBuyFairy(&xmlDoc); break;
        case USING_FAIRY:
            XmlParseUsingFairy(&xmlDoc); break;
        case BUY_SKILLSLOT_BY_STARCANDY_TRY:
        case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            XmlParseBuySkillSlot(&xmlDoc); break;
        case BUY_PROPERTY_TRY:
        case SELECT_PROPERTY_TRY:
            XmlParseBuySkillProperty(&xmlDoc); break;
        case KAKAO_UNREGISTER:
            XmlParseKakaoUnregister(&xmlDoc); break;
        case RANKUP_BOAST:
            XmlParseGetPotionStatus(&xmlDoc); break;
        case PURCHASE_SKILL_BY_TOPAZ_TRY:
            XmlParsePurchaseSkill(&xmlDoc); break;
    }
}

void NoImage::XmlParseBuyTopaz(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else // 10 : // 잘못된 토파즈 id
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 토파즈, 별사탕을 갱신한다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUY_TOPAZ_OK, BTN_1);
    }
}

void NoImage::XmlParseBuyStarCandy(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3)
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else // 10 : 잘못된 별사탕 id
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 토파즈, 별사탕을 갱신한다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUY_STARCANDY_OK, BTN_1);
    }
}

void NoImage::XmlParseBuyPotion(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 금액 부족 (토파즈)
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 로그인 기본 정보를 받는다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetMoney(topaz, starcandy);
        myInfo->SetPotion(potion, remainTime);
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUYPOTION_OK, BTN_1);
    }
}

void NoImage::XmlParseSendPotion(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        /*else if (code == 10) // 포션 수신 거부 상태
        {
            friendList[d[0]]->SetPotionMsgStatus(0);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_REJECT, BTN_1);
        }*/
        else if (code == 11) // 친구가 아님
        {
            friendList[d[0]]->SetPotionMsgStatus(0);
            friendList[d[0]]->SetRemainPotionTime(0);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_NO_FRIEND, BTN_1);
        }
        else if (code == 12) // 포션을 보낸 지 1시간 경과하지 않음
        {
            friendList[d[0]]->SetRemainPotionTime(3600);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_EARLY, BTN_1);
        }
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 그 사이에 블락된 경우 (다른 사람이) => 카카오톡 메시지 보내지 마라.
        int isBlocked = 1 - nodeResult.child("user-blocked").text().as_int();
        friendList[d[0]]->SetPotionMsgStatus(isBlocked);
        
        if (!friendList[d[0]]->IsMessageBlocked() && friendList[d[0]]->GetPotionMsgStatus() > 0)
        {
            // 카카오 api 호출 (초대메시지 템플릿 이용)
            sendLinkType = type;
            std::string templateId = KAKAO_MSG_TEMPLATE_SENDPOTION;
            std::string executeUrl = "";
            char temp[128];
            sprintf(temp, "{\"sender_name\":\"%s\"}", MyInfo::GetName().c_str());
            std::string metaInfo = temp;
            //CCLog("metaInfo = %s", metaInfo.c_str());
            KakaoNativeExtension::getInstance()->sendLinkMessage(std::bind(&NoImage::onSendLinkMessageComplete, this), std::bind(&NoImage::onSendLinkMessageErrorComplete, this, std::placeholders::_1, std::placeholders::_2), templateId, friendList[d[0]]->GetKakaoId(), "", executeUrl, metaInfo);
        }
        
        // 성공 팝업창 띄우기
        friendList[d[0]]->SetRemainPotionTime(3600);
        friendList[d[0]]->SetPotionSprite();
        ReplaceScene("NoImage", POTION_SEND_OK, BTN_1);
    }
}

void NoImage::XmlParseMsg(xml_document *xmlDoc)
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
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 포션 정보 갱신
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, remainTime);
        
        // 메시지 리스트 갱신
        for (int i = 0 ; i < msgData.size() ; i++)
            delete msgData[i];
        msgData.clear();
        
        int id, type;
        int rewardCount;
        std::string friendKakaoId;
        std::string content, profileUrl, noticeUrl;
        std::string name;
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("message-list").children("message");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            friendKakaoId = -1;
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "type") type = ait->as_int();
            }
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "content") content = ait->as_string();
                else if (name == "friend-profile-image-url") profileUrl = ait->as_string();
                else if (name == "reward-count") rewardCount = ait->as_int();
                else if (name == "notice-url") noticeUrl = "";
                else if (type == 5 && name == "friend-kakao-id") friendKakaoId = ait->as_string();
            }
            msgData.push_back( new Msg(id, type, rewardCount, content, profileUrl, noticeUrl, friendKakaoId) );
        }
        
        myInfo->SetMsgCnt((int)msgData.size());
        
        // Notification : Ranking / Message 화면에 데이터 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Message", param);
        ReplaceScene("NoImage", MESSAGE_ALL_OK, BTN_1);
    }
}

void NoImage::XmlParseSendTopaz(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
        else if (code == 10) // 없는 토파즈 아이디
            ReplaceScene("NoImage", SEND_TOPAZ_FAIL, BTN_1);
        else if (code == 11) // 친구가 아님
            ReplaceScene("NoImage", SEND_TOPAZ_FAIL, BTN_1);
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", SEND_TOPAZ_OK, BTN_1);
    }
}

void NoImage::XmlParseUpgradeStaff(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 잔액 부족
        {
            if (type == UPGRADE_STAFF_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 11) // 이미 지팡이 만렙
            ReplaceScene("NoImage", UPGRADE_STAFF_FULL_LEVEL, BTN_1);
        else if (code == 12) // MP 500 미만이라 강화 불가능.
            ReplaceScene("NoImage", UPGRADE_STAFF_INSUFFICIENT_MP, BTN_1);
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffFailPoint = nodeResult.child("coco").attribute("magic-staff-fail-point").as_int();
        myInfo->SetCoco(mp, mpStaffPercent, mpFairy, staffLv, staffFailPoint);
        
        // 포션 보상에 따른 개수 변화
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, potionRemainTime);
        
        // MP 보상에 따른 변화
        int isPotionMax = nodeResult.child("mp-reward").attribute("potion-max").as_int();
        int addTopazNum = nodeResult.child("mp-reward").attribute("add-topaz-num").as_int();
        myInfo->SetReward(isPotionMax, addTopazNum);
        
        
        // 성공/실패 팝업창으로 넘어간다.
        int result = nodeResult.child("upgrade-result").text().as_int();
        if (result == 1)
        {
            ReplaceScene("NoImage", UPGRADE_STAFF_OK, BTN_1);
            CCString* param = CCString::create("3");
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        }
        else
            ReplaceScene("NoImage", UPGRADE_STAFF_FAIL, BTN_1);
        
        // GameReady, CocoRoom에 topaz, starcandy, mp 정보 변경시킨다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    }
}

void NoImage::XmlParseUpgradeFairy(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 잔액 부족
        {
            if (type == UPGRADE_FAIRY_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 12) // 이미 요정 만렙
            ReplaceScene("NoImage", UPGRADE_FAIRY_FULL_LEVEL, BTN_1);
        else // 10 (잘못된 업그레이드 타입) , 11 (요정 정보가 없을 때)
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // fairy list 갱신
        myInfo->ClearFairyList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("fairy-list").children("fairy");
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
        
        // 코코 정보 갱신
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffFailPoint = nodeResult.child("coco").attribute("magic-staff-fail-point").as_int();
        myInfo->SetCoco(mp, mpStaffPercent, mpFairy, staffLv, staffFailPoint);
        
        // 포션 보상에 따른 개수 변화
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, potionRemainTime);
        
        // MP 보상에 따른 변화
        int isPotionMax = nodeResult.child("mp-reward").attribute("potion-max").as_int();
        int addTopazNum = nodeResult.child("mp-reward").attribute("add-topaz-num").as_int();
        myInfo->SetReward(isPotionMax, addTopazNum);
        
        // 성공/실패 팝업창으로 넘어간다.
        int result = nodeResult.child("upgrade-result").text().as_int();
        if (result == 1)
            ReplaceScene("NoImage", UPGRADE_FAIRY_OK, BTN_1);
        else
            ReplaceScene("NoImage", UPGRADE_FAIRY_FAIL, BTN_1);
        
        // GameReady, CocoRoom에 topaz, starcandy, mp 정보 변경시킨다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        // 게임준비에 요정 정보 변경
        param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    }
}

void NoImage::XmlParseBuyFairy(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 잔액 부족
        {
            if (type == BUY_FAIRY_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code >= 10 && code <= 12) // 10 : 없는 요정 ID , 11 : 이미 보유하고 있는 요정 ID , 12 : 결제 방법 틀림 (활성화 조건 미충족)
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
        else // 재부팅 시키기
            ReplaceScene("NoImage", NEED_TO_REBOOT, BTN_1);
    }
    
    else if (code == 0)
    {
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // myFairy list 갱신
        myInfo->ClearFairyList();
        
        std::string name;
        int cfi, ufi, level, isUse, failPoint;
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("fairy-list").children("fairy");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "common-fairy-id") cfi = ait->as_int();
                else if (name == "level") level = ait->as_int();
                else if (name == "user-fairy-id") ufi = ait->as_int();
                else if (name == "is-use") isUse = ait->as_int();
                else if (name == "fairy-fail-point") failPoint = ait->as_int();
            }
            myInfo->AddFairy(cfi, ufi, level, isUse, failPoint);
        }
        
        // 코코 정보 갱신
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        int staffFailPoint = nodeResult.child("coco").attribute("magic-staff-fail-point").as_int();
        myInfo->SetCoco(mp, mpStaffPercent, mpFairy, staffLv, staffFailPoint);
        
        // 포션 보상에 따른 개수 변화
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int potionRemainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, potionRemainTime);
        
        // MP 보상에 따른 변화
        int isPotionMax = nodeResult.child("mp-reward").attribute("potion-max").as_int();
        int addTopazNum = nodeResult.child("mp-reward").attribute("add-topaz-num").as_int();
        myInfo->SetReward(isPotionMax, addTopazNum);
        
        // CocoRoomFairyTown에 요정리스트 정보 갱신한다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoomFairyTown", param);
        
        // CocoRoom에 요정슬롯 + (토파즈/별사탕/MP) 정보 갱신한다.
        param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        param = CCString::create("8");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        if ((int)myInfo->GetFairyList().size() == 1)
        {
            // 요정을 사고 난 뒤 갱신된 요정리스트에 요정이 1개다? => 처음 구입한 요정이다.
            // 따라서, 자동으로 그 요정이 착용되도록 using_fairy.php를 호출해 준다.
            char temp[150];
            std::string url = URL_USING_FAIRY;
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "user_fairy_id=%d", myInfo->GetFairyList()[0]->GetUserId());
            param += temp;
            //CCLog("url = %s", url.c_str());
            
            type = USING_FAIRY;
            
            HttpRequest(url, param);
        }
        else
        {
            // 그게 아닌 경우
            ReplaceScene("NoImage", BUY_FAIRY_OK, BTN_1);
        }
    }
}

void NoImage::XmlParseUsingFairy(xml_document *xmlDoc)
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
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // init
        myInfo->ClearFairyList();
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("fairy-list").children("fairy");
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
        
        // 정보 갱신 (게임준비, 코코방_요정, 친구리스트의 내정보)
        CCString* param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        Friend::ChangeMyFairyInfo();
        param = CCString::create("7");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        ReplaceScene("NoImage", BUY_FAIRY_OK, BTN_1);
    }
}

void NoImage::XmlParseBuySkillSlot(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 금액 부족
        {
            if (type == BUY_SKILLSLOT_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else // 10 : 이미 구매한 슬롯 or 이전 단계 구매하지 않음 , 11 : 슬롯 개수 MAX
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // skill-slot 갱신
        myInfo->ClearSkillSlot();
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
        
        // 부모 scene에 슬롯 정보 갱신된 것 전달
        CCString* param = CCString::create("9");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);

        ReplaceScene("NoImage", BUY_SKILLSLOT_OK, BTN_1);
    }
}

void NoImage::XmlParseBuySkillProperty(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else if (code == 3) // 금액 부족 (토파즈)
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else // 10 : 해당 속성 이미 보유 , 11 : 마스터 타입은 구매 불가
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // property 정보 갱신
        int fire = nodeResult.child("properties").attribute("fire").as_int();
        int water = nodeResult.child("properties").attribute("water").as_int();
        int land = nodeResult.child("properties").attribute("land").as_int();
        int master = nodeResult.child("properties").attribute("master").as_int();
        int fireByTopaz = nodeResult.child("properties").attribute("fire-purchase-topaz").as_int();
        int waterByTopaz = nodeResult.child("properties").attribute("water-purchase-topaz").as_int();
        int landByTopaz = nodeResult.child("properties").attribute("land-purchase-topaz").as_int();
        myInfo->SetProperties(fire, water, land, master, fireByTopaz, waterByTopaz, landByTopaz);
        
        for (int i = 0 ; i < friendList.size() ; i++)
        {
            if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
                friendList[i]->SetProperties(fire, water, land, master);
        }
        
        // 각 scene에 property 정보 갱신된 것 전달
        CCString* param = CCString::create("8");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        // 슬롯 정보 갱신
        param = CCString::create("9");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 돈 정보 갱신
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp, learntime;
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
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        // 스킬 슬롯 갱신
        myInfo->ClearSkillSlot();
        its = nodeResult.child("skill-slot").children("slot");
        int id;
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
        // 부모의 스킬 슬롯 UI 갱신
        param = CCString::create("9");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 프로필 문구를 정하기 위해 서버로 업데이트
        Common::UpdateProfileTitle();
        
        isHintForBuyingNextProperty = false; // 다음 속성 무료 구매 힌트 팝업창에 대한 flag 초기화

        // OK 창으로 넘어가자.
        if (type == BUY_PROPERTY_TRY)
            ReplaceScene("NoImage", BUY_PROPERTY_OK, BTN_1);
        else
            ReplaceScene("NoImage", SELECT_PROPERTY_OK, BTN_1);
    }
}

void NoImage::XmlParseKakaoUnregister(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3)
            Network::ShowCommonError(code);
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    else
    {
        // 모든 클라이언트 데이터 초기화
        CCUserDefault::sharedUserDefault()->setStringForKey("refresh_token", "");
        CCUserDefault::sharedUserDefault()->setStringForKey("access_token", "");
        CCUserDefault::sharedUserDefault()->setIntegerForKey("gameVersion", -1);
        CCUserDefault::sharedUserDefault()->setIntegerForKey("binaryVersion", -1);
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_00", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_01", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_02", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_03", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("item_04", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("is_inGameTutorial_done", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("is_tutorial_done", false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_0", true);
        CCUserDefault::sharedUserDefault()->setBoolForKey("setting_option_1", true);
        CCUserDefault::sharedUserDefault()->setStringForKey("todayCandy_1", "");
        CCUserDefault::sharedUserDefault()->setStringForKey("todayCandy_2", "");
        CCUserDefault::sharedUserDefault()->setStringForKey("todayCandy_3", "");
        CCUserDefault::sharedUserDefault()->setStringForKey("todayCandy_4", "");
        char name[30];
        for (int i = 0 ; i < fairyInfo.size() ; i++)
        {
            sprintf(name, "buyingFairy_%d", fairyInfo[i]->GetId());
            CCUserDefault::sharedUserDefault()->setBoolForKey(name, false);
        }
        
        
        // 카카오 회원탈퇴 후 게임종료
        KakaoNativeExtension::getInstance()->unregister(std::bind(&NoImage::onUnregisterComplete, this), std::bind(&NoImage::onUnregisterErrorComplete, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void NoImage::XmlParseGetPotionStatus(xml_document *xmlDoc) // 친구의 카톡메시지수신 여부 받기
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
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 그 사이에 블락된 경우 (다른 사람이) => 카카오톡 메시지 보내지 마라.
        int isBlocked = 1 - nodeResult.child("user-blocked").text().as_int();
        friendList[d[0]]->SetPotionMsgStatus(isBlocked);
        
        // 카카오 메시지 블락도 풀려있고, 환경설정 수신거부도 해제되어 있을 때 보내야지!
        if (!friendList[d[0]]->IsMessageBlocked() && friendList[d[0]]->GetPotionMsgStatus() > 0)
        {
            // 카카오 api 호출 (초대메시지 템플릿 이용)
            std::string templateId = KAKAO_MSG_TEMPLATE_BOAST;
            std::string executeUrl = "";
            char temp[100];
            sprintf(temp, "{\"sender_name\":\"%s\"}", MyInfo::GetName().c_str());
            std::string metaInfo = temp;
            //CCLog("metaInfo = %s", metaInfo.c_str());
            
            KakaoNativeExtension::getInstance()->sendLinkMessage(std::bind(&NoImage::onSendLinkMessageComplete, this), std::bind(&NoImage::onSendLinkMessageErrorComplete, this, std::placeholders::_1, std::placeholders::_2), templateId, friendList[d[0]]->GetKakaoId(), "", executeUrl, metaInfo);
        }
        // 자랑하기 다시 못하게 하기
        CCString* param = CCString::create("5");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("RankUp", param);

        EndScene();
    }
}


void NoImage::XmlParsePurchaseSkill(xml_document *xmlDoc) // 토파즈로 스킬 구매
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {
        std::vector<int> nullData;
        if (code <= MAX_COMMON_ERROR_CODE && code != 3) // '돈 모자람'의 경우는 따로 처리한다.
            Network::ShowCommonError(code);
        else if (code == 3) // 돈 모자람.
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        else if (code >= 10 && code <= 15)
        {
            // code 10 : 존재하지 않는 스킬 ID.
            // code 11 : 해당 스킬 속성을 지닌 마법사가 아님.
            // code 12 : 요구 MP 미달.
            // code 13 : 요구 지팡이의 레벨 미달.
            // code 14 : 요구 스킬의 소유or레벨 미달.
            // code 15 : 이미 배운 스킬임.
            ReplaceScene("NoImage", PURCHASE_SKILL_FAIL, BTN_1);
        }
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
    
    else if (code == 0)
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp, learntime;
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
        }
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬
        
        // 돈 정보 화면 갱신
        //CCString* param = CCString::create("2");
        //int from = ((Sketchbook*)this->getParent())->FromWhere();
        //if (from == 0) // Ranking의 돈 정보 갱신
        //    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        //else if (from == 1) // GameReady의 돈 정보 갱신
        //    CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 스케치북 돈+MP 정보 갱신
        CCString* param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        // 스케치북 스킬리스트 화면 정보 갱신
        param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        // 게임준비 연습스킬 정보 갱신
        //param = CCString::create("5");
        //CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 현재 팝업창, 부모(스케치북상세화면)팝업창 모두 날린다.
        EndScene();
        void* p = Depth::GetCurPointer();
        ((SketchDetail*)p)->EndScene(true);
        
        // 성공/실패 팝업창 띄우기
        std::vector<int> data;
        data.push_back(d[0]); // 스킬 common id
        Common::ShowPopup(Depth::GetCurPointer(), "Sketchbook", "NoImage", false, PURCHASE_SKILL_OK, BTN_1, data);
    }
}

