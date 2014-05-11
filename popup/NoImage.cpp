#include "NoImage.h"
#include "BuyPotion.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

static int type;
static int btn;
static std::vector<int> d;
static int fromWhere;
//static int priority;

static int newSkillType;

CCScene* NoImage::scene(int popupType, int btnType, std::vector<int> data, int etc, int prio)
{
    // data
    type = popupType;
    btn = btnType;
    d = data;
    
    fromWhere = etc;
    //priority = prio;
    
    CCScene* pScene = CCScene::create();
    NoImage* pLayer = NoImage::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void NoImage::onEnter()
{
    CCLog("NoImage : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void NoImage::onExit()
{
    CCLog("NoImage : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void NoImage::keyBackClicked()
{
    sound->playClick();
    EndScene();
}


bool NoImage::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("NoImage");

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("NoImage : touch prio = %d", this->getTouchPriority());
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    /*
    // notification post
    CCString* param = CCString::create("1");
    if (type == BUY_STARCANDY_TRY)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (type == BUYPOTION_1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    else if (type == MESSAGE_OK_STARCANDY || type == MESSAGE_OK_TOPAZ || type == MESSAGE_OK_POTION || type == MESSAGE_EMPTY || type == MESSAGE_ALL_TRY || type == MESSAGE_ALL_OK)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Message", param);
    //else if (type == UPGRADE_STAFF_BY_STARCANDY_NOMONEY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == UPGRADE_STAFF_BY_TOPAZ_NOMONEY || type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_FAIL || type == UPGRADE_STAFF_FULL_LEVEL || type == UPGRADE_STAFF_OK)
    else if (type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == UPGRADE_STAFF_FULL_LEVEL || fromWhere == 3)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    else if (type == UPGRADE_SKILL_OK || type == UPGRADE_SKILL_FAIL || type == PURCHASE_SKILL_OK || type == PURCHASE_SKILL_FAIL || type == BUY_PROPERTY_TRY || fromWhere == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    else if (fromWhere == 2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    */
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
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
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                    ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "NoImage", this, 1) );
    
    // text (각 팝업 상황에 맞는 문구를 만들어 출력한다)
    int deltaX = 0;
    CCPoint deltaSize = ccp(0, 0);
    char text[150], cost[10];
    switch (type)
    {
        case NETWORK_FAIL:
            sprintf(text, "[오류] 다시 시도해 주세요."); break;
        case BUY_TOPAZ_TRY:
            sprintf(text, "토파즈 %d개를 구매하시겠습니까?", priceTopaz[d[0]]->GetCount()); break;
        case BUY_TOPAZ_OK:
            sprintf(text, "토파즈를 성공적으로 구매하였습니다."); break;
        case BUY_STARCANDY_TRY:
            sprintf(text, "토파즈를 사용하여 별사탕 %d개를 구매하시겠습니까?", priceStarCandy[d[0]]->GetCount()); break;
        case BUY_STARCANDY_OK:
            sprintf(text, "별사탕을 성공적으로 구매하였습니다."); break;
        case BUY_STARCANDY_FAIL:
            sprintf(text, "토파즈가 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case POPUP_EXIT:
            sprintf(text, "정말 그만둘꼬얌?"); break;
        case BUYPOTION_1:
            sprintf(text, "토파즈 5개를 사용하여 포션 5개를 구매하시겠습니까?"); break;
        case BUYPOTION_OK:
            sprintf(text, "포션 5개를 구매하였습니다."); break;
        case BUYPOTION_FAIL:
            sprintf(text, "토파즈가 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case POTION_SEND_TRY:
            sprintf(text, "%s님에게 포션을 1개 보내시겠습니까?", friendList[d[0]]->GetNickname().c_str()); break;
        case POTION_SEND_OK:
            sprintf(text, "성공적으로 포션을 보냈습니다."); break;
        case POTION_SEND_REJECT:
            sprintf(text, "수신 거부 상태인 친구입니다."); break;
        case POTION_SEND_NO_FRIEND:
            sprintf(text, "친구가 아닙니다."); break;
        case POTION_SEND_EARLY:
            sprintf(text, "포션을 보낸지 1시간이 경과하지 않았습니다."); break;
        case MESSAGE_OK_STARCANDY:
            sprintf(text, "별사탕 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_OK_TOPAZ:
            sprintf(text, "토파즈 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_OK_POTION:
            sprintf(text, "포션 %d개를 받았습니다.", d[0]); break;
        case MESSAGE_EMPTY:
            sprintf(text, "삭제된 메시지입니다."); break;
        case MESSAGE_ALL_TRY:
            sprintf(text, "포션을 모두 받으시겠사와요~?"); break;
        case MESSAGE_ALL_OK:
            sprintf(text, "모든 포션을 받았습니다."); break;
        case SEND_TOPAZ_TRY:
            sprintf(text, "%s님에게 토파즈 %d개를 선물하시겠습니까?", friendList[d[0]]->GetNickname().c_str(), priceTopaz[d[1]]->GetCount()); break;
        case SEND_TOPAZ_OK:
            sprintf(text, "토파즈를 선물하였습니다!"); break;
        case SEND_TOPAZ_FAIL:
            sprintf(text, "선물을 보내지 못하였습니다. 다시 시도해 주세요."); break;
        case UPGRADE_STAFF_BY_TOPAZ_TRY:
            deltaX = 150;
            deltaSize = ccp(-200, 100);
            sprintf(text, "지팡이 능력치를 +%d%%에서 +%d%%로 강화하시겠습니까?\n(강화 확률이 높아요!)", myInfo->GetMPStaffPercent(),
                    magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetBonusMPPercent()); break;
        case UPGRADE_STAFF_BY_STARCANDY_TRY:
            deltaX = 150;
            deltaSize = ccp(-200, 100);
            sprintf(text, "지팡이 능력치를 +%d%%에서 +%d%%로 강화하시겠습니까?", myInfo->GetMPStaffPercent(), magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetBonusMPPercent()); break;
        case UPGRADE_STAFF_BY_TOPAZ_NOMONEY:
        case BUY_FAIRY_BY_TOPAZ_NOMONEY:
            sprintf(text, "토파즈가 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case UPGRADE_STAFF_BY_STARCANDY_NOMONEY:
        case BUY_FAIRY_BY_STARCANDY_NOMONEY:
            sprintf(text, "별사탕이 부족합니다. 구매 창으로 이동하시겠습니까?"); break;
        case UPGRADE_STAFF_OK:
            sound->playLvUpSuccess();
            deltaX = 150;
            deltaSize = ccp(-200, 100);
            sprintf(text, "강화 성공!\nMP가 많이 증가했어요!"); break;
        case UPGRADE_STAFF_FAIL:
            sound->playLvUpFail();
            deltaX = 150;
            deltaSize = ccp(-200, 100);
            sprintf(text, "강화 실패!\nㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ"); break;
        case UPGRADE_STAFF_FULL_LEVEL:
            sprintf(text, "어머? 이미 만렙이에요~"); break;
        case BUY_FAIRY_BY_STARCANDY_TRY:
        case BUY_FAIRY_BY_TOPAZ_TRY:
            sprintf(text, "'%s' 요정을 구매하시겠습니까?", fairyInfo[d[0]]->GetName().c_str()); break;
        case BUY_FAIRY_OK:
            sprintf(text, "요정을 구매하였습니다!"); break;
        case BUY_FAIRY_FAIL:
            sprintf(text, "요정을 구매하지 못하였습니다. 다시 시도해 주세요."); break;
        case UPGRADE_SKILL_OK:
            sprintf(text, "축하해요! '%s'의 레벨이 %d가 되었어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str(), d[1]); break;
        case UPGRADE_SKILL_FAIL:
        case PURCHASE_SKILL_FAIL:
            sprintf(text, "에러 발생. 다시 시도해주세요."); break;
        case PURCHASE_SKILL_OK:
            sprintf(text, "축하해요! '%s' 마법을 새로 배웠어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str()); break;
        case BUY_SKILLSLOT_BY_STARCANDY_TRY:
        case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            sprintf(text, "%d번째 슬롯을 구매하시겠습니까?", d[0]); break;
        case BUY_SKILLSLOT_OK:
            sprintf(text, "슬롯을 성공적으로 구매하였습니다."); break;
        case BUY_SKILLSLOT_FAIL:
            sprintf(text, "슬롯 구매를 실패하였습니다. 다시 시도해 주세요."); break;
        case BUY_SKILLSLOT_FULL:
            sprintf(text, "이미 모든 슬롯을 구매하였습니다."); break;
        case BUY_PROPERTY_TRY:
            switch (d[0])
            {
                case 1: sprintf(text, "'불'속성 마법을 배우게 됩니다. '불'속성을 습득하시겠습니까?"); break;
                case 2: sprintf(text, "'물'속성 마법을 배우게 됩니다. '물'속성을 습득하시겠습니까?"); break;
                case 3: sprintf(text, "'땅'속성 마법을 배우게 됩니다. '땅'속성을 습득하시겠습니까?"); break;
                case 4: sprintf(text, "'궁극'속성 마법을 배우게 됩니다. '궁극'속성을 습득하시겠습니까?"); break;
            }
            break;
        case BUY_PROPERTY_FAIL:
            sprintf(text, "속성 습득을 실패하였습니다. 다시 시도해 주세요."); break;
        case BUY_PROPERTY_OK:
            sprintf(text, "새로운 속성을 습득하였습니다. 더욱 풍부해진 마법들을 새로 익혀 보세요!"); break;
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 52, ccp(0.5, 0.5), ccp(49+982/2+deltaX, 640+623/2+50), ccc3(78,47,8), CCSize(782+deltaSize.x, 300+deltaSize.y), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );
    
    // 가격표
    if (type == BUY_STARCANDY_TRY || type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_TOPAZ_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_PROPERTY_TRY)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "NoImage", this, 5) );
        
        if (type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(513, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(513, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        
        sprintf(cost, "x %d", d[1]);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 36, ccp(0, 0), ccp(580, 743), ccc3(255,255,255), "", "NoImage", this, 5) );
    }
    
    
    // 취소 버튼이 필요할 경우 넣는다.
    if (btn == BTN_2)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png",
                    ccp(0, 0), ccp(126, 711), CCSize(0, 0), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 5) );
    }
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                    ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "NoImage", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5) );
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool NoImage::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        // 'x'나 '취소'를 누를 경우
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                return true;
            }
        }
        // 팝업창에서 '확인' 버튼 하나만 있는 경우.
        /*
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png" &&
                 (type == BUY_STARCANDY_OK || type == BUYPOTION_OK || type == POTION_SEND_OK || type == POTION_SEND_REJECT || type == POTION_SEND_NO_FRIEND || type == POTION_SEND_EARLY || type == MESSAGE_OK_STARCANDY || type == MESSAGE_OK_TOPAZ || type == MESSAGE_OK_POTION || type == MESSAGE_EMPTY || type == MESSAGE_ALL_OK || type == SEND_TOPAZ_OK || type == SEND_TOPAZ_FAIL || type == BUY_TOPAZ_OK || type == NETWORK_FAIL || type == UPGRADE_STAFF_FULL_LEVEL || type == UPGRADE_STAFF_OK || type == UPGRADE_STAFF_FAIL || type == BUY_FAIRY_OK || type == BUY_FAIRY_FAIL || type == UPGRADE_SKILL_OK || type == UPGRADE_SKILL_FAIL ||
                  type == PURCHASE_SKILL_FAIL || type == PURCHASE_SKILL_OK || type == BUY_SKILLSLOT_FAIL || type == BUY_SKILLSLOT_FULL || type == BUY_SKILLSLOT_OK || type == BUY_PROPERTY_OK || type == BUY_PROPERTY_FAIL) )
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png" && btn == BTN_1)
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                return true;
            }
        }
        */
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                char temp[255];
                
                if (btn == BTN_1) // 팝업창에서 '확인' 버튼 하나만 있는 경우.
                {
                    //CCNode* parent = this->getParent();
                    EndScene();
                    /*
                     // 특정 scene은 그 부모의 scene까지 end 시킨다.
                     if (type == BUY_STARCANDY_OK || type == BUYPOTION_OK)
                     {
                     parent->removeFromParentAndCleanup(true);
                     }
                     */
                    return true;
                }
                else if (type == POPUP_EXIT)
                {
                    CCDirector::sharedDirector()->end();
                    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                    exit(0);
                    #endif
                }
                else if (type == BUY_TOPAZ_TRY)
                {
                    // 토파즈 구입하기. (미결제 버전)
                    std::string url = "http://14.63.225.203/cogma/game/purchase_topaz.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "topaz_id=%d&", priceTopaz[d[0]]->GetId());
                    url += temp;
                    
                    HttpRequest(url);
                }
                else if (type == BUY_STARCANDY_TRY)
                {
                    std::string url = "http://14.63.225.203/cogma/game/purchase_starcandy.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "starcandy_id=%d&", priceStarCandy[d[0]]->GetId());
                    url += temp;
                    sprintf(temp, "cost_value=%d", priceStarCandy[d[0]]->GetPrice());
                    url += temp;
                    CCLog("url : %s", url.c_str());
                    
                    HttpRequest(url);
                }
                else if (type == BUY_STARCANDY_FAIL)
                {
                    // 토파즈를 구매하시곘습니까 : 예 -> 토파즈 구매 창으로 이동.
                    // (코드 상에서는 BuyStarCandy에서 BuyTopaz로 이동한 것처럼 구현한다)
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, "BuyStarCandy", "BuyTopaz", false, 3);
                    break;
                }
                else if (type == BUYPOTION_1)
                {
                    if (myInfo->GetTopaz() < 5) // 토파즈 개수 < 5 라면 네트워크 시도 X.
                        ReplaceScene("NoImage", BUYPOTION_FAIL, BTN_2);
                    else
                    {
                        // 포션 구매 프로토콜을 요청한다.
                        std::string url = "http://14.63.225.203/cogma/game/purchase_potion.php?";
                        sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                        url += temp;
                        
                        HttpRequest(url);
                    }
                }
                else if (type == BUYPOTION_FAIL)
                {
                    // 토파즈를 구매하시곘습니까 : 예 -> 토파즈 구매 창으로 이동.
                    // (코드 상에서는 BuyPotion에서 BuyTopaz로 이동한 것처럼 구현한다)
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, "BuyPotion", "BuyTopaz", false, 2);
                    break;
                }
                else if (type == POTION_SEND_TRY)
                {
                    // 포션 구매 프로토콜을 요청한다.
                    std::string url = "http://14.63.225.203/cogma/game/send_potion.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "friend_kakao_id=%d", friendList[d[0]]->GetKakaoId());
                    url += temp;
                    
                    HttpRequest(url);
                }
                else if (type == MESSAGE_ALL_TRY)
                {
                    // 포션 모두 받기 프로토콜 요청.
                    std::string url = "http://14.63.225.203/cogma/game/receive_message_all_potion.php?";
                    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                    url += temp;
                    
                    HttpRequest(url);
                }
                else if (type == SEND_TOPAZ_TRY)
                {
                    // 토파즈 선물하기. (미결제 버전)
                    std::string url = "http://14.63.225.203/cogma/game/send_topaz.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "friend_kakao_id=%d&", friendList[d[0]]->GetKakaoId());
                    url += temp;
                    sprintf(temp, "topaz_id=%d", priceTopaz[d[1]]->GetId());
                    url += temp;
                    
                    HttpRequest(url);
                }
                else if (type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY)
                {
                    // 토파즈vs별사탕 구분 및 가격
                    int costType = (type == UPGRADE_STAFF_BY_TOPAZ_TRY) ? 2 : 1;
                    int cost = magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_Topaz();
                    if (costType == 1) // 일반강화는 별사탕
                        cost = magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetCost_StarCandy();
                    
                    // 잔액 부족
                    if (costType == 2 && myInfo->GetTopaz() < cost)
                        ReplaceScene("NoImage", UPGRADE_STAFF_BY_TOPAZ_NOMONEY, BTN_2);
                    else if (costType == 1 && myInfo->GetStarCandy() < cost)
                        ReplaceScene("NoImage", UPGRADE_STAFF_BY_STARCANDY_NOMONEY, BTN_2);
                    else
                    {
                        // 지팡이 강화 (by 별사탕, by 토파즈 모두 통용됨)
                        std::string url = "http://14.63.225.203/cogma/game/upgrade_staff.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "cost_type=%d&", costType);
                        url += temp;
                        sprintf(temp, "cost_value=%d", cost);
                        url += temp;
                        CCLog("upgradestaff url = %s", url.c_str());
                        HttpRequest(url);
                    }
                }
                else if (type == BUY_FAIRY_BY_TOPAZ_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY)
                {
                    // 토파즈vs별사탕 구분 및 가격
                    int costType = (type == BUY_FAIRY_BY_TOPAZ_TRY) ? 2 : 1;
                    int cost = fairyInfo[d[0]]->GetCostTopaz();
                    if (costType == 1) // 별사탕으로 산다면,
                        cost = fairyInfo[d[0]]->GetCostStarCandy();
                 
                    // 잔액 부족
                    if (costType == 2 && myInfo->GetTopaz() < cost)
                        ReplaceScene("NoImage", BUY_FAIRY_BY_TOPAZ_NOMONEY, BTN_2);
                    else if (costType == 1 && myInfo->GetStarCandy() < cost)
                        ReplaceScene("NoImage", BUY_FAIRY_BY_STARCANDY_NOMONEY, BTN_2);
                    else
                    {
                        // 요정 구입 (by 별사탕, by 토파즈 모두 통용됨)
                        std::string url = "http://14.63.225.203/cogma/game/purchase_fairy.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "fairy_id=%d&", fairyInfo[d[0]]->GetId());
                        url += temp;
                        sprintf(temp, "cost_type=%d&", costType);
                        url += temp;
                        sprintf(temp, "cost_value=%d", cost);
                        url += temp;
                        CCLog("buy fairy url = %s", url.c_str());
                        HttpRequest(url);
                    }
                }
                else if (type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY)
                {
                    CCLog("슬롯 구매");
                    // 스킬 슬롯 구매
                    //http://14.63.225.203/cogma/game/upgrade_skill_slot.php?kakao_id=1000&slot_id=1&cost_value=1000
                    char temp[255];
                    std::string url = "http://14.63.225.203/cogma/game/upgrade_skill_slot.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "slot_id=%d&", d[0]);
                    url += temp;
                    sprintf(temp, "cost_value=%d", d[1]);
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    HttpRequest(url);
                }
                else if (type == BUY_PROPERTY_TRY)
                {
                    CCLog("스킬 열기");
                    // 스킬 새 속성 열기
                    //http://14.63.225.203/cogma/game/purchase_skill_type.php?kakao_id=1000&skill_type=2&cost_value=0
                    char temp[255];
                    std::string url = "http://14.63.225.203/cogma/game/purchase_skill_type.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    // 서버랑 클라이언트랑 불/물 숫자가 서로 반대여서 부득이하게 아래처럼 판별하도록 한다.
                    if (d[0] == 1) newSkillType = 2;
                    else if (d[0] == 2) newSkillType = 1;
                    else newSkillType = d[0];
                    sprintf(temp, "skill_type=%d&", newSkillType);
                    url += temp;
                    sprintf(temp, "cost_value=%d", d[1]);
                    url += temp;
                    CCLog("url = %s", url.c_str());
                    
                    HttpRequest(url);
                }
            }
        }
    }
    
    return true;
}


