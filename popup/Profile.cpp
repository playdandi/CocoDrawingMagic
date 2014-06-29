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
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void Profile::onExit()
{
    CCLog("Profile :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void Profile::keyBackClicked()
{
    EndScene();
}


bool Profile::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("Profile", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("Profile : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(Profile::Notification), Depth::GetCurName(), NULL);
    
    // notification post
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    InitSprites();
    InitFairy();
    InitSkill();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);

    return true;
}

void Profile::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
    
    if (param->intValue() <= 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        CCLog("Profile : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("Profile : 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
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
    
    CCLayer* profileLayer = CCLayer::create();
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
    /*
    if (friendList[idx]->GetImageUrl() != "")
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[idx]->GetProfile(), ccp(0    , 0), ccp(102+5, 36+10), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255, 0.85f) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png", ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, friendList[idx]->GetProfile(), ccp(0    , 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5, 0, 255, 1.0f) );
    }
    */
    CCSprite* profile = ProfileSprite::GetProfile(friendList[idx]->GetImageUrl());
    if (friendList[idx]->GetImageUrl() != "")
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(102+5, 36+11), CCSize(0,0), "", "Layer", profileLayer, 5, 0, 255, 0.85f) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png", ccp(0, 0), ccp(102, 36), CCSize(0, 0), "", "Layer", profileLayer, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateFromSprite(0, profile, ccp(0, 0), ccp(102, 36), CCSize(0,0), "", "Layer", profileLayer, 5) );
    }

    // nickname
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(friendList[idx]->GetNickname(), fontList[0], 48, ccp(0, 0), ccp(252, 120), ccc3(78,47,8), "", "Layer", profileLayer, 5) );
    
    // 최고점수
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("최고점수", fontList[0], 36,
                    ccp(0, 0), ccp(282, 70), ccc3(117,86,47), "", "Layer", profileLayer, 5) );
    
    char score[12];
    sprintf(score, "%d", friendList[idx]->GetHighScore());
    CCLayer* scoreLayer = Common::MakeImageNumberLayer(score, 0);
    scoreLayer->setPosition(ccp(282, 36-25));
    profileLayer->addChild(scoreLayer, 5);
    
    spriteClass->layers.push_back(scoreLayer);
    spriteClass->layers.push_back(profileLayer);
    
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
        pos.x += 92;
        //pos = ccp(191, 940);
    }
    if (friendList[idx]->IsWater())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png2",
                ccp(0, 0), pos, CCSize(0, 0), "", "Profile", this, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png2",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 1, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png2"), CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 1, 1) );
        //pos = ccp(280, 940);
        pos.x += 92;
    }
    if (friendList[idx]->IsLand())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png3",
                ccp(0, 0), pos, CCSize(0, 0), "", "Profile", this, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png3",
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 1, 1) );
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png", ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_yellow_mini.png3"), CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 1, 1) );
        pos.x += 92;
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
    CCLayer* fairyLayer = CCLayer::create();
    fairyLayer->setAnchorPoint(ccp(0, 0));
    fairyLayer->setPosition(ccp(382, 797-25));
    this->addChild(fairyLayer, 10);
    
    // 배경보드
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2", ccp(0, 0), ccp(0, 0), CCSize(309, 236), "", "Layer", fairyLayer, 5) );
    
    int fid = friendList[profile_index]->GetFairyId();
    int flv = friendList[profile_index]->GetFairyLv();
    CCLog("%d %d", fid, flv);
    FairyInfo* f = FairyInfo::GetObj(fid);
    
    // 요정 그림
    CCLayer* picture = Fairy::GetFairy(fid);
    picture->setAnchorPoint(ccp(0, 0));
    if (fid == 0)
    {
        picture->setPosition(ccp(309/2, 236/2+23));
        picture->setScale(0.9f);
    }
    else if (fid == 1)
    {
        picture->setPosition(ccp(309/2+10, 236/2+23));
        picture->setScale(0.63f);
    }
    else if (fid == 2)
    {
        picture->setPosition(ccp(309/2, 236/2+15));
        picture->setScale(0.7f);
    }
    fairyLayer->addChild(picture, 6);
    
    spriteClass->layers.push_back(picture);
    spriteClass->layers.push_back(fairyLayer);
    
    // 요정 등급
    char fname[30];
    if (fid > 0)
    {
        if (f->GetGrade() == 1) sprintf(fname, "letter/letter_grade_a.png");
        else if (f->GetGrade() == 2) sprintf(fname, "letter/letter_grade_b.png");
        else if (f->GetGrade() == 3) sprintf(fname, "letter/letter_grade_c.png");
        else if (f->GetGrade() == 4) sprintf(fname, "letter/letter_grade_d.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(11, 165), CCSize(0, 0), "", "Layer", fairyLayer, 5) );
    }
    
    // 요정 레벨 (+그 배경)
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_petlevel.png", ccp(0, 0), ccp(55, 187), CCSize(0, 0), "", "Layer", fairyLayer, 7) );
    sprintf(fname, "%d Lv", flv);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 24, ccp(0.5, 0.5), ccp(97, 202), ccc3(255,255,255), "", "Layer", fairyLayer, 7) );
    
    if (fid > 0)
    {
        // 요정 이름
        if (fid == 0) sprintf(fname, "요정 없음");
        else sprintf(fname, "%s", f->GetName().c_str());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[0], 30, ccp(0, 0), ccp(13, 85), ccc3(255,255,255), "", "Layer", fairyLayer, 7) );
    }
    
    // 요정 특수능력 (+그 배경)
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(19, 22), CCSize(274, 53), "", "Layer", fairyLayer, 7) );

    if (fid == 0) sprintf(fname, "요정 없음");
    else sprintf(fname, "%s", FairyInfo::GetAbilityDesc(f->GetType()).c_str());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2), ccc3(121,71,0), "", "Layer", fairyLayer, 7) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(fname, fontList[2], 30, ccp(0.5, 0.5), ccp(19+274/2, 22+53/2+3), ccc3(255,219,53), "", "Layer", fairyLayer, 7) );
}

