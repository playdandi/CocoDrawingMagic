#include "GameReady.h"
#include "Ranking.h"
#include "MagicList.h"

CCScene* GameReady::scene()
{
    CCScene* pScene = CCScene::create();
    GameReady* pLayer = GameReady::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void GameReady::onEnter()
{
    CCLog("GameReady :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void GameReady::onExit()
{
    CCLog("GameReady :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void GameReady::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool GameReady::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    names.push_back("icon/icon_topaz.png");
    names.push_back("icon/icon_starcandy.png");
    names.push_back("icon/icon_magicpoint.png");
    names.push_back("background/bg_property.png1");
    names.push_back("background/bg_property.png2");
    names.push_back("background/bg_property.png3");
    names.push_back("background/bg_roof.png");
    names.push_back("button/btn_topinfo_plus.png1"); // for topaz
    names.push_back("button/btn_topinfo_plus.png2"); // for starcandy
    names.push_back("background/bg_potion_time.png");
    names.push_back("button/btn_topinfo_plus.png3"); // for potion_time
    names.push_back("strap/strap_blue.png");
    names.push_back("strap/strap_title_gameready.png");
    names.push_back("button/btn_x_yellow.png"); // X
    names.push_back("letter/letter_grade_a.png"); // 요정 등급
    names.push_back("background/bg_petlevel.png"); // 요정 레벨 배경
    names.push_back("background/bg_skill_brown.png1"); // item1
    names.push_back("background/bg_skill_brown.png2"); // item2
    names.push_back("background/bg_skill_brown.png3"); // item3
    names.push_back("background/bg_skill_brown.png4"); // item4
    names.push_back("background/bg_skill_brown.png5"); // item5
    names.push_back("button/btn_plus_big.png"); // slot plus
    names.push_back("button/btn_blue.png"); // gamestart btn
    //names.push_back("letter/letter_gamestart.png");
    
    
    pos.push_back(ccp(15, 1656));
    pos.push_back(ccp(317, 1660));
    pos.push_back(ccp(696, 1669));
    pos.push_back(ccp(848, 1666-55));
    pos.push_back(ccp(904, 1666-55));
    pos.push_back(ccp(959, 1666-55));
    pos.push_back(ccp(10, 1433));
    pos.push_back(ccp(80+230-55, 1666+13)); // for topaz
    pos.push_back(ccp(390+290-55, 1666+13)); // for starcandy
    pos.push_back(ccp(506, 1493));
    pos.push_back(ccp(645, 1498)); // for potion_time
    pos.push_back(ccp(14, 1343));
    pos.push_back(ccp(409, 1389));
    pos.push_back(ccp(875, 1391)); // X
    pos.push_back(ccp(408+11, 1143+165)); // 요정등급
    pos.push_back(ccp(408+55, 1143+187)); // 요정레벨배경
    pos.push_back(ccp(84, 972));
    pos.push_back(ccp(277, 972));
    pos.push_back(ccp(470, 972));
    pos.push_back(ccp(663, 972));
    pos.push_back(ccp(856, 972));
    pos.push_back(ccp(896, 572)); // slot puls
    pos.push_back(ccp(318, 193)); // gamestart btn
    //pos.push_back(ccp(318+70, 193+45));

    
    // for 9-sprites
    names9.push_back("background/bg_topinfo.png"); // for topaz
    names9.push_back("background/bg_topinfo.png"); // for starcandy
    names9.push_back("background/bg_topinfo.png"); // for magicpoint
    names9.push_back("background/bg_board_brown.png1"); // 전체
    names9.push_back("background/bg_board_yellow.png1"); // 위(코코/요정/스킬)
    names9.push_back("background/bg_board_yellow.png2"); // 중간설명
    names9.push_back("background/bg_board_brown.png2"); // 코코
    names9.push_back("background/bg_board_brown.png3"); // 요정
    names9.push_back("background/bg_board_brown.png4"); // 스킬
    names9.push_back("background/bg_board_brown.png5"); // 슬롯판
    names9.push_back("background/bg_board_yellow.png3"); // 슬롯판
    names9.push_back("background/bg_gameready_name.png1"); // coco 설명
    names9.push_back("background/bg_gameready_name.png2"); // 요정 설명
    names9.push_back("background/bg_gameready_name.png3"); // skill 설명
    names9.push_back("background/bg_gameready_name.png4"); // slot number bg
    pos9.push_back(ccp(80, 1666));
    pos9.push_back(ccp(390, 1666));
    pos9.push_back(ccp(765, 1666));
    pos9.push_back(ccp(49, 687)); // brown - 전체
    pos9.push_back(ccp(77, 1126)); // yellow - 위
    pos9.push_back(ccp(77, 726)); // yellow - 중간설명
    pos9.push_back(ccp(92, 1143));
    pos9.push_back(ccp(408, 1143));
    pos9.push_back(ccp(725, 1143));
    pos9.push_back(ccp(49, 458));
    pos9.push_back(ccp(77, 488));
    pos9.push_back(ccp(112, 1165));
    pos9.push_back(ccp(425, 1165));
    pos9.push_back(ccp(744, 1165));
    pos9.push_back(ccp(867, 497));
    size9.push_back(ccp(230, 75));
    size9.push_back(ccp(290, 75));
    size9.push_back(ccp(290, 75));
    size9.push_back(ccp(982, 725));
    size9.push_back(ccp(929, 268));
    size9.push_back(ccp(929, 200));
    size9.push_back(ccp(309, 236));
    size9.push_back(ccp(309, 236));
    size9.push_back(ccp(263, 236));
    size9.push_back(ccp(982, 223));
    size9.push_back(ccp(782, 177));
    size9.push_back(ccp(274, 53));
    size9.push_back(ccp(274, 53));
    size9.push_back(ccp(228, 53));
    size9.push_back(ccp(136, 63));
    
    
    
    for (int i = 0 ; i < 5 ; i++)
        itemSelected[i] = false;
    
    InitSprites();
    MakeScroll();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
    return true;
}

void GameReady::InitSprites()
{
    CCSprite* pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, winSize.width, winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(150);
    this->addChild(pBlack, 0);
    
    for (int i = 0 ; i < names.size() ; i++)
    {
        CCSprite* temp;
        if (names[i].substr(names[i].find('.')) != ".png")
            temp = CCSprite::createWithSpriteFrameName(names[i].substr(0, names[i].size()-1).c_str());
        else
            temp = CCSprite::createWithSpriteFrameName(names[i].c_str());
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(pos[i]);

        if (names[i] == "background/bg_roof.png") {
            CCSprite* temp2 = CCSprite::createWithSpriteFrameName(names[i].c_str());
            temp2->setAnchorPoint(ccp(0, 0));
            temp2->setPosition(ccp((int)pos[i].x+temp2->getContentSize().width, (int)pos[i].y));
            temp2->setFlipX(true);
            this->addChild(temp2, 0);
        }
        else if (names[i].substr(0, names[i].size()-1) == "background/bg_skill_brown.png")
        {
            // skill item인 경우, 파란 테두리와 체크 표시도 같이 놓아둔다 (투명도 0)
            CCSprite* border = CCSprite::createWithSpriteFrameName("background/bg_skill_select.png");
            border->setAnchorPoint(ccp(0.5, 0.5));
            border->setPosition(ccp(temp->getContentSize().width/2, temp->getContentSize().height/2));
            border->setOpacity(0);
            border->setTag(names[i][names[i].size()-1]);
            temp->addChild(border, 3);
            CCSprite* check = CCSprite::createWithSpriteFrameName("icon/icon_check.png");
            check->setAnchorPoint(ccp(0, 0));
            check->setPosition(ccp(100, 110));
            check->setOpacity(0);
            check->setTag(names[i][names[i].size()-1]);
            border->addChild(check, 3);
        }
        
        if (names[i] == "background/bg_roof.png")
            this->addChild(temp, 0);
        else
            this->addChild(temp, 1);
        
        sprites.push_back(temp);
    }
    
    // 여러 번 sprite만드는 경우
    // 1) potion
    int numOfPotion = 3;
    for (int i = 0 ; i < 5; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("icon/icon_potion_empty.png");
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(ccp(89+83*i, 1480));
        this->addChild(temp, 0);
        // potion
        if (i < numOfPotion) {
            CCSprite* potion = CCSprite::createWithSpriteFrameName("icon/icon_potion.png");
            potion->setAnchorPoint(ccp(0.5, 0.5));
            potion->setPosition(ccp(temp->getContentSize().width/2, temp->getContentSize().height/2));
            temp->addChild(potion, 0);
        }
    }
    
    // 9-sprites
    for (int i = 0 ; i < names9.size() ; i++)
    {
        CCScale9Sprite* temp;
        if (names9[i].substr(names9[i].find('.')) != ".png")
            temp = CCScale9Sprite::createWithSpriteFrameName(names9[i].substr(0, names9[i].size()-1).c_str());
        else
            temp = CCScale9Sprite::createWithSpriteFrameName(names9[i].c_str());
    
        temp->setContentSize(CCSizeMake((int)size9[i].x, (int)size9[i].y));
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(pos9[i]);
        this->addChild(temp, 0);
        
        if (names9[i] == "background/bg_gameready_name.png1" || names9[i] == "background/bg_gameready_name.png2" || names9[i] == "background/bg_gameready_name.png3")
            temp->setOpacity(128);
        
        sprites9.push_back(temp);
    }
    
    
    // labels
    CCLabelTTF* coco_ability2 = CCLabelTTF::create("지팡이 24Lv", "NanumGothicExtraBold", 30);
    coco_ability2->setAnchorPoint(ccp(0.5, 0.5));
    coco_ability2->setPosition(ccp(250, 1190));
    coco_ability2->setColor(ccc3(121, 71, 0));
    this->addChild(coco_ability2, 3);
    CCLabelTTF* coco_ability = CCLabelTTF::create("지팡이 24Lv", "NanumGothicExtraBold", 30);
    coco_ability->setAnchorPoint(ccp(0.5, 0.5));
    coco_ability->setPosition(ccp(250, 1193));
    coco_ability->setColor(ccc3(255, 219, 53));
    this->addChild(coco_ability, 3);
    
    CCLabelTTF* elf_ability2 = CCLabelTTF::create("특수능력없음", "NanumGothicExtraBold", 30);
    elf_ability2->setAnchorPoint(ccp(0.5, 0.5));
    elf_ability2->setPosition(ccp(550, 1190));
    elf_ability2->setColor(ccc3(121, 71, 0));
    this->addChild(elf_ability2, 3);
    CCLabelTTF* elf_ability = CCLabelTTF::create("특수능력없음", "NanumGothicExtraBold", 30);
    elf_ability->setAnchorPoint(ccp(0.5, 0.5));
    elf_ability->setPosition(ccp(550, 1193));
    elf_ability->setColor(ccc3(255, 219, 53));
    this->addChild(elf_ability, 3);
    
    CCLabelTTF* skill_ability2 = CCLabelTTF::create("따뜻한 불꽃그림", "NanumGothicExtraBold", 30);
    skill_ability2->setAnchorPoint(ccp(0.5, 0.5));
    skill_ability2->setPosition(ccp(860, 1190));
    skill_ability2->setColor(ccc3(121, 71, 0));
    this->addChild(skill_ability2, 3);
    CCLabelTTF* skill_ability = CCLabelTTF::create("따뜻한 불꽃그림", "NanumGothicExtraBold", 30);
    skill_ability->setAnchorPoint(ccp(0.5, 0.5));
    skill_ability->setPosition(ccp(860, 1193));
    skill_ability->setColor(ccc3(255, 255, 255));
    this->addChild(skill_ability, 3);
    
    CCLabelTTF* skill_desc = CCLabelTTF::create("미쳐버리겠네...", "NanumGothicExtraBold", 36, CCSize(909, 180), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    skill_desc->setColor(ccc3(0, 0, 0));
    skill_desc->setAnchorPoint(ccp(0, 0));
    skill_desc->setPosition(ccp(97, 736));
    //skill_desc->setPosition(ccp(30, pDescription->getContentSize().height/2));
    this->addChild(skill_desc, 2);
    //pDescription->addChild(desc);
    
    /*
    pBackground = new CCSprite();
    pBackground->initWithTexture(tBackground, CCRectMake(0, 0, 1080, 1920));
    pBackground->setAnchorPoint(ccp(0, 0));
    pBackground->setPosition(ccp(0, 0));
    pBackground->setOpacity(0);
    this->addChild(pBackground, 0);

    pDescription = new CCSprite();
    pDescription->initWithTexture(tDescription, CCRectMake(0, 0, 927, 198));
    pDescription->setAnchorPoint(ccp(0, 0));
    pDescription->setPosition(ccp(91-10-5, 716+10));
    this->addChild(pDescription, 0);
    
    desc = CCLabelTTF::create("사랑합니다.", "Arial", 40, CCSize(907, 178), kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter);
    desc->setColor(ccc3(0, 0, 0));
    desc->setAnchorPoint(ccp(0, 0.5));
    desc->setPosition(ccp(30, pDescription->getContentSize().height/2));
    pDescription->addChild(desc);
    
    pItemNormal = new CCSprite();
    pItemNormal->initWithTexture(tItemNormal, CCRectMake(0, 0, 183, 178));
    pItemNormal->setAnchorPoint(ccp(0.5, 0.5));
    pItemNormal->setPosition(ccp(350, 1041));
    pItemNormal->setOpacity(0);
    pBackground->addChild(pItemNormal);
    
    pItemClicked = new CCSprite();
    pItemClicked->initWithTexture(tItemClicked, CCRectMake(0, 0, 183, 178));
    pItemClicked->setAnchorPoint(ccp(0.5, 0.5));
    pItemClicked->setPosition(ccp(350, 1041));
    pItemClicked->setOpacity(0);
    pBackground->addChild(pItemClicked);
    
    CCActionInterval* action = CCFadeIn::create(0.5f);
    pBackground->runAction((CCActionInterval*)action->copy()->autorelease());
    pDescription->runAction((CCActionInterval*)action->copy()->autorelease());
    pItemNormal->runAction((CCActionInterval*)action->copy()->autorelease());
    desc->runAction((CCActionInterval*)action->copy()->autorelease());
    */
}

void GameReady::MakeScroll()
{
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 0));
    scrollContainer->setPosition(ccp(100, 500));
    int numOfSlots = 7;
    
    for (int i = 0 ; i < numOfSlots ; i++)
    {
        CCSprite* temp = CCSprite::createWithSpriteFrameName("background/bg_skill_yellow.png");
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(ccp(146*i, 0));
        scrollContainer->addChild(temp, 5);
    }
    
    scrollContainer->setContentSize(CCSizeMake(146*numOfSlots, 146));

    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionHorizontal);
    scrollView->setViewSize(CCSizeMake(782-38, 146));
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(100, 500));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    //scrollView->setContentOffset(ccp(-(numOfSlots-5)*146, 0), false);
    //scrollView->setContentOffset(ccp((numOfSlots)*146, 0), false);
    this->addChild(scrollView, 4);
    
    /*
     for (int i = 0 ; i < 10 ; i++)
     {
     CCSprite* temp = new CCSprite();
     temp->initWithTexture(tScrollBg, CCRectMake(0, 0, 860, 209));
     temp->setAnchorPoint(ccp(0, 1));
     temp->setPosition(ccp(107, (10-i)*200));
     scrollContainer->addChild(temp);
     }
     
     scrollContainer->setContentSize(CCSizeMake(850, 10*200));
     
     CCScrollView* scrollView = CCScrollView::create();
     scrollView->retain();
     scrollView->setDirection(kCCScrollViewDirectionVertical);
     scrollView->setViewSize(CCSizeMake(850, 818));
     scrollView->setContentSize(scrollContainer->getContentSize());
     scrollView->setAnchorPoint(ccp(0, 0));
     scrollView->setPosition(ccp(107, 512));
     scrollView->setContainer(scrollContainer);
     scrollView->setDelegate(this);
     
     scrollView->setContentOffset(ccp(0, 818-(10*200)), false);
     addChild(scrollView);
     */

}


bool GameReady::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    if (isTouched)
        return false;
    isTouched = true;
    isScrolling = false;
    
    CCPoint point = pTouch->getLocation();
    CCLog("GameReady : (%d , %d)", (int)point.x, (int)point.y);
    
    
    for (int i = 0 ; i < names.size() ; i++)
    {
        if (sprites[i]->boundingBox().containsPoint(point))
        {
            //CCLog("%s", names[i].c_str());
            if (names[i].substr(0, names[i].size()-1) == "background/bg_skill_brown.png")
            {
                CCSprite* border = (CCSprite*)sprites[i]->getChildByTag(names[i][names[i].size()-1]);
                CCSprite* check = (CCSprite*)border->getChildByTag(names[i][names[i].size()-1]);
                
                int tag = (int)border->getTag() - '0';
                itemSelected[tag-1] = !itemSelected[tag-1];
                border->setOpacity(255-border->getOpacity());
                check->setOpacity(255-check->getOpacity());
            }
            else if (names[i] == "button/btn_x_yellow.png")
            {
                //sound->playClickX();
                this->removeFromParentAndCleanup(true);
            }
        }
    }
    
    for (int i = 0 ; i < names9.size() ; i++)
    {
        if (names9[i] == "background/bg_board_brown.png2")
        {
            if (sprites9[i]->boundingBox().containsPoint(point))
            {
                // coco의 방
                Common::ShowNextScene(this, "GameReady", "CocoRoom", false);
            }
        }
    }
    
    /*
    if (903 <= (int)point.x && (int)point.x <= 1004 &&
        1504 <= (int)point.y && (int)point.y <= 1591)
    {
        EndLayer();
    }
    else if (54 <= (int)point.x && (int)point.x <= 879 &&
        454 <= (int)point.y && (int)point.y <= 642)
    {
        // slot part
        ShowMagicSlot();
    }
    
    else if (pItemNormal->boundingBox().containsPoint(point))
    {
        // item 부분 클릭
        bItem = !bItem;
        if (bItem)
        {
            pItemClicked->setOpacity(255);
            desc->setString("[마법 폭탄] 양방향 대각선으로 4개씩 총 8칸이 터집니다. 히힛~");
        }
        else
        {
            pItemClicked->setOpacity(0);
            desc->setString("[마법 폭탄] 양방향 대각선으로 4개씩 총 8칸이 터집니다. 히힛~");
        }
    }
    */
    return true;
}