void NoImage::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void NoImage::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void NoImage::ReplaceScene(std::string to, int type, int btnType)
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;

    Common::ShowPopup(this, "NoImage", to, true, type, btnType, d, fromWhere);
    //Common::ShowPopup(this, "NoImage", to, true, type, btnType, d, fromWhere, priority);
    
    this->removeFromParentAndCleanup(true);
}

void NoImage::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    this->removeFromParentAndCleanup(true);
    
    /*
    CCString* param = CCString::create("0");
    if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (type == BUY_STARCANDY_TRY || type == BUY_STARCANDY_OK || type == BUY_STARCANDY_FAIL)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == BUYPOTION_1 || type == BUYPOTION_OK || type == BUYPOTION_FAIL)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    else if (type == MESSAGE_OK_STARCANDY || type == MESSAGE_OK_TOPAZ || type == MESSAGE_OK_POTION || type == MESSAGE_EMPTY || type == MESSAGE_ALL_TRY || type == MESSAGE_ALL_OK)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Message", param);
    else if (type == UPGRADE_STAFF_OK || type == UPGRADE_STAFF_FAIL || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_FULL_LEVEL || type == UPGRADE_STAFF_BY_STARCANDY_NOMONEY || type == UPGRADE_STAFF_BY_TOPAZ_NOMONEY || fromWhere == 3)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    else if (type == UPGRADE_SKILL_OK || type == UPGRADE_SKILL_FAIL || type == PURCHASE_SKILL_OK || type == PURCHASE_SKILL_FAIL || type == BUY_PROPERTY_TRY || type == BUY_PROPERTY_FAIL || type == BUY_PROPERTY_OK || fromWhere == 1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    else if (fromWhere == 2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
    */
}

