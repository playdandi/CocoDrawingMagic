#include "Ranking.h"
#include "cocos-ext.h"
#include "BuyTopaz.h"
#include "GameReady.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

enum
{
	zBackground = 0,
	zGameObject = 1,
    zGameObjectSP = 2,
    zPieceConn = 3,
};

CCScene* Ranking::scene()
{
	CCScene* pScene = CCScene::create();
    
	Ranking* pLayer = Ranking::create();
	pScene->addChild(pLayer);
    
	return pScene;
}

void Ranking::onEnter()
{
    CCLog("Ranking :: onEnter");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Ranking::onPause()
{
    CCLog("Ranking :: onPause");
    //CCDirector* pDirector = CCDirector::sharedDirector();
    //pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCLayer::onEnter();
}
void Ranking::onExit()
{
    CCLog("Ranking :: onExit");
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

void Ranking::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
    //Common::ShowNextScene(this, "Ranking", "Exit", false);
}

bool Ranking::init()
{
	if (!CCLayer::init())
	{
		return false;
	}
    
    srand(time(NULL));
    
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
   // names.push_back("potion_time_plus.png");
    names.push_back("strap/strap_red.png");
    names.push_back("strap/strap_title_rank.png");
    names.push_back("background/bg_rank_time.png");
    names.push_back("button/btn_sketchbook.png");
    names.push_back("letter/letter_sketchbook.png");
    names.push_back("button/btn_addfriend.png");
    names.push_back("letter/letter_addfriend.png");
    names.push_back("button/btn_red.png");
    names.push_back("letter/letter_gameready.png");
    names.push_back("button/btn_setting.png");
    names.push_back("button/btn_msg.png");
    //names.push_back("background/bg_msg_setting.png1");
    //names.push_back("background/bg_msg_setting.png2");
    
    //CCSprite* setting = CCSprite::createWithSpriteFrameName("setting.png");
    //setting->setAnchorPoint(ccp(0, 0));
    //setting->setPosition(ccp(750, 1498)); // roof 위
    //setting->setPosition(ccp(319+446/4, 191+160+15)); // 게임준비 버튼 위
    //this->addChild(setting, 2);
    //CCSprite* setting_bg = CCSprite::createWithSpriteFrameName("msg_setting_bg.png");
    //setting_bg->setAnchorPoint(ccp(0, 0));
    //setting_bg->setPosition(ccp(setting->getContentSize().width-30, setting->getContentSize().height-30));
    //setting->addChild(setting_bg, 2);
    
    //CCSprite* msg = CCSprite::createWithSpriteFrameName("msg.png");
    //msg->setAnchorPoint(ccp(0, 0));
    //msg->setPosition(ccp(750+90+35, 1498));  // roof 위
    //msg->setPosition(ccp(319+446/4+90+35, 191+160+15)); // 게임준비 버튼 위
    //this->addChild(msg, 2);
    //CCSprite* msg_bg = CCSprite::createWithSpriteFrameName("msg_setting_bg.png");
    //msg_bg->setAnchorPoint(ccp(0, 0));
    //msg_bg->setPosition(ccp(msg->getContentSize().width-30, msg->getContentSize().height-30));
    //msg->addChild(msg_bg, 2);

    
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
    //pos.push_back(ccp(645, 1498));
    pos.push_back(ccp(14, 1343));
    pos.push_back(ccp(118, 1920-441-88)); // 주간랭킹
    pos.push_back(ccp(423, 1391));
    pos.push_back(ccp(67, 1920-1569-96)); // sketchbook
    pos.push_back(ccp(53, 1920-1674-44));
    pos.push_back(ccp(912, 1920-1582-86));
    pos.push_back(ccp(886, 1920-1674-44));
    pos.push_back(ccp(319, 191));
    pos.push_back(ccp(0, 0));
    //pos.push_back(ccp(319+446/4, 191+160+15));
    //pos.push_back(ccp(319+446/4+90+35, 191+160+15));
    pos.push_back(ccp(750, 1498));
    pos.push_back(ccp(750+90+35, 1498));
    
    // for 9sprites
    names9.push_back("background/bg_topinfo.png"); // for topaz
    names9.push_back("background/bg_topinfo.png"); // for starcandy
    names9.push_back("background/bg_topinfo.png"); // for magicpoint
    names9.push_back("background/bg_board_brown.png");
    names9.push_back("background/bg_board_yellow.png");
    pos9.push_back(ccp(80, 1666));
    pos9.push_back(ccp(390, 1666));
    pos9.push_back(ccp(765, 1666));
    pos9.push_back(ccp(49, 458));
    pos9.push_back(ccp(75, 492));
    size9.push_back(ccp(230, 75));
    size9.push_back(ccp(290, 75));
    size9.push_back(ccp(290, 75));
    size9.push_back(ccp(982, 954));
    size9.push_back(ccp(929, 904));
    
    
    /*
     - RGBA8888(kTexture2DPixelFormat_RGBA8888) : 최대 품질을 보장하며 32비트 픽셀 포맷이다. 단점은 다른 포맷들에 비해 메모리를 많이 소모한다는 점이다. 16비트 텍스처의 두 배나 되기 때문에 렌더링도 느리다. 이 픽셀 포맷이 적용된 이미지는 1024×1024 해상도에서 4MB, 2048×2048 해상도에서 16MB의 메모리를 사용한다. 다음에 설명할 RGB4444를 적용하면 이를 반으로 줄일 수 있다.
     - RGBA4444(kTexture2DPixelFormat_RGBA4444) : 16비트 픽셀 포맷으로 적당한 수준의 품질과 속도를 보장하며, 메모리도 그다지 많이 사용하지 않는다.
     - RGB5_A1( kTexture2DPixelFormat_RGB5A) : 역시 16비트 픽셀 포맷이며 RGB 채널에는 상대적으로 높은 품질을 보장하지만, 알파 채널에는 1비트만 할당했기 때문에 상세한 표현이 불가능하다. 평균적인 속도와 메모리 사용량을 갖는다.
     - RGB565(kTexture2DPixelFormat_RGB565) : 알파 채널을 지원하지 않지만 16비트 텍스체 중 가장 나은 RGB 품질을 갖는다.
     */
    //CCTexture2D::setDefaultAlphaPixelFormat(kTexture2DPixelFormat_RGBA4444);
    /*
    CCTexture2D::setDefaultAlphaPixelFormat(kTexture2DPixelFormat_RGBA4444);
    
    // load images
    tBackground = CCTextureCache::sharedTextureCache()->addImage("images/ranking.png");
    //CCLog("%s", tBackground->stringForFormat());
    tPotion1 = CCTextureCache::sharedTextureCache()->addImage("images/ranking_potion1.png");
    tPotion2 = CCTextureCache::sharedTextureCache()->addImage("images/ranking_potion2.png");
    tPotion3 = CCTextureCache::sharedTextureCache()->addImage("images/ranking_potion3.png");
    tProfile = CCTextureCache::sharedTextureCache()->addImage("images/ranking_profile.png");
    tNameScore = CCTextureCache::sharedTextureCache()->addImage("images/ranking_namescore.png");
    tScrollBg = CCTextureCache::sharedTextureCache()->addImage("images/ranking_scrollbg.png");
     */
    
    /*
    
    */

    /*
    // remove CCTexture2Ds
    CCTextureCache::sharedTextureCache()->removeTextureForKey("images/ranking.png");
    tBackground = NULL;
    */
    

    /*
    CCSprite* s1 = CCSprite::create();
    CCSpriteFrame* f1 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("topaz");
    s1->setDisplayFrame(f1);
    s1->setPosition(ccp(100,100));
    batch->addChild(s1);
    */
    
    /*CCTexture2D* bgbg = CCTextureCache::sharedTextureCache()->addImage("images/ranking.png");
    //CCSprite* bg = CCSprite::create("images/ranking_scrollbg.png");
    CCSprite* bg = new CCSprite();
    bg->initWithTexture(bgbg, CCRectMake(0, 0, 1080, 1920));
    bg->setAnchorPoint(ccp(0, 0));
    bg->setPosition(ccp(0, 0));
    bg->setOpacity(120);
    this->addChild(bg, 0);*/
    
    //CCSpriteBatchNode *batch = CCSpriteBatchNode::create("images/icon.png");
    //this->addChild(batch, 0);
    
    
    //plistDict = CCDictionary::createWithContentsOfFile("images/test.plist");
    //frames = (CCDictionary*)plistDict->objectForKey("frames");
    
    /*
    CCString* strXML = CCString::createWithFormat("images/icon.plist");
    unsigned long fsize = 0;
    unsigned char* buffer = CCFileUtils::sharedFileUtils()->getFileData(strXML->getCString(), "r", &fsize);
    xml_document doc;
    xml_parse_result result = doc.load_buffer(buffer, fsize);
    if (!result) {
        CCLog("%s", result.description());
    }
    
    //const char* k = doc.child("dict").children("key");
    xml_object_range it = doc.child("dict").children("key");
    CCLog("key: %s", k);
    
    xml_node nodeRes = doc.child("frames");
    int temp = nodeRes.child("topaz").child("posX").text().as_int();//const char* temp
    int intt = nodeRes.child("topaz").first_attribute().as_int();
    CCLog("%d", temp);
    CCLog("%d", intt);
    //for (xml_node_iterator it = temp.begin(); it != temp.end(); ++it) {
        //CCLog(it);
    //}
    int posX = nodeRes.child("topaz").attribute("posX").as_int();
    int posY = nodeRes.child("topaz").attribute("posY").as_int();
    CCLog("%d %d", posX, posY);
    */
    
    AddSprites();
    MakeScroll();
    
    //sound->PreLoadSound();
    //sound->playBackgroundSound();
    
    this->setKeypadEnabled(true);
    this->setTouchEnabled(true);
    
	return true;
}

void Ranking::AddSprites()
{
    /*
    CCSprite* temp = CCSprite::create("images/ranking.png");
    temp->setAnchorPoint(ccp(0, 0));
    temp->setPosition(ccp(0, 0));
    temp->setOpacity(100);
    this->addChild(temp, 0);
    */
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/icons.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("images/icons2.plist");
    
    
    for (int i = 0 ; i < names.size() ; i++)
    {
        CCSprite* temp;
        if (names[i].substr(names[i].find('.')) != ".png")
            temp = CCSprite::createWithSpriteFrameName(names[i].substr(0, names[i].size()-1).c_str());
        else
            temp = CCSprite::createWithSpriteFrameName(names[i].c_str());
        //temp->setAnchorPoint(Common::GetAnchorPoint((CCDictionary*)frames->objectForKey(names[i])));
        //temp->setPosition(Common::GetPosition((CCDictionary*)frames->objectForKey(names[i])));
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(pos[i]);
        if (names[i] == "letter/letter_gameready.png") {
            temp->setAnchorPoint(ccp(0.5, 0.5));
            temp->setPosition(ccp(319+446/2, 191+160/2+5));
        }
        else if (names[i] == "background/bg_roof.png") {
            CCSprite* temp2 = CCSprite::createWithSpriteFrameName(names[i].c_str());
            temp2->setAnchorPoint(ccp(0, 0));
            temp2->setPosition(ccp((int)pos[i].x+temp2->getContentSize().width, (int)pos[i].y));
            temp2->setFlipX(true);
            this->addChild(temp2, 0);
        }
        else if (names[i] == "button/btn_setting.png") {
            /*
            CCSprite* setting_bg = CCSprite::createWithSpriteFrameName("background/bg_msg_setting.png");
            setting_bg->setAnchorPoint(ccp(0, 0));
            setting_bg->setPosition(ccp(temp->getContentSize().width-30, temp->getContentSize().height-30));
            temp->addChild(setting_bg, 2);
            
            CCLabelTTF* setting_new = CCLabelTTF::create("N", fontList[0].c_str(), 30);
            setting_new->setAnchorPoint(ccp(0.5, 0.5));
            setting_new->setPosition(ccp(setting_bg->getContentSize().width/2, setting_bg->getContentSize().height/2));
            setting_new->setColor(ccc3(255, 255, 255));
            setting_bg->addChild(setting_new, 2);
             */
        }
        else if (names[i] == "button/btn_msg.png") {
            CCSprite* msg_bg = CCSprite::createWithSpriteFrameName("background/bg_msg_setting.png");
            msg_bg->setAnchorPoint(ccp(0, 0));
            msg_bg->setPosition(ccp(temp->getContentSize().width-30, temp->getContentSize().height-30));
            temp->addChild(msg_bg, 2);
            
            CCLabelTTF* msg_number = CCLabelTTF::create("88", fontList[0].c_str(), 30);
            msg_number->setAnchorPoint(ccp(0.5, 0.5));
            msg_number->setPosition(ccp(msg_bg->getContentSize().width/2, msg_bg->getContentSize().height/2));
            msg_number->setColor(ccc3(255, 255, 255));
            msg_bg->addChild(msg_number, 2);
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
    for (int i = 0 ; i < 5; i++) {
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
        CCScale9Sprite* temp = CCScale9Sprite::createWithSpriteFrameName(names9[i].c_str());
        temp->setContentSize(CCSizeMake((int)size9[i].x, (int)size9[i].y));
        temp->setAnchorPoint(ccp(0, 0));
        temp->setPosition(pos9[i]);
        this->addChild(temp, 0);
        sprites9.push_back(temp);
    }
    
    
    // labels
    CCLabelTTF* topazNumber = CCLabelTTF::create("88,888", fontList[0].c_str(), 36);
    topazNumber->setAnchorPoint(ccp(0, 0));
    topazNumber->setPosition(ccp(105, 1686));
    topazNumber->setColor(ccc3(255, 255, 255));
    this->addChild(topazNumber, 1);
    CCLabelTTF* starcandyNumber = CCLabelTTF::create("200,000", fontList[0].c_str(), 36);
    starcandyNumber->setAnchorPoint(ccp(0, 0));
    starcandyNumber->setPosition(ccp(415, 1686));
    starcandyNumber->setColor(ccc3(255, 255, 255));
    this->addChild(starcandyNumber, 1);
    CCLabelTTF* mpNumber = CCLabelTTF::create("5,200,000", fontList[0].c_str(), 36);
    mpNumber->setAnchorPoint(ccp(0, 0));
    mpNumber->setPosition(ccp(795, 1686));
    mpNumber->setColor(ccc3(255, 255, 255));
    this->addChild(mpNumber, 1);
    
    CCLabelTTF* potionTime = CCLabelTTF::create("11:59", fontList[0].c_str(), 36);
    potionTime->setAnchorPoint(ccp(0, 0));
    potionTime->setPosition(ccp(530, 1508));
    potionTime->setColor(ccc3(255, 255, 255));
    this->addChild(potionTime, 1);
    
    CCLabelTTF* rankTime2 = CCLabelTTF::create("23:50", fontList[0].c_str(), 48);
    rankTime2->setAnchorPoint(ccp(0.5, 0.5));
    rankTime2->setPosition(ccp(498, 1445));
    rankTime2->setColor(ccc3(152, 49, 64));
    this->addChild(rankTime2, 2);
    CCLabelTTF* rankTime1 = CCLabelTTF::create("23:50", fontList[0].c_str(), 48);
    rankTime1->setAnchorPoint(ccp(0.5, 0.5));
    rankTime1->setPosition(ccp(498, 1448));
    rankTime1->setColor(ccc3(255, 255, 255));
    this->addChild(rankTime1, 2);
    CCLabelTTF* rankTime3 = CCLabelTTF::create("남음", fontList[0].c_str(), 30);
    rankTime3->setAnchorPoint(ccp(0, 0));
    rankTime3->setPosition(ccp(550, 1400));
    rankTime3->setColor(ccc3(212, 212, 212));
    this->addChild(rankTime3, 2);
    
    
    /*
    // scroll bg
    pScrollBg = new CCSprite();
    pScrollBg->initWithTexture(tScrollBg, CCRectMake(107, 512, 860, 837));
    pScrollBg->setAnchorPoint(ccp(0, 0));
    pScrollBg->setPosition(ccp(107, 512));
    addChild(pScrollBg, zBackground);
    */
    // scroll 배경 범위 어디까지?
    // x : 112~962
    // y : 512~1330
    /*
    background = new CCSprite();
    background->initWithTexture(pPuzzleBackground, CCRectMake(0, 0, 768, 1024));
    background->setAnchorPoint(ccp(0, 0));
    background->setPosition(ccp(0, 0));
    addChild(background, 0);
    
    pieceLayer = CCLayer::create();
    pieceLayer->setPosition(ccp(0, 0));
    CCSize visibleSize(768, 768);
    
    CCDrawNode* stencil = CCDrawNode::create();
    CCPoint ver[] = {ccp(0, 0), ccp(768, 0), ccp(768, 6*108+6), ccp(768-6-108+32, 6*108+6),
        ccp(768-6-108, 6*108+6+32), ccp(768-6-108, 7*108+6-32), ccp(768-6-108-32, 7*108+6),
        ccp(6+108+32, 7*108+6), ccp(6+108, 7*108+6-32), ccp(6+108, 6*108+6+32),
        ccp(6+108-32, 6*108+6), ccp(6+32, 6*108+6), ccp(6, 6*108+6-32)};
    stencil->drawPolygon(ver, 13, ccc4f(0,0,0,255), 0, ccc4f(0,0,0,255));
    
    CCClippingNode* clip = CCClippingNode::create(stencil);
    clip->addChild(pieceLayer);
    addChild(clip);
    
    puzzleFrame = new CCSprite();
    puzzleFrame->initWithTexture(pPuzzleFrame, CCRectMake(0, 0, 768, 768));
    puzzleFrame->setAnchorPoint(ccp(0, 0));
    puzzleFrame->setPosition(ccp(0, 0));
    pieceLayer->addChild(puzzleFrame, 0);
    */
}

void Ranking::MakeScroll()
{
    // usernames (temporary)
    usernames.push_back("박일진");
    usernames.push_back("동해물과");
    usernames.push_back("정연준");
    usernames.push_back("Yhlas Beganov");
    usernames.push_back("Sera");
    usernames.push_back("AHRORO");
    usernames.push_back("TREVI");
    usernames.push_back("칸쵸");
    usernames.push_back("로드리게스");
    usernames.push_back("yo so sexy");
    usernames.push_back("kimochi!");
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_NOTHING);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_X);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_X);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_SEND);
    userPotionStates.push_back(POTION_REMAIN);
    userPotionStates.push_back(POTION_SEND);
    
    
    // make scroll
    CCLayer* scrollContainer = CCLayer::create();
    scrollContainer->setAnchorPoint(ccp(0, 1));
    scrollContainer->setPosition(ccp(77, 492+904));
    int numOfList = 11;
    
    char rankNum[3];
    for (int i = 0 ; i < numOfList ; i++)
    {
        CCLayer* profileLayer = CCLayer::create();
        profileLayer->setContentSize(CCSizeMake(862, 166));
        //profileLayer->setAnchorPoint(ccp(0, 1));
        profileLayer->setPosition(ccp(34, (numOfList-i-1)*166));
        scrollContainer->addChild(profileLayer, 2);
        
        // my profile bg
        if (usernames[i] == "정연준")
        {
            CCScale9Sprite* my_profile_bg = CCScale9Sprite::createWithSpriteFrameName("background/bg_my_profile.png");
            my_profile_bg->setContentSize(CCSizeMake(862, 166-10));
            my_profile_bg->setAnchorPoint(ccp(0, 0));
            my_profile_bg->setPosition(ccp(0, 15));
            profileLayer->addChild(my_profile_bg, 2);
        }
        
        // rank number
        if (i < 3) // 1~3위는 이미지 사용
        {
            char number[28];
            sprintf(number, "icon/icon_rank_number_%d.png", i+1);
            CCSprite* rank = CCSprite::createWithSpriteFrameName(number);
            rank->setAnchorPoint(ccp(0, 0));
            if (i == 0)
                rank->setPosition(ccp(0, 65));
            else
                rank->setPosition(ccp(8, 77));
            profileLayer->addChild(rank, 2);
        }
        else // 4위부터는 일반 font 사용
        {
            sprintf(rankNum, "%d", i+1);
            CCLabelTTF* rank = CCLabelTTF::create(rankNum, fontList[0].c_str(), 36);
            //CCRenderTexture* stroke = Common::CreateStroke(rank, 2, ccc3(0, 0, 0), 255);
            rank->setAnchorPoint(ccp(0, 0));
            rank->setPosition(ccp(32, 115));
            rank->setColor(ccc3(78, 47, 8));
            profileLayer->addChild(rank, 2);
            //profileLayer->addChild(stroke, 2);
        }
        
        
        // profile image
        CCSprite* profile = CCSprite::createWithSpriteFrameName("background/bg_profile.png");
        profile->setAnchorPoint(ccp(0, 0));
        profile->setPosition(ccp(102, 36));
        profileLayer->addChild(profile, 2);
        profiles.push_back(profile);
        //names.push_back("background/bg_profile.png");
        //sprites.push_back(profile);
        //CCLog("%d : %d, %d", i, (int)profile->getPosition().x, (int)profile->getPosition().y);
         
        /*
        CCScale9Sprite* profile = CCScale9Sprite::createWithSpriteFrameName("background/bg_profile.png");
        profile->setAnchorPoint(ccp(0, 0));
        profile->setContentSize(CCSize(120, 125));
        CCControlButton* profilebtn = CCControlButton::create(profile);
        profilebtn->setAnchorPoint(ccp(0, 0));
        profilebtn->setPosition(ccp(102, 36));
        profilebtn->addTargetWithActionForControlEvents(this, cccontrol_selector(Ranking::touchDownAction), CCControlEventTouchDown);
        profileLayer->addChild(profilebtn, 2);
        profilebtn->setTouchPriority(-100);
        */
        
        // user name
        CCLabelTTF* username = CCLabelTTF::create(usernames[i].c_str(), fontList[0].c_str(), 48);
        //CCRenderTexture* stroke = Common::CreateStroke(username, 2, ccc3(0, 0, 0), 255);
        username->setAnchorPoint(ccp(0, 0));
        username->setPosition(ccp(252, 110));
        username->setColor(ccc3(78, 47, 8));
        profileLayer->addChild(username, 2);
        //profileLayer->addChild(stroke, 2);
        
        // user score
        /*
        CCLabelTTF* score2 = CCLabelTTF::create("12,345,678", "NanumGothicExtraBold", 50);
        score2->setPosition(ccp(284, 34));
        score2->setColor(ccc3(78, 47, 8));
        score2->setAnchorPoint(ccp(0, 0));
        profileLayer->addChild(score2, 2);
        CCLabelTTF* score = CCLabelTTF::create("12,345,678", "NanumGothicExtraBold", 50);
        score->setPosition(ccp(282, 36));
        score->setAnchorPoint(ccp(0, 0));
        profileLayer->addChild(score, 2);
        */
        /*
        CCLabelAtlas* score = CCLabelAtlas::create("12/345/678", "images/number_score.png", 44, 60, '/');
        score->setPosition(ccp(282, 36));
        profileLayer->addChild(score, 2);
        */
        CCLayer* scoreLayer = Common::MakeImageNumberLayer("14,083,569");
        scoreLayer->setPosition(ccp(282, 36));
        profileLayer->addChild(scoreLayer, 2);

        
        // potion state
        if (userPotionStates[i] != POTION_NOTHING)
        {
            char potionState[18];
            if (userPotionStates[i] == POTION_REMAIN)
                sprintf(potionState, "icon/icon_potion_remain.png");
            else if (userPotionStates[i] == POTION_SEND)
                sprintf(potionState, "icon/icon_potion_send.png");
            else
                sprintf(potionState, "icon/icon_potion_x.png");
            
            CCSprite* potion_state = CCSprite::createWithSpriteFrameName(potionState);
            potion_state->setAnchorPoint(ccp(0, 0));
            potion_state->setPosition(ccp(724, 24));
            profileLayer->addChild(potion_state, 2);
        }
        
        // dotted line
        if (i < numOfList-1)
        {
            CCSprite* dot = CCSprite::createWithSpriteFrameName("background/bg_dotted_line.png");
            dot->setAnchorPoint(ccp(0, 0));
            dot->setPosition(ccp(0, 5));
            profileLayer->addChild(dot, 2);
        }
    }
    
    // scrollview 내용 전체크기
    scrollContainer->setContentSize(CCSizeMake(862, numOfList*166));
    // scrollView 생성
    scrollView = CCScrollView::create();
    scrollView->retain();
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setViewSize(CCSizeMake(929, 904-80)); // (내용 1개 크기, 노란보드 세로크기)
    scrollView->setContentSize(scrollContainer->getContentSize());
    scrollView->setAnchorPoint(ccp(0, 0));
    scrollView->setPosition(ccp(77, 492+20));
    scrollView->setContainer(scrollContainer);
    scrollView->setDelegate(this);
    scrollView->setContentOffset(ccp(0, 904-80-(numOfList*166)), false);
    this->addChild(scrollView, 3);
    //CCLog("%d", scrollView->getTouchPriority());
    //scrollView->setTouchPriority(2);
    
    
    /*
    for (int i = 0 ; i < 10 ; i++)
    {
        CCSprite* temp = new CCSprite();
        temp->initWithTexture(tScrollBg, CCRectMake(0, 0, 860, 209));
        temp->setAnchorPoint(ccp(0, 1));
        temp->setPosition(ccp(107, (10-i)*200));
        scrollContainer->addChild(temp);
        
        CCSprite* profile = new CCSprite();
        profile->initWithTexture(tProfile, CCRectMake(0, 0, 159, 162));
        profile->setAnchorPoint(ccp(0, 0));
        profile->setPosition(ccp(0, 15));
        temp->addChild(profile);
        
        CCSprite* namescore = new CCSprite();
        namescore->initWithTexture(tNameScore, CCRectMake(0, 0, 385, 115));
        namescore->setAnchorPoint(ccp(0, 1));
        namescore->setPosition(ccp(0+159+15, temp->getContentSize().height-15));
        temp->addChild(namescore);
        
        CCSprite* potion = new CCSprite();
        potion->initWithTexture(tPotion1, CCRectMake(0, 0, 139, 132));
        potion->setAnchorPoint(ccp(1, 0));
        potion->setPosition(ccp(temp->getContentSize().width-130, 40));
        temp->addChild(potion);
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

void Ranking::touchDownAction(CCObject* sender, CCControlEvent controlEvent)
{
    CCLog("touch down action!");
}

bool Ranking::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
    isScrolling = false;
    CCPoint point = pTouch->getLocation();
    CCLog("Ranking : (%d , %d)", (int)point.x, (int)point.y);
    
    // topaz_plus
    // starcandy_plus
    // potion_plus
    // profile (in scroll)
    // potion_send (in scroll)
    // sketchbook
    // msg
    // setting
    // addfriend
    // button_red
    
    //return CCControlButton::ccTouchBegan(pTouch, pEvent);
//    return super::ccTouchBegan(pTouch, pEvent);
    //return ccTouchBegan(pTouch, pEvent);
    
    //CCLog("%d", scrollView->getContainer()->getChildrenCount());
    
    //size9.push_back(ccp(929, 904));
    //pos9.push_back(ccp(75, 492));
    CCScale9Sprite* s9;
    for (int i = 0 ; i < sprites9.size() ; i++) {
        if (names9[i] == "background/bg_board_yellow.png")
            s9 = sprites9[i];
    }
    
    if (scrollView->boundingBox().containsPoint(point)) {
        CCLog("contained");
    }
    for (int i = 0 ; i < profiles.size() ; i++) {
        if (profiles[i]->boundingBox().containsPoint(point)) {
            CCLog("%d ~~~~~~~~~~~~~~~~~~~", i);
        }
        //CCRect rect = profiles[i]->boundingBox();
        //CCLog("%d %d %d %d", (int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height);
//        CCLog("%d %d %d %d", rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY());
    }
    
    for (int i = 0 ; i < names.size() ; i++)
    {
        if (sprites[i]->boundingBox().containsPoint(point))
        {
            if (names[i] == "button/btn_red.png") {
                //sound->playClickBtn();
                Common::ShowNextScene(this, "Ranking", "GameReady", false);
            }
            else if (names[i] == "button/btn_topinfo_plus.png1")
                Common::ShowNextScene(this, "Ranking", "BuyTopaz", false);
            else if (names[i] == "button/btn_topinfo_plus.png2")
                Common::ShowNextScene(this, "Ranking", "BuyStarCandy", false);
            else if (names[i] == "button/btn_topinfo_plus.png3")
                Common::ShowNextScene(this, "Ranking", "BuyPotion", false);
            
            else if (names[i] == "button/btn_msg.png")
                Common::ShowNextScene(this, "Ranking", "Message", false);
            else if (names[i] == "button/btn_setting.png")
                Common::ShowNextScene(this, "Ranking", "Setting", false);
            
            else if (names[i] == "button/btn_sketchbook.png")
                Common::ShowNextScene(this, "Ranking", "Sketchbook", false);
            else if (names[i] == "button/btn_addfriend.png")
                Common::ShowNextScene(this, "Ranking", "InviteFriend", false);
        }
    }
    
    return true;
}


void Ranking::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
    //CCPoint point = pTouch->getLocation();
}

void Ranking::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
    CCPoint point = pTouch->getLocation();
    CCLog("Ranking END : (%d , %d)", (int)point.x, (int)point.y);
    for (int i = 0 ; i < 11; i++) {
        CCPoint p = profiles[i]->convertToNodeSpace(point);
        // convertToNodeSpace 설명
        // ex) sprite1->convertToNodeSpace(sprite2->getPosition());
        // sprite1 좌측하단부터 sprite2의 anchorPoint까지의 거리
        
        //CCPoint p = s9->convertToNodeSpace(sprites[i]->getPosition());
        //CCPoint p2 = profiles[i]->convertToWorldSpace(point);
        //CCPoint p2 = profiles[i]->getParent()->getParent()->convertTouchToNodeSpace(pTouch);
        if (!isScrolling && (int)p.x >= 0 && (int)p.y >= 0 && (int)p.x <= 120 && (int)p.y <= 125) {
            CCLog("(%d) OOOOO : %d, %d", i, (int)p.x, (int)p.y);
            Common::ShowNextScene(this, "Ranking", "Profile", false);
        }
        else
            CCLog("p : %d, %d", (int)p.x, (int)p.y);
        //CCLog("p2 : %d, %d", (int)p2.x, (int)p2.y);
        //CCLayer* temp = (CCLayer*)scrollView->getContainer()->getChildren()->objectAtIndex(i);
        //CCRect rect = temp->boundingBox();
        //CCLog("%d %d %d %d", (int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height);
    }
    
    isScrolling = false;
}


void Ranking::ShowBuyTopaz()
{
    CCScene* pScene = BuyTopaz::scene();
    this->addChild(pScene, 2000, 2000);
    //CCDirector::sharedDirector()->pushScene(CCTransitionMoveInB::create(0.1f, pScene));
}

void Ranking::ShowGameReady()
{
    CCScene* pScene = GameReady::scene();
    this->addChild(pScene, 2000, 2000);
}

void Ranking::scrollViewDidScroll(CCScrollView* view)
{
    isScrolling = true;
}

void Ranking::scrollViewDidZoom(CCScrollView* view)
{
}

//void Ranking::registerWithTouchDispatcher() {
    //CCDirector::sharedDirector()->getTouchDispatcher()->
    //addTargetedDelegate(this, CCLayer::getTouchPriority(), true);
//}
