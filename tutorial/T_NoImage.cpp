#include "T_NoImage.h"

static int type;
static int btn;
static std::vector<int> d;
static int fromWhere;

CCScene* T_NoImage::scene(int popupType, int btnType, std::vector<int> data, int etc)
{
    // data
    type = popupType;
    btn = btnType;
    d = data;
    
    fromWhere = etc;
    
    CCScene* pScene = CCScene::create();
    T_NoImage* pLayer = T_NoImage::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void T_NoImage::onEnter()
{
    CCLog("NoImage : onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void T_NoImage::onExit()
{
    CCLog("NoImage : onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void T_NoImage::keyBackClicked()
{
}


bool T_NoImage::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_NoImage", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("NoImage : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(T_NoImage::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    ttrArrow = CCSprite::create("images/tutorial_arrow.png");
    ttrPos = CCSprite::create("images/tutorial_position.png");
    
    // ccp(717+5, 711+offset) (offset = 0)
    ttrArrow->setAnchorPoint(ccp(0.5, 0));
    ttrArrow->setPosition(ccp(722+233/2, 711+115+10));
    CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
    ttrArrow->runAction(CCRepeatForever::create(action));
    this->addChild(ttrArrow, 101);
    
    ttrPos->setAnchorPoint(ccp(0, 0));
    ttrPos->setPosition(ccp(722, 711));
    ttrPos->setScaleX( (float)233 / (float)162 );
    ttrPos->setScaleY( (float)115 / (float)89 );
    this->addChild(ttrPos, 101);
    
    isEnded = false;
    isTouched = false;
    
    return true;
}

void T_NoImage::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("T_NoImage : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        // 젤리 버튼 초기화
        ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"))->setColor(ccc3(255,255,255));
        ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->setColor(ccc3(255,255,255));
    }
}

void T_NoImage::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    spriteClass = new SpriteClass();

    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "T_NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "T_NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "T_NoImage", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "T_NoImage", this, 1) );

    
    
    // text
    char text[250];
    std::string title = "마법 배우기";
    sprintf(text, "축하해요! '%s' 마법을 새로 배웠어요!", SkillInfo::GetSkillInfo(d[0])->GetName().c_str());

    // 내용 문장
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea(text, fontList[0], 40, ccp(0.5, 0.5), ccp(winSize.width/2, winSize.height/2), ccc3(78,47,8), CCSize(829-20, 250), kCCTextAlignmentCenter, kCCVerticalTextAlignmentCenter, "", "T_NoImage", this, 5) );
    
    // 팝업창 타이틀
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png", ccp(0, 1), ccp(126+5, 678+562- 35), CCSize(759-126+52, 90), "", "T_NoImage", this, 1) );
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x+2, pos.y+3-1), ccc3(0,0,0), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(title, fontList[0], 48, ccp(0.5,0.5), ccp(pos.x, pos.y+3), ccc3(242,242,242), "background/bg_degree_desc.png", "1", NULL, 2, 1) );
    
    // 확인 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(717+5, 711), CCSize(0, 0), "", "T_NoImage", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png",ccp(0.5, 0), ccp(spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width/2, 24), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
    
    // 버튼 젤리 움직임
    CCSprite* temp = ((CCSprite*)spriteClass->FindSpriteByName("button/btn_red_mini.png"));
    CCSize t = temp->getContentSize();
    temp->setAnchorPoint(ccp(0.5, 0.5));
    temp->setPosition(ccp(temp->getPosition().x+t.width/2, temp->getPosition().y+t.height/2));
    CCActionInterval* action = CCSequence::create( CCScaleTo::create(1.0f, 1.02f, 0.97f), CCScaleTo::create(1.0f, 0.98f, 1.03f), NULL );
    temp->runAction(CCRepeatForever::create(action));
    ((CCSprite*)spriteClass->FindSpriteByName("letter/letter_confirm_mini.png"))->runAction(CCRepeatForever::create((CCActionInterval*)action->copy()));
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool T_NoImage::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                EndScene();
                return true;
            }
        }
    }
    
    return true;
}


void T_NoImage::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_NoImage::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}


void T_NoImage::EndScene()
{
    isEnded = true;
    isTouched = true;
    
    // release depth tree
    Depth::RemoveCurDepth();
    
    // 튜토리얼 상태 한 번 넘기기
    CCString* param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    ttrArrow->removeFromParentAndCleanup(true);
    ttrPos->removeFromParentAndCleanup(true);
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}