void NoImage::EndSceneCallback()
{
}



/////////////////////////////////////////////////////////////////////////////////////

void NoImage::HttpRequest(std::string url)
{
    // post request
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestType(CCHttpRequest::kHttpPost);
    req->setResponseCallback(this, httpresponse_selector(NoImage::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(req);
    req->release();
}

void NoImage::onHttpRequestCompleted(CCNode *sender, void *data)
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
    
    // parse xml data
    switch (type)
    {
        case BUY_TOPAZ_TRY:
            XmlParseBuyTopaz(dumpData, buffer->size()); break;
        case BUY_STARCANDY_TRY:
            XmlParseBuyStarCandy(dumpData, buffer->size()); break;
        case BUYPOTION_1:
            XmlParseBuyPotion(dumpData, buffer->size()); break;
        case POTION_SEND_TRY:
            XmlParseSendPotion(dumpData, buffer->size()); break;
        case MESSAGE_ALL_TRY:
            XmlParseMsg(dumpData, buffer->size()); break;
        case SEND_TOPAZ_TRY:
            XmlParseSendTopaz(dumpData, buffer->size()); break;
        case UPGRADE_STAFF_BY_TOPAZ_TRY:
        case UPGRADE_STAFF_BY_STARCANDY_TRY:
            XmlParseUpgradeStaff(dumpData, buffer->size()); break;
        case BUY_FAIRY_BY_TOPAZ_TRY:
        case BUY_FAIRY_BY_STARCANDY_TRY:
            XmlParseBuyFairy(dumpData, buffer->size()); break;
        case BUY_SKILLSLOT_BY_STARCANDY_TRY:
        case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            XmlParseBuySkillSlot(dumpData, buffer->size()); break;
        case BUY_PROPERTY_TRY:
            XmlParseBuySkillProperty(dumpData, buffer->size()); break;
    }
}

void NoImage::XmlParseBuyTopaz(char* data, int size)
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
        // 토파즈, 별사탕을 갱신한다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUY_TOPAZ_OK, BTN_1);
    }
    else if (code == 10)
    {
        // 잘못된 토파즈 id
        ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
}

