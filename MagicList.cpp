#include "MagicList.h"

CCScene* MagicList::scene()
{
    CCScene* pScene = CCScene::create();
    MagicList* pLayer = MagicList::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void MagicList::onEnter()
{
    CCLog("MagicList :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void MagicList::onExit()
{
    CCLog("MagicList :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void MagicList::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}


bool MagicList::init()
{
    CCLog("MagicList :: Init");
	if (!CCLayer::init())
	{
		return false;
	}
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    InitSprites();
    
    return true;
}

void MagicList::InitSprites()
{
    spriteClass = new SpriteClass();
    
    layer = CCLayer::create();
    layer->setPosition(0, winSize.height);
    this->addChild(layer, 0);
    
    // background
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_brown.png",
                        ccp(0, 0), ccp(49, 688), CCSize(982, 1066), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png1",
                        ccp(0, 0), ccp(77, 952), CCSize(929, 672), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_board_yellow.png2",
                        ccp(0, 0), ccp(77, 726), CCSize(929, 200), "", "Layer", layer, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_x_brown.png",
                        ccp(0, 0), ccp(900, 1638), CCSize(0, 0), "", "Layer", layer, 20) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "letter/letter_subtitle_sketchbookindex.png",
                        ccp(0, 0), ccp(106, 1657), CCSize(0, 0), "", "Layer", layer, 20) );
    
    char name[50], name2[50];
    for (int i = 0 ; i < 4; i++)
    {
        for (int j = 0 ; j < 4; j++)
        {
            sprintf(name, "background/bg_skill_yellow.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name,
                        ccp(0, 0), ccp(127+j*229, 1451-i*160), CCSize(0, 0), "", "Layer", layer, 20) );
            
            if (i == 0) sprintf(name2, "icon/icon_skill_division_red.png%d", j);
            else if (i == 1) sprintf(name2, "icon/icon_skill_division_blue.png%d", j);
            else if (i == 2) sprintf(name2, "icon/icon_skill_division_green.png%d", j);
            else sprintf(name2, "icon/icon_skill_division_purple.png%d", j);
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                        ccp(0, 0), ccp(127+j*229, 1539-i*160), CCSize(0, 0), "", "Layer", layer, 20) );
            /*
            sprintf(name2, "background/bg_skill_select.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                ccp(0.5, 0.5), spriteClass->FindParentCenterPos(name), CCSize(0, 0), "", "Layer", layer, 20) );
            sprintf(name2, "background/bg_skill_yellow.png%c", (4*i+j)+'a');
            spriteClass->spriteObj.push_back( SpriteObject::Create(0, name2,
                        ccp(0, 0), ccp(1545-i*160, 127+j*229), CCSize(0, 0), "", "Layer", layer, 20) );
             */
        }
    }
    
    // 스킬 선택/비선택할 때 설명 text
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabelArea("", fontList[0],
                36, ccp(0, 0), ccp(77+15, 726+15), ccc3(0,0,0), CCSize(929-15*2, 200-15*2),
                kCCTextAlignmentLeft, kCCVerticalTextAlignmentCenter, "", "Layer", layer, 10) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        spriteClass->AddChild(i);
    }

    // action
    CCActionInterval* move = CCMoveTo::create(0.5f, ccp(0, 0));
    CCActionInterval* action = CCEaseElasticOut::create((CCActionInterval*)move, 0.5f);
    layer->runAction(action);
}


bool MagicList::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    CCLog("MagicList : (%d , %d)", (int)point.x, (int)point.y);
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_x_brown.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
                EndLayer();
        }
    }
    
    return true;
}


void MagicList::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void MagicList::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
}


void MagicList::EndLayer()
{
    CCFiniteTimeAction* action =
    CCSequence::create(CCMoveTo::create(0.2f, ccp(0, winSize.height)),
                       CCCallFunc::create(this, callfunc_selector(MagicList::EndLayerCallback)), NULL);
    layer->runAction(action);
}

void MagicList::EndLayerCallback()
{
    //this->removeChild(pBlack);
    //this->removeChild(pBackground);
    //pBackground->autorelease();
    //tBackground->autorelease();
    //CCTextureCache::sharedTextureCache()->removeTextureForKey("images/gameready_MagicList_detail.png");
    //CCLog("MagicList :: ELC - tBackground retain : %d", tBackground->retainCount());
    //CCLog("MagicList :: init - pBackground retain : %d", pBackground->retainCount());
    this->removeFromParentAndCleanup(true);
    //CCLog("MagicList :: ELC - tBackground retain : %d", tBackground->retainCount());
    //CCLog("MagicList :: init - pBackground retain : %d", pBackground->retainCount());
}
