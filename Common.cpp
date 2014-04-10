#include "Common.h"
#include "pugixml/pugixml.hpp"
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
#include "popup/DegreeInfo.h"
#include "popup/Profile.h"
#include "popup/FairyInfo.h"
#include "popup/NoImage.h"
#include "puzzle/Puzzle.h"
#include "ParticleTest.h"
#include "Splash.h"

using namespace pugi;
/*
int iCash;
int iGold;
int iRemainingHeartTime;
int iRemainingHeartNum;
int iRemainingObjectTime;
int iAge;
int iType;
int iWeight;
int iMaxScore;
std::string sUsername;
std::vector<int> vEnabledMaterial;
std::vector<int> vStoredMaterial;
std::vector<struct friendScore> vScoreList;
*/
Sound* sound;

std::string fontList[] = {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    "NanumGothicExtraBold",
    "NanumGothic",
    "NanumGothicBold",
    "InkInTheMeat-Tial",
#else
    "fonts/NanumGothicExtraBold.ttf",
    "fonts/NanumGothic.ttf",
    "fonts/NanumGothicBold.ttf",
    "fonts/INKITMT.otf",
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
        }
        else
        {
            if (type == 0)
                sprintf(name, "number/rank_%c.png", commaNumber[i]);
            else if (type == 1)
                sprintf(name, "number/count_%c.png", commaNumber[i]);
            temp = CCSprite::createWithSpriteFrameName(name);
        }
        
        temp->setTag(i);
        temp->setAnchorPoint(ccp(0, 0));
        
        if (i == 0)
        {
            temp->setPosition(ccp(0, 0));
        }
        else
        {
            CCSize size = sprites[sprites.size()-1]->getContentSize();
            if (commaNumber[i] == ',')
                temp->setPosition(ccp(totalWidth, 0));
            else
                temp->setPosition(ccp(totalWidth, offset[commaNumber[i]-'0']));
        }
        
        layer->addChild(temp, 100);
        
        totalWidth += (int)temp->getContentSize().width;
        if (commaNumber[i]-'0' == 1)
            totalWidth += 3;
        
        sprites.push_back(temp);
    }
    sprites.clear();
    
    return layer;
}