void NoImage::XmlParseBuyStarCandy(char* data, int size)
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
        // 토파즈, 별사탕을 갱신한다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUY_STARCANDY_OK, BTN_1);
    }
    else if (code == 3)
    {
        // 토파즈가 부족해 실패한 경우.
        ReplaceScene("NoImage", BUY_STARCANDY_FAIL, BTN_2);
    }
    else if (code == 10)
    {
        // 잘못된 별사탕 id
    }
}

void NoImage::XmlParseBuyPotion(char* data, int size)
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
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetMoney(topaz, starcandy);
        myInfo->SetPotion(potion, remainTime);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUYPOTION_OK, BTN_1);
    }
    else if (code == 3)
    {
        // 토파즈가 부족해 실패한 경우.
        ReplaceScene("NoImage", BUYPOTION_FAIL, BTN_2);
    }
}

void NoImage::XmlParseSendPotion(char* data, int size)
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
    switch (code)
    {
        case 0: // 포션 보내기 성공
            friendList[d[0]]->SetRemainPotionTime(3600);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_OK, BTN_1);
            break;
        case 10: // 포션 수신 거부 상태
            friendList[d[0]]->SetPotionMsgStatus(0);
            friendList[d[0]]->SetRemainPotionTime(0);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_REJECT, BTN_1);
            break;
        case 11: // 친구가 아님
            friendList[d[0]]->SetPotionMsgStatus(0);
            friendList[d[0]]->SetRemainPotionTime(0);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_NO_FRIEND, BTN_1);
            break;
        case 12: // 포션을 보낸지 1시간 경과하지 않음
            friendList[d[0]]->SetRemainPotionTime(3600);
            friendList[d[0]]->SetPotionSprite();
            ReplaceScene("NoImage", POTION_SEND_EARLY, BTN_1);
            break;
    }
}

