#include "Common.h"
#include "pugixml/pugixml.hpp"
#include "Loading.h"
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
#include "puzzle/Puzzle.h"
#include "puzzle/PuzzleResult.h"
#include "puzzle/PuzzlePause.h"
#include "Splash.h"
#include "RankUp.h"

using namespace pugi;
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

/*
CCRenderTexture* Common::CreateStroke( CCSprite* label, int size, ccColor3B color, GLubyte opacity )
{
    CCRenderTexture* rt = CCRenderTexture::create(
                                                  label->getTexture()->getContentSize().width + size * 2,
                                                  label->getTexture()->getContentSize().height+size * 2
                                                  );
    
    CCPoint originalPos = label->getPosition();
    
    ccColor3B originalColor = label->getColor();
    
    GLubyte originalOpacity = label->getOpacity();
    
    bool originalVisibility = label->isVisible();
    
    label->setColor(color);
    
    label->setOpacity(opacity);
    
    label->setVisible(true);
    
    ccBlendFunc originalBlend = label->getBlendFunc();
    
    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE};
    
    label->setBlendFunc(bf);
    
    
    CCPoint bottomLeft = ccp(
                             label->getTexture()->getContentSize().width * label->getAnchorPoint().x + size,
                             label->getTexture()->getContentSize().height * label->getAnchorPoint().y + size);
    
    CCPoint positionOffset= ccp(
                                label->getTexture()->getContentSize().width  * label->getAnchorPoint().x - label->getTexture()->getContentSize().width / 2,
                                label->getTexture()->getContentSize().height * label->getAnchorPoint().y - label->getTexture()->getContentSize().height / 2);
    
    
    CCPoint position = ccpSub(originalPos, positionOffset);
    
    //
    //rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();
    
    for (int i=0; i<360; i+= 15) // you should optimize that for your needs
    {
        label->setPosition(
                           ccp(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i))*size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i))*size)
                           );
        label->visit();
    }
    rt->end();
    
    label->setPosition(originalPos);
    label->setColor(originalColor);
    label->setBlendFunc(originalBlend);
    label->setVisible(originalVisibility);
    label->setOpacity(originalOpacity);
    
    rt->setPosition(position);
    
    return rt;
}
*/

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
    char temp[10];
    sprintf(temp, "%d", number);
    std::string num = temp;
    std::string result = "";
    for (int i = num.size()-1 ; i >= 0 ; i--)
    {
        result = num[i] + result;
        if (i > 0 && (num.size() - i) % 3 == 0)
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

CCLayer* Common::MakeItemNumberLayer(std::string number)
{
    CCLayer* layer = CCLayer::create();
    
    int offset[] = {0, 1, 1, 0, 0, 0, -1, 1, -1, 0};
    int offsetX[] = {2, 0, 2, 2, 2, 2, 2, 2, 2, 2};
    char name[30];
    int totalWidth = 0;

    std::vector<CCSprite*> sprites;
    for (int i = number.size()-1 ; i >= 0 ; i--)
    {
        sprintf(name, "number/count_%c.png", number[i]);
        CCSprite* temp = CCSprite::createWithSpriteFrameName(name);
        
        temp->setAnchorPoint(ccp(1, 0));
        if (i == number.size()-1)
        {
            temp->setPosition(ccp(0, offset[number[i]-'0']));
            totalWidth -= (int)temp->getContentSize().width;
        }
        else
        {
            temp->setPosition(ccp(totalWidth + offsetX[number[i]-'0'], offset[number[i]-'0']));
            totalWidth -= ((int)temp->getContentSize().width - offsetX[number[i]-'0']);
        }

        layer->addChild(temp, 100);
        sprites.push_back(temp);
    }
    sprites.clear();
    
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
void Common::ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc, int priority)
{
    if (from == "Sketchbook")
        ((Sketchbook*)obj)->SetTouchLock(false);
    
    CCScene* nextScene;
    if (to == "Ranking") nextScene = Ranking::scene(etc);
    else if (to == "GameReady") nextScene = GameReady::scene(priority);
    else if (to == "Message") nextScene = Message::scene();
    else if (to == "MagicList") nextScene = MagicList::scene(etc, priority);
    else if (to == "CocoRoom") nextScene = CocoRoom::scene(etc, priority);
    else if (to == "CocoRoomTodayCandy") nextScene = CocoRoomTodayCandy::scene(priority);
    else if (to == "CocoRoomFairyTown") nextScene = CocoRoomFairyTown::scene();
    else if (to == "InviteFriend") nextScene = InviteFriend::scene();
    else if (to == "BuyTopaz") nextScene = BuyTopaz::scene(etc);
    else if (to == "BuyStarCandy") nextScene = BuyStarCandy::scene(etc);
    else if (to == "BuyPotion") nextScene = BuyPotion::scene(etc);
    else if (to == "SendTopaz") nextScene = SendTopaz::scene(etc);
    else if (to == "RequestTopaz") nextScene = RequestTopaz::scene();
    else if (to == "RequestPotion") nextScene = RequestPotion::scene();
    else if (to == "Setting") nextScene = Setting::scene();
    else if (to == "Sketchbook")
    {
        if (from == "Ranking") nextScene = Sketchbook::scene(etc, 0, priority);
        else if (from == "GameReady") nextScene = Sketchbook::scene(etc, 1, priority);
    }
    else if (to == "Profile") nextScene = Profile::scene(etc);
    else if (to == "DegreeInfo") nextScene = DegreeInfo::scene();
    else if (to == "FairyOneInfo") nextScene = FairyOneInfo::scene(etc);
    else if (to == "SketchDetail") nextScene = SketchDetail::scene(etc, priority);
    
    else if (to == "WeeklyRankResult") nextScene = WeeklyRankResult::scene();
    else if (to == "GetDegree") nextScene = GetDegree::scene();
    
    else if (to == "Loading") nextScene = Loading::scene(etc);
    else if (to == "RankUp") nextScene = RankUp::scene();
    
    else if (to == "Puzzle") nextScene = Puzzle::scene(etc);
    else if (to == "PuzzleResult") nextScene = PuzzleResult::scene();
    else if (to == "PuzzlePause") nextScene = PuzzlePause::scene(etc);
    
    // go
    if (from == "Splash")
    {
        if (isReplaced)
        {
            CCDirector::sharedDirector()->replaceScene(nextScene);
            ((Splash*)obj)->EndScene();
        }
    }
    else if (from == "Ranking")
    {
        if (isReplaced)
            CCDirector::sharedDirector()->replaceScene(nextScene);
        else
            ((Ranking*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "Loading")
    {
        if (isReplaced)
            CCDirector::sharedDirector()->replaceScene(nextScene);
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
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(nextScene, 200, 200);
    else if (from == "WeeklyRankResult") ((WeeklyRankResult*)obj)->addChild(nextScene, 200, 200);
    else if (from == "MagicList") ((MagicList*)obj)->addChild(nextScene, 200, 200);
    else if (from == "InviteFriend") ((InviteFriend*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Setting") ((Setting*)obj)->addChild(nextScene, 200, 200);
    else if (from == "FairyOneInfo") ((FairyOneInfo*)obj)->addChild(nextScene, 200, 200);
    else if (from == "RankUp")
    {
        if (isReplaced)
        {
            CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
        }
    }
    else if (from == "Puzzle")
    {
        if (isReplaced)
        {
            CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
        }
        else
            ((Puzzle*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "NoImage") ((NoImage*)obj)->addChild(nextScene, 200, 200);
}

// 공통된 팝업창 (버튼 1~2개, 텍스트만 변경되는 고정된 디자인의 팝업창)
void Common::ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data, int etc, int priority)
{
    if (from == "Sketchbook")
        ((Sketchbook*)obj)->SetTouchLock(false);
    
    CCNode* parent;
    // replace = true 면, 순서상 EndScene을 먼저 해 줘야한다... (나중에 다시 리팩토링 해야 함)
    if (from == "SketchDetail") {
        parent = ((SketchDetail*)obj)->getParent();
        ((SketchDetail*)obj)->EndScene(false);
    }

    
    CCScene* popup;
    if (to == "NoImage") popup = NoImage::scene(popupType, btnType, data, etc);
    
    if (from == "Splash") ((Splash*)obj)->addChild(popup, 200, 200);
    else if (from == "Ranking") ((Ranking*)obj)->addChild(popup, 200, 200);
    else if (from == "GameReady") ((GameReady*)obj)->addChild(popup, 200, 200);
    else if (from == "WeeklyRankResult") ((WeeklyRankResult*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyStarCandy") ((BuyStarCandy*)obj)->addChild(popup, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(popup, 200, 200);
    else if (from == "RequestPotion") ((RequestPotion*)obj)->addChild(popup, 200, 200);
    else if (from == "RequestTopaz") ((RequestTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "Message") ((Message*)obj)->addChild(popup, 200, 200);
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(popup, 200, 200);
    else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(popup, 200, 200);
    else if (from == "InviteFriend") ((InviteFriend*)obj)->addChild(popup, 200, 200);
    else if (from == "FairyOneInfo") ((FairyOneInfo*)obj)->addChild(popup, 200, 200);
    else if (from == "MagicList") ((MagicList*)obj)->addChild(popup, 200, 200);
    else if (from == "RankUp") ((RankUp*)obj)->addChild(popup, 200, 200);
    else if (from == "Setting") ((Setting*)obj)->addChild(popup, 200, 200);
    else if (from == "Sketchbook") ((Sketchbook*)obj)->addChild(popup, 200, 200);
    else if (from == "SketchDetail") {
        if(isReplaced) {
            //CCNode* parent = ((SketchDetail*)obj)->getParent();
            //((SketchDetail*)obj)->EndScene(false);
            parent->addChild(popup, 200, 200);
        }
        else ((SketchDetail*)obj)->addChild(popup, 200, 200);
    }
    else if (from == "NoImage") {
        //if (isReplaced) {
            ((NoImage*)obj)->addChild(popup, 200, 200);
        /*}
        else {
            ((NoImage*)obj)->addChild(popup, 200, 200);
        }*/
    }
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
    
    // z-order
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
        else if (obj->parentType == "Sketchbook") p = ((Sketchbook*)obj->parent);
        else if (obj->parentType == "SketchDetail") p = ((SketchDetail*)obj->parent);
        else if (obj->parentType == "FairyOneInfo") p = ((FairyOneInfo*)obj->parent);
        else if (obj->parentType == "NoImage") p = ((NoImage*)obj->parent);
        else if (obj->parentType == "Puzzle") p = ((Puzzle*)obj->parent);
        else if (obj->parentType == "PuzzleResult") p = ((PuzzleResult*)obj->parent);
        else if (obj->parentType == "PuzzlePause") p = ((PuzzlePause*)obj->parent);
        else if (obj->parentType == "RankUp") p = ((RankUp*)obj->parent);
        
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
        else if (spriteObj[i]->type == 1 && spriteObj[i]->sprite->getTag() == tag)
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
                    CCLog("priority %d , idx %d , type %d , name %s", curPriority, i, spriteObj[i]->type, spriteObj[i]->label->getString());
                else
                    CCLog("priority %d , idx %d , type %d , name %s", curPriority, i, spriteObj[i]->type, spriteObj[i]->name.c_str());
                */
                spriteObj[i]->name.clear();
                spriteObj[i]->parentName.clear();
                spriteObj[i]->parentType.clear();
                if (spriteObj[i]->type == 0)
                {
                    //spriteObj[i]->sprite->autorelease();
                    //spriteObj[i]->sprite->release();
                    spriteObj[i]->sprite->removeFromParentAndCleanup(true);
                    //CCLog("type 0 : %d", spriteObj[i]->sprite->retainCount());
                }
                else if (spriteObj[i]->type == 1)
                {
                    //spriteObj[i]->sprite9->autorelease();
                    //spriteObj[i]->sprite9->release();
                    spriteObj[i]->sprite9->removeFromParentAndCleanup(true);
                    //CCLog("type 1 : %d", spriteObj[i]->sprite9->retainCount());
                }
                else
                {
                    //spriteObj[i]->label->autorelease();
                    //spriteObj[i]->label->release();
                    spriteObj[i]->label->removeFromParentAndCleanup(true);
                    //CCLog("type 2 : %d", spriteObj[i]->label->retainCount());
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
        
        if (Depth::GetCurNameString() == "Splash") ((Splash*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Ranking") ((Ranking*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "GameReady") ((GameReady*)cur)->EndSceneCallback(NULL, cur);
        else if (Depth::GetCurNameString() == "MagicList") ((MagicList*)cur)->EndScene();
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
        else if (Depth::GetCurNameString() == "FairyOneInfo") ((FairyOneInfo*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "NoImage") ((NoImage*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "Puzzle") ((Puzzle*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "PuzzleResult") ((PuzzleResult*)cur)->EndSceneCallback(NULL, cur);
        else if (Depth::GetCurNameString() == "PuzzlePause") ((PuzzlePause*)cur)->EndScene();
        else if (Depth::GetCurNameString() == "RankUp") ((RankUp*)cur)->EndScene();
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
    for (int i = 0 ; i < prerequisiteInfo.size() ; i++) delete prerequisiteInfo[i];
    prerequisiteInfo.clear();
    for (int i = 0 ; i < fairyInfo.size() ; i++) delete fairyInfo[i];
    fairyInfo.clear();
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++) delete fairyBuildUpInfo[i];
    fairyBuildUpInfo.clear();
    for (int i = 0 ; i < skillInfo.size() ; i++) delete skillInfo[i];
    skillInfo.clear();
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++) delete skillBuildUpInfo[i];
    skillBuildUpInfo.clear();
    for (int i = 0 ; i < skillPropertyInfo.size() ; i++) delete skillPropertyInfo[i];
    skillPropertyInfo.clear();
    for (int i = 0 ; i < lastWeeklyRank.size() ; i++) delete lastWeeklyRank[i];
    lastWeeklyRank.clear();
    
    inGameSkill.clear();
    todayCandyKakaoId.clear();
    
    Depth::ClearDepth();
    
    
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


/*
 static const std::string base64_chars =
 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
 "abcdefghijklmnopqrstuvwxyz"
 "0123456789+/";
*/
static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
static inline bool is_base64(unsigned char c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}
*/
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

/*
// 안드로이드 결제 직후 검증 & 상품지급 프로토콜을 호출하기 위한 함수 (java에서 넘어온다)
void Common::verifyPayloadAndProvideItem(const char* data, const char* signature)
{
    Network* net = new Network();
    net->verifyPayloadAndProvideItem(data, signature);
}
*/