void GameReady::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void GameReady::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (!isScrolling && scrollView->boundingBox().containsPoint(point))
    {
        //sound->playMenuComeup();
        Common::ShowNextScene(this, "GameReady", "MagicList", false);
    }
    
    isTouched = false;
    isScrolling = false;
}

void GameReady::ShowMagicSlot()
{
    //CCScene* pScene = MagicSlot::scene();
    //this->addChild(pScene, 2000, 2000);
}

void GameReady::EndLayer()
{
    /*
    CCFiniteTimeAction* action =
    CCSequence::create(CCFadeOut::create(0.1f),
                       CCCallFunc::create(this, callfunc_selector(GameReady::EndLayerCallback)), NULL);
    pBackground->runAction((CCActionInterval*)action->copy()->autorelease());
    pItemNormal->runAction((CCActionInterval*)action->copy()->autorelease());
    if (bItem)
        pItemClicked->runAction((CCActionInterval*)action->copy()->autorelease());
     */
}

void GameReady::EndLayerCallback()
{
    /*
    this->removeChild(pItemNormal);
    this->removeChild(pItemClicked);
    this->removeChild(pBackground);
    this->removeChild(pDescription);
    this->removeChild(pBlack);
    pItemNormal->autorelease();
    pItemClicked->autorelease();
    pBackground->autorelease();
    pDescription->autorelease();
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_description.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_item_normal.png");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_item_clicked.png");
    
    this->removeFromParentAndCleanup(true);
     */
}

void GameReady::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void GameReady::scrollViewDidZoom(CCScrollView* view)
{
}