void NoImage::XmlParseMsg(char* data, int size)
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
        CCLog("모두받기 xml");
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
        std::string content, profileUrl, noticeUrl;
        std::string name;
        xml_object_range<xml_named_node_iterator> msg = nodeResult.child("message-list").children("message");
        for (xml_named_node_iterator it = msg.begin() ; it != msg.end() ; ++it)
        {
            for (xml_attribute_iterator ait = it->attributes_begin() ; ait != it->attributes_end() ; ++ait)
            {
                name = ait->name();
                if (name == "id") id = ait->as_int();
                else if (name == "type") type = ait->as_int();
                else if (name == "content") content = ait->as_string();
                else if (name == "friend-profile-image-url") profileUrl = ait->as_string();
                else if (name == "reward-count") rewardCount = ait->as_int();
                else if (name == "notice-url") noticeUrl = "";
            }
            msgData.push_back( new Msg(id, type, rewardCount, content, profileUrl, noticeUrl) );
        }
        
        myInfo->SetMsgCnt((int)msgData.size());
        
        // Notification : Ranking 화면에 데이터 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        // Notification : Message 화면에 데이터 갱신
        //param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Message", param);
        ReplaceScene("NoImage", MESSAGE_ALL_OK, BTN_1);
    }
    else
    {
        CCLog("FAILED : code = %d", code);
    }
}

