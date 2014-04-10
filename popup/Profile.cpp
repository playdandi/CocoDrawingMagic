#include "Profile.h"

static int profile_index;

Profile::~Profile(void)
{
    //CCLog("Profile destructor");
}

CCScene* Profile::scene(int idx)
{
    profile_index = idx;
    
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
    EndScene();
}


bool Profile::init()
{
    CCLog("Profile :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Profile::Notification), "Profile", NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    // make sprites
    spriteClass = new SpriteClass();
    InitSprites();
    InitFairy();
    InitSkill();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    CCLog("========================================");
    for (int i = 0 ; i < spriteClass->spriteObj.size(); i++)
    {
        if (spriteClass->spriteObj[i]->type == 0)
            CCLog("cur retain (0) : %d", spriteClass->spriteObj[i]->sprite->retainCount());
        else if (spriteClass->spriteObj[i]->type == 1)
            CCLog("cur retain (1) : %d", spriteClass->spriteObj[i]->sprite9->retainCount());
        else
            CCLog("cur retain (2) : %d", spriteClass->spriteObj[i]->label->retainCount());
    }
    CCLog("========================================");
    
    return true;
}

void Profile::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() == 0)
    {
        // 터치 활성
        this->setKeypadEnabled(true);
        this->setTouchEnabled(true);
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
        isTouched = false;
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
    }
}


void Profile::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
                ccp(0, 0), ccp(49, 640-25), CCSize(982, 623+50), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png",
                ccp(0, 0), ccp(76, 678-25), CCSize(929, 562+50), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png",
                ccp(0, 0), ccp(809, 1039+25), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                ccp(0, 0), ccp(900, 1132+25), CCSize(0, 0), "", "Profile", this, 1) );
    
    profileLayer = CCLayer::create();
    profileLayer->setContentSize(CCSizeMake(862, 166));
    profileLayer->setPosition(ccp(115, 1050));
    this->addChild(profileLayer, 5);
    
    // rank
    if (profile_index+1 == 1) // 1~3위는 이미지 사용
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_1.png",
                    ccp(0, 0), ccp(0, 65), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else if (profile_index+1 == 2)
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_2.png",
                    ccp(0, 0), ccp(8, 77), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else if (profile_index+1 == 3)
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_rank_number_3.png",
                    ccp(0, 0), ccp(8, 77), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    else {
        char num[4];
        sprintf(num, "%d", profile_index+1);
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(num, fontList[0], 40,
                    ccp(0, 0), ccp(32, 115), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
    }

    int idx = profile_index;
    
    // profile
    if (friendList[idx]->GetImageUrl() != "")
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[idx]->GetProfile(), ccp(0    , 0), ccp(102+5, 36+10), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255, 0.85f) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png", ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[idx]->GetProfile(), ccp(0    , 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255, 1.0f) );
    }

    // nickname
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[idx]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(252, 120), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
    
    // 최고점수
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("최고점수", fontList[0], 36,
                    ccp(0, 0), ccp(282, 70), ccc3(117,86,47), "", "Layer", profileLayer, 5) );
    //255,219,53
    
    char score[12];
    sprintf(score, "%d", friendList[idx]->GetHighScore());
    scoreLayer = Common::MakeImageNumberLayer(score, 0);
    scoreLayer->setPosition(ccp(282, 36-25));
    profileLayer->addChild(scoreLayer, 5);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png",
                ccp(0, 0), ccp(0, 5-20), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    
    
    // property (불/물/땅) => 궁극은 나중에 어떻게 넣나?
    CCPoint pos = ccp(99, 940);
    if (friendList[idx]->IsFire())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png1",
                ccp(0, 0), pos, CCSize(0, 0), "", "Profile", this, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png1",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png1"), CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 1, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png1"), CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 1, 1) );
        pos = ccp(191, 940);
    }
    if (friendList[idx]->IsWater())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png2",
                ccp(0, 0), pos, CCSize(0, 0), "", "Profile", this, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png2",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 1, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 1, 1) );
        pos = ccp(280, 940);
    }
    if (friendList[idx]->IsLand())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png3",
                ccp(0, 0), pos, CCSize(0, 0), "", "Profile", this, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png3",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 1, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 1, 1) );
    }
    
    // degree
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                ccp(0, 0), ccp(114, 701-25), CCSize(858, 76), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question_mini.png",
                ccp(0, 0), ccp(908, 710-25), CCSize(0, 0), "", "Profile", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 48,
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 1, 1) );
}

