#include "T_MagicList.h"

using namespace pugi;

//static int from;
static int sid[16] = {22, 25, 26, 28, 12, 15, 16, 18, 32, 35, 36, 38, 42, 43, 45, -1};

static int scid;

CCScene* T_MagicList::scene()
{
    CCScene* pScene = CCScene::create();
    T_MagicList* pLayer = T_MagicList::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void T_MagicList::onEnter()
{
    CCLog("T_MagicList :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void T_MagicList::onExit()
{
    CCLog("T_MagicList :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void T_MagicList::keyBackClicked()
{
}

static int offset;

bool T_MagicList::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("T_MagicList", this);
    
    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    CCLog("T_MagicList : touch prio = %d", this->getTouchPriority());
    
    // notification observer
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(T_MagicList::Notification), Depth::GetCurName(), NULL);
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
    
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    // background
    //offset = 0;
    offset = -260;
    //if (from == 1)
    //    offset = -260;
    
    spriteClassSlot = new SpriteClass();
    // scrollview SLOT init.
    scrollViewSlot = CCScrollView::create();
    scrollViewSlot->setDirection(kCCScrollViewDirectionHorizontal);
    scrollViewSlot->setViewSize(CCSizeMake(782-40, 177-20));
    scrollViewSlot->setAnchorPoint(ccp(0, 0));
    scrollViewSlot->setPosition(ccp(77+20, 488+12+offset));
    scrollViewSlot->setDelegate(this);
    scrollViewSlot->setTouchPriority(Depth::GetCurPriority());
    this->addChild(scrollViewSlot, 5);
    
    ttrArrow = CCSprite::create("images/tutorial/tutorial_arrow.png");
    ttrArrow->retain();
    ttrPos = CCScale9Sprite::create("images/tutorial/tutorial_position.png");
    ttrPos->retain();
    
    InitSprites();
    //InitBtn();
    MakeScrollSlot(false);
    
    isTouched = false;
    isScrolling = false;
    isScrollViewTouched = false;
    
    return true;
}

void T_MagicList::Notification(CCObject* obj)
{
    CCString* param = (CCString*)obj;
 
    if (param->intValue() == -1)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollViewSlot->setTouchEnabled(true);
        CCLog("Ranking : 터치 활성 (Priority = %d)", this->getTouchPriority());
    }
    else if (param->intValue() == 0)
    {
        // 터치 활성
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority()+1, true);
        this->setTouchPriority(Depth::GetCurPriority());
        isTouched = false;
        scrollViewSlot->setTouchEnabled(true);
        CCLog("T_MagicList : 터치 활성 (Priority = %d)", this->getTouchPriority());
        
        // 스킬 슬롯 정보 갱신
        spriteClassSlot->RemoveAllObjects();
        MakeScrollSlot(false);
        char name[7];
        sprintf(name, "%d", (int)myInfo->GetSlot().size());
        ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->setString(name);
    }
    else if (param->intValue() == 1)
    {
        // 터치 비활성
        CCLog("T_MagicList 터치 비활성");
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        scrollViewSlot->setTouchEnabled(false);
    }
    else if (param->intValue() == 10)
    {
        // 터치 풀기 (백그라운드에서 돌아올 때)
        isTouched = false;
    }
}


void T_MagicList::InitSprites()
{
    spriteClass = new SpriteClass();
    
    layer = CCLayer::create();
    layer->setPosition(0, winSize.height);
    this->addChild(layer, 5);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png", ccp(0, 0), ccp(49, 688+offset), CCSize(982, 1066), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1", ccp(0, 0), ccp(77, 952+offset), CCSize(929, 672), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2", ccp(0, 0), ccp(77, 726+offset), CCSize(929, 200), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png", ccp(0, 0), ccp(900, 1638+offset), CCSize(0, 0), "", "Layer", layer, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_sketchbookindex.png", ccp(0, 0), ccp(106, 1657+offset), CCSize(0, 0), "", "Layer", layer, 20) );
    
    char name[50], name2[50];
    for (int i = 0 ; i < 4; i++)
    {
        for (int j = 0 ; j < 4; j++)
        //for (int j = 0 ; j < 1 ; j++)
        {
            sprintf(name, "background/bg_skill_yellow.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name, ccp(0, 0), ccp(127+j*229, 1451-i*160+offset), CCSize(0, 0), "", "Layer", layer, 20) );
            sprintf(name2, "background/bg_skill_select.png%d", sid[i*4+j]);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), name, "0", NULL, 21, 1, 0) );
            
            if (i == 0) sprintf(name2, "icon/icon_skill_division_red.png%d", j);
            else if (i == 1) sprintf(name2, "icon/icon_skill_division_blue.png%d", j);
            else if (i == 2) sprintf(name2, "icon/icon_skill_division_green.png%d", j);
            else sprintf(name2, "icon/icon_skill_division_purple.png%d", j);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(127+j*229, 1539-i*160+offset), CCSize(0, 0), "", "Layer", layer, 20) );
            
            // 스킬 문양
            SkillInfo* si = SkillInfo::GetSkillInfo(sid[i*4+j]);
            if (si == NULL)
                continue;
            
            bool flag = false;
            for (int k = 0 ; k < myInfo->GetSkillList().size() ; k++)
            {
                if (myInfo->GetSkillList()[k]->GetCommonId() == si->GetId())
                    flag = true;
            }
            
            if (flag)
            {
                /*
                flag = false;
                for (int k = 0 ; k < myInfo->GetSlot().size() ; k++)
                {
                    if (myInfo->GetSlot()[k]->GetCommonId() == sid[i*4+j])
                        flag = true;
                }
                if (flag)
                {
                    sprintf(name2, "background/bg_skill_select.png%d", sid[i*4+j]);
                    ((CCSprite*)spriteClass->FindSpriteByName(name2))->setOpacity(255);
                }
                */
                
                sprintf(name2, "skill_%d.png", si->GetId());
                spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2, ccp(0, 0), ccp(127+j*229+12, 1451-i*160+offset+12), CCSize(0, 0), "", "Layer", layer, 20, 0, 255, si->GetId()) );
                
                
                scid = sid[i*4+j];
                
                ttrArrow->setAnchorPoint(ccp(0.5, 0));
                ttrArrow->setPosition(ccp(127+j*229+12+146/2-5, 1451-i*160+offset+12+146));
                CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
                ttrArrow->runAction(CCRepeatForever::create(action));
                layer->addChild(ttrArrow, 5005);
                
                ttrPos->setAnchorPoint(ccp(0, 0));
                ttrPos->setPosition(ccp(127+j*229+12-14, 1451-i*160+offset+12-10));
                ttrPos->setContentSize(CCSize(153, 153));
                layer->addChild(ttrPos, 5005);
            }
        }
    }
    
    // 스킬 선택/비선택할 때 설명 text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("스킬 설명 부분", fontList[0], 36, ccp(0.5, 0.5), ccp(77+929/2, 726+offset+200/2), ccc3(78,47,8), CCSize(929-80, 200-10), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", layer, 10, 0, 255, 999) );
    
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png3", ccp(0, 0), ccp(49, 458+offset), CCSize(982, 223), "", "T_MagicList", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png3", ccp(0, 0), ccp(77, 488+offset), CCSize(782, 177), "", "T_MagicList", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", ccp(0, 0), ccp(896, 572+offset), CCSize(0, 0), "", "T_MagicList", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png4", ccp(0, 0), ccp(867, 497+offset), CCSize(136, 63), "", "T_MagicList", this, 1) );
    
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 48, ccp(0, 0), ccp(892, 505+offset), ccc3(255,219,53), "", "T_MagicList", this, 5, 0, 255, 100) ); // 현재 슬롯 개수
    sprintf(name, "/ %d", (int)skillSlotInfo.size());
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel(name, fontList[0], 36, ccp(0, 0), ccp(927, 505+offset), ccc3(182,142,142), "", "T_MagicList", this, 5) ); // 젼체 슬롯 개수
    
    // add child
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
    
    // action
    CCActionInterval* move = CCMoveTo::create(0.5f, ccp(0, 0));
    CCActionInterval* action = CCEaseElasticOut::create((CCActionInterval*)move, 0.5f);
    layer->runAction(action);
}


