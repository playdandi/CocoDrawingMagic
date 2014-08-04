#include "DegreeInfo.h"

CCScene* DegreeInfo::scene()
{
    CCScene* pScene = CCScene::create();
    DegreeInfo* pLayer = DegreeInfo::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void DegreeInfo::onEnter()
{
    //CCLog("DegreeInfo :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
    
    // 전체화면 액션
    CCActionInterval* action = CCSequence::create( CCSpawn::create(CCMoveTo::create(0.3f, ccp(0, 0)), CCScaleTo::create(0.3f, 1.0f), NULL), CCCallFunc::create(this, callfunc_selector(DegreeInfo::SceneCallback)), NULL );
    tLayer->runAction(CCEaseExponentialOut::create(action));
}
void DegreeInfo::SceneCallback()
{
    isTouched = false;
}
void DegreeInfo::onExit()
{
    //CCLog("DegreeInfo :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void DegreeInfo::keyBackClicked()
{
    if (isKeybackTouched || isTouched)
        return;
    isKeybackTouched = true;
    
    sound->playClick();
    EndScene();
}


bool DegreeInfo::init()
{
    //CCLog("DegreeInfo :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    isTouched = false;
    
    // make depth tree
    Depth::AddCurDepth("DegreeInfo", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    //CCLog("DegreeInfo : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(DegreeInfo::Notification), Depth::GetCurName(), NULL);
    
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

void DegreeInfo::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        isKeybackTouched = false;
        //CCLog("DegreeInfo : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        //CCLog("DegreeInfo : 터치 비활성");
        isTouched = true;
        isKeybackTouched = true;
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
        isKeybackTouched = false;
    }
}

void DegreeInfo::InitSprites()
{
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "Layer", tLayer, 1) );
    
    // 학사,석사,박사
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("학사 (Bachelor of Magician)", fontList[0], 40, ccp(0, 0), ccp(130, 1140), ccc3(78,47,8), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 1주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 1090), ccc3(117,86,47), "", "Layer", tLayer, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("석사 (Master of Magician)", fontList[0], 40, ccp(0, 0), ccp(130, 990), ccc3(78,47,8), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 4주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 940), ccc3(117,86,47), "", "Layer", tLayer, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("박사 (Doctor of Magician)", fontList[0], 48, ccp(0, 0), ccp(130, 830), ccc3(78,47,8), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 8주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 780), ccc3(117,86,47), "", "Layer", tLayer, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("궁극의 마법사", fontList[0],
          36, ccp(0, 0), ccp(307, 740), ccc3(117,86,47), "", "Layer", tLayer, 1) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                    ccp(0, 0), ccp(110, 1060), CCSize(0, 0), "", "Layer", tLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                    ccp(0, 0), ccp(110, 900), CCSize(0, 0), "", "Layer", tLayer, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool DegreeInfo::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
                EndScene();
                break;
            }
        }
    }
    
    return true;
}


void DegreeInfo::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void DegreeInfo::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void DegreeInfo::EndScene()
{
    sound->playClick();
    
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    tLayer->removeAllChildren();
    tLayer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}


