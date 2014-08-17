#include "Common.h"
#include "Kakao/Plugins/KakaoNativeExtension.h"
#include "Loading.h"
#include "LoadingPuzzle.h"
#include "Ranking.h"
#include "GameReady.h"
#include "Sound.h"
#include "Message.h"
#include "MagicList.h"
#include "CocoRoom.h"
#include "CocoRoomTodayCandy.h"
#include "CocoRoomFairyTown.h"
#include "InviteFriend.h"
#include "BuyTopaz.h"
#include "BuyStarCandy.h"
#include "BuyPotion.h"
#include "SendTopaz.h"
#include "RequestTopaz.h"
#include "RequestPotion.h"
#include "Setting.h"
#include "Sketchbook.h"
#include "popup/WeeklyRankResult.h"
#include "popup/GetDegree.h"
#include "popup/SketchDetail.h"
#include "popup/DegreeInfo.h"
#include "popup/Profile.h"
#include "popup/FairyOneInfo.h"
#include "popup/NoImage.h"
#include "popup/SelectProperty.h"
#include "popup/Coupon.h"
#include "puzzle/Puzzle.h"
#include "puzzle/PuzzleResult.h"
#include "puzzle/PuzzlePause.h"
#include "tutorial/T_Sketchbook.h"
#include "tutorial/T_MagicList.h"
#include "tutorial/T_NoImage.h"
#include "tutorial/T_SketchDetail.h"
#include "tutorial/puzzle/T_Puzzle.h"
#include "tutorial/puzzle/T_Skip.h"
#include "Splash.h"
#include "RankUp.h"
#include "popup/AttendReward.h"
#include "puzzle/Puzzle_BuyItem.h"

Sound* sound;

std::string fontList[] = {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    "NanumGothicExtraBold",
    "NanumGothic",
    "NanumGothicBold",
    "InkInTheMeat-Tial",
    "NanumPen",
#else
    "fonts/NanumGothicExtraBold.ttf",
    "fonts/NanumGothic.ttf",
    "fonts/NanumGothicBold.ttf",
    "fonts/INKITMT.otf",
    "fonts/NanumPen.ttf",
#endif
};


std::string Common::GetTip()
{
    int c = rand()%100;
    int category;
    if (c < 10) category = 1; // 개소리 : 10%
    else category = 2; // 게임팁 : 90%
    
    // 카테고리의 idx를 모은다.
    std::vector<int> ids;
    for (int i = 0 ; i < tipContent.size() ; i++)
        if (tipContent[i]->GetCategory() == category)
            ids.push_back(i);
    
    // 랜덤하게 선택한다.
    c = rand() % (int)(ids.size());
    
    int p = ids[c];
    ids.clear();
    
    return tipContent[p]->GetContent();
}

CCLayer* Common::MakeCombo(int num)
{
    CCLayer* layer = CCLayer::create();

    char name[30];
    int totalWidth = 0;
    int totalHeight = 0;
    
    std::string commaNumber = Common::MakeComma(num);
    
    std::vector<CCSprite*> sprites;
    for (int i = 0 ; i < commaNumber.size() ; i++)
    {
        if (commaNumber[i] == ',')
            continue;
        
        sprintf(name, "number/combo_%c.png", commaNumber[i]);
        CCSprite* temp = CCSprite::createWithSpriteFrameName(name);
        temp->setTag(i);
        temp->setAnchorPoint(ccp(0, 0));
        //temp->setScale(0.7f);
        
        if (i == 0)
            temp->setPosition(ccp(0, 0));
        else
            temp->setPosition(ccp(totalWidth, totalHeight));
        layer->addChild(temp, 100);
        
        //totalWidth += (int)(temp->getContentSize().width * 0.6f) - 2;
        totalWidth += (int)temp->getContentSize().width;
        totalHeight += 3;
        
        //if (commaNumber[i]-'0' == 1)
        //    totalWidth += 2;
        
        sprites.push_back(temp);
    }
    sprites.clear();
    
    // '콤보' 글자
    CCSprite* temp = CCSprite::createWithSpriteFrameName("number/combo_combo.png");
    temp->setTag((int)commaNumber.size());
    temp->setAnchorPoint(ccp(0, 0));
    temp->setPosition(ccp(totalWidth+10, totalHeight-10));
    layer->addChild(temp, 100);
    
    layer->setTag((int)commaNumber.size());
    //layer->setContentSize(CCSize(totalWidth+10, totalHeight+10));
    return layer;
}