void T_MagicList::MakeScrollSlot(bool isAutoMove)
{
    scrollViewSlot->removeAllChildren();
    
    int numOfList = myInfo->GetSlot().size();
    
    // container 초기화
    containerSlot = CCLayer::create();
    containerSlot->setContentSize(CCSizeMake(numOfList*(146+5), 146));
    
    char fname[50], fname2[20];
    //int scid;
    for (int i = 0 ; i < numOfList ; i++)
    {
        sprintf(fname, "background/bg_skill_yellow.png%d", i);
        spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname, ccp(0, 0), ccp(i*(146+5), 0), CCSize(0, 0), "", "Layer", containerSlot, 3) );
        
        if (tutorialState == 10+2 && i == 0)
        {
            
            //scid = myInfo->GetSlot()[i]->GetCommonId();
            //if (scid > 0) // 슬롯에 스킬이 있다면 문양을 표시한다.
            //{
                sprintf(fname2, "skill_%d.png", scid);
                spriteClassSlot->spriteObj.push_back( SpriteObject::Create(0, fname2, ccp(0.5, 0.5), spriteClassSlot->FindParentCenterPos(fname), CCSize(0, 0), fname, "0", NULL, 4, 1) );
            //}
        }
    }
    
    for (int i = 0 ; i < spriteClassSlot->spriteObj.size() ; i++)
        spriteClassSlot->AddChild(i);
    
    // scrollView의 container 재설정
    scrollViewSlot->setContainer(containerSlot);
    scrollViewSlot->setContentSize(containerSlot->getContentSize());
    
    if (isAutoMove)
        scrollViewSlot->setContentOffsetInDuration(ccp(scrollViewSlot->minContainerOffset().x, 0), 0.4f);
}


