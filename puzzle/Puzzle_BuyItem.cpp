#include "Puzzle_BuyItem.h"

static int ingame_item_type;
static int ofs;

CCScene* Puzzle_BuyItem::scene(int id)
{
    ingame_item_type = INGAME_ITEM_3PAINT;
    ofs = 100;
    
    CCScene* pScene = CCScene::create();
    Puzzle_BuyItem* pLayer = Puzzle_BuyItem::create();
    pScene->addChild(pLayer);
	return pScene;
}

void Puzzle_BuyItem::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(Puzzle_BuyItem::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void Puzzle_BuyItem::SceneCallback()
{
    isTouched = false;
}
void Puzzle_BuyItem::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Puzzle_BuyItem::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene(false);
}


bool Puzzle_BuyItem::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    
    // make depth tree
    Depth::AddCurDepth("Puzzle_BuyItem", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("SketchDetail : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Puzzle_BuyItem::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    tLayer = CCLayer::create();
    tLayer->setAnchorPoint(ccp(0, 0));
    tLayer->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(tLayer, 1);
    tLayer->setScale(0);
    
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    return true;
}

void Puzzle_BuyItem::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("SketchDetail : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("SketchDetail : 터치 비활성");
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
            /*
            if (btnStatus == 1)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_buildup_red.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 2)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 3)
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_get.png"))->setColor(ccc3(255,255,255));
            else if (btnStatus == 4)
            {
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"))->setColor(ccc3(255,255,255));
            }
            */
        }
    }
}


void Puzzle_BuyItem::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 615-ofs), CCSize(982, 673+ofs*2), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 653-ofs), CCSize(929, 612+ofs*2), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809-1, 1064+ofs), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1157+ofs), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    // 타이틀
    std::string title = "강력아이템 : 세줄 색깔붓";
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(131, 1228+ofs), CCSize(759-126+52, 90), "", "Layer", tLayer, 2) );
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(105, 1110+ofs), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // '닫기' 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_system.png", ccp(0, 0), ccp(126, 673-ofs), CCSize(0, 0), "", "Layer", tLayer, 2) );
    pos = spriteClass->FindParentCenterPos("button/btn_system.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_close.png", ccp(0.5, 0.5), ccp(pos.x, pos.y+3), CCSize(0,0), "button/btn_system.png", "0", NULL, 6) );
    
    // '구매' 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green_mini.png", ccp(0, 0), ccp(760, 673-ofs), CCSize(0, 0), "", "Layer", tLayer, 5, 0, 255, ingame_item_type) );
    pos = spriteClass->FindParentCenterPos("button/btn_green_mini.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_purchase.png", ccp(0.5, 0.5), ccp(pos.x, pos.y+3), CCSize(0,0), "button/btn_green_mini.png", "0", NULL, 6) );

    // 버튼 젤리 움직임 ('구매' 버튼)
    CCSprite* temp2 = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"));
    CCSize t = temp2->getContentSize();
    temp2->setAnchorPoint(ccp(0.5, 0.5));
    temp2->setPosition(ccp(temp2->getPosition().x+t.width/2, temp2->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
    temp2->runAction(CCRepeatForever::create(action));
    
    MakeOpenedSkillSprites();
}


void Puzzle_BuyItem::MakeOpenedSkillSprites()
{
    // 내가 원하는 그림
    CCSize imgSize = CCSizeMake(840, 400);
    int height = 689;
    char temp[30];
    sprintf(temp, "images/ingame_item/3paint.png");
    sp = CCSprite::create(temp);
    sp->setPosition(ccp(120+imgSize.width/2, height+imgSize.height/2 +ofs));
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(120-3, height-3 +ofs), CCSize(imgSize.width+6, imgSize.height+6), "", "Layer", tLayer, 3) );
    
    // mask sprite
    mask = CCScale9Sprite::createWithSpriteFrameName("background/bg_degree_desc.png");
    mask->setPosition(120, height +ofs);
    mask->setAnchorPoint(ccp(0, 0));
    mask->setContentSize(CCSize(imgSize.width, imgSize.height));
    
    clip = CCClippingNode::create();
    clip->setInverted(false);
    clip->setAlphaThreshold(0);
    clip->addChild(sp);
    
    node = CCNode::create();
    node->addChild(mask);
    clip->setStencil(node);
    tLayer->addChild(clip, 5);
    
    // 가격표 (스킬을 마스터했을 경우 표시하지 않는다)
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(540, 688-ofs), CCSize(201, 77), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(550, 695-ofs), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // 가격
    sprintf(temp, "x %d", InGameItem::GetObj(1)->GetCostValue()); // 1 : 세줄붓
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(temp, fontList[0], 36, ccp(0, 0), ccp(617, 708-ofs), ccc3(255,255,255), "", "Layer", tLayer, 5) );

    // 스킬 설명
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(SkillDescription(), fontList[0], 34, ccp(0.5, 0), ccp(540, 675), ccc3(78,47,8), CCSize(840, 110), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5) );
}


std::string Puzzle_BuyItem::SkillDescription()
{
    switch (ingame_item_type)
    {
        case INGAME_ITEM_3PAINT:
            return "게임을 시작할 때 강한 색깔붓 2개를 가지고 시작해요.\n세 줄을 같은 색깔로 바꿔줘요!"; break;
    }
    return "";
}



bool Puzzle_BuyItem::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png" ||
            spriteClass->spriteObj[i]->name == "button/btn_system.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene(false);
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_green_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                spriteClass->spriteObj[i]->sprite->setColor(ccc3(170,170,170));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(170,170,170));
                
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_CONFIRM;
                idx = i;
                return true;
            }
        }
    }
    
    return true;
}


void Puzzle_BuyItem::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void Puzzle_BuyItem::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            //EndScene(true);

            // Loading 화면으로 MESSAGE request 넘기기
            Common::ShowNextScene(this, "Puzzle_BuyItem", "Loading", false, LOADING_MESSAGE);
            
            // upgrade skill
            char temp[255];
            std::string param = "";
            sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
            param += temp;
            sprintf(temp, "ingame_item_id=1");
            param += temp;
            
            Network::HttpPost(param, URL_PURCHASE_INGAME_ITEM, this, httpresponse_selector(Puzzle_BuyItem::onHttpRequestCompleted));
        }
    }
    
    isTouched = false;
}

void Puzzle_BuyItem::EndScene(bool use)
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();

    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    sp->removeFromParentAndCleanup(true);
    mask->removeFromParentAndCleanup(true);
    node->removeFromParentAndCleanup(true);
    clip->removeFromParentAndCleanup(true);
    
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
    
    if (use) // 아이템 사용함을 알리기
    {
        CCString* param = CCString::create("101");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    }
    
    // 게임시작 (레디->고)
    CCString* param = CCString::create("100");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
}



void Puzzle_BuyItem::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
    
    XmlParsePurchaseInGameItem(&xmlDoc);
}

void Puzzle_BuyItem::XmlParsePurchaseInGameItem(xml_document *xmlDoc)
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
            Common::ShowPopup(this, "Puzzle_BuyItem", "NoImage", false, NEED_TO_BUY_STARCANDY, BTN_2, nullData);
        else if (code == 10) // 없는 아이템
            Common::ShowPopup(this, "Puzzle_BuyItem", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
        else
            Common::ShowPopup(this, "Puzzle_BuyItem", "NoImage", false, NETWORK_FAIL, BTN_1, nullData);
    }
    else
    {
        // 돈 갱신
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        myInfo->SetMoney(topaz, starcandy);
        
        EndScene(true);
    }
}