void NoImage::XmlParseSendTopaz(char* data, int size)
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
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", SEND_TOPAZ_OK, BTN_1);
    }
    else if (code == 10)
    {
        // 없는 토파즈 아이디
        ReplaceScene("NoImage", SEND_TOPAZ_FAIL, BTN_1);
    }
    else if (code == 11)
    {
        // 친구가 아님
        ReplaceScene("NoImage", SEND_TOPAZ_FAIL, BTN_1);
    }
}

void NoImage::XmlParseUpgradeStaff(char* data, int size)
{
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
        int result = nodeResult.child("upgrade-result").text().as_int();
        
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        myInfo->SetCoco(mp, mpStaffPercent, mpFairy, staffLv);
        
        // 성공/실패 팝업창으로 넘어간다.
        if (result == 1)
            ReplaceScene("NoImage", UPGRADE_STAFF_OK, BTN_1);
        else
            ReplaceScene("NoImage", UPGRADE_STAFF_FAIL, BTN_1);
        
        // Ranking에 topaz, starcandy, mp 정보 변경시킨다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
    }
    else if (code == 3)
    {
        // 잔액 부족
        if (type == UPGRADE_STAFF_BY_TOPAZ_TRY)
            ReplaceScene("NoImage", UPGRADE_STAFF_BY_TOPAZ_NOMONEY, BTN_2);
        else
            ReplaceScene("NoImage", UPGRADE_STAFF_BY_STARCANDY_NOMONEY, BTN_2);
    }
    else if (code == 11)
    {
        // 이미 지팡이 만렙
        ReplaceScene("NoImage", UPGRADE_STAFF_FULL_LEVEL, BTN_1);
    }
    else
    {
        ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
}