bool T_MagicList::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    
    // 슬롯 터치 판단
    if (scrollViewSlot->boundingBox().containsPoint(point))
        isScrollViewTouched = true;
    
    if (tutorialState == 10+2)
    {
        isTouched = false;
        return true;
    }
 
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name.substr(0, 5) == "skill")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                char name[40];
                int sid = spriteClass->spriteObj[i]->sprite->getTag();
                    
                // 튜토리얼 상태 9 -> 10 이동
                CCString* param = CCString::create("2");
                CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetParentName(), param);
                
                ttrPos->setAnchorPoint(ccp(0, 0));
                ttrPos->setPosition(ccp(77, 228));
                //ttrPos->setScaleX( (float)792 / (float)782 );
                //ttrPos->setScaleY( (float)187 / (float)177 );
                ttrPos->setContentSize(CCSize(792, 187));
                
                ttrArrow->removeFromParent();
                ttrArrow->setAnchorPoint(ccp(0.5, 0));
                ttrArrow->setPosition(ccp(77+782-100, 228+187+10));
                //CCActionInterval* action = CCSequence::create( CCMoveBy::create(0.5f, ccp(0, -5)), CCMoveBy::create(0.5f, ccp(0, 5)), NULL);
                //ttrArrow->runAction(CCRepeatForever::create(action));
                this->addChild(ttrArrow, 5005);
                
                //ttrArrow->setAnchorPoint(ccp(0.5, 0));
                //ttrArrow->setPosition(ccp(77+782-100, 228+187+10));
                //}
                
                sprintf(name, "background/bg_skill_select.png%d", sid);
                ((CCSprite*)spriteClass->FindSpriteByName(name))->setOpacity(255);
                
                //RenewSlot( (j >= 5) );
                RenewSlot(false);
                sound->playClick();
                
                // 설명 글 갱신
                ((CCLabelTTF*)spriteClass->FindLabelByTag(999))->setString( (SkillInfo::GetSkillInfo(sid)->GetName()+"\n "+SkillInfo::GetFullDesc(sid)).c_str());
                
                return true;
            }
        }
    }
    
    return true;
}


void T_MagicList::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void T_MagicList::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (isScrollViewTouched && !isScrolling && scrollViewSlot->boundingBox().containsPoint(point))
    {
        if (tutorialState == 10+2)
            EndScene();
    }
    
    isTouched = false;
    isScrollViewTouched = false;
    isScrolling = false;
}

void T_MagicList::RenewSlot(bool isAutoMove)
{
    // 스킬 슬롯 정보 갱신
    spriteClassSlot->RemoveAllObjects();
    MakeScrollSlot(isAutoMove);
    char name[7];
    sprintf(name, "%d", (int)myInfo->GetSlot().size());
    ((CCLabelTTF*)spriteClass->FindLabelByTag(100))->setString(name);
}


void T_MagicList::EndScene()
{
    CCFiniteTimeAction* action =
    CCSequence::create(CCMoveTo::create(0.2f, ccp(0, winSize.height)),
                       CCCallFunc::create(this, callfunc_selector(T_MagicList::EndSceneCallback)), NULL);
    layer->runAction(action);
}

void T_MagicList::EndSceneCallback()
{
    // remove this notification
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, Depth::GetCurName());
    // release depth tree
    Depth::RemoveCurDepth();
    
    // touch 넘겨주기 (GetCurName = 위에서 remove를 했기 때문에 결국 여기 입장에서는 부모다)
    CCString* param = CCString::create("0");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    // 튜토리얼 상태 하나 넘기기
    param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    // 슬롯 갱신
    param = CCString::create("9");
    CCNotificationCenter::sharedNotificationCenter()->postNotification(Depth::GetCurName(), param);
    
    this->setKeypadEnabled(false);
    this->setTouchEnabled(false);
    
    ttrArrow->release();
    ttrPos->release();
    ttrArrow->removeFromParentAndCleanup(true);
    ttrPos->removeFromParentAndCleanup(true);
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    spriteClassSlot->RemoveAllObjects();
    delete spriteClassSlot;
    //spriteClassBtn->RemoveAllObjects();
    //delete spriteClassBtn;
    
    layer->removeAllChildren();
    layer->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}

void T_MagicList::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void T_MagicList::scrollViewDidZoom(CCScrollView* view)
{
}