CCLayer* Common::MakeScoreLayer(int num)
{
    CCLayer* layer = CCLayer::create();
   
    int offset[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char name[30];
    int totalWidth = 0;
    
    std::string commaNumber = Common::MakeComma(num);
    
    std::vector<CCSprite*> sprites;
    for (int i = 0 ; i < commaNumber.size() ; i++)
    {
        CCSprite* temp;
        if (commaNumber[i] == ',')
            temp = CCSprite::createWithSpriteFrameName("number/result_comma.png");
        else
        {
            sprintf(name, "number/result_%c.png", commaNumber[i]);
            temp = CCSprite::createWithSpriteFrameName(name);
        }
        
        temp->setTag(i);
        temp->setAnchorPoint(ccp(0, 0));
        temp->setScale(0.7f);
        
        if (i == 0)
        {
            temp->setPosition(ccp(0, 0));
        }
        else
        {
            //CCSize size = sprites[sprites.size()-1]->getContentSize();
            if (commaNumber[i] == ',')
                temp->setPosition(ccp(totalWidth-2, 0));
            else
                temp->setPosition(ccp(totalWidth-2, offset[commaNumber[i]-'0']));
        }
        
        layer->addChild(temp, 100);
        
        totalWidth += (int)(temp->getContentSize().width * 0.6f) - 2;
        
        if (commaNumber[i]-'0' == 1)
            totalWidth += 2;
        
        sprites.push_back(temp);
    }
    sprites.clear();
    
    layer->setContentSize(CCSize(totalWidth, totalWidth));
    return layer;
}

std::string Common::MakeComma(int number)
{
    char temp[20];
    sprintf(temp, "%d", number);
    std::string num = temp;
    std::string result = "";
    for (int q = num.size()-1 ; q >= 0 ; q--)
    {
        result = num[q] + result;
        if (q > 0 && (num.size() - q) % 3 == 0)
            result = "," + result;
    }
    return result;
}

std::string Common::InsertComma(std::string number)
{
    std::string result = "";
    for (int i = number.size()-1 ; i >= 0 ; i--)
    {
        result = number[i] + result;
        if (i > 0 && (number.size() - i) % 3 == 0)
            result = "," + result;
    }
    return result;
}

CCLayer* Common::MakeItemNumberLayer(std::string number, float scale)
{
    CCLayer* layer = CCLayer::create();
    
    float offset[] = {0, 1, 1, 0, 0, 0, -1, 1, -1, 0};
    float offsetX[] = {2, 0, 2, 2, 2, 2, 2, 2, 2, 2};
    char name[30];
    int totalWidth = 0;
    int tag = 0;
    
    std::vector<CCSprite*> sprites;
    for (int i = number.size()-1 ; i >= 0 ; i--)
    {
        if (number[i] == ',')
            sprintf(name, "number/count_comma.png");
        else
            sprintf(name, "number/count_%c.png", number[i]);
        CCSprite* temp = CCSprite::createWithSpriteFrameName(name);
        
        //float scale = 1.0f;
        //if (type == 1) scale = 1.2f;
        //else if (type == 2) scale = 1.44f;
        temp->setScale(scale);
        
        temp->setAnchorPoint(ccp(1, 0));
        if (i == number.size()-1)
        {
            temp->setPosition(ccp(0, offset[number[i]-'0']*scale));
            totalWidth -= (temp->getContentSize().width*scale);
        }
        else
        {
            if (number[i] == ',')
            {
                temp->setPosition(ccp(totalWidth-2, 0));
                totalWidth -= (temp->getContentSize().width*scale);
            }
            else
            {
                temp->setPosition(ccp(totalWidth + offsetX[number[i]-'0']*scale, offset[number[i]-'0']*scale));
                totalWidth -= (temp->getContentSize().width*scale - offsetX[number[i]-'0']*scale);
            }
        }

        layer->addChild(temp, 100);
        temp->setTag(tag++);
        sprites.push_back(temp);
    }
    sprites.clear();
    
    layer->setContentSize(CCSize(-totalWidth, 0));
    
    return layer;
}
CCLayer* Common::MakeImageNumberLayer(std::string number, int type)
{
    CCLayer* layer = CCLayer::create();
    
    if (number == "-1") // no score
    {
        CCSprite* noscore = CCSprite::createWithSpriteFrameName("letter/letter_noscore.png");
        noscore->setAnchorPoint(ccp(0, 0));
        noscore->setPosition(ccp(0, 0));
        noscore->setScale(0.95f);
        noscore->setTag(-1);
        layer->addChild(noscore, 100);
        return layer;
    }
    
    std::string commaNumber = Common::InsertComma(number);
    
    int offset[] = {0, 1, 1, 0, 0, 0, -1, 1, -1, 0};
    char name[30];
    int totalWidth = 0;
    
    if (type == 2)
        for (int i = 0 ; i < 10 ; i++)
            offset[i] = 0;
    
    std::vector<CCSprite*> sprites;
    for (int i = 0 ; i < commaNumber.size() ; i++)
    {
        CCSprite* temp;
        if (commaNumber[i] == ',')
        {
            if (type == 0)
                temp = CCSprite::createWithSpriteFrameName("number/rank_comma.png");
            else if (type == 1)
                temp = CCSprite::createWithSpriteFrameName("number/count_comma.png");
            else if (type == 2)
                temp = CCSprite::createWithSpriteFrameName("number/result_comma.png");
        }
        else
        {
            if (type == 0)
                sprintf(name, "number/rank_%c.png", commaNumber[i]);
            else if (type == 1)
                sprintf(name, "number/count_%c.png", commaNumber[i]);
            else if (type == 2)
                sprintf(name, "number/result_%c.png", commaNumber[i]);
            temp = CCSprite::createWithSpriteFrameName(name);
        }
        
        temp->setTag(i);
        temp->setAnchorPoint(ccp(0, 0));
        
        // 퍼즐결과 화면에 쓰이면 2배 확대해 보자.
        if (type == 2)
            temp->setScale(0.8f);
        
        if (i == 0)
        {
            temp->setPosition(ccp(0, 0));
        }
        else
        {
            //CCSize size = sprites[sprites.size()-1]->getContentSize();
            if (commaNumber[i] == ',')
                temp->setPosition(ccp(totalWidth, 0));
            else
                temp->setPosition(ccp(totalWidth, offset[commaNumber[i]-'0']));
        }
        
        layer->addChild(temp, 100);
        
        if (type == 2)
            totalWidth += (int)(temp->getContentSize().width * 0.8f);
        else
            totalWidth += (int)temp->getContentSize().width;
        if (commaNumber[i]-'0' == 1)
            totalWidth += 3;
        
        sprites.push_back(temp);
    }
    sprites.clear();
    
    layer->setContentSize(CCSize(totalWidth, totalWidth));
    return layer;
}


// 다음 scene으로 이동 (popup도 가능)
// obj        : 이 함수를 불러온 scene obj
// from       : 이 함수를 불러온 scene 이름
// to         : 이동할 scene 이름
// isReplaced : replaceScene을 할 경우 true, otherwise, false.
void Common::ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc, int etc2, int etc3)
{
    if (from == "Sketchbook")
        ((Sketchbook*)obj)->SetTouchLock(false);
    
    CCScene* nextScene;
    if (to == "Ranking") nextScene = Ranking::scene(etc);
    else if (to == "GameReady") nextScene = GameReady::scene();
    else if (to == "Message") nextScene = Message::scene();
    else if (to == "MagicList") nextScene = MagicList::scene(etc);
    else if (to == "CocoRoom") nextScene = CocoRoom::scene(etc);
    else if (to == "CocoRoomTodayCandy") nextScene = CocoRoomTodayCandy::scene();
    else if (to == "CocoRoomFairyTown") nextScene = CocoRoomFairyTown::scene();
    else if (to == "InviteFriend") nextScene = InviteFriend::scene();
    else if (to == "BuyTopaz") nextScene = BuyTopaz::scene(etc);
    else if (to == "BuyStarCandy") nextScene = BuyStarCandy::scene(etc);
    else if (to == "BuyPotion") nextScene = BuyPotion::scene(etc);
    else if (to == "SendTopaz") nextScene = SendTopaz::scene(etc);
    else if (to == "RequestTopaz") nextScene = RequestTopaz::scene();
    else if (to == "RequestPotion") nextScene = RequestPotion::scene();
    else if (to == "Setting") nextScene = Setting::scene();
    else if (to == "Coupon") nextScene = Coupon::scene();
    else if (to == "Sketchbook")
    {
        std::string fromWhere = Depth::GetCurNameString();
        if (fromWhere != "Ranking" && fromWhere != "GameReady")
            fromWhere = Depth::GetParentNameString();
        
        if (fromWhere == "Ranking") nextScene = Sketchbook::scene(0);
        else if (fromWhere == "GameReady") nextScene = Sketchbook::scene(1);
    }
    else if (to == "Profile") nextScene = Profile::scene(etc);
    else if (to == "DegreeInfo") nextScene = DegreeInfo::scene();
    else if (to == "FairyOneInfo") nextScene = FairyOneInfo::scene(etc);
    else if (to == "SketchDetail") nextScene = SketchDetail::scene(etc);
    
    else if (to == "WeeklyRankResult") nextScene = WeeklyRankResult::scene();
    else if (to == "GetDegree") nextScene = GetDegree::scene();
    else if (to == "SelectProperty") nextScene = SelectProperty::scene(etc);
    
    else if (to == "Loading") nextScene = Loading::scene(etc);
    else if (to == "LoadingPuzzle") nextScene = LoadingPuzzle::scene();

    else if (to == "RankUp") nextScene = RankUp::scene();
    
    else if (to == "AttendReward") nextScene = AttendReward::scene();
    
    else if (to == "PuzzleResult") nextScene = PuzzleResult::scene();
    else if (to == "PuzzlePause") nextScene = PuzzlePause::scene(etc);
    
    else if (to == "T_Sketchbook") nextScene = T_Sketchbook::scene(etc);
    else if (to == "T_SketchDetail") nextScene = T_SketchDetail::scene(etc);
    else if (to == "T_MagicList") nextScene = T_MagicList::scene();
    else if (to == "T_Puzzle") nextScene = T_Puzzle::scene(etc);
    else if (to == "T_Skip") nextScene = T_Skip::scene(etc, etc2);
    
    else if (to == "Puzzle_BuyItem") nextScene = Puzzle_BuyItem::scene(etc);
    
    // go
    if (from == "Splash")
    {
        if (isReplaced)
            CCDirector::sharedDirector()->replaceScene(nextScene);
    }
    else if (from == "Ranking")
    {
        if (isReplaced)
            CCDirector::sharedDirector()->replaceScene(nextScene);
        else
            ((Ranking*)obj)->addChild(nextScene, 20000, 20000);
    }
    else if (from == "Loading")
    {
        ((Loading*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "LoadingPuzzle")
    {
        if (isReplaced) // to Puzzle
            CCDirector::sharedDirector()->replaceScene(Puzzle::scene(etc, etc2, etc3));
    }
        
    else if (from == "Profile") ((Profile*)obj)->addChild(nextScene, 200, 200);
    else if (from == "GameReady") ((GameReady*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "RequestTopaz") ((RequestTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyStarCandy") ((BuyStarCandy*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(nextScene, 200, 200);
    else if (from == "RequestPotion") ((RequestPotion*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Message") ((Message*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Sketchbook") ((Sketchbook*)obj)->addChild(nextScene, 200, 200);
    else if (from == "SketchDetail") ((SketchDetail*)obj)->addChild(nextScene, 200, 200);
    else if (from == "SelectProperty") ((SelectProperty*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoomTodayCandy") ((CocoRoomTodayCandy*)obj)->addChild(nextScene, 200, 200);
    else if (from == "WeeklyRankResult") ((WeeklyRankResult*)obj)->addChild(nextScene, 200, 200);
    else if (from == "MagicList") ((MagicList*)obj)->addChild(nextScene, 200, 200);
    else if (from == "InviteFriend") ((InviteFriend*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Setting") ((Setting*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Coupon") ((Coupon*)obj)->addChild(nextScene, 200, 200);
    else if (from == "FairyOneInfo") ((FairyOneInfo*)obj)->addChild(nextScene, 200, 200);
    else if (from == "RankUp")
    {
        if (isReplaced)
        {
            CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
        }
        else
            ((RankUp*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "Puzzle")
    {
        if (isReplaced)
        {
            if (to == "RankUp")
                CCDirector::sharedDirector()->replaceScene(nextScene);
            else
            {
                CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
                CCDirector::sharedDirector()->replaceScene(transition);
            }
        }
        else
            ((Puzzle*)obj)->addChild(nextScene, 9999, 9999);
    }
    else if (from == "PuzzlePause") ((PuzzlePause*)obj)->addChild(nextScene, 200, 200);
    else if (from == "PuzzleResult") ((PuzzleResult*)obj)->addChild(nextScene, 200, 200);
    else if (from == "NoImage") ((NoImage*)obj)->addChild(nextScene, 200, 200);
    
    else if (from == "T_Sketchbook") ((T_Sketchbook*)obj)->addChild(nextScene, 200, 200);
    else if (from == "T_MagicList") ((T_MagicList*)obj)->addChild(nextScene, 200, 200);
    else if (from == "T_SketchDetail") ((T_SketchDetail*)obj)->addChild(nextScene, 200, 200);
    else if (from == "T_NoImage") ((T_NoImage*)obj)->addChild(nextScene, 200, 200);
    else if (from == "T_Puzzle")
    {
        if (isReplaced)
            CCDirector::sharedDirector()->replaceScene(nextScene);
        else
            ((T_Puzzle*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "AttendReward") ((AttendReward*)obj)->addChild(nextScene, 20000, 20000);
    else if (from == "Puzzle_BuyItem") ((Puzzle_BuyItem*)obj)->addChild(nextScene, 20000, 20000);
}

// 공통된 팝업창 (버튼 1~2개, 텍스트만 변경되는 고정된 디자인의 팝업창)
void Common::ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data, int etc, int priority)
{
    if (from == "Sketchbook")
        ((Sketchbook*)obj)->SetTouchLock(false);
    else if (from == "T_Sketchbook")
        ((T_Sketchbook*)obj)->SetTouchLock(false);

    CCNode* parent;
    // replace = true 면, 순서상 EndScene을 먼저 해 줘야한다... (나중에 다시 리팩토링 해야 함)
    if (from == "SketchDetail" && isReplaced) {
        parent = ((SketchDetail*)obj)->getParent();
        ((SketchDetail*)obj)->EndScene(false);
    }
    else if (from == "T_SketchDetail") {
        parent = ((T_SketchDetail*)obj)->getParent();
        ((T_SketchDetail*)obj)->EndScene(false);
    }
    
    CCScene* popup;
    std::vector<std::string> nulld;
    if (to == "NoImage") popup = NoImage::scene(popupType, btnType, data, etc, nulld);
    else if (to == "T_NoImage") popup = T_NoImage::scene(popupType, btnType, data, etc);
    
    if (from == "Splash") ((Splash*)obj)->addChild(popup, 200, 200);
    else if (from == "Ranking") ((Ranking*)obj)->addChild(popup, 200, 200);
    else if (from == "GameReady") ((GameReady*)obj)->addChild(popup, 200, 200);
    else if (from == "WeeklyRankResult") ((WeeklyRankResult*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(popup, 20000, 20000);
    else if (from == "BuyStarCandy") ((BuyStarCandy*)obj)->addChild(popup, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(popup, 200, 200);
    else if (from == "RequestPotion") ((RequestPotion*)obj)->addChild(popup, 200, 200);
    else if (from == "RequestTopaz") ((RequestTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "Message") ((Message*)obj)->addChild(popup, 200, 200);
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(popup, 200, 200);
    else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(popup, 200, 200);
    else if (from == "CocoRoomTodayCandy") ((CocoRoomTodayCandy*)obj)->addChild(popup, 200, 200);
    else if (from == "InviteFriend") ((InviteFriend*)obj)->addChild(popup, 200, 200);
    else if (from == "FairyOneInfo") ((FairyOneInfo*)obj)->addChild(popup, 200, 200);
    else if (from == "MagicList") ((MagicList*)obj)->addChild(popup, 200, 200);
    else if (from == "RankUp") ((RankUp*)obj)->addChild(popup, 200, 200);
    else if (from == "Setting") ((Setting*)obj)->addChild(popup, 200, 200);
    else if (from == "Coupon") ((Coupon*)obj)->addChild(popup, 200, 200);
    else if (from == "Sketchbook") ((Sketchbook*)obj)->addChild(popup, 200, 200);
    else if (from == "SelectProperty") ((SelectProperty*)obj)->addChild(popup, 200, 200);
    else if (from == "SketchDetail") {
        if(isReplaced)
            parent->addChild(popup, 200, 200);
        else
            ((SketchDetail*)obj)->addChild(popup, 200, 200);
    }
    else if (from == "T_SketchDetail") {
        if(isReplaced)
            parent->addChild(popup, 200, 200);
        else
            ((T_SketchDetail*)obj)->addChild(popup, 200, 200);
    }
    else if (from == "T_Sketchbook") ((T_Sketchbook*)obj)->addChild(popup, 200, 200);
    else if (from == "NoImage") ((NoImage*)obj)->addChild(popup, 200, 200);
    else if (from == "Puzzle") ((Puzzle*)obj)->addChild(popup, 200, 200);
    else if (from == "PuzzlePause") ((PuzzlePause*)obj)->addChild(popup, 200, 200);
    else if (from == "PuzzleResult") ((PuzzleResult*)obj)->addChild(popup, 20000, 20000);
    else if (from == "Puzzle_BuyItem") ((Puzzle_BuyItem*)obj)->addChild(popup, 20000, 20000);
    else if (from == "AttendReward") ((AttendReward*)obj)->addChild(popup, 20000, 20000);
}


SpriteObject* SpriteObject::Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha, int tag)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->name = name;
    
    obj->type = spriteType;
    if (spriteType == 0) // sprite
    {
        if (name.substr(name.find('.')) != ".png")
        {
            int remain = name.size() - (name.find('.')+4);
            obj->sprite = CCSprite::createWithSpriteFrameName(name.substr(0, name.size()-remain).c_str());
        }
        else
            obj->sprite = CCSprite::createWithSpriteFrameName(name.c_str());
        obj->sprite->setAnchorPoint(ap);
        obj->sprite->setPosition(pos);
        obj->sprite->setOpacity(alpha);
        if (tag != -1)
            obj->sprite->setTag(tag);
    }
    else if (spriteType == 1) // sprite-9
    {
        if (name.substr(name.find('.')) != ".png")
        {
            int remain = name.size() - (name.find('.')+4);
            obj->sprite9 = CCScale9Sprite::createWithSpriteFrameName(name.substr(0, name.size()-remain).c_str());
        }
        else
            obj->sprite9 = CCScale9Sprite::createWithSpriteFrameName(name.c_str());
        obj->sprite9->setAnchorPoint(ap);
        obj->sprite9->setPosition(pos);
        obj->sprite9->setContentSize(size);
        obj->sprite9->setOpacity(alpha);
        if (tag != -1)
            obj->sprite9->setTag(tag);
    }

    // parent 관련 대입
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    // z-order
    obj->zOrder = zOrder;
    
    obj->priority = priority;
    
    return obj;
}

SpriteObject* SpriteObject::CreateFromSprite(int spriteType, CCSprite* spr, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha, float scale, int tag)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->name = "";
    
    obj->type = spriteType;
    
    obj->sprite = CCSprite::create();
    obj->sprite->setDisplayFrame(spr->displayFrame());
    obj->sprite->setAnchorPoint(ap);
    obj->sprite->setPosition(pos);
    obj->sprite->setOpacity(alpha);
    obj->sprite->setScale(scale);
    obj->sprite->setTag(tag);

    // parent 관련 대입
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    // z-order
    obj->zOrder = zOrder;
    
    obj->priority = priority;
    
    return obj;
}

void SpriteClass::ChangeSprite(int tag, CCSprite* sp) // 스프라이트 바꿔치기
{
    SpriteObject* so = NULL;
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i] == NULL)
            return;
        if (spriteObj[i]->type == 0 && spriteObj[i]->sprite->getTag() == tag)
            so = spriteObj[i];
        else if (spriteObj[i]->type == 1 && spriteObj[i]->sprite9->getTag() == tag)
            so = spriteObj[i];
        else if (spriteObj[i]->type == 2 && spriteObj[i]->label->getTag() == tag)
            so =spriteObj[i];
        if (so != NULL)
            break;
    }
    
    if (so != NULL)
    {
        if (so->type == 0)
        {
            so->sprite->setDisplayFrame(sp->displayFrame());
            CCPoint p = so->sprite->getPosition();
            so->sprite->setPosition(ccp(p.x+5, p.y+11));
            so->sprite->setScale(0.95f);
        }
    }
}


SpriteObject* SpriteObject::CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha, int tag)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->type = 2;
    obj->label = CCLabelTTF::create(text.c_str(), font.c_str(), size);
    obj->label->setAnchorPoint(ap);
    obj->label->setPosition(pos);
    obj->label->setColor(color);
    obj->label->setOpacity(alpha);
    obj->label->setTag(tag);
    
    // parent
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    obj->zOrder = zOrder;
    obj->priority = priority;
    
    return obj;
}

SpriteObject* SpriteObject::CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha, int tag)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->type = 2;
    obj->label = CCLabelTTF::create(text.c_str(), font.c_str(), size, range, align, align_vertical);
    obj->label->setAnchorPoint(ap);
    obj->label->setPosition(pos);
    obj->label->setColor(color);
    obj->label->setOpacity(alpha);
    obj->label->setTag(tag);
    
    // parent
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    // z-order
    obj->zOrder = zOrder;
    
    obj->priority = priority;
    
    return obj;
}



CCPoint SpriteClass::FindParentCenterPos(std::string parentName)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i]->name == parentName) {
            CCSize size;
            if (spriteObj[i]->type == 0)
                size = spriteObj[i]->sprite->getContentSize();
            else if(spriteObj[i]->type == 1)
                size = spriteObj[i]->sprite9->getContentSize();
            return ccp(size.width/2, size.height/2);
        }
    }
    return ccp(0, 0);
}

CCSize SpriteClass::GetContentSizeByName(std::string name)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i]->name == name)
        {
            if (spriteObj[i]->type == 0)
                return spriteObj[i]->sprite->getContentSize();
            else if (spriteObj[i]->type == 1)
                return spriteObj[i]->sprite9->getContentSize();
        }
    }
    return CCSizeMake(0, 0);
}