void Profile::InitFairy()
{
    // elf board
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
                ccp(0, 0), ccp(382, 797-25), CCSize(309, 236), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_grade_a.png",
                ccp(0, 0), ccp(11, 165), CCSize(0, 0), "background/bg_board_brown.png2", "1", NULL, 5, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png",
                ccp(0, 0), ccp(55, 187), CCSize(0, 0), "background/bg_board_brown.png2", "1", NULL, 5, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1",
                ccp(0, 0), ccp(19, 22), CCSize(274, 53), "background/bg_board_brown.png2", "1", NULL, 5, 1) );
}

void Profile::InitSkill()
{
    // skill board
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3",
                ccp(0, 0), ccp(699, 797-25), CCSize(263, 236), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2",
                ccp(0, 0), ccp(19, 22), CCSize(228, 53), "background/bg_board_brown.png3", "1", NULL, 5, 1) );
    
    // 배운 스킬이 없으면 아래를 넣자.
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png",
                ccp(0, 0), ccp(760, 850), CCSize(0, 0), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("스킬 없음", fontList[2], 30, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_gameready_name.png2"), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 2) );
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
            {
                CCLog("========================================");
                for (int i = 0 ; i < spriteClass->spriteObj.size(); i++)
                {
                    if (spriteClass->spriteObj[i]->type == 0)
                        CCLog("cur retain (0) : %d", spriteClass->spriteObj[i]->sprite->retainCount());
                    else if (spriteClass->spriteObj[i]->type == 1)
                        CCLog("cur retain (1) : %d", spriteClass->spriteObj[i]->sprite9->retainCount());
                    else
                        CCLog("cur retain (2) : %d", spriteClass->spriteObj[i]->label->retainCount());
                }
                CCLog("========================================");
                
                
                EndScene();
                return false;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_question_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Profile", "DegreeInfo", false);
            }
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
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "Profile");
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    // score layer
    if (friendList[profile_index]->GetHighScore() == -1)
    {
        //scoreLayer->getChildByTag(-1)->autorelease();
        //scoreLayer->getChildByTag(-1)->removeFromParentAndCleanup(true);
        //CCLog("scorelayer(-1) : %d", scoreLayer->getChildByTag(-1)->retainCount());
    }
    else
    {
        /*
        int total = scoreLayer->getChildrenCount();
        for (int i = 0 ; i < total ; i++)
        {
            //scoreLayer->getChildByTag(i)->autorelease();
            scoreLayer->getChildByTag(i)->removeFromParentAndCleanup(true);
            //CCLog("scorelayer(%d) : %d", i, scoreLayer->getChildByTag(i)->retainCount());
        }
        */
        //scoreLayer->removeAllChildrenWithCleanup(true);
    }
    //scoreLayer->autorelease();
    //scoreLayer->removeFromParentAndCleanup(true);
    
    // other layers
    //profileLayer->autorelease();
    //profileLayer->removeFromParentAndCleanup(true);
    //pBlack->removeFromParentAndCleanup(true);
    //pBlack->autorelease();
    
    //CCLog("pblack count : %d", pBlack->retainCount());
    //CCLog("scoreLayer count : %d", scoreLayer->retainCount());
    //CCLog("profileLayer count : %d", profileLayer->retainCount());
    
    // finally remove all children in this scene.
    //this->removeAllChildrenWithCleanup(true);
    //this->removeAllComponents();

    // end scene
    this->removeFromParentAndCleanup(true);
    
    // 텍스쳐랑 spriteCache에서 둘 다 없애야 한다.
    //CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName("background/bg_board_yellow.png");
    //CCTextureCache::sharedTextureCache()->removeTextureForKey("background/bg_board_yellow.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
}

void Profile::EndSceneCallback()
{
}
