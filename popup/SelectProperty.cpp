#include "SelectProperty.h"

static int isInGameTutorialStart;

CCScene* SelectProperty::scene(int isGameStart)
{
    isInGameTutorialStart = isGameStart; // 속성을 선택하고 나면 바로 인게임튜토리얼을 시작하는지에 대한 flag
    
    CCScene* pScene = CCScene::create();
    SelectProperty* pLayer = SelectProperty::create();
    pScene->addChild(pLayer);
	return pScene;
}

void SelectProperty::onEnter()
{
    CCLog("SelectProperty :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void SelectProperty::onExit()
{
    CCLog("SelectProperty :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void SelectProperty::keyBackClicked()
{
    sound->playClick();
    EndScene(false);
}


bool SelectProperty::init()
{
	if (!CCLayer::init())
		return false;

    
    // make depth tree
    Depth::AddCurDepth("SelectProperty", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("SelectProperty : touch prio = %d", Depth::GetCurPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SelectProperty::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    return true;
}

void SelectProperty::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("SelectProperty : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("SelectProperty : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 2)
    {
        // 속성 하나를 성공적으로 배웠을 때 : 스케치북 튜토리얼 시작
        EndScene(false);
        CCLog("현재 : %s", Depth::GetCurNameString().c_str());
        int from;
        if (Depth::GetCurNameString() == "Ranking")
            from = 0;
        else
            from = 1;
        Common::ShowNextScene(Depth::GetCurPointer(), Depth::GetCurNameString(), "T_Sketchbook", false, from);
    }
    else if (param->intValue() == 3)
    {
        // 속성 하나를 성공적으로 배웠을 떄 : 인게임 튜토리얼 시작
        EndScene(true);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void SelectProperty::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();
    
    // 기본 팝업창 틀
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(982, 970), "", "SelectProperty", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), CCSize(929, 920), "", "SelectProperty", this, 0) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, winSize.height/2+970/2-225), CCSize(0, 0), "", "SelectProperty", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, winSize.height/2+970/2-131), CCSize(0, 0), "", "SelectProperty", this, 1) );
   
    // 타이틀
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(126+5, winSize.height/2+970/2-50), CCSize(759-126+52, 90), "", "SelectProperty", this, 1) );
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("속성 선택하기", fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("속성 선택하기", fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    // dotted-lines
    int base_h = winSize.height/2-970/2 + 200;
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0.5, 0.5), ccp(winSize.width/2, base_h), CCSize(0, 0), "", "SelectProperty", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0.5, 0.5), ccp(winSize.width/2, base_h+200), CCSize(0, 0), "", "SelectProperty", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0.5, 0.5), ccp(winSize.width/2, base_h+400), CCSize(0, 0), "", "SelectProperty", this, 5) );
    
    // 속성 아이콘
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png1", ccp(0.5, 0), ccp(220, base_h+400+30), CCSize(0, 0), "", "SelectProperty", this, 5) );
    CCPoint p = spriteClass->FindParentCenterPos("background/bg_skill_brown.png1");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire.png", ccp(0.5, 0.5), p, CCSize(0, 0), "background/bg_skill_brown.png1", "0", NULL, 5, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png2", ccp(0.5, 0), ccp(220, base_h+200+30), CCSize(0, 0), "", "SelectProperty", this, 5) );
    p = spriteClass->FindParentCenterPos("background/bg_skill_brown.png2");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water.png", ccp(0.5, 0.5), ccp(p.x-3, p.y), CCSize(0, 0), "background/bg_skill_brown.png2", "0", NULL, 5, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png3", ccp(0.5, 0), ccp(220, base_h+30), CCSize(0, 0), "", "SelectProperty", this, 5) );
    p = spriteClass->FindParentCenterPos("background/bg_skill_brown.png3");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land.png", ccp(0.5, 0.5), p, CCSize(0, 0), "background/bg_skill_brown.png3", "0", NULL, 5, 1) );
    
    
    // 선택 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue_mini.png1", ccp(0.5, 0.5), ccp(835, base_h+400+100), CCSize(0, 0), "", "SelectProperty", this, 5) );
    p = spriteClass->FindParentCenterPos("button/btn_blue_mini.png1");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_select.png1", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_blue_mini.png1", "0", NULL, 5, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue_mini.png2", ccp(0.5, 0.5), ccp(835, base_h+200+100), CCSize(0, 0), "", "SelectProperty", this, 5) );
    p = spriteClass->FindParentCenterPos("button/btn_blue_mini.png2");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_select.png2", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_blue_mini.png2", "0", NULL, 5, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_blue_mini.png3", ccp(0.5, 0.5), ccp(835, base_h+100), CCSize(0, 0), "", "SelectProperty", this, 5) );
    p = spriteClass->FindParentCenterPos("button/btn_blue_mini.png3");
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_select.png3", ccp(0.5, 0.5), ccp(p.x, p.y+3), CCSize(0, 0), "button/btn_blue_mini.png3", "0", NULL, 5, 1) );
    
    
    // 각 속성 설명 문구
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("타오르는 힘으로,\n강력한 마법과\n붉은용을 소환해요!", fontList[0], 32, ccp(0, 0.5), ccp(320, base_h+400+100), ccc3(78,47,8), CCSize(829, 200), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SelectProperty", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("생성하는 힘으로,\n시간을 얼리고\n여신의 은총을 받아요!", fontList[0], 32, ccp(0, 0.5), ccp(320, base_h+200+100), ccc3(78,47,8), CCSize(829, 200), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SelectProperty", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("자연의 힘으로,\n바람을 일으키고\n고대나무를 찾아요!", fontList[0], 32, ccp(0, 0.5), ccp(320, base_h+100), ccc3(78,47,8), CCSize(829, 200), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SelectProperty", this, 5) );
    
    
    // 마지막 설명 문구
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("* 한번 선택하면 바꿀 수 없습니다.\n(한 속성을 모두 배우면 다른 속성을 배울 수 있어요)", fontList[0], 32, ccp(0.5, 1), ccp(winSize.width/2+30, base_h-30), ccc3(78,47,8), CCSize(929-100, 100), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "SelectProperty", this, 5) );
}




bool SelectProperty::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene(false);
                break;
            }
        }
        else if (spriteClass->spriteObj[i]->name.substr(0, 24) == "button/btn_blue_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                int num = atoi(spriteClass->spriteObj[i]->name.substr(24).c_str());
                std::vector<int> data;
                data.push_back(num);
                data.push_back(isInGameTutorialStart); // 인게임 튜토리얼 시작 여부 (1 = 시작)
                
                Common::ShowPopup(this, "SelectProperty", "NoImage", false, SELECT_PROPERTY_TRY, BTN_2, data);
                break;
            }
        }
    }
    
    return true;
}


void SelectProperty::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void SelectProperty::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void SelectProperty::EndScene(bool isInGameTtrStart)
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
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
    
    // 인게임 튜토리얼 시작하는 경우
    if (isInGameTtrStart)
    {
        CCString* param = CCString::create("7"); // 부모는 GameReady
        CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    }
}