void SpriteClass::SetOpacity(int idx, int alpha)
{
    SpriteObject* obj = spriteObj[idx];
    if (obj->type == 0) obj->sprite->setOpacity(alpha);
    else if (obj->type == 1) obj->sprite9->setOpacity(alpha);
    else if (obj->type == 2) obj->label->setOpacity(alpha);
}
void SpriteClass::AddChild(int idx)
{
    SpriteObject* obj = spriteObj[idx];

    // 0 : 이 안의 sprite, 1 : 이 안의 sprite9
    if (obj->parentType == "0") // spriteObject중 하나의 sprite
    {
        obj->parent = FindParentSprite(idx, obj->parentName);
        obj->priority = FindParentPriority(idx, obj->parentName) + 1;
    }
    else if (obj->parentType == "1") // spriteObject중 하나의 sprite-9
    {
        obj->parent = FindParentSprite9(idx, obj->parentName);
        obj->priority = FindParentPriority(idx, obj->parentName) + 1;
    }

    // 이외에는 이미 obj->parent에 부모가 저장되어 있다.
    
    // 실제 addChild
    if (obj->parentType == "0") // 부모가 sprite
    {
        if (obj->type == 0)      ((CCSprite*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CCSprite*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CCSprite*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "1") // 부모가 sprite-9
    {
        if (obj->type == 0)      ((CCScale9Sprite*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CCScale9Sprite*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CCScale9Sprite*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "Layer") // 부모가 layer
    {
        if (obj->type == 0)      ((CCLayer*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CCLayer*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CCLayer*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else // 부모가 어떤 scene
    {
        CCNode* p;
        if (obj->parentType == "Splash") p = ((Splash*)obj->parent);
        else if (obj->parentType == "WeeklyRankResult") p = ((WeeklyRankResult*)obj->parent);
        else if (obj->parentType == "GetDegree") p = ((GetDegree*)obj->parent);
        else if (obj->parentType == "Ranking") p = ((Ranking*)obj->parent);
        else if (obj->parentType == "GameReady") p = ((GameReady*)obj->parent);
        else if (obj->parentType == "MagicList") p = ((MagicList*)obj->parent);
        else if (obj->parentType == "DegreeInfo") p = ((DegreeInfo*)obj->parent);
        else if (obj->parentType == "Profile") p = ((Profile*)obj->parent);
        else if (obj->parentType == "Message") p = ((Message*)obj->parent);
        else if (obj->parentType == "CocoRoom") p = ((CocoRoom*)obj->parent);
        else if (obj->parentType == "CocoRoomTodayCandy") p = ((CocoRoomTodayCandy*)obj->parent);
        else if (obj->parentType == "CocoRoomFairyTown") p = ((CocoRoomFairyTown*)obj->parent);
        else if (obj->parentType == "InviteFriend") p = ((InviteFriend*)obj->parent);
        else if (obj->parentType == "BuyTopaz") p = ((BuyTopaz*)obj->parent);
        else if (obj->parentType == "BuyStarCandy") p = ((BuyStarCandy*)obj->parent);
        else if (obj->parentType == "BuyPotion") p = ((BuyPotion*)obj->parent);
        else if (obj->parentType == "SendTopaz") p = ((SendTopaz*)obj->parent);
        else if (obj->parentType == "RequestTopaz") p = ((RequestTopaz*)obj->parent);
        else if (obj->parentType == "RequestPotion") p = ((RequestPotion*)obj->parent);
        else if (obj->parentType == "Setting") p = ((Setting*)obj->parent);
        else if (obj->parentType == "Coupon") p = ((Coupon*)obj->parent);
        else if (obj->parentType == "Sketchbook") p = ((Sketchbook*)obj->parent);
        else if (obj->parentType == "SketchDetail") p = ((SketchDetail*)obj->parent);
        else if (obj->parentType == "SelectProperty") p = ((SelectProperty*)obj->parent);
        else if (obj->parentType == "FairyOneInfo") p = ((FairyOneInfo*)obj->parent);
        else if (obj->parentType == "NoImage") p = ((NoImage*)obj->parent);
        else if (obj->parentType == "Puzzle") p = ((Puzzle*)obj->parent);
        else if (obj->parentType == "PuzzleResult") p = ((PuzzleResult*)obj->parent);
        else if (obj->parentType == "PuzzlePause") p = ((PuzzlePause*)obj->parent);
        else if (obj->parentType == "RankUp") p = ((RankUp*)obj->parent);
        else if (obj->parentType == "T_Sketchbook") p = ((T_Sketchbook*)obj->parent);
        else if (obj->parentType == "T_SketchDetail") p = ((T_SketchDetail*)obj->parent);
        else if (obj->parentType == "T_MagicList") p = ((T_MagicList*)obj->parent);
        else if (obj->parentType == "T_NoImage") p = ((T_NoImage*)obj->parent);
        else if (obj->parentType == "T_Puzzle") p = ((T_Puzzle*)obj->parent);
        else if (obj->parentType == "T_Skip") p = ((T_Skip*)obj->parent);
        else if (obj->parentType == "AttendReward") p = ((AttendReward*)obj->parent);
        else if (obj->parentType == "Puzzle_BuyItem") p = ((Puzzle_BuyItem*)obj->parent);
        
        if (obj->type == 0)      p->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) p->addChild(obj->sprite9, obj->zOrder);
        else                     p->addChild(obj->label, obj->zOrder);
    }
}

int SpriteClass::FindParentPriority(int idx, std::string parentName)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (i != idx && spriteObj[i]->name == parentName)
            return spriteObj[i]->priority;
    }
    return -1;
}

void* SpriteClass::FindParentSprite(int idx, std::string parentName)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (i != idx && spriteObj[i]->name == parentName)
            return (void*)spriteObj[i]->sprite;
    }
    return NULL;
}

void* SpriteClass::FindParentSprite9(int idx, std::string parentName)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (i != idx && spriteObj[i]->name == parentName)
            return (void*)spriteObj[i]->sprite9;
    }
    return NULL;
}

