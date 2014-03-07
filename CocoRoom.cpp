#include "CocoRoom.h"

CCScene* CocoRoom::scene()
{
    CCScene* pScene = CCScene::create();
    CocoRoom* pLayer = CocoRoom::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void CocoRoom::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void CocoRoom::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void CocoRoom::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool CocoRoom::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    coco = CCLayer::create();
    fairy = CCLayer::create();
    candy = CCLayer::create();
    coco->setPosition(ccp(0, 0));
    fairy->setPosition(ccp(0, 0));
    candy->setPosition(ccp(0, 0));
    this->addChild(coco, 5);
    this->addChild(fairy, 5);
    this->addChild(candy, 5);
    
    spriteClass = new SpriteClass();
    spriteClassCoco = new SpriteClass();
    spriteClassFairy = new SpriteClass();
    spriteClassCandy = new SpriteClass();
    
    InitSprites();
    curState = -1;
    MakeSprites(0);
    
    return true;
}

void CocoRoom::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_purple.png",
            ccp(0, 0), ccp(14, 1586), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_yellow.png",
            ccp(0, 0), ccp(875, 1634), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "strap/strap_title_cocoroom.png",
            ccp(0, 0), ccp(409, 1632), CCSize(0, 0), "", "CocoRoom", this, 2) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png1",
            ccp(0, 0), ccp(101, 1417), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png2",
            ccp(0, 0), ccp(297, 1417), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_sketchbook_select.png3",
            ccp(0, 0), ccp(495, 1417), CCSize(0, 0), "", "CocoRoom", this, 2) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_coco.png",
            ccp(0, 0), ccp(101+40, 1447+40), CCSize(0, 0), "", "CocoRoom", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_fairy.png",
            ccp(0, 0), ccp(297+55, 1447+10), CCSize(0, 0), "", "CocoRoom", this, 3) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_menu_starcandy.png",
            ccp(0, 0), ccp(495+50, 1447+23), CCSize(0, 0), "", "CocoRoom", this, 3) );
    
    // 오늘의 별사탕 띠
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_today_starcandy.png",
                    ccp(0, 0), ccp(630, 1494), CCSize(0, 0), "", "CocoRoom", this, 5) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png1",
            ccp(0, 0), ccp(49, 458), CCSize(982, 954+20), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
            ccp(0, 0), ccp(75, 492), CCSize(929, 904+10), "", "CocoRoom", this, 1) );
    
    // slot part
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png2",
            ccp(0, 0), ccp(49, 198), CCSize(982, 223), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
            ccp(0, 0), ccp(77, 228), CCSize(782, 177), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_plus_big.png", // slot plus
            ccp(0, 0), ccp(896, 317), CCSize(0, 0), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_gameready_name.png",
            ccp(0, 0), ccp(867, 242), CCSize(136, 63), "", "CocoRoom", this, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("4", fontList[0], 48, ccp(0, 0),
            ccp(892, 248), ccc3(255,219,53), "", "CocoRoom", this, 5) );
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("/ 7", fontList[0], 36, ccp(0, 0),
            ccp(927, 248), ccc3(182,142,142), "", "CocoRoom", this, 5) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }
    
    /*
    // make scroll
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782, 177)); //782-38, 146));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 228));
    scrollView->setDelegate(this);
    this->addChild(scrollView, 4);
    */
}

void CocoRoom::MakeSprites(int state)
{
    if (curState == state)
        return;
    
    coco->removeAllChildren();
    spriteClassCoco->spriteObj.clear();
    fairy->removeAllChildren();
    spriteClassFairy->spriteObj.clear();
    candy->removeAllChildren();
    spriteClassCandy->spriteObj.clear();
    
    // slot '+' 밑부분 숫자를, 오늘의별사탕에서는 다른 것으로 대체
    int size = spriteClass->spriteObj.size();
    spriteClass->spriteObj[size-3]->sprite9->setOpacity(255);
    spriteClass->spriteObj[size-2]->label->setOpacity(255);
    spriteClass->spriteObj[size-1]->label->setOpacity(255);
    
    if (state == 0)
    {
        MakeSpritesCoco();
        MakeScrollCoco();
    }
    else if (state == 1)
    {
        MakeSpritesFairy();
        MakeScrollFairy();
    }
    else
    {
        spriteClass->spriteObj[size-3]->sprite9->setOpacity(0);
        spriteClass->spriteObj[size-2]->label->setOpacity(0);
        spriteClass->spriteObj[size-1]->label->setOpacity(0);
        MakeSpritesCandy();
    }
    
    
    curState = state;
}

