#include "FairyOneInfo.h"

int common_fairy_id;

CCScene* FairyOneInfo::scene(int cfi)
{
    common_fairy_id = cfi;
    
    CCScene* pScene = CCScene::create();
    FairyOneInfo* pLayer = FairyOneInfo::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void FairyOneInfo::onEnter()
{
    //CCLog("FairyOneInfo : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(FairyOneInfo::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void FairyOneInfo::SceneCallback()
{
    isTouched = false;
}
void FairyOneInfo::onExit()
{
    //CCLog("FairyOneInfo : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void FairyOneInfo::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool FairyOneInfo::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    idx = -1;
    isTouched = true;
    isSomeFairyBought = false;

    // make depth tree
    Depth::AddCurDepth("FairyOneInfo", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("FairyOneInfo : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(FairyOneInfo::Notification), Depth::GetCurName(), NULL);
    
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

void FairyOneInfo::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0 || param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("FairyOneInfo : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("FairyOneInfo : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 요정 구매 성공 후에 돌아올 경우 : 즉시 종료
        isSomeFairyBought = true;
        EndScene();
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
        if (idx > -1)
        {
            if (isAlreadyBought)
            {
                spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
            }
            else
            {
                spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
                ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
            }
        }
    }
}

void FairyOneInfo::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    FairyInfo* fi = FairyInfo::GetObj(abs(common_fairy_id));
    
    // 별사탕 vs 토파즈
    isByTopaz = (common_fairy_id < 0);
    
    // 요정 이미지
    CCLayer* picture;
    switch (fi->GetId())
    {
        case 1: picture = Fairy::MakeFlower(); picture->setScale(0.7f); break;
        case 2: picture = Fairy::MakeSun(); picture->setScale(0.8f); break;
        case 3: picture = Fairy::MakeCloud(true); picture->setScale(0.8f); break;
    }
    picture->setAnchorPoint(ccp(0, 0));
    picture->setPosition(ccp(250, 1100));
    tLayer->addChild(picture, 10);
    spriteClass->layers.push_back(picture);
    

    // name + grade background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(0, 0), ccp(404, 1121), CCSize(440, 90), "", "Layer", tLayer, 5) );
    
    // grade
    char fname[30];
    if (fi->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png");
    else if (fi->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png");
    else if (fi->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png");
    else if (fi->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(424, 1136), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    // name
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fi->GetName(), fontList[0], 52, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5) );
    
    // 기본속성
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2", ccp(0, 0), ccp(404, 1036), CCSize(520, 58), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(424, 1042), ccc3(121,71,0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(424, 1042+3), ccc3(255,219,53), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("보너스 MP", fontList[0], 36, ccp(0, 0), ccp(574, 1042+3), ccc3(0,167,222), "", "Layer", tLayer, 5) );
    
    // 특수능력
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3", ccp(0, 0), ccp(404, 971), CCSize(520, 58), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(424, 977), ccc3(121,71,0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(424, 977+3), ccc3(255,219,53), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(FairyInfo::GetAbilityDesc(fi->GetType(), false), fontList[0], 36, ccp(0, 0), ccp(574, 977+3), ccc3(255,255,255), "", "Layer", tLayer, 5) );
    
    // 요정 묘사
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(fi->GetDescription(), fontList[0], 36, ccp(0, 0), ccp(210, 875), ccc3(117,86,47), CCSize(730, 100), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", tLayer, 5) );
    
    // 도전 문장의 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4", ccp(0, 0), ccp(210, 822), CCSize(730, 58), "", "Layer", tLayer, 5) );
    
    // 이미 구매한 요정인지 체크한다. ( flag = true(구매X), false(구매함) )
    isAlreadyBought = false;
    for (int i = 0 ; i < myInfo->GetFairyList().size() ; i++)
    {
        if (myInfo->GetFairyList()[i]->GetId() == fi->GetId())
        {
            isAlreadyBought = true;
            break;
        }
    }
    
    if (!isAlreadyBought) // 구매하지 않았다면 표시할 것들
    {
        // 도전 내용
        if (isByTopaz)
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png", ccp(0, 0), ccp(141, 822), CCSize(0, 0), "", "Layer", tLayer, 5) );
            
            int requireMP = FairyInfo::GetObj(abs(common_fairy_id))->GetRefVal();
            char name[50];
            sprintf(name, "도전 : MP %d 달성", requireMP);
            
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "Layer", tLayer, 5) );
            CCSize s = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->label->getContentSize();
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 달성시 별사탕으로 구매가능", fontList[2], 30, ccp(0, 0), ccp(230+s.width+10, 835), ccc3(255,255,255), "", "Layer", tLayer, 5, 170) );
        }
        else
        {
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("도전 과제 완료!", fontList[0], 36, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "Layer", tLayer, 5) );
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("(별사탕으로 구매 가능합니다)", fontList[2], 30, ccp(0, 0), ccp(505-5, 835), ccc3(255,255,255), "", "Layer", tLayer, 5, 170) );
        }

        // 가격 표시 배경
        spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 0), ccp(493, 723), CCSize(201, 77), "", "Layer", tLayer, 5) );

        if (isByTopaz) // 토파즈 가격
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_topaz_mini.png", ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "Layer", tLayer, 5) );
            
            sprintf(fname, "%s", Common::MakeComma(fi->GetCostTopaz()).c_str());
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(588, 738), ccc3(255,255,255), "", "Layer", tLayer, 5) );
        }
        else // 별사탕 가격
        {
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy_mini.png", ccp(0, 0), ccp(503, 730), CCSize(0, 0), "", "Layer", tLayer, 5) );
            
            sprintf(fname, "%s", Common::MakeComma(fi->GetCostStarCandy()).c_str());
            spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 36, ccp(0, 0), ccp(588, 738), ccc3(255,255,255), "", "Layer", tLayer, 5) );
        }
    
        // '구매' 버튼
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green_mini.png", ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "Layer", tLayer, 5) );
        CCPoint p = spriteClass->FindParentCenterPos("button/btn_green_mini.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_purchase.png", ccp(0.5, 0), ccp(717+p.x, 711+25), CCSize(0, 0), "", "Layer", tLayer, 5) );
        
        // 버튼 젤리 움직임
        CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_green_mini.png"));
        CCSize t = temp->getContentSize();
        temp->setAnchorPoint(ccp(0.5, 0.5));
        temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
        CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.03f, 0.96f), CCScaleTo::create(1.0f, 0.97f, 1.04f), NULL );
        temp->runAction(CCRepeatForever::create(action));
    }
    else // 구매했다면, 확인 버튼을 넣자.
    {
        // 구매한 경우, 도전 문장에 다른 말을 써 놓자.
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_check.png", ccp(0, 0), ccp(141, 822), CCSize(0, 0), "", "Layer", tLayer, 5) );
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("코코가 요정을 이미 들고 있어요.", fontList[0], 30, ccp(0, 0), ccp(230, 830), ccc3(255,255,255), "", "Layer", tLayer, 5) );
        
        // '확인' 버튼
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717, 711), CCSize(0, 0), "", "Layer", tLayer, 5) );
        CCPoint p = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0), ccp(717+p.x, 711+25), CCSize(0, 0), "", "Layer", tLayer, 5) );
        
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