void* SpriteClass::FindSpriteByName(std::string name)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i]->name == name)
        {
            if (spriteObj[i]->type == 0)
                return (void*)spriteObj[i]->sprite;
            else if (spriteObj[i]->type == 1)
                return (void*)spriteObj[i]->sprite9;
        }
    }
    return NULL;
}

void* SpriteClass::FindLabelByTag(int tag)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i]->type == 2 && spriteObj[i]->label->getTag() == tag)
            return (void*)spriteObj[i]->label;
    }
    return NULL;
}
void* SpriteClass::FindSpriteByTag(int tag)
{
    for (int i = 0 ; i < spriteObj.size() ; i++)
    {
        if (spriteObj[i]->type == 0 && spriteObj[i]->sprite->getTag() == tag)
            return (void*)spriteObj[i]->sprite;
        else if (spriteObj[i]->type == 1 && spriteObj[i]->sprite9->getTag() == tag)
            return (void*)spriteObj[i]->sprite9;
    }
    return NULL;
}

void SpriteClass::RemoveAllObjects()
{
    int curPriority = -999999999;
    for (int i = 0 ; i < spriteObj.size() ; i++)
        curPriority = std::max(curPriority, spriteObj[i]->priority);
    
    while (curPriority >= 0)
    {
        for (int i = 0 ; i < spriteObj.size() ; i++)
        {
            if (spriteObj[i]->priority == curPriority)
            {
                /*
                if (spriteObj[i]->type == 2)
                    //CCLog("priority %d , idx %d , type %d , name %s", curPriority, i, spriteObj[i]->type, spriteObj[i]->label->getString());
                else
                    //CCLog("priority %d , idx %d , type %d , name %s", curPriority, i, spriteObj[i]->type, spriteObj[i]->name.c_str());
                */
                spriteObj[i]->name.clear();
                spriteObj[i]->parentName.clear();
                spriteObj[i]->parentType.clear();
                if (spriteObj[i]->type == 0)
                {
                    spriteObj[i]->sprite->removeFromParentAndCleanup(true);
                }
                else if (spriteObj[i]->type == 1)
                {
                    spriteObj[i]->sprite9->removeFromParentAndCleanup(true);
                }
                else
                {
                    spriteObj[i]->label->removeFromParentAndCleanup(true);
                }
            }
        }
        curPriority--;
    }
    
    // free memory
    for (int i = 0 ; i < spriteObj.size() ; i++)
        delete spriteObj[i];
    
    // clear vector
    spriteObj.clear();
    
    // clear layers
    for (int i = 0 ; i < layers.size() ; i++)
        layers[i]->removeAllChildren();
    layers.clear();
}