void CocoRoom::MakeSpritesCoco()
{
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_degree_desc.png",
                        ccp(0, 0), ccp(441, 1290), CCSize(543, 61), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("오늘도 곰을 백마리 잡은 마법사", fontList[0], 36, ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_degree_desc.png"), ccc3(255,255,255), "background/bg_degree_desc.png", "1", NULL, 5) );
    
    // skill 그림
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png1",
                        ccp(0, 0), ccp(608, 1182), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png2",
                        ccp(0, 0), ccp(699, 1182), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png3",
                        ccp(0, 0), ccp(790, 1182), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_skill_yellow_mini.png4",
                        ccp(0, 0), ccp(882, 1182), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png1",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png1"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png2",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png2"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png3",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png3"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "background/bg_property_big.png4",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png4"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png4", "0", NULL, 5) );
    
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_fire_mini.png",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png1"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png1", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_water_mini.png",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png2"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png2", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_land_mini.png",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png3"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png3", "0", NULL, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_property_master_mini.png",
        ccp(0.5, 0.5), spriteClassCoco->FindParentCenterPos("background/bg_skill_yellow_mini.png4"),
        CCSize(0, 0), "background/bg_skill_yellow_mini.png4", "0", NULL, 5) );
    
    // 설명창 배경
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1084), CCSize(439, 76), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 999), CCSize(439, 58), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 937), CCSize(439, 58), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 875), CCSize(439, 58), "", "Layer", coco, 5) );
    // 설명
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_magicpoint.png",
        ccp(0, 0), ccp(549, 1084), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("기본 MP", fontList[2], 36, ccp(0, 0), ccp(549, 1006), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("기본 MP", fontList[2], 36, ccp(0, 0), ccp(549, 1009), ccc3(255,219,53), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(549, 944), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(549, 947), ccc3(255,219,53), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("지팡이", fontList[2], 36, ccp(0, 0), ccp(549, 882), ccc3(121,71,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("지팡이", fontList[2], 36, ccp(0, 0), ccp(549, 885), ccc3(255,219,53), "", "Layer", coco, 5) );
    
    // buttons
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png1",
                        ccp(0, 0), ccp(94, 509), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png2",
                        ccp(0, 0), ccp(541, 509), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_staff.png1",
                        ccp(0, 0), ccp(134, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_normal.png",
                        ccp(0, 0), ccp(299, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_staff.png2",
                        ccp(0, 0), ccp(581, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_buildup_fine.png",
                        ccp(0, 0), ccp(746, 587), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png",
                        ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png",
                        ccp(0, 0), ccp(671, 531), CCSize(0, 0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(300, 541), ccc3(22,56,0), "", "Layer", coco, 5) );
    spriteClassCoco->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(750, 541), ccc3(22,56,0), "", "Layer", coco, 5) );
    
    for (int i = 0 ; i < spriteClassCoco->spriteObj.size() ; i++)
        spriteClassCoco->AddChild(i);
}

void CocoRoom::MakeSpritesFairy()
{
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0, 0), ccp(524, 1271), CCSize(439, 89), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0, 0), ccp(524, 1186), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0, 0), ccp(524, 1124), CCSize(439, 58), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0, 0), ccp(524, 1026), CCSize(439, 58), "", "Layer", fairy, 5) );
    
    // description
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("잠만자양", fontList[0], 52, ccp(0.5, 0.5), spriteClassFairy->FindParentCenterPos("background/bg_cocoroom_desc.png1"), ccc3(255,255,255), "background/bg_cocoroom_desc.png1", "1", NULL, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1193), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("기본속성", fontList[2], 36, ccp(0, 0), ccp(570, 1196), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1131), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("추가속성", fontList[2], 36, ccp(0, 0), ccp(570, 1134), ccc3(255,219,53), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1033), ccc3(121,71,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("특수능력", fontList[2], 36, ccp(0, 0), ccp(570, 1036), ccc3(255,219,53), "", "Layer", fairy, 5) );
    
    // description detail
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 100", fontList[0], 36, ccp(0, 0), ccp(720, 1196), ccc3(255,255,255), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("MP + 50", fontList[0], 36, ccp(0, 0), ccp(720, 1134), ccc3(0,167,222), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("요정공격 확률증가", fontList[0], 36, ccp(0, 0), ccp(720, 1036), ccc3(255,255,255), "", "Layer", fairy, 5) );
    
    // buttons
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png1",
                    ccp(0, 0), ccp(94, 509), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_green.png2",
                    ccp(0, 0), ccp(541, 509), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_normal.png",
                    ccp(0, 0), ccp(176, 588), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tonic.png1",
                    ccp(0, 0), ccp(291, 586), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fine.png",
                    ccp(0, 0), ccp(623, 588), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_tonic.png2",
                    ccp(0, 0), ccp(738, 586), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_starcandy.png",
                    ccp(0, 0), ccp(224, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_receive_topaz.png",
                    ccp(0, 0), ccp(681, 531), CCSize(0, 0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(297, 541), ccc3(22,56,0), "", "Layer", fairy, 5) );
    spriteClassFairy->spriteObj.push_back( SpriteObject::CreateLabel("x 2", fontList[0], 36, ccp(0, 0), ccp(741, 541), ccc3(22,56,0), "", "Layer", fairy, 5) );
    
    for (int i = 0 ; i < spriteClassFairy->spriteObj.size() ; i++)
        spriteClassFairy->AddChild(i);
}

