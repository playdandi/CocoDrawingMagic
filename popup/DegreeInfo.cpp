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
    CCLog("MagicSlot :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void DegreeInfo::onExit()
{
    CCLog("MagicSlot :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void DegreeInfo::keyBackClicked()
{
    EndScene();
}


bool DegreeInfo::init()
{
    CCLog("DegreeInfo :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Profile", param);
    
    InitSprites();
    
    return true;
}

void DegreeInfo::InitSprites()
{
    spriteClass = new SpriteClass();
    
    // pop-up 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1039), CCSize(0, 0), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(
        0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "DegreeInfo", this, 1) );
    
    // 학사,석사,박사
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("학사 (Bachelor of Magician)", fontList[0], 40, ccp(0, 0), ccp(130, 1140), ccc3(78,47,8), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 1주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 1090), ccc3(117,86,47), "", "DegreeInfo", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("석사 (Master of Magician)", fontList[0], 40, ccp(0, 0), ccp(130, 990), ccc3(78,47,8), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 4주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 940), ccc3(117,86,47), "", "DegreeInfo", this, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("박사 (Doctor of Magician)", fontList[0], 48, ccp(0, 0), ccp(130, 830), ccc3(78,47,8), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("조건 : 8주 랭킹 1위 (5명 이상 경쟁)", fontList[0], 36, ccp(0, 0), ccp(210, 780), ccc3(117,86,47), "", "DegreeInfo", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("궁극의 마법사", fontList[0],
          36, ccp(0, 0), ccp(307, 740), ccc3(117,86,47), "", "DegreeInfo", this, 1) );
    
    // dotted line
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                    ccp(0, 0), ccp(110, 1060), CCSize(0, 0), "", "DegreeInfo", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                    ccp(0, 0), ccp(110, 900), CCSize(0, 0), "", "DegreeInfo", this, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
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
                EndScene();
        }
    }
    
    return true;
}


void DegreeInfo::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void DegreeInfo::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void DegreeInfo::EndScene()
{
    sound->playClick();
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Profile", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    this->removeFromParentAndCleanup(true);
}

void DegreeInfo::EndSceneCallback()
{
}