void Common::RebootSystem(void* p)
{
    isRebooting = true;
    void* cur;
    
    while (depth.size() > 0)
    {
        cur = Depth::GetCurPointer();
        //CCLog("%s", Depth::GetCurNameString().c_str());
        
        if (Depth::GetCurNameString() == "Splash") ((Splash*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Ranking") ((Ranking*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "GameReady") ((GameReady*)cur)->EndSceneCallback(NULL, cur);
        else if (Depth::GetCurNameString() == "MagicList") ((MagicList*)cur)->EndSceneCallback();
        else if (Depth::GetCurNameString() == "DegreeInfo") ((DegreeInfo*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Profile") ((Profile*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "InviteFriend") ((InviteFriend*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Message") ((Message*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "WeeklyRankResult") ((WeeklyRankResult*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "GetDegree") ((GetDegree*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "CocoRoom") ((CocoRoom*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "CocoRoomTodayCandy") ((CocoRoomTodayCandy*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "CocoRoomFairyTown") ((CocoRoomFairyTown*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "BuyTopaz") ((BuyTopaz*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "BuyStarCandy") ((BuyStarCandy*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "BuyPotion") ((BuyPotion*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "SendTopaz") ((SendTopaz*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "RequestTopaz") ((RequestTopaz*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "RequestPotion") ((RequestPotion*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Setting") ((Setting*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Sketchbook") ((Sketchbook*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "SketchDetail") ((SketchDetail*)cur)->EndScene(true);
        else if (Depth::GetCurNameString() == "SelectProperty") ((SelectProperty*)cur)->EndScene(false);
        else if (Depth::GetCurNameString() == "FairyOneInfo") ((FairyOneInfo*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "NoImage") ((NoImage*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Puzzle") ((Puzzle*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "PuzzleResult") ((PuzzleResult*)cur)->EndSceneCallback(NULL, cur);
        else if (Depth::GetCurNameString() == "PuzzlePause") ((PuzzlePause*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "RankUp") ((RankUp*)cur)->EndSceneFromReboot();
        else if (Depth::GetCurNameString() == "T_Sketchbook") ((T_Sketchbook*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "T_SketchDetail") ((T_SketchDetail*)cur)->EndScene(true);
        else if (Depth::GetCurNameString() == "T_MagicList") ((T_MagicList*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "T_NoImage") ((T_NoImage*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "T_Skip") ((T_Skip*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "T_Puzzle") ((T_Puzzle*)cur)->EndScene();
        
        else if (Depth::GetCurNameString() == "AttendReward") ((AttendReward*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Puzzle_BuyItem") ((Puzzle_BuyItem*)cur)->EndScene(false);
    }
    
    // 모든 전역변수 초기화 (profiles만 제외하고)
    for (int i = 0 ; i < friendList.size() ; i++) delete friendList[i];
    friendList.clear();
    for (int i = 0 ; i < msgData.size() ; i++) delete msgData[i];
    msgData.clear();
    for (int i = 0 ; i < priceTopaz.size() ; i++) delete priceTopaz[i];
    priceTopaz.clear();
    for (int i = 0 ; i < priceStarCandy.size() ; i++) delete priceStarCandy[i];
    priceStarCandy.clear();
    for (int i = 0 ; i < magicStaffBuildupInfo.size() ; i++) delete magicStaffBuildupInfo[i];
    magicStaffBuildupInfo.clear();
    for (int i = 0 ; i < skillSlotInfo.size() ; i++) delete skillSlotInfo[i];
    skillSlotInfo.clear();
    for (int i = 0 ; i < fairyInfo.size() ; i++) delete fairyInfo[i];
    fairyInfo.clear();
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++) delete fairyBuildUpInfo[i];
    fairyBuildUpInfo.clear();
    for (int i = 0 ; i < skillInfo.size() ; i++) delete skillInfo[i];
    skillInfo.clear();
    for (int i = 0 ; i < skillBuildupMPInfo.size() ; i++) delete skillBuildupMPInfo[i];
    skillBuildupMPInfo.clear();
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++) delete skillBuildUpInfo[i];
    skillBuildUpInfo.clear();
    for (int i = 0 ; i < skillPropertyInfo.size() ; i++) delete skillPropertyInfo[i];
    skillPropertyInfo.clear();
    for (int i = 0 ; i < lastWeeklyRank.size() ; i++) delete lastWeeklyRank[i];
    lastWeeklyRank.clear();
    
    inGameSkill.clear();
    todayCandyKakaoId.clear();
    
    Depth::ClearDepth();
    
    CCTextureCache::sharedTextureCache()->removeAllTextures();
    
    // reload SPLASH scene
    CCScene *pScene = Splash::scene();
    CCDirector::sharedDirector()->replaceScene(pScene);
}


std::string Common::GetMissionContent(int type, int val, int refVal)
{
    char s[100];
    switch (type)
    {
        case 1:
            if (refVal == 1) sprintf(s, "파란 피스 %d개 그리기", missionVal);
            else if (refVal == 2) sprintf(s, "빨간 피스 %d개 그리기", missionVal);
            else if (refVal == 3) sprintf(s, "초록 피스 %d개 그리기", missionVal);
            break;
        case 2:
            sprintf(s, "'%s' %d번 그리기", SkillInfo::GetSkillInfo(refVal)->GetName().c_str(), val);
            break;
        case 3:
            sprintf(s, "마법 %d번 그리기", missionVal);
            break;
        case 4:
            sprintf(s, "피스 %d개 그리기", missionVal);
            break;
    }
    
    std::string ret = s;
    return ret;
}

std::string Common::GetProfileTitle(int idx)
{
    int id = friendList[idx]->GetProfileTitleId();
    
    int pIdx = 0;
    for (int i = 0 ; i < profileTitle.size() ; i++)
    {
        if (profileTitle[i]->GetId() == id)
        {
            pIdx = i;
            break;
        }
    }
    std::string title = profileTitle[pIdx]->GetTitle();
    
    if (friendList[idx]->GetProfileTitleId() != 1)
    {
        if (friendList[idx]->GetCertificateType() == 1) title += " 마법학사";
        else if (friendList[idx]->GetCertificateType() == 2) title += " 마법석사";
        else if (friendList[idx]->GetCertificateType() == 3) title += " 마법박사";
        else
        {
            if (profileTitle[pIdx]->GetPropertyType() == 1) title += " 물 마법사";
            else if (profileTitle[pIdx]->GetPropertyType() == 2) title += " 불 마법사";
            else if (profileTitle[pIdx]->GetPropertyType() == 3) title += " 땅 마법사";
        }
    }
    return title;
}

void Common::UpdateProfileTitle()
{
    int titleIdx = 1;
    int k, type;
    
    if (myInfo->HasNoProperty())
    {
        titleIdx = 1;
    }
    else
    {
        while(1)
        {
            k = rand()%profileTitle.size();
            type = profileTitle[k]->GetPropertyType();
            if ( (type == 1 && myInfo->IsWater()) ||
                (type == 2 && myInfo->IsFire())  ||
                (type == 3 && myInfo->IsLand()) )
            {
                titleIdx = profileTitle[k]->GetId();
                break;
            }
        }
    }
    
    // 친구리스트에서 본인을 찾아 profile title 정보를 업데이트한다.
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
        {
            friendList[i]->SetProfileTitleId(titleIdx);
            break;
        }
    }

    char temp[255];
    std::string param = "";
    sprintf(temp, "kakao_id=%s&", myInfo->GetKakaoId().c_str());
    param += temp;
    sprintf(temp, "name_title_id=%d", titleIdx);
    param += temp;
    
    Network::HttpPost(param, URL_UPDATE_NAME_TITLE, NULL, NULL);
}



static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Common::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            
            for(i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';
        
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        
        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
        
        while((i++ < 3))
            ret += '=';
        
    }
    
    return ret;
}

static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string Common::base64_decode(std::string const& encoded_string)
{
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;
    
    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
            for (i = 0; i <4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i) {
        for (j = i; j <4; j++)
            char_array_4[j] = 0;
        
        for (j = 0; j <4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        
        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }
    
    return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////

std::string Common::GetVerifyParams(int type, int topazId, const char* kakaoId, const char* friendkakaoId, const char* purchaseData, const char* dataSignature, int consumeIdx)
{
    // parameter 만들기
    char temp[1024];
    std::string param = "";
    sprintf(temp, "kakao_id=%s&", kakaoId);
    param += temp;
    sprintf(temp, "topaz_id=%d&", topazId);
    param += temp;
    sprintf(temp, "purchase_data=%s&", purchaseData);
    param += temp;
    sprintf(temp, "signature=%s", dataSignature);
    param += temp;
    if (type == 2) // 선물하기
    {
        sprintf(temp, "&friend_kakao_id=%s", friendkakaoId);
        param += temp;
    }
    
    std::string encrypted = Network::Encrypt_PS() + "#@!!@#" + Network::Encrypt_a(param);
    
    std::string url;
    if (type == 1) url = URL_PURCHASE_TOPAZ;
    else if (type == 2) url = URL_SEND_TOPAZ;
    
    return encrypted + "#@!!@#" + url;
}

void Common::XmlParseVerifyPurchaseResult(const char* data, int size, int consumeIdx, const char* fkid)
{
    //CCLog("start verify purchase result xml parse");
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        //CCLog("error description: %s", result.description());
        //CCLog("error offset: %d", result.offset);
        return;
    }
    
    //CCLog("========= DATA =========");
    //CCLog("%s", data);
    //CCLog("========================");
    
    // get data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    if (code == 0)
    {
        //CCLog("토파즈 구매 성공!");
        
        // 토파즈, 별사탕을 갱신한다.
        int topaz = nodeResult.child("money").attribute("topaz").as_int();
        int starcandy = nodeResult.child("money").attribute("star-candy").as_int();
        //CCLog("토파즈 = %d", topaz);
        //CCLog("별사탕 = %d", starcandy);
        myInfo->SetMoney(topaz, starcandy);
        ////CCLog("after 토파즈 = %d", myInfo->GetTopaz());
        ////CCLog("after 별사탕 = %d", myInfo->GetStarCandy());
        
        // 부모 scene에 갱신
        //CCString* param = CCString::create("2");
        //CCNotificationCenter::sharedNotificationCenter()->postNotification("Ranking", param);
        //CCNotificationCenter::sharedNotificationCenter()->postNotification("GameReady", param);
        //CCNotificationCenter::sharedNotificationCenter()->postNotification("CocoRoom", param);
        
        // Android에서 Consume하기
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo t;
        if (JniHelper::getStaticMethodInfo(t,
                                           "com/playDANDi/CocoMagic/InAppBilling",
                                           "Consume",
                                           "(I)V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID, consumeIdx);
            // Release
            t.env->DeleteLocalRef(t.classID);
        }
        #endif
        
        /*
        // 카카오 메시지 보내기
        std::vector<int> nullData;
        std::string friendKakaoId = fkid;
        if (!Friend::GetObj(friendKakaoId)->IsMessageBlocked())
        {
            // 카카오 api 호출
            std::string templateId = KAKAO_MSG_TEMPLATE_SENDTOPAZ;
            std::string executeUrl = "";
            char temp[128];
            sprintf(temp, "{\"sender_name\":\"%s\"}", friendKakaoId.c_str());
            std::string metaInfo = temp;
            //CCLog("metaInfo = %s", metaInfo.c_str());
            KakaoNativeExtension::getInstance()->sendLinkMessage(std::bind(&BuyTopaz::onSendLinkMessageComplete, (BuyTopaz*)Depth::GetCurPointer()), std::bind(&BuyTopaz::onSendLinkMessageErrorComplete, (BuyTopaz*)Depth::GetCurPointer(), std::placeholders::_1, std::placeholders::_2), templateId, friendKakaoId, "", executeUrl, metaInfo);
            
            Common::ShowPopup(Depth::GetCurPointer(), "BuyTopaz", "NoImage", false, SEND_TOPAZ_OK, BTN_1, nullData);
        }
        else
            Common::ShowPopup(Depth::GetCurPointer(), "BuyTopaz", "NoImage", false, SEND_TOPAZ_OK_NOKAKAOMSG, BTN_1, nullData);
        */
    }
    else
    {
        // purchase_topaz_google.php
        //CCLog("failed code = %d", code);
        /*
        if (code == 10) CCLog("서버인증실패");
        else if (code == 11) CCLog("payload 다름");
        else if (code == 12) CCLog("이미 지급한 토파즈");
        else if (code == 13) CCLog("토파즈 id 이상함");
        */
        
        // send_topaz_google.php
        // "kakao_id", "friend_kakao_id", "topaz_id", "purchase_data", "signature"
        // 실패코드 10:서버인증 실패, 11:페이로드다름, 12:이미지급한토파즈, 13:토파즈아이디가 이상함, 14:친구관계가아님
    }
}