void CocoRoom::MakeSpritesCandy()
{
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "rank/bg_group.png",
                        ccp(0, 0), ccp(108, 518), CCSize(0, 0), "", "Layer", candy, 5) );
    
    // 5 profiles
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png1",
                        ccp(0, 0), ccp(471, 1200), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png2",
                        ccp(0, 0), ccp(184, 1051), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png3",
                        ccp(0, 0), ccp(267, 671), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png4",
                        ccp(0, 0), ccp(696, 671), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "background/bg_profile.png5",
                        ccp(0, 0), ccp(787, 1051), CCSize(0, 0), "", "Layer", candy, 5) );
    
    // names
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1",
                        ccp(0.5, 0), ccp(471+60, 1140), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2",
                        ccp(0.5, 0), ccp(184+60, 987), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png3",
                        ccp(0.5, 0), ccp(267+60, 607), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png4",
                        ccp(0.5, 0), ccp(696+60, 607), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png5",
                        ccp(0.5, 0), ccp(787+60, 987), CCSize(150, 58), "", "Layer", candy, 5, 160) );
    
    // question
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_question.png",
                        ccp(0, 0), ccp(916, 515), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "button/btn_red_mini.png",
                        ccp(0, 0), ccp(425, 842), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_fairy_select.png",
                ccp(0.5, 0), ccp(spriteClassCandy->spriteObj[spriteClassCandy->spriteObj.size()-1]->sprite->getContentSize().width/2, 30), CCSize(0, 0), "button/btn_red_mini.png", "0", NULL, 5) );
    
    // 보상 설명 부분
    spriteClassCandy->spriteObj.push_back( SpriteObject::Create(0, "icon/icon_starcandy.png",
                        ccp(0, 0), ccp(350, 300), CCSize(0, 0), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("오늘의 보상 :", fontList[0], 36, ccp(0, 0), ccp(110, 330), ccc3(78,47,8), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("x 500", fontList[0], 36, ccp(0, 0), ccp(450, 320), ccc3(78,47,8), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구와 함께 보상을 받으세요!", fontList[0], 36, ccp(0, 0), ccp(240, 250), ccc3(255,255,255), "", "Layer", candy, 5) );
    spriteClassCandy->spriteObj.push_back( SpriteObject::CreateLabel("친구목록", fontList[0], 36, ccp(0, 0), ccp(870, 247), ccc3(255,255,255), "", "Layer", candy, 5) );
    
    for (int i = 0 ; i < spriteClassCandy->spriteObj.size() ; i++)
        spriteClassCandy->AddChild(i);
}

void CocoRoom::MakeScrollCoco()
{
    containerCoco = CCLayer::create();
    containerCoco->setPosition(ccp(91, 242));
    int numOfSlots = 7;
    
    char name[50];
    for (int i = 0 ; i < numOfSlots ; i++)
    {
        sprintf(name, "background/bg_skill_yellow.png%d", i);
        spriteClassCoco->spriteObj.push_back( SpriteObject::Create(0, name,
                    ccp(0, 0), ccp(148*i, 0), CCSize(0, 0), "", "Layer", containerCoco, 10) );
        spriteClassCoco->AddChild(spriteClassCoco->spriteObj.size()-1);
    }
    
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782, 177)); //782-38, 146));
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 228));
    containerCoco->setContentSize(CCSizeMake(146*numOfSlots, 146));
    scrollView->setContentSize(containerCoco->getContentSize());
    scrollView->setContainer(containerCoco);
    scrollView->setDelegate(this);
    this->addChild(scrollView, 5);
    // 안 보임... 수정해야함
}

void CocoRoom::MakeScrollFairy()
{
    containerFairy = CCLayer::create();
    containerFairy->setPosition(ccp(91, 242));
    
    // ...
}


bool CocoRoom::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png1")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                MakeSprites(0);
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png2")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                MakeSprites(1);
        }
        else if (spriteClass->spriteObj[i]->name == "background/bg_sketchbook_select.png3")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                MakeSprites(2);
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_x_yellow.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndScene();
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_plus_big.png")
        {
            if (curState == 0 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                ; // 슬롯 구매
            else if (curState == 1 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomFairyTown", false); // 요정의 마을
            else if (curState == 2 && spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                Common::ShowNextScene(this, "CocoRoom", "CocoRoomTodayCandy", false); // 오.별 친구고르기
        }
    }
    
    return true;
}


void CocoRoom::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void CocoRoom::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
    isTouched = false;
}

void CocoRoom::EndScene()
{
    this->removeFromParentAndCleanup(true);
}

void CocoRoom::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
    CCLog("cocoroom - scrolling~");
}

void CocoRoom::scrollViewDidZoom(CCScrollView* view)
{
}

