#include "PuzzlePause.h"
#include "Puzzle.h"

using namespace cocos2d;

static int height;

CCScene* PuzzlePause::scene(int h)
{
    height = h;
    CCScene* pScene = CCScene::create();
    PuzzlePause* pLayer = PuzzlePause::create();
    pScene->addChild(pLayer);
    
	return pScene;
}

void PuzzlePause::onEnter()
{
    CCLog("PuzzlePause :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, Depth::GetCurPriority(), true);
    CCLayer::onEnter();
}
void PuzzlePause::onExit()
{
    CCLog("PuzzlePause :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void PuzzlePause::keyBackClicked()
{
    /*
    if (!isKeybackTouched)
    {
        isKeybackTouched = true;
        ResumeGame();
    }
     */
}


bool PuzzlePause::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    // make depth tree
    Depth::AddCurDepth("PuzzlePause", this);
    
    //this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    this->setTouchPriority(Depth::GetCurPriority());
    
    // notification
    CCString* param = CCString::create("1");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    
    m_winSize = CCDirector::sharedDirector()->getWinSize();
    
    spriteClass = new SpriteClass();
    
    InitSprites();
    //InitSprites2();
    
    isKeybackTouched = false;
    
    CCLog("PuzzlePause : init - done");
    
    return true;
}

void PuzzlePause::InitSprites2()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(130);
    this->addChild(pBlack, 0);

    /*
    char name[50];
    sprintf(name, "background/bg_degree_desc.png");
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, name, ccp(0, 0), ccp(300, height), CCSize(400+40+40, 80+20+10+50+20), "", "PuzzlePause", this, 9) );
    CCPoint p = spriteClass->FindParentCenterPos(name);
    spriteClass->spriteObj.push_back( SpriteObject::CreateLabel("잠시 쉬고 있는 중이에요.", fontList[0], 36, ccp(0, 0), ccp(p.x-20-200+5, p.y+15), ccc3(255,255,255), name, "1", NULL, 10, 1) );
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png1", ccp(1, 1), ccp(p.x-10, p.y-20), CCSize(200, 80), name, "1", NULL, 10, 1) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(1, "background/bg_cocoroom_desc.png2", ccp(0, 1), ccp(p.x+10, p.y-20), CCSize(200, 80), name, "1", NULL, 10, 1) );
    */
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "PuzzlePause", this, 9) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}

void PuzzlePause::InitSprites()
{
    pBlack = CCSprite::create("images/ranking_scrollbg.png", CCRectMake(0, 0, m_winSize.width, m_winSize.height));
    pBlack->setPosition(ccp(0, 0));
    pBlack->setAnchorPoint(ccp(0, 0));
    pBlack->setColor(ccc3(0, 0, 0));
    pBlack->setOpacity(180);
    this->addChild(pBlack, 0);
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "background/bg_pause_window.png", ccp(0.5, 0.5), ccp(m_winSize.width/2, m_winSize.height/2), CCSize(0,0), "", "PuzzlePause", this, 9) );
    ((CCSprite*)spriteClass->FindSpriteByName("background/bg_pause_window.png"))->setScale(1.05f);
    
    
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_cont.png", ccp(1, 0.5), ccp(m_winSize.width/2-10, m_winSize.height/2-70), CCSize(0,0), "", "PuzzlePause", this, 10) );
    spriteClass->spriteObj.push_back( SpriteObject::Create(0, "button/btn_pause_end.png", ccp(0, 0.5), ccp(m_winSize.width/2+10, m_winSize.height/2-70), CCSize(0,0), "", "PuzzlePause", this, 10) );
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
        spriteClass->AddChild(i);
}


bool PuzzlePause::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    rect = CCRectZero;
    kind = -1;
    idx = -1;
    
    for (int i = 0 ; i < spriteClass->spriteObj.size() ; i++)
    {
        if (spriteClass->spriteObj[i]->name == "button/btn_pause_cont.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                //ResumeGame();
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_pause_cont.png"))->setColor(ccc3(140,140,140));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_RESUMEGAME;
                idx = i;
                return true;
            }
        }
        else if (spriteClass->spriteObj[i]->name == "button/btn_pause_end.png")
        {
            if (spriteClass->spriteObj[i]->sprite->boundingBox().containsPoint(point))
            {
                //EndGame();
                ((CCSprite*)spriteClass->FindSpriteByName("button/btn_pause_end.png"))->setColor(ccc3(140,140,140));
                rect = spriteClass->spriteObj[i]->sprite->boundingBox();
                kind = BTN_MENU_ENDGAME;
                idx = i;
                return true;
            }
        }
    }
    
    return true;
}


void PuzzlePause::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
}

void PuzzlePause::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    
    if (idx > -1)
    {
        spriteClass->spriteObj[idx]->sprite->setColor(ccc3(255,255,255));
    }
    if (rect.containsPoint(point))
    {
        if (kind == BTN_MENU_RESUMEGAME)
            ResumeGame();
        else if (kind == BTN_MENU_ENDGAME)
            EndGame();
    }
}


void PuzzlePause::EndGame()
{
    // 먼저, 끝났다는 flag를 미리 보낸다. (isGameOver = true로 만든다)
    CCString* param = CCString::create("3");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    
    EndScene();
    
    // Puzzle에게도 끝내고 Ranking으로 돌아가라고 알려준다.
    param = CCString::create("2");
    CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
}


void PuzzlePause::ResumeGame()
{
    // Loading 화면으로 MESSAGE request 넘기기
    Common::ShowNextScene(this, "PuzzlePause", "Loading", false, LOADING_MESSAGE);
    
    std::string param = "";
    char temp[40];
    sprintf(temp, "kakao_id=%s", myInfo->GetKakaoId().c_str());
    param += temp;
    
    Network::HttpPost(param, URL_SESSION_CHECK, this, httpresponse_selector(PuzzlePause::onHttpRequestCompleted));
}

void PuzzlePause::EndScene()
{
    // release depth tree
    Depth::RemoveCurDepth();
    
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking_scrollbg.png");
    
    // remove all objects
    spriteClass->RemoveAllObjects();
    delete spriteClass;
    pBlack->removeFromParentAndCleanup(true);
    
    this->removeFromParentAndCleanup(true);
}



void PuzzlePause::onHttpRequestCompleted(CCNode *sender, void *data)
{
    // Loading 창 끄기
    ((Loading*)Depth::GetCurPointer())->EndScene();
    
    CCHttpResponse* res = (CCHttpResponse*) data;
    
    xml_document xmlDoc;
    Network::GetXMLFromResponseData(res, xmlDoc);
 
    XmlParseSessionCheck(&xmlDoc);
}

void PuzzlePause::XmlParseSessionCheck(xml_document *xmlDoc)
{
    xml_node nodeResult = xmlDoc->child("response");
    int code = nodeResult.child("code").text().as_int();
    
    // 에러일 경우 code에 따라 적절히 팝업창 띄워줌.
    if (code != 0)
    {   
        if (code <= MAX_COMMON_ERROR_CODE)
            Network::ShowCommonError(code);
    }
    
    else if (code == 0)
    {
        EndScene();
        isInGamePause = false;
        
        CCString* param = CCString::create("0");
        CCNotificationCenter::sharedNotificationCenter()->postNotification("Puzzle", param);
    }
}


