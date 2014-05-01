#include "SketchDetail.h"

static int skill_common_id;

SketchDetail::~SketchDetail(void)
{
    CCLog("SketchDetail destructor");
}

CCScene* SketchDetail::scene(int id)
{
    skill_common_id = id;
    
    CCScene* pScene = CCScene::create();
    SketchDetail* pLayer = SketchDetail::create();
    pScene->addChild(pLayer);
	return pScene;
}

void SketchDetail::onEnter()
{
    CCLog("SketchDetail :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void SketchDetail::onExit()
{
    CCLog("SketchDetail :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void SketchDetail::keyBackClicked()
{
    EndScene();
}


bool SketchDetail::init()
{
    CCLog("SketchDetail :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(SketchDetail::Notification), "SketchDetail", NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    
    // make sprites
    spriteClass = new SpriteClass();
    InitSprites();
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);

    return true;
}

void SketchDetail::Notification(CCObject* obj)
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


void SketchDetail::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // make pop-up background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1", ccp(0, 0), ccp(49, 615), CCSize(982, 623+50), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png", ccp(0, 0), ccp(76, 653), CCSize(929, 562+50), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_popup_rightup.png", ccp(0, 0), ccp(809, 1064), CCSize(0, 0), "", "SketchDetail", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1157), CCSize(0, 0), "", "SketchDetail", this, 1) );
    
    char name[40], name2[40];
    int scid = skill_common_id;
    SkillInfo* sInfo = SkillInfo::GetSkillInfo(scid);
    MySkill* ms = MySkill::GetObj(scid);
    bool isOpened = (ms != NULL);
    
    // 스킬 그림의 배경
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_brown.png", ccp(0, 0), ccp(120, 1090), CCSize(0, 0), "", "SketchDetail", this, 5) );
    
    // 스킬그림 (+ 스킬속성표시, 스킬레벨)
    if (isOpened)
    {
        sprintf(name, "skill/skill_%d.png", scid);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(100+8, 1157+8), CCSize(0, 0), "", "SketchDetail", this, 5) );
        // 스킬속성표시
        if (scid / 10 == 2) sprintf(name, "icon/icon_skill_division_red.png");
        else if (scid / 10 == 1) sprintf(name, "icon/icon_skill_division_blue.png");
        else if (scid / 10 == 3) sprintf(name, "icon/icon_skill_division_green.png");
        else sprintf(name, "icon/icon_skill_division_purple.png");
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(100, 1157+81), CCSize(0, 0), "", "SketchDetail", this, 5) );
        // 스킬레벨
        int offset = 0;
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "number/level_lv.png", ccp(0, 0), ccp(120, 1162), CCSize(0,0), "", "SketchDetail", this, 5) );
        if (ms->GetLevel() >= 10)
        {
            sprintf(name, "number/level_%d.png", ms->GetLevel() / 10);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120+43+3, 1162), CCSize(0,0), "",   "SketchDetail", this, 5) );
            offset = spriteClass->spriteObj[spriteClass->spriteObj.size()-1]->sprite->getContentSize().width;
        }
        sprintf(name, "number/level_%d.png", ms->GetLevel() % 10);
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(120+43+3+offset, 162), CCSize(0,0), "", "SketchDetail", this, 5) );
    }
    else
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_question_skill.png", ccp(0, 0), ccp(165, 1118), CCSize(0, 0), "", "SketchDetail", this, 5) );
    }
    
    // 스킬 이름 배경
    //spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png1", ccp(0, 0), ccp(300, 1177), CCSize(442, 89), "", "SketchDetail", this, 5) );
    // 스킬 이름
    CCPoint pos = spriteClass->FindParentCenterPos("background/bg_gameready_name.png1");
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 60, ccp(0, 0), ccp(290, 1157), ccc3(0,0,0), "", "SketchDetail", this, 5) );
    //spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(sInfo->GetName(), fontList[0], 48, ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), ccc3(0,0,0), "background/bg_gameready_name.png1", "1", NULL, 5, 1) );
    
    // '패시브' 스킬에 대해 '자동효과' 문구 넣기
    if (isOpened && !sInfo->IsActive())
    {
        spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_auto_effect.png", ccp(0, 0), ccp(114, 163), CCSize(0, 0), "", "SketchDetail", this, 5) );
    }
    
    // 연습량 프로그레스바
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_progress_bar.png", ccp(0, 0), ccp(290, 1117), CCSize(412, 31), "", "SketchDetail", this, 5) );
    
    // 현재 경험치 (연습량) + 레벨업을 위한 max경험치 (연습량)
    if (isOpened)
    {
        sprintf(name, "%d", ms->GetExp());
        sprintf(name2, "/%d", SkillBuildUpInfo::GetMaxExp(ms->GetCommonId(), ms->GetLevel()));
    }
    else // '?'인 경우
    {
        sprintf(name, "0");
        sprintf(name2, "/0");
    }
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[2], 30, ccp(0, 0), ccp(300+412+10, 1132), ccc3(255,255,255), "", "SketchDetail", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name2, fontList[2], 30, ccp(0, 0), ccp(300+412+10+33, 1127), ccc3(182,142,142), "", "SketchDetail", this, 5) );
    
    // 문구
    if (isOpened)
    {
        
    }
    else
    {
        
    }
    
    // 버튼
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png", ccp(0, 0), ccp(800, 753), CCSize(0, 0), "", "SketchDetail", this, 5, 0, 255, scid) );
    pos = spriteClass->FindParentCenterPos("button/btn_red_mini.png");
    if (isOpened)
    {
        // 레벨업 해야하는 경우 (강화)
        if (ms->GetExp() == SkillBuildUpInfo::GetMaxExp(scid, ms->GetLevel()))
        {
            btnStatus = 1;
            // '강화' 글자 필요함
            
        }
        // 일반적인 경우
        else
        {
            btnStatus = 2;
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_confirm_mini.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
            
        }
    }
    else
    {
        // '?'스킬의 요구조건을 모두 충족한 경우
        if (myInfo->GetMPTotal() >= sInfo->GetRequiredMP() && myInfo->GetStaffLv() >= sInfo->GetRequiredStaffLv() &&
            MySkill::GetObj(sInfo->GetRequiredSkillId())->GetLevel() >= sInfo->GetRequiredSkillLv())
        {
            btnStatus = 3;
            // '배움' 글자 필요함
        }
        // 아닌 경우
        else
        {
            btnStatus = 4;
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_lock_white.png", ccp(0.5, 0.5), ccp((int)pos.x, (int)pos.y+2), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5, 1) );
        }
    }
    

    
    //spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_dotted_line.png", ccp(0, 0), ccp(0, 5-20), CCSize(0, 0), "", "Layer", SketchDetailLayer, 5) );
}


bool SketchDetail::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
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
                EndScene();
                return false;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_red_mini.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                sound->playClick();
                
                if (btnStatus == 1) // 레벨업 해야하는 경우 (강화)
                {
                    // upgrade skill
                    //http://14.63.225.203/cogma/game/upgrade_skill.php?kakao_id=1000&user_skill_id=1&cost_value=8000
                }
                else if (btnStatus == 2) // 일반적인 경우
                {
                    EndScene();
                    break;
                }
                else if (btnStatus == 3) // '?'스킬의 요구조건을 모두 충족한 경우
                {
                    // purchase skill
                    //http://14.63.225.203/cogma/game/purchase_skill.php?kakao_id=1000&skill_id=22&cost_value=0
                }
            }
        }
    }
    
    return true;
}


void SketchDetail::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void SketchDetail::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    isTouched = false;
}

void SketchDetail::EndScene()
{
    sound->playClick();
    
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Sketchbook", param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, "SketchDetail");
    
    // remove all CCNodes
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    
    this->removeFromParentAndCleanup(true);
}

void SketchDetail::EndSceneCallback()
{
}