void NoImage::XmlParseBuyFairy(char* data, int size)
{
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
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // myFairy list 갱신
        myInfo->ClearFairyList();
        
        std::string name;
        int cfi, ufi, level, isUse;
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
            }
            myInfo->AddFairy(cfi, ufi, level, isUse);
        }
        
        // GameReady에 topaz, starcandy, mp 정보 변경시킨다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // CocoRoomFairyTown에 요정리스트 정보 갱신한다.
        
        // CocoRoom에 요정슬롯 정보 갱신한다.
    }
    else if (code == 3)
    {
        // 잔액 부족
        if (type == BUY_FAIRY_BY_TOPAZ_TRY)
            ReplaceScene("NoImage", BUY_FAIRY_BY_TOPAZ_NOMONEY, BTN_2);
        else
            ReplaceScene("NoImage", BUY_FAIRY_BY_STARCANDY_NOMONEY, BTN_2);
    }
    else
    {
        ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
}

void NoImage::XmlParseBuySkillSlot(char* data, int size)
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
        if (fromWhere == 1)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        else if (fromWhere == 2)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        else if (fromWhere == 3)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        ReplaceScene("NoImage", BUY_SKILLSLOT_OK, BTN_1);
    }
    else
    {
        if (code == 3) CCLog("슬롯구매 : 금액 부족");
        else if (code == 4) CCLog("슬롯구매 : 금액이 맞지 않음. 재부팅.");
        else if (code == 10) CCLog("슬롯구매 : 이미 구매한 슬롯 or 이전 단계 구매하지 않음");
        else if (code == 11) CCLog("슬롯구매 : 슬롯 개수 MAX");
        else CCLog("슬롯구매 : (code = %d) 기타 에러", code);
        
        ReplaceScene("NoImage", BUY_SKILLSLOT_FAIL, BTN_1);
    }
}

