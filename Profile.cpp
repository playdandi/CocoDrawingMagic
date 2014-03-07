#include "Profile.h"

CCScene* Profile::scene()
{
    CCScene* pScene = CCScene::create();
    Profile* pLayer = Profile::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void Profile::onEnter()
{
    CCLog("Profile :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Profile::onExit()
{
    CCLog("Profile :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Profile::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool Profile::init()
{
    CCLog("Profile :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
}

void Profile::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    
    spriteClass = new SpriteClass();
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                ccp(0, 0), ccp(49, 640), CCSize(982, 623), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                ccp(0, 0), ccp(76, 678), CCSize(929, 562), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                ccp(0, 0), ccp(810, 1039), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                ccp(0, 0), ccp(900, 1132), CCSize(0, 0), "", "Profile", this, 1) );
    
    CCLayer* profileLayer = CCLayer::create();
    profileLayer->setContentSize(CCSizeMake(862, 166));
    profileLayer->setPosition(ccp(115, 1050));
    this->addChild(profileLayer, 5);
    
    int rank = 1; // test
    if (rank == 1) // 1~3위는 이미지 사용
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_1.png",
                    ccp(0, 0), ccp(0, 65), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else if (rank == 2)
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_2.png",
                    ccp(0, 0), ccp(8, 77), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else if (rank == 3)
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_3.png",
                    ccp(0, 0), ccp(8, 77), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else {
        char num[4];
        sprintf(num, "%d", rank);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(num, fontList[0], 36,
                    ccp(0, 0), ccp(32, 115), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
    }

    // profile info
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png",
                ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("AHRORO", fontList[0], 48,
                ccp(0, 0), ccp(252, 120), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("53,000,000", fontList[0], 60,
                ccp(0, 0), ccp(282, 36), ccc3(255,255,255), "", "Layer", profileLayer, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                ccp(0, 0), ccp(0, 5), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    
    // property (불/물/땅) => 궁극은 나중에 어떻게 넣나?
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png1",
                ccp(0, 0), ccp(99, 940), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png1",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png1"), CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png2",
                ccp(0, 0), ccp(191, 940), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png2",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png3",
                ccp(0, 0), ccp(280, 940), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png3",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 1) );
    
    // elf board
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                ccp(0, 0), ccp(382, 797), CCSize(309, 236), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png",
                ccp(0, 0), ccp(11, 165), CCSize(0, 0), "background/bg_board_brown.png2", "1", NULL, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png",
                ccp(0, 0), ccp(55, 187), CCSize(0, 0), "background/bg_board_brown.png2", "1", NULL, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1",
                ccp(0, 0), ccp(19, 22), CCSize(274, 53), "background/bg_board_brown.png2", "1", NULL, 5) );
    
    // skill board
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3",
                ccp(0, 0), ccp(699, 797), CCSize(263, 236), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2",
                ccp(0, 0), ccp(19, 22), CCSize(228, 53), "background/bg_board_brown.png3", "1", NULL, 5) );
    
    // degree
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                ccp(0, 0), ccp(114, 701), CCSize(858, 76), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question_mini.png",
                ccp(0, 0), ccp(908, 710), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 48,
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 1) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
}


bool Profile::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    
    CCPoint point = pTouch->getLocation();
    //CCLog("DegreeInfo : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_question_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                Common::ShowNextScene(this, "Profile", "DegreeInfo", false);
        }
    }
    
    return true;
}


void Profile::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void Profile::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void Profile::EndScene()
{
    this->removeFromParentAndCleanup(true);
}
