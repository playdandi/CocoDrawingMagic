#include "NoImage.h"
#include "BuyPotion.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

static int type;
static int btn;
static std::vector<int> d;
static int fromWhere;

static int newSkillType;

CCScene* NoImage::scene(int popupType, int btnType, std::vector<int> data, int etc)
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
    if (!isKeybackTouched)
    {
        isKeybackTouched = true;
        sound->playClick();
        EndScene();
    }
}


bool NoImage::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("NoImage", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("NoImage : touch prio = %d", this->getTouchPriority());
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    isEnded = false;
    isTouched = false;
    isTouchDone = false;
    isKeybackTouched = false;
    
    return true;
}

void NoImage::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;

    if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
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
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                    ccp(0, 0), ccp(49, 640+offset), CCSize(982, 623-offset*2), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                    ccp(0, 0), ccp(76, 678+offset), CCSize(929, 562-offset*2), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                    ccp(0, 0), ccp(809, 1039-offset), CCSize(0, 0), "", "NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                    ccp(0, 0), ccp(900, 1132-offset), CCSize(0, 0), "", "NoImage", this, 1) );
    
    
    bool hasImage = false;
    if (type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == UPGRADE_STAFF_OK ||
        type == BUY_TOPAZ_TRY || type == BUY_TOPAZ_OK || type == NEED_TO_BUY_TOPAZ ||
        type == NEED_TO_BUY_STARCANDY || type == BUY_STARCANDY_OK || type == BUY_STARCANDY_TRY ||
        type == NEED_TO_BUY_POTION || type == BUYPOTION_1 || type == BUYPOTION_OK ||
        type == UPGRADE_FAIRY_OK || type == UPGRADE_FAIRY_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_TOPAZ_TRY ||
        type == BUY_PROPERTY_TRY || type == BUY_PROPERTY_OK ||
        type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY || type == BUY_SKILLSLOT_OK ||
        type == TODAYCANDY_RESULT_LOSE || type == TODAYCANDY_RESULT_WIN)
    {
        hasImage = true;
        
        switch (type)
        {
            case UPGRADE_STAFF_BY_TOPAZ_TRY:
            case UPGRADE_STAFF_BY_STARCANDY_TRY:
            case UPGRADE_STAFF_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/magicstaff.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->setScale(0.85f);
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->setRotation(20);
                ((CCSprite*)spriteClass->FindSpriteByName("image/magicstaff.png"))->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.0f, -10), CCRotateBy::create(2.0f, 20), CCRotateBy::create(1.0f, -10), NULL)));
                break;
            case NEED_TO_BUY_TOPAZ:
            case BUY_TOPAZ_TRY:
            case BUY_TOPAZ_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_topaz.png"))->setScale(1.5f);
                break;
            case NEED_TO_BUY_STARCANDY:
            case BUY_STARCANDY_TRY:
            case BUY_STARCANDY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_starcandy.png"))->setScale(1.5f);
                break;
            case NEED_TO_BUY_POTION:
            case BUYPOTION_1:
            case BUYPOTION_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_potion.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_potion.png"))->setScale(2.0f);
                break;
            case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            case UPGRADE_FAIRY_BY_TOPAZ_TRY:
            case UPGRADE_FAIRY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_capsule.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_capsule.png"))->setScale(1.0f);
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_capsule.png"))->setRotation(20);
                ((CCSprite*)spriteClass->FindSpriteByName("icon/icon_capsule.png"))->runAction(CCRepeatForever::create(CCSequence::create(CCRotateBy::create(1.0f, -10), CCRotateBy::create(2.0f, 20), CCRotateBy::create(1.0f, -10), NULL)));
                break;
            case BUY_PROPERTY_TRY:
            case BUY_PROPERTY_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "image/coco_room.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("image/coco_room.png"))->setScale(0.62f);
                //((CCSprite*)spriteClass->FindSpriteByName("image/coco_room.png"))->setFlipX(true);
                break;
            case BUY_SKILLSLOT_BY_STARCANDY_TRY:
            case BUY_SKILLSLOT_BY_TOPAZ_TRY:
            case BUY_SKILLSLOT_OK:
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2) );
                ((CCSprite*)spriteClass->FindSpriteByName("background/bg_skill_yellow.png"))->setScale(1.3f);
                break;
            case TODAYCANDY_RESULT_WIN:
            case TODAYCANDY_RESULT_LOSE:
                CCSprite* profile = ProfileSprite::GetProfile(friendList[d[1]]->GetImageUrl());
                if (friendList[d[1]]->GetImageUrl() != "")
                {
                    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0,0), "", "NoImage", this, 2, 0, 255, 1.5f) );
                    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png", ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "NoImage", this, 3) );
                    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_profile.png"))->setScale(1.65f);
                }
                else
                {
                    spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0.5,0.5), ccp(126+254/2, winSize.height/2), CCSize(0, 0), "", "NoImage", this, 2, 0, 255, 1.65f) );
                }
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
        case NEED_TO_REBOOT:
            title = "다시 시작하기";
            sprintf(text, "세션 종료 혹은 업데이트로 인해 게임을 재부팅합니다."); break;
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
            sprintf(text, "%s님에게 포션을 1개 보내시겠습니까?\n(1시간에 한 번 보낼 수 있습니다)", friendList[d[0]]->GetNickname().c_str()); break;
        case POTION_SEND_OK:
            title = "포션 보내기";
            sprintf(text, "성공적으로 포션을 보냈습니다."); break;
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
            sprintf(text, "토파즈를 선물하였습니다!"); break;
        case SEND_TOPAZ_FAIL:
            title = "토파즈 선물하기";
            sprintf(text, "선물을 보내지 못하였습니다. 다시 시도해 주세요."); break;
            
        case UPGRADE_STAFF_BY_TOPAZ_TRY:
            title = "지팡이 강화하기";
            sprintf(text, "지팡이 능력치를 +%d%%에서 +%d%%로 강화하시겠습니까?\n(강화 확률이 높아요!)", myInfo->GetMPStaffPercent(),
                    magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetBonusMPPercent()); break;
        case UPGRADE_STAFF_BY_STARCANDY_TRY:
            title = "지팡이 강화하기";
            sprintf(text, "지팡이 능력치를 +%d%%에서 +%d%%로 강화하시겠습니까?", myInfo->GetMPStaffPercent(), magicStaffBuildupInfo[myInfo->GetStaffLv()+1-1]->GetBonusMPPercent()); break;
        case UPGRADE_STAFF_OK:
            sound->playLvUpSuccess();
            title = "지팡이 강화하기";
            sprintf(text, "강화 성공!\nMP가 많이 증가했어요!"); break;
        case UPGRADE_STAFF_FAIL:
            title = "지팡이 강화하기";
            sound->playLvUpFail();
            sprintf(text, "강화 실패...\n한 번 더 시도해 보세요~"); break;
        case UPGRADE_STAFF_FULL_LEVEL:
            title = "지팡이 강화하기";
            sprintf(text, "이미 최고 레벨입니다."); break;
        
        case UPGRADE_FAIRY_BY_TOPAZ_TRY:
            title = "요정 강화하기";
            sprintf(text, "요정의 능력을 강화하시겠습니까?\n(강화 확률이 높아요!)"); break;
        case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            title = "요정 강화하기";
            sprintf(text, "요정의 능력을 강화하시겠습니까?"); break;
        case UPGRADE_FAIRY_OK:
            title = "요정 강화하기";
            sound->playLvUpSuccess();
            sprintf(text, "강화 성공!\n 요정의 능력치가 증가했어요!"); break;
        case UPGRADE_FAIRY_FAIL:
            title = "요정 강화하기";
            sound->playLvUpFail();
            sprintf(text, "강화 실패...\n한 번 더 시도해 보세요~"); break;
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
        case PURCHASE_SKILL_OK:
            title = "마법 배우기";
            sprintf(text, "축하해요! '%s' 마법을 새로 배웠어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str()); break;
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
                case 1: sprintf(text, "'불'속성 마법을 배우게 됩니다. '불'속성을 습득하시겠습니까?"); break;
                case 2: sprintf(text, "'물'속성 마법을 배우게 됩니다. '물'속성을 습득하시겠습니까?"); break;
                case 3: sprintf(text, "'땅'속성 마법을 배우게 됩니다. '땅'속성을 습득하시겠습니까?"); break;
                case 4: sprintf(text, "'궁극'속성 마법을 배우게 됩니다. '궁극'속성을 습득하시겠습니까?"); break;
            }
            break;
        case BUY_PROPERTY_FAIL:
            title = "마법 속성 열기";
            sprintf(text, "속성 습득을 실패하였습니다. 다시 시도해 주세요."); break;
        case BUY_PROPERTY_OK:
            title = "마법 속성 열기";
            sprintf(text, "새로운 속성을 습득하였습니다. 더욱 풍부해진 마법들을 새로 익혀 보세요!"); break;
            
        case INVITE_FRIEND_OK:
            title = "친구 초대하기";
            sprintf(text, "친구를 초대하였습니다. 별사탕 1,000개와 포션 1개를 지급해 드렸습니다."); break;
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
            sprintf(text, "친구를 10명 초대하여 별사탕 15,000개가 추가로 지급되었습니다."); break;
        case INVITE_FRIEND_20:
            title = "친구 초대하기";
            sprintf(text, "친구를 20명 초대하여 토파즈 10개가 추가로 지급되었습니다."); break;
        case INVITE_FRIEND_30:
            title = "친구 초대하기";
            sprintf(text, "친구를 30명 초대하여 토파즈 25개가 추가로 지급되었습니다."); break;
            
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
            char present[15];
            switch (myInfo->GetTodayCandyType())
            {
                case 1: sprintf(present, "별사탕"); break;
                case 2: sprintf(present, "토파즈"); break;
                case 3: sprintf(present, "포션"); break;
            }
            title = present;
            title += " 당첨!";
            fontSize = 32;
            //sprintf(text, "축하합니다! %s님께서 오늘의 별사탕에 당첨되셨어요! %s %d개를 지급해 드렸습니다. 참가한 친구들에게는 %s %d개를 보냈습니다.", present, myInfo->GetTodayCandyValueChoice(), present, myInfo->GetTodayCandyValueMiss());
            sprintf(text, "축하합니다!\n%s님께서 %s %d개에 당첨되셨어요.\n(메시지함으로 전송되었습니다)", MyInfo::GetName().c_str(), present, myInfo->GetTodayCandyValueChoice());
            break;
        case TODAYCANDY_RESULT_LOSE:
            switch (myInfo->GetTodayCandyType())
            {
                case 1: sprintf(present, "별사탕"); break;
                case 2: sprintf(present, "토파즈"); break;
                case 3: sprintf(present, "포션"); break;
            }
            title = present;
            title += " 당첨!";
            fontSize = 32;
            //sprintf(text, "참가상으로 %s %d개를 지급해 드렸습니다! 오늘은 \"%s\"님이 당첨되어 %s %d개를 보냈습니다. 카카오톡 메시지를 전달하시겠습니까?", present, myInfo->GetTodayCandyValueMiss(), Friend::GetObj(d[0])->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice());
            sprintf(text, "축하합니다!\n%s님께서 %s %d개에 당첨되셨어요.\n%s님께 %s %d개를 보냅니다.\n(카카오톡 메시지가 전송됩니다)", Friend::GetObj(d[0])->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice(), Friend::GetObj(d[0])->GetNickname().c_str(), present, myInfo->GetTodayCandyValueChoice());
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
    }
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 52, ccp(0.5, 0.5), ccp(49+982/2+deltaX, 640+623/2+50), ccc3(78,47,8), CCSize(782+deltaSize.x, 300+deltaSize.y), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );

    // 내용 문장
    if (!hasImage)
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 40, ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), ccc3(78,47,8), CCSize(829-20, 250), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );
    else
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], fontSize, ccp(0, 0.5), ccp(126+254+50, winSize.height/2), ccc3(78,47,8), CCSize(525, 250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );
    
    // 팝업창 타이틀 (종료할 때만 띄우지 않는다)
    if (type != POPUP_EXIT)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(126+5, 678+562-offset - 35), CCSize(759-126+52, 90), "", "NoImage", this, 1) );
        CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    }
    
    
    // 가격표
    if (type == BUY_TOPAZ_TRY) // 유일하게 '별사탕'/'토파즈' 아이콘이 없는 경우임
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(0, 0), ccp(493, 723+offset), CCSize(201, 77), "", "NoImage", this, 5) );
        CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png2");
        
        std::string val = "";
        if (myInfo->GetDeviceType() == 1) // ANDROID
            val = "₩ " + Common::MakeComma(d[1]);
        else if (myInfo->GetDeviceType() == 2) // iPHONE
            val = "$ " + Common::MakeComma(d[1]/100) + "." + Common::MakeComma(d[1]%100);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 40, ccp(0.5, 0.5), ccp(pos.x+2, pos.y-2), ccc3(0,0,0), "background/bg_degree_desc.png2", "1", NULL, 5, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(val, fontList[0], 40, ccp(0.5, 0.5), pos, ccc3(255,255,255), "background/bg_degree_desc.png2", "1", NULL, 5, 1) );
    }
    else if (type == BUY_STARCANDY_TRY || type == UPGRADE_STAFF_BY_TOPAZ_TRY || type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_TOPAZ_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_TOPAZ_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == BUY_PROPERTY_TRY || type == BUYPOTION_1 || type == UPGRADE_FAIRY_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_TOPAZ_TRY)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png2", ccp(1, 0), ccp(493+201, 723+offset), CCSize(201+30, 77), "", "NoImage", this, 5) );
        
        if (type == UPGRADE_STAFF_BY_STARCANDY_TRY || type == BUY_FAIRY_BY_STARCANDY_TRY || type == BUY_SKILLSLOT_BY_STARCANDY_TRY || type == UPGRADE_FAIRY_BY_STARCANDY_TRY)
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(513-25, 730+offset), CCSize(0, 0), "", "NoImage", this, 5) );
        else
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(513-25, 730+offset), CCSize(0, 0), "", "NoImage", this, 5) );
        
        char cost[10];
        sprintf(cost, "%s", Common::MakeComma(d[1]).c_str());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 40, ccp(0, 0), ccp(580+2-25, 743+offset-2), ccc3(0,0,0), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 40, ccp(0, 0), ccp(580-25, 743+offset), ccc3(255,255,255), "", "NoImage", this, 5) );
    }
    
    
    // 취소 버튼이 필요할 경우 넣는다.
    if (btn == BTN_2)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(0, 0), ccp(126, 711+offset), CCSize(0, 0), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_cancel.png",
            ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_system.png", "0", NULL, 5, 1) );
    }
    
    
    if (type == TODAYCANDY_NOTENOUGH_FRIENDS)
    {
        // 오.별 사람수 모자란 팝업창에서만 버튼이 다르다.
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue.png", ccp(0, 0), ccp(717+5-200, 711-7+offset), CCSize(0, 0), "", "NoImage", this, 5) );
        CCPoint pos = spriteClass->FindParentCenterPos("button/btn_blue.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_friendinvite.png", ccp(0.5,0.5), ccp(pos.x, pos.y+7), CCSize(0, 0), "button/btn_blue.png", "0", NULL, 5, 1) );
    }
    else
    {
        // 확인 버튼
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717+5, 711+offset), CCSize(0, 0), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    }
    
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool NoImage::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched || isTouchDone)
        return false;
    isTouched = true;
    isTouchDone = true;
    
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
                if (type == YOU_WERE_BLOCKED)
                {
                    Exit();
                }
                else
                {
                    EndScene();
                }
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_blue.png")
        {
            // 오.별에서 친구 부족하다는 팝업창에서 '친구 초대하기' 버튼 눌렀을 때
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                Common::ShowNextScene(Depth::GetParentPointer(), "CocoRoom", "InviteFriend", false);
                EndScene();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                char temp[255];
                
                if (btn == BTN_1) // 팝업창에서 '확인' 버튼 하나만 있는 경우.
                {
                    if (type == YOU_WERE_BLOCKED)
                    {
                        Exit();
                    }
                    else if (type == NEED_TO_REBOOT)
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
                    else if (type == PRACTICE_SKILL_FULL_EXP)
                    {
                        EndScene();
                        Common::ShowNextScene(Depth::GetCurPointer(), "GameReady", "Sketchbook", false, 0);
                    }
                    else
                    {
                        EndScene();
                    }
                    /*
                    //CCNode* parent = this->getParent();
                    EndScene();
                    
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
                    Exit();
                }
                else if (type == NEED_TO_BUY_POTION)
                {
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, Depth::GetCurName(), "BuyPotion", false, 3); // curName == 결국 부모
                    return true;
                }
                else if (type == NEED_TO_BUY_TOPAZ)
                {
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, Depth::GetCurName(), "BuyTopaz", false, 3); // curName == 결국 부모
                    return true;
                }
                else if (type == NEED_TO_BUY_STARCANDY)
                {
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, Depth::GetCurName(), "BuyStarCandy", false, 3); // curName == 결국 부모
                    return true;
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
                    if (myInfo->GetTopaz() < priceStarCandy[d[0]]->GetPrice()) // 토파즈 구매 창으로 이동
                        ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
                    else
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
                    return true;
                }
                else if (type == BUYPOTION_1)
                {
                    if (myInfo->GetTopaz() < 5) // 토파즈 구매 창으로 이동
                        ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
                    else
                    {
                        // 포션 구매 프로토콜을 요청한다.
                        std::string url = "http://14.63.225.203/cogma/game/purchase_potion.php?";
                        sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                        url += temp;
                        CCLog("url : %s", url.c_str());
                        HttpRequest(url);
                    }
                    return true;
                }
                else if (type == POTION_SEND_TRY)
                {
                    // 포션 보내기 (랭킹 화면에서)
                    std::string url = "http://14.63.225.203/cogma/game/send_potion.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "friend_kakao_id=%d", friendList[d[0]]->GetKakaoId());
                    url += temp;
                    CCLog("url : %s", url.c_str());
                    HttpRequest(url);
                    return true;
                }
                else if (type == MESSAGE_ALL_TRY)
                {
                    // 포션 모두 받기 프로토콜 요청.
                    std::string url = "http://14.63.225.203/cogma/game/receive_message_all_potion.php?";
                    sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                    url += temp;
                    CCLog("url : %s", url.c_str());
                    HttpRequest(url);
                    return true;
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
                    CCLog("url : %s", url.c_str());
                    HttpRequest(url);
                    return true;
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
                        std::string url = "http://14.63.225.203/cogma/game/upgrade_staff.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "cost_type=%d&", costType);
                        url += temp;
                        sprintf(temp, "cost_value=%d", cost);
                        url += temp;
                        CCLog("url = %s", url.c_str());
                        HttpRequest(url);
                    }
                    return true;
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
                        std::string url = "http://14.63.225.203/cogma/game/upgrade_fairy.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "user_fairy_id=%d&",  myInfo->GetActiveFairyUserId());
                        url += temp;
                        sprintf(temp, "cost_type=%d&", costType);
                        url += temp;
                        sprintf(temp, "cost_value=%d", cost);
                        url += temp;
                        CCLog("url = %s", url.c_str());
                        HttpRequest(url);
                    }
                    return true;
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
                        std::string url = "http://14.63.225.203/cogma/game/purchase_fairy.php?";
                        sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                        url += temp;
                        sprintf(temp, "fairy_id=%d&", fi->GetId());
                        url += temp;
                        sprintf(temp, "cost_type=%d&", costType);
                        url += temp;
                        sprintf(temp, "cost_value=%d", cost);
                        url += temp;
                        CCLog("url = %s", url.c_str());
                        HttpRequest(url);
                    }
                    return true;
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
                    return true;
                }
                else if (type == BUY_PROPERTY_TRY)
                {
                    if (myInfo->GetTopaz() < d[1]) // 토파즈 구매 창으로 이동
                        ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
                    else
                    {
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
                    return true;
                }
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
    if (!isEnded)
        isTouched = false;
}


void NoImage::ReplaceScene(std::string to, int type, int btnType)
{
    isEnded = true;
    isTouched = true;
    
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
    
    CCNode* parent = this->getParent();
    this->removeFromParentAndCleanup(true);
    Common::ShowPopup(parent, "NoImage", to, false, type, btnType, d, fromWhere);
}

void NoImage::EndScene()
{
    isEnded = true;
    isTouched = true;
    
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
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
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

void NoImage::HttpRequest(std::string url)
{
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, Depth::GetCurNameString(), "Loading", false, LOADING_MESSAGE);
    
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
        ReplaceScene("NoImage", NETWORK_FAIL, BTN_1); // network fail로 간주
        return;
    }
    
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
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
        case UPGRADE_FAIRY_BY_TOPAZ_TRY:
        case UPGRADE_FAIRY_BY_STARCANDY_TRY:
            XmlParseUpgradeFairy(dumpData, buffer->size()); break;
        case BUY_FAIRY_BY_TOPAZ_TRY:
        case BUY_FAIRY_BY_STARCANDY_TRY:
            XmlParseBuyFairy(dumpData, buffer->size()); break;
        case USING_FAIRY:
            XmlParseUsingFairy(dumpData, buffer->size()); break;
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
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
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
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUY_STARCANDY_OK, BTN_1);
    }
    else
    {
        if (code == 3) ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        if (code == 10) ; // 잘못된 별사탕 id
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
        
        // 부모 scene에 갱신
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        
        // 성공한 팝업창으로 넘어간다.
        ReplaceScene("NoImage", BUYPOTION_OK, BTN_1);
    }
    else
    {
        if (code == 3) ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
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
            //friendList[d[0]]->SetRemainPotionTime(0);
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
        // 포션 정보 갱신
        int potion = nodeResult.child("potion").attribute("potion-count").as_int();
        int remainTime = nodeResult.child("potion").attribute("remain-time").as_int();
        myInfo->SetPotion(potion, remainTime);
        
        // 메시지 리스트 갱신
        for (int i = 0 ; i < msgData.size() ; i++)
            delete msgData[i];
        msgData.clear();
        
        int id, type;
        int rewardCount, friendKakaoId;
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
                else if (type == 5 && name == "friend-kakao-id") friendKakaoId = ait->as_int();
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
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        int mp = nodeResult.child("coco").attribute("magic-point").as_int();
        int staffLv = nodeResult.child("coco").attribute("magic-staff-level").as_int();
        int mpStaffPercent = nodeResult.child("coco").attribute("magic-staff-bonus-mp").as_int();
        int mpFairy = nodeResult.child("coco").attribute("fairy-bonus-mp").as_int();
        myInfo->SetCoco(mp, mpStaffPercent, mpFairy, staffLv);
        
        // 성공/실패 팝업창으로 넘어간다.
        int result = nodeResult.child("upgrade-result").text().as_int();
        if (result == 1)
            ReplaceScene("NoImage", UPGRADE_STAFF_OK, BTN_1);
        else
            ReplaceScene("NoImage", UPGRADE_STAFF_FAIL, BTN_1);
        
        // GameReady, CocoRoom에 topaz, starcandy, mp 정보 변경시킨다.
        CCString* param = CCString::create("2");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
    }
    else
    {
        if (code == 3) // 잔액 부족
        {
            if (type == UPGRADE_STAFF_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 4)
            ReplaceScene("NoImage", NEED_TO_REBOOT, BTN_1);
        else if (code == 11) // 이미 지팡이 만렙
            ReplaceScene("NoImage", UPGRADE_STAFF_FULL_LEVEL, BTN_1);
        else
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
    }
}

void NoImage::XmlParseUpgradeFairy(char* data, int size)
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
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        // fairy list 갱신
        myInfo->ClearFairyList();
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("fairy-list").children("fairy");
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
    }
    else
    {
        CCLog("failed code = %d", code);
        if (code == 3) // 잔액 부족
        {
            if (type == UPGRADE_FAIRY_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 4)
            ReplaceScene("NoImage", NEED_TO_REBOOT, BTN_1);
        else if (code == 12) // 이미 요정 만렙
            ReplaceScene("NoImage", UPGRADE_FAIRY_FULL_LEVEL, BTN_1);
        else // 10 (잘못된 업그레이드 타입) , 11 (요정 정보가 없을 때)
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
            std::string url = "http://14.63.225.203/cogma/game/using_fairy.php?";
            sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
            url += temp;
            sprintf(temp, "user_fairy_id=%d", myInfo->GetFairyList()[0]->GetUserId());
            url += temp;
            CCLog("url = %s", url.c_str());
            
            type = USING_FAIRY;
            
            HttpRequest(url);
        }
        else
        {
            // 그게 아닌 경우
            ReplaceScene("NoImage", BUY_FAIRY_OK, BTN_1);
        }
    }
    else
    {
        CCLog("failed code = %d", code);
        if (code == 3) // 잔액 부족
        {
            if (type == BUY_FAIRY_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 4) // 서버와 결제 내용 값이 다름 (재부팅)
            ReplaceScene("NoImage", NEED_TO_REBOOT, BTN_1);
        else if (code == 10) // 없는 요정 ID
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
        else if (code == 11) // 이미 보유하고 있는 요정 ID
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
        else if (code == 12) // 결제 방법 틀림 (활성화 조건 미충족)
            ReplaceScene("NoImage", NETWORK_FAIL, BTN_1);
        else // 재부팅 시키기
            ReplaceScene("NoImage", NEED_TO_REBOOT, BTN_1);
    }
}

void NoImage::XmlParseUsingFairy(char* data, int size)
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
        // init
        myInfo->ClearFairyList();
        
        xml_object_range<xml_named_node_iterator> its = nodeResult.child("fairy-list").children("fairy");
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
        
        // 정보 갱신 (게임준비, 코코방_요정, 친구리스트의 내정보)
        CCString* param = CCString::create("4");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        Friend::ChangeMyFairyInfo();
        param = CCString::create("7");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        ReplaceScene("NoImage", BUY_FAIRY_OK, BTN_1);
    }
    else
    {
        CCLog("XmlParseFairyList : failed code = %d", code);
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
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        /*
        if (fromWhere == 1)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        else if (fromWhere == 2)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        else if (fromWhere == 3)
            CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        */
        ReplaceScene("NoImage", BUY_SKILLSLOT_OK, BTN_1);
    }
    else
    {
        if (code == 3) {
            CCLog("슬롯구매 : 금액 부족");
            if (type == BUY_SKILLSLOT_BY_TOPAZ_TRY)
                ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
            else
                ReplaceScene("NoImage", NEED_TO_BUY_STARCANDY, BTN_2);
        }
        else if (code == 4) CCLog("슬롯구매 : 금액이 맞지 않음. 재부팅.");
        else if (code == 10) CCLog("슬롯구매 : 이미 구매한 슬롯 or 이전 단계 구매하지 않음");
        else if (code == 11) CCLog("슬롯구매 : 슬롯 개수 MAX");
        else CCLog("슬롯구매 : (code = %d) 기타 에러", code);
        
        //ReplaceScene("NoImage", BUY_SKILLSLOT_FAIL, BTN_1);
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
        // 돈 정보 갱신
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
        
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
        myInfo->SortMySkillByCommonId(); // common-skill-id 오름차순 정렬

        // OK 창으로 넘어가자. 
        ReplaceScene("NoImage", BUY_PROPERTY_OK, BTN_1);
    }
    else
    {
        if (code == 3) {
            CCLog("스킬속성구매 : 금액 부족");
            ReplaceScene("NoImage", NEED_TO_BUY_TOPAZ, BTN_2);
        }
        else if (code == 4) CCLog("스킬속성구매 : 금액이 맞지 않음. 재부팅.");
        else if (code == 10) CCLog("스킬속성구매 : 해당 속성 이미 보유");
        else if (code == 11) CCLog("스킬속성구매 : 마스터 타입은 구매불가");
        else CCLog("스킬속성구매 : (code = %d) 기타 에러", code);
        
        //ReplaceScene("NoImage", BUY_PROPERTY_FAIL, BTN_1);
    }
}