void NoImage::XmlParseBuySkillProperty(char* data, int size)
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
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // property 정보 갱신
        int fire = nodeResult.child("properties").attribute("fire").as_int();
        int water = nodeResult.child("properties").attribute("water").as_int();
        int land = nodeResult.child("properties").attribute("land").as_int();
        int master = nodeResult.child("properties").attribute("master").as_int();
        myInfo->SetProperties(fire, water, land, master);
        
        // 각 scene에 property 정보 갱신된 것 전달
        CCString* param = CCString::create("8");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        // 돈 정보 갱신
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
        /*
        //http://14.63.225.203/cogma/game/purchase_skill.php?kakao_id=1000&skill_id=22&cost_value=0
        // 1번 스킬 배우기
        char temp[255];
        std::string url = "http://14.63.225.203/cogma/game/purchase_skill.php?";
        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
        url += temp;
        sprintf(temp, "skill_id=%d&", newSkillType*10+1); // 1이면 11번 스킬을 배우는 식
        url += temp;
        sprintf(temp, "cost_value=%d", SkillBuildUpInfo::GetCost(newSkillType*10+1, 1)); // 새로 배우니 당연히 레벨 1
        url += temp;
        CCLog("url = %s", url.c_str());
        
        CCHttpRequest* req = new CCHttpRequest();
        req->setUrl(url.c_str());
        req->setRequestType(CCHttpRequest::kHttpPost);
        req->setResponseCallback(this, httpresponse_selector(NoImage::onHttpRequestCompleted));
        req->setTag("99999");
        CCHttpClient::getInstance()->send(req);
        req->release();
         */
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp;
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
        }
        DataProcess::SortMySkillByCommonId(myInfo->GetSkillList()); // common-skill-id 오름차순 정렬

        // OK 창으로 넘어가자. 
        ReplaceScene("NoImage", BUY_PROPERTY_OK, BTN_1);
    }
    else
    {
        if (code == 3) CCLog("스킬속성구매 : 금액 부족");
        else if (code == 4) CCLog("스킬속성구매 : 금액이 맞지 않음. 재부팅.");
        else if (code == 10) CCLog("스킬속성구매 : 해당 속성 이미 보유");
        else if (code == 11) CCLog("스킬속성구매 : 마스터 타입은 구매불가");
        else CCLog("스킬속성구매 : (code = %d) 기타 에러", code);
        
        ReplaceScene("NoImage", BUY_PROPERTY_FAIL, BTN_1);
    }
}

/*
void NoImage::XmlParseGetFirstSkill(char* data, int size)
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
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // 나의 스킬 리스트 갱신
        myInfo->ClearSkillList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("skill-list").children("skill");
        int csi, usi, level, exp;
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
        }
        DataProcess::SortMySkillByCommonId(myInfo->GetSkillList()); // common-skill-id 오름차순 정렬
        
        ReplaceScene("NoImage", BUY_PROPERTY_OK, BTN_1);
    }
}
 */