void Profile::InitSkill()
{
    int idx = profile_index;
    
    // skill board
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3",
                ccp(0, 0), ccp(699, 797-25), CCSize(263, 236), "", "Profile", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png2",
                ccp(0, 0), ccp(19, 22), CCSize(228, 53), "background/bg_board_brown.png3", "1", NULL, 5, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(760, 850), CCSize(0, 0), "", "Profile", this, 5) );
    
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_gameready_name.png2");
    
    // 배운 스킬이 없으면 아래를 넣자.
    if (friendList[idx]->GetSkillId() == 0)
    {
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("스킬 없음", fontList[2], 30, ccp(0.5, 0.5), ccp(pos.x, pos.y+2), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 2) );
    }
    else
    {
        char skillName[20];
        
        //sprintf(skillName, "icon/icon_skill_division_red.png");
        // 스킬 옆에 삼각형 아이콘 색깔 (속성별로)
        if (friendList[idx]->GetSkillId() / 10 == 2) // fire
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_red.png", ccp(0, 0), ccp(60, 160), CCSize(0, 0), "background/bg_board_brown.png3", "1", NULL, 5, 1) );
        else if (friendList[idx]->GetSkillId() / 10 == 1) // water
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_blue.png", ccp(0, 0), ccp(60, 160), CCSize(0, 0), "background/bg_board_brown.png3", "1", NULL, 5, 1) );
        else if (friendList[idx]->GetSkillId() / 10 == 3) // land
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_green.png", ccp(0, 0), ccp(60, 160), CCSize(0, 0), "background/bg_board_brown.png3", "1", NULL, 5, 1) );
        else // master
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_skill_division_purple.png", ccp(0, 0), ccp(60, 160), CCSize(0, 0), "background/bg_board_brown.png3", "1", NULL, 5, 1) );

        sprintf(skillName, "skill_%d.png", friendList[idx]->GetSkillId());
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0.5, 0.5), spriteClass->FindParentCenterPos("background/bg_skill_brown.png"), CCSize(0, 0), "background/bg_skill_brown.png", "0", NULL, 5, 1) );
        
        CCLog("스킬이름 : %s", DataProcess::FindSkillNameById(friendList[idx]->GetSkillId()).c_str());
        spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(DataProcess::FindSkillNameById(friendList[idx]->GetSkillId()), fontList[2], 30, ccp(0.5, 0.5), ccp(pos.x, pos.y+2), ccc3(255,255,255), "background/bg_gameready_name.png2", "1", NULL, 5, 2) );
        
        // Lv. <- 이 그림
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(797, 850), CCSize(0, 0), "", "Profile", this, 5) );
        // 레벨 숫자 이미지
        CCSize size = ((CCSprite*)spriteClass->FindSpriteByName("number/level_lv.png"))->getContentSize();
        sprintf(skillName, "number/level_%d.png", friendList[idx]->GetSkillLv() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, skillName, ccp(0, 0), ccp(797+size.width, 850+3), CCSize(0, 0), "", "Profile", this, 5) );
    }
}


bool Profile::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
        else if (spriteClass->spriteObj[i]->name == "button/btn_question_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                Common::ShowNextScene(this, "Profile", "DegreeInfo", false);
                break;
            }
        }
    }
    
    return true;
}


void Profile::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void Profile::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void Profile::EndScene()
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
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);

    this->removeFromParentAndCleanup(true);
}


