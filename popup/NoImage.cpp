#include "NoImage.h"
#include "BuyPotion.h"
#include "../pugixml/pugixml.hpp"

using namespace pugi;

static int type;
static int btn;
static std::vector<int> d;

CCScene* NoImage::scene(int popupType, int btnType, std::vector<int> data)
{
    // data
    type = popupType;
    btn = btnType;
    d = data;
    
    CCScene* pScene = CCScene::create();
    NoImage* pLayer = NoImage::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void NoImage::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void NoImage::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void NoImage::keyBackClicked()
{
    sound->playClick();
    EndScene();
}


bool NoImage::init()
{
    CCLog("NoImage :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    if (type == POPUP_STARCANDY_0)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (type == BUYPOTION_1)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
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
    char text[150], cost[10];
    switch (type)
    {
        case POPUP_STARCANDY_0:
            sprintf(text, "토파즈를 사용하여 별사탕 %d개를 구매하시겠습니까?", d[0]); break;
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
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 52, ccp(0.5, 0.5), ccp(49+982/2, 640+623/2+50), ccc3(78,47,8), CCSize(982-200, 300), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "NoImage", this, 5) );
    
    // 가격표
    if (type == POPUP_STARCANDY_0)
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                    ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "NoImage", this, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png",
                    ccp(0, 0), ccp(513, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png",
        //            ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "NoImage", this, 5) );
        sprintf(cost, "x %d", 13);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(cost, fontList[0], 36, ccp(0, 0), ccp(588, 743), ccc3(255,255,255), "", "NoImage", this, 5) );
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
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png" ||
            (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png" &&
             (type == BUYPOTION_OK || type == POTION_SEND_OK || type == POTION_SEND_REJECT || type == POTION_SEND_NO_FRIEND || type == POTION_SEND_EARLY)) )
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                /*
                if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png" && type == BUYPOTION_OK)
                {
                    // 이전 창까지 삭제.
                    ((BuyPotion*)(this->getParent()))->EndScene();
                }
                 */
                EndScene();
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                if (type == POPUP_EXIT)
                {
                    CCDirector::sharedDirector()->end();
                    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                    exit(0);
                    #endif
                }
                else if (type == POPUP_STARCANDY_0)
                {
                    ReplaceScene("NoImage", POPUP_STARCANDY_1, BTN_2);
                }
                else if (type == BUYPOTION_1)
                {
                    if (myInfo->GetTopaz() < 5) // 토파즈 개수 < 5 라면 네트워크 시도 X.
                        ReplaceScene("NoImage", BUYPOTION_FAIL, BTN_2);
                    else
                    {
                        CCLog("to server : purchase_potion");
                        // 포션 구매 프로토콜을 요청한다.
                        char temp[255];
                        std::string url = "http://14.63.225.203/cogma/game/purchase_potion.php?";
                        sprintf(temp, "kakao_id=%d", myInfo->GetKakaoId());
                        url += temp;
                        
                        // post request
                        CCHttpRequest* req = new CCHttpRequest();
                        req->setUrl(url.c_str());
                        req->setRequestType(CCHttpRequest::kHttpPost);
                        req->setResponseCallback(this, httpresponse_selector(NoImage::onHttpRequestCompleted));
                        CCHttpClient::getInstance()->send(req);
                        req->release();
                    }
                }
                else if (type == BUYPOTION_FAIL)
                {
                    CCLog("BUYPOTION_FAIL : click '확인'");
                    // 토파즈를 구매하시곘습니까 : 예 -> 토파즈 구매 창으로 이동.
                    // (코드 상에서는 BuyPotion에서 BuyTopaz로 이동한 것처럼 구현한다)
                    
                    CCNode* parent = this->getParent();
                    EndScene();
                    Common::ShowNextScene(parent, "BuyPotion", "BuyTopaz", false, 2);
                    //Common::ShowNextScene(this, "NoImage", "BuyTopaz", true);
                    break;
                }
                else if (type == POTION_SEND_TRY)
                {
                    CCLog("to server : send_potion");
                    // 포션 구매 프로토콜을 요청한다.
                    char temp[255];
                    std::string url = "http://14.63.225.203/cogma/game/send_potion.php?";
                    sprintf(temp, "kakao_id=%d&", myInfo->GetKakaoId());
                    url += temp;
                    sprintf(temp, "friend_kakao_id=%d", friendList[d[0]]->GetKakaoId());
                    url += temp;
                    
                    // post request
                    CCHttpRequest* req = new CCHttpRequest();
                    req->setUrl(url.c_str());
                    req->setRequestType(CCHttpRequest::kHttpPost);
                    req->setResponseCallback(this, httpresponse_selector(NoImage::onHttpRequestCompleted));
                    CCHttpClient::getInstance()->send(req);
                    req->release();
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
    Common::ShowPopup(this, "NoImage", to, true, type, btnType, d);
    this->removeFromParentAndCleanup(true);
}

void NoImage::EndScene()
{
    CCString* param = CCString::create("0");
    if (type == POPUP_EXIT)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    else if (type == POPUP_STARCANDY_0 || type == POPUP_STARCANDY_1 || type == POPUP_STARCANDY_2)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyStarCandy", param);
    else if (type == BUYPOTION_1 || type == BUYPOTION_OK || type == BUYPOTION_FAIL)
        CCNotificationCenter::sharedNotificationCenter()->postNotification("BuyPotion", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void NoImage::EndSceneCallback()
{
}


/////////////////////////////////////////////////////////////////////////////////////

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
        case BUYPOTION_1:
            XmlParseBuyPotion(dumpData, buffer->size()); break;
        case POTION_SEND_TRY:
            XmlParseSendPotion(dumpData, buffer->size()); break;
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