// 다음 scene으로 이동 (popup도 가능)
// obj        : 이 함수를 불러온 scene obj
// from       : 이 함수를 불러온 scene 이름
// to         : 이동할 scene 이름
// isReplaced : replaceScene을 할 경우 true, otherwise, false.
void Common::ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc)
{
    CCScene* nextScene;
    if (to == "Ranking") nextScene = Ranking::scene();
    else if (to == "GameReady") nextScene = GameReady::scene();
    else if (to == "Message") nextScene = Message::scene();
    else if (to == "MagicList") nextScene = MagicList::scene();
    else if (to == "CocoRoom") nextScene = CocoRoom::scene(etc);
    else if (to == "CocoRoomTodayCandy") nextScene = CocoRoomTodayCandy::scene();
    else if (to == "CocoRoomFairyTown") nextScene = CocoRoomFairyTown::scene();
    else if (to == "InviteFriend") nextScene = InviteFriend::scene();
    else if (to == "BuyTopaz") nextScene = BuyTopaz::scene(etc);
    else if (to == "BuyStarCandy") nextScene = BuyStarCandy::scene(etc);
    else if (to == "BuyPotion") nextScene = BuyPotion::scene(etc);
    else if (to == "SendTopaz") nextScene = SendTopaz::scene();
    else if (to == "RequestTopaz") nextScene = RequestTopaz::scene();
    else if (to == "RequestPotion") nextScene = RequestPotion::scene();
    else if (to == "Setting") nextScene = Setting::scene();
    else if (to == "Sketchbook") nextScene = Sketchbook::scene(etc);
    
    else if (to == "Profile") nextScene = Profile::scene(etc);
    else if (to == "DegreeInfo") nextScene = DegreeInfo::scene();
    else if (to == "FairyInfo") nextScene = FairyInfo::scene();
    
    else if (to == "Puzzle") nextScene = Puzzle::scene();
    
    //else if (to == "ParticleTest") nextScene = ParticleTest::scene();
    
    
    // go
    if (from == "Splash")
    {
        if (isReplaced)
        {
            CCScene* transition = CCTransitionFade::create(1.0f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
            ((Splash*)obj)->EndScene();
        }
    }
    if (from == "Ranking") {
        ((Ranking*)obj)->addChild(nextScene, 200, 200);
        //CCLog("nextScene retain : %d", ((Ranking*)obj));
    }
    else if (from == "Profile") ((Profile*)obj)->addChild(nextScene, 200, 200);
    else if (from == "GameReady")
    {
        if (isReplaced)
        {
            CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
            ((GameReady*)obj)->EndScene();
        }
        else
            ((GameReady*)obj)->addChild(nextScene, 200, 200);
    }
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(nextScene, 200, 200);
    
    else if (from == "Puzzle")
    {
        if (isReplaced)
        {
            CCLog("back to RANKING");
            CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("images/game.plist");
            CCScene* transition = CCTransitionFade::create(0.5f, nextScene);
            CCDirector::sharedDirector()->replaceScene(transition);
            ((Puzzle*)obj)->EndScene();
        }
    }
}

// 공통된 팝업창 (버튼 1~2개, 텍스트만 변경되는 고정된 디자인의 팝업창)
void Common::ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data)
{
    CCScene* popup;
    if (to == "NoImage") popup = NoImage::scene(popupType, btnType, data);
    
    if (from == "Ranking") ((Ranking*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyStarCandy") ((BuyStarCandy*)obj)->addChild(popup, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(popup, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(popup, 200, 200);
    else if (from == "NoImage") {
        //CCNode* parent = ((NoImage*)obj)->getParent();
        //parent->addChild(popup, 200, 200);
        //((NoImage*)obj)->removeFromParentAndCleanup(true);
        if (isReplaced) ((NoImage*)obj)->getParent()->addChild(popup, 200, 200);
        else            ((NoImage*)obj)->addChild(popup, 200, 200);
    }
    
    //else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(popup, 200, 200);
    //else if (from == "CocoRoomFairyTown") ((CocoRoomFairyTown*)obj)->addChild(popup, 200, 200);
}


SpriteObject* SpriteObject::Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha)
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
        //CCLog("sprite retain : %d", obj->sprite->retainCount());
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
        //CCLog("sprite9 retain : %d", obj->sprite9->retainCount());
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

SpriteObject* SpriteObject::CreateFromSprite(int spriteType, CCSprite* spr, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha, float scale)
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
    //CCLog("sprite retain(file) : %d", obj->sprite->retainCount());


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
    //obj->label->retain();
    //CCLog("label retain : %d", obj->label->retainCount());
    
    // parent
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    // z-order
    obj->zOrder = zOrder;
    
    obj->priority = priority;
    
    
    return obj;
}