bool FairyOneInfo::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
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


void FairyOneInfo::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void FairyOneInfo::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        if (isAlreadyBought)
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
        }
        else
        {
            spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
            ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_purchase.png"))->setColor(ccc3(255,255,255));
        }
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_CONFIRM)
        {
            if (isAlreadyBought)
            {
                EndScene();
            }
            else
            {
                FairyInfo* fi = FairyInfo::GetObj(abs(common_fairy_id));
                std::vector<int> data;
                
                int idx;
                for (int i = 0 ; i < fairyInfo.size() ; i++)
                {
                    if (fairyInfo[i]->GetId() == abs(common_fairy_id))
                        idx = i;
                }
                data.push_back(idx); // fairy info 데이터의 idx
                
                if (isByTopaz)
                {
                    data.push_back(fi->GetCostTopaz());
                    Common::ShowPopup(this, "FairyOneInfo", "NoImage", false, BUY_FAIRY_BY_TOPAZ_TRY, BTN_2, data);
                }
                else
                {
                    data.push_back(fi->GetCostStarCandy());
                    Common::ShowPopup(this, "FairyOneInfo", "NoImage", false, BUY_FAIRY_BY_STARCANDY_TRY, BTN_2, data);
                }
            }
        }
    }

    isTouched = false;
}


void FairyOneInfo::EndScene()
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    if (isSomeFairyBought)
    {
        param = CCString::create("3");
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
    
    
    // 보상 관련 notification
    param = CCString::create("11");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
}