SpriteObject* SpriteObject::CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int priority, int alpha)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->type = 2;
    obj->label = CCLabelTTF::create(text.c_str(), font.c_str(), size, range, align, align_vertical);
    obj->label->setAnchorPoint(ap);
    obj->label->setPosition(pos);
    obj->label->setColor(color);
    obj->label->setOpacity(alpha);
    
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

    //CCLog("%d : %s // %s", idx, obj->parentName.c_str(), obj->parentType.c_str());
    
    // -1 : 현재 scene, 0 : 이 안의 sprite, 1 : 이 안의 sprite9, 2 : 다른 곳의 layer
    if (obj->parentType == "0") // spriteObject중 하나의 sprite
        obj->parent = FindParentSprite(idx, obj->parentName);
    else if (obj->parentType == "1") // spriteObject중 하나의 sprite-9
        obj->parent = FindParentSprite9(idx, obj->parentName);
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
    else if (obj->parentType == "Ranking") // 부모가 layer
    {
        if (obj->type == 0)      ((Ranking*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Ranking*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Ranking*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "GameReady") // 부모가 layer
    {
        if (obj->type == 0)      ((GameReady*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((GameReady*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((GameReady*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "DegreeInfo") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((DegreeInfo*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((DegreeInfo*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((DegreeInfo*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "Profile") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((Profile*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Profile*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Profile*)obj->parent)->addChild(obj->label, obj->zOrder);
        
        //if (obj->type == 0) CCLog("added (0) : %d", obj->sprite->retainCount());
        //if (obj->type == 1) CCLog("added (1) : %d", obj->sprite9->retainCount());
        //if (obj->type == 2) CCLog("added (2) : %d", obj->label->retainCount());
    }
    else if (obj->parentType == "Message") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((Message*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Message*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Message*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "CocoRoom") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((CocoRoom*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CocoRoom*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CocoRoom*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "CocoRoomTodayCandy") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((CocoRoomTodayCandy*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CocoRoomTodayCandy*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CocoRoomTodayCandy*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "CocoRoomFairyTown") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((CocoRoomFairyTown*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((CocoRoomFairyTown*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((CocoRoomFairyTown*)obj->parent)->addChild(obj->label, obj->zOrder);
    }

    else if (obj->parentType == "InviteFriend") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((InviteFriend*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((InviteFriend*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((InviteFriend*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "BuyTopaz") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((BuyTopaz*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((BuyTopaz*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((BuyTopaz*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "BuyStarCandy") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((BuyStarCandy*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((BuyStarCandy*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((BuyStarCandy*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "BuyPotion") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((BuyPotion*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((BuyPotion*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((BuyPotion*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "SendTopaz") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((SendTopaz*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((SendTopaz*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((SendTopaz*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "RequestTopaz") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((RequestTopaz*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((RequestTopaz*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((RequestTopaz*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "RequestPotion") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((RequestPotion*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((RequestPotion*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((RequestPotion*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "Setting") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((Setting*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Setting*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Setting*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "Sketchbook") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((Sketchbook*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Sketchbook*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Sketchbook*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "FairyInfo") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((FairyInfo*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((FairyInfo*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((FairyInfo*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    else if (obj->parentType == "NoImage") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((NoImage*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((NoImage*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((NoImage*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
    
    else if (obj->parentType == "Puzzle") // 부모가 어떤 scene
    {
        if (obj->type == 0)      ((Puzzle*)obj->parent)->addChild(obj->sprite, obj->zOrder);
        else if (obj->type == 1) ((Puzzle*)obj->parent)->addChild(obj->sprite9, obj->zOrder);
        else                     ((Puzzle*)obj->parent)->addChild(obj->label, obj->zOrder);
    }
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
            else if (spriteObj[i]->type == 0)
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
                //CCLog("priority %d , idx %d , type %d", curPriority, i, spriteObj[i]->type);
                spriteObj[i]->name.clear();
                spriteObj[i]->parentName.clear();
                spriteObj[i]->parentType.clear();
                if (spriteObj[i]->type == 0) { // delete sprite
                    //spriteObj[i]->sprite->autorelease();
                    //spriteObj[i]->sprite->release();
                    //spriteObj[i]->sprite->removeFromParentAndCleanup(true);
                    //CCLog("type 0 : %d", spriteObj[i]->sprite->retainCount());
                    
                }
                else if (spriteObj[i]->type == 1) { // delete sprite-9
                    //spriteObj[i]->sprite9->autorelease();
                    //spriteObj[i]->sprite9->release();
                    //spriteObj[i]->sprite9->removeFromParentAndCleanup(true);
                    //CCLog("type 1 : %d", spriteObj[i]->sprite9->retainCount());
                    
                }
                else { // delete label
                    //spriteObj[i]->label->autorelease();
                    //spriteObj[i]->label->release();
                    //spriteObj[i]->label->removeFromParentAndCleanup(true);
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
}





