#include "Common.h"
#include "GameReady.h"
#include "DegreeInfo.h"
#include "Profile.h"
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
#include "Exit.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;

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
float Common::ComputeX(float x)
{
	return floorf(x*OCTA_WIDTH);
}

float Common::ComputeY(float y)
{
    return floorf(y*OCTA_HEIGHT+25)+OCTA_HEIGHT;
}

CCPoint Common::ComputeXY(float x, float y)
{
	return ccp(ComputeX(x), ComputeY(y));
}

int Common::ComputeBoardX(float x)
{
	return (int)(x/floorf(OBJECT_WIDTH));
}

int Common::ComputeBoardY(float y)
{
    return (int)((y-OBJECT_HEIGHT-25)/floorf(OBJECT_HEIGHT));
}
*/

/*
CCPoint Common::GetAnchorPoint(CCDictionary* dic)
{
    return ccp(dic->valueForKey("apX")->intValue(), dic->valueForKey("apY")->intValue());
}

CCPoint Common::GetPosition(CCDictionary* dic)
{
    return ccp(dic->valueForKey("posX")->intValue(), dic->valueForKey("posY")->intValue());
}
*/


bool Common::XmlParsePuzzleEnd(char* data, int size)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return false;
    }
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code == 0)
    {
        iAge = nodeResult.child("raise").attribute("age").as_int();
        iType = nodeResult.child("raise").attribute("type").as_int();
        iWeight = nodeResult.child("raise").attribute("weight").as_int();
        iMaxScore = nodeResult.child("puzzle").attribute("max-score").as_int();
        iRemainingHeartNum = nodeResult.child("puzzle").attribute("heart-num").as_int();
        iRemainingHeartTime = nodeResult.child("puzzle").attribute("heart-remain-time").as_int();
        vEnabledMaterial.clear();
        vStoredMaterial.clear();
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-a").as_int());
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-b").as_int());
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-c").as_int());
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-d").as_int());
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-e").as_int());
        vEnabledMaterial.push_back(nodeResult.child("material").attribute("enabled-f").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-a").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-b").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-c").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-d").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-e").as_int());
        vStoredMaterial.push_back(nodeResult.child("material").attribute("stored-f").as_int());

        return true;
    }
    else
    {
        // failed msg
    }
    
    return false;
}

bool Common::XmlParsePuzzleStart(std::vector<char>* buffer)
{
	const char *data;
	std::string str(buffer->begin(), buffer->end());
	data = str.c_str();
	int size = strlen(data);	

	 // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return false;
    }
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    //CCLog("code = %d", code);
    
    if (code == 0)
    {
        // http 통신 성공 - data를 모두 받는다.
        iAge = nodeResult.child("raise").attribute("age").as_int();
        iType = nodeResult.child("raise").attribute("type").as_int();
        iWeight = nodeResult.child("raise").attribute("weight").as_int();
        iMaxScore = nodeResult.child("puzzle").attribute("max-score").as_int();
        iRemainingHeartNum = nodeResult.child("puzzle").attribute("heart-num").as_int();
        iRemainingHeartTime = nodeResult.child("puzzle").attribute("heart-remain-time").as_int();
        
        return true;
    }
    else if (code == 3)
    {
        // no heart msg
        return false;
    }
    
    return false;
}

bool Common::XmlParseRanking(std::vector<char>* buffer)
{
	const char *data;
	std::string str(buffer->begin(), buffer->end());
	data = str.c_str();
	int size = strlen(data);	

	 // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return false;
    }
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    
    if (code == 0)
    {
		xml_node nodeRank = nodeResult.child("rank");
		vScoreList.clear();
		xml_node_iterator it;
		for(it = nodeRank.children().begin() ; it != nodeRank.children().end() ; ++it)
		{
			struct friendScore fs;
			strcpy(fs.name, it->attribute("name").as_string());
            fs.score = it->attribute("score").as_int();
			vScoreList.push_back(fs);
		}
        return true;
    }

    return false;
}

bool Common::XmlParseMoneyRaisePuzzle(char* data, int size, bool hasMoney)
{
    // xml parsing
    xml_document xmlDoc;
    xml_parse_result result = xmlDoc.load_buffer(data, size);
    
    if (!result)
    {
        CCLog("error description: %s", result.description());
        CCLog("error offset: %d", result.offset);
        return false;
    }
    
    // get several data
    xml_node nodeResult = xmlDoc.child("response");
    int code = nodeResult.child("code").text().as_int();
    //CCLog("code = %d", code);
    
    if (code == 0)
    {
        // http 통신 성공 - data를 모두 받는다.
        if (hasMoney)
        {
            iCash = nodeResult.child("money").attribute("cash").as_int();
            iGold = nodeResult.child("money").attribute("gold").as_int();
        }
        iAge = nodeResult.child("raise").attribute("age").as_int();
        iType = nodeResult.child("raise").attribute("type").as_int();
        iWeight = nodeResult.child("raise").attribute("weight").as_int();
        iMaxScore = nodeResult.child("puzzle").attribute("max-score").as_int();
        iRemainingHeartNum = nodeResult.child("puzzle").attribute("heart-num").as_int();
        iRemainingHeartTime = nodeResult.child("puzzle").attribute("heart-remain-time").as_int();
        
        return true;
    }
    else if (code == 3)
    {
        // no heart msg
        return false;
    }
    
    return false;
}




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

CCLayer* Common::MakeImageNumberLayer(std::string number)
{
    int offset[] = {0, 1, 1, 0, 0, 0, -1, 1, -1, 0};
    
    char name[30];
    CCLayer* layer = CCLayer::create();
    
    int totalWidth = 0;
    
    std::vector<CCSprite*> sprites;
    for (int i = 0 ; i < number.size() ; i++)
    {
        CCSprite* temp;
        if (number[i] == ',')
        {
            temp = CCSprite::createWithSpriteFrameName("number/rank_comma.png");
        }
        else
        {
            sprintf(name, "number/rank_%c.png", number[i]);
            temp = CCSprite::createWithSpriteFrameName(name);
        }
        
        temp->setAnchorPoint(ccp(0, 0));
        
        if (i == 0)
        {
            temp->setPosition(ccp(0, 0));
            layer->addChild(temp, 100);
        }
        else
        {
            CCSize size = sprites[sprites.size()-1]->getContentSize();
            if (number[i] == ',')
                temp->setPosition(ccp(totalWidth, 0));
            else
                temp->setPosition(ccp(totalWidth, offset[number[i]-'0']));
            //sprites[sprites.size()-1]->addChild(temp, 100);
            layer->addChild(temp, 100);
        }
        
        totalWidth += (int)temp->getContentSize().width;
        if (number[i]-'0' == 1)
            totalWidth += 3;
        
        sprites.push_back(temp);
    }
    
    return layer;
}


// 다음 scene으로 이동 (popup도 가능)
// obj        : 이 함수를 불러온 scene obj
// from       : 이 함수를 불러온 scene 이름
// to         : 이동할 scene 이름
// isReplaced : replaceScene을 할 경우 true, otherwise, false.
void Common::ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced)
{
    CCScene* nextScene;
    if (to == "GameReady") nextScene = GameReady::scene();
    else if (to == "Profile") nextScene = Profile::scene();
    else if (to == "DegreeInfo") nextScene = DegreeInfo::scene();
    else if (to == "Message") nextScene = Message::scene();
    else if (to == "MagicList") nextScene = MagicList::scene();
    else if (to == "CocoRoom") nextScene = CocoRoom::scene();
    else if (to == "CocoRoomTodayCandy") nextScene = CocoRoomTodayCandy::scene();
    else if (to == "CocoRoomFairyTown") nextScene = CocoRoomFairyTown::scene();
    else if (to == "InviteFriend") nextScene = InviteFriend::scene();
    else if (to == "Exit") nextScene = Exit::scene();
    else if (to == "BuyTopaz") nextScene = BuyTopaz::scene();
    else if (to == "BuyStarCandy") nextScene = BuyStarCandy::scene();
    else if (to == "BuyPotion") nextScene = BuyPotion::scene();
    else if (to == "SendTopaz") nextScene = SendTopaz::scene();
    else if (to == "RequestTopaz") nextScene = RequestTopaz::scene();
    else if (to == "RequestPotion") nextScene = RequestPotion::scene();
    else if (to == "Setting") nextScene = Setting::scene();
    else if (to == "Sketchbook") nextScene = Sketchbook::scene();
    
    // go
    if (from == "Ranking") ((Ranking*)obj)->addChild(nextScene, 200, 200);
    else if (from == "Profile") ((Profile*)obj)->addChild(nextScene, 200, 200);
    else if (from == "GameReady") ((GameReady*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyTopaz") ((BuyTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "SendTopaz") ((SendTopaz*)obj)->addChild(nextScene, 200, 200);
    else if (from == "BuyPotion") ((BuyPotion*)obj)->addChild(nextScene, 200, 200);
    else if (from == "CocoRoom") ((CocoRoom*)obj)->addChild(nextScene, 200, 200);
}


SpriteObject* SpriteObject::Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha)
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
    }

    // parent 관련 대입
    obj->parentName = parentName;
    obj->parentType = parentType;
    obj->parent = parent;
    
    // z-order
    obj->zOrder = zOrder;
    
    return obj;
}

SpriteObject* SpriteObject::CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha)
{
    SpriteObject* obj = new SpriteObject();
    
    obj->type = 2;
    obj->label = CCLabelTTF::create(text.c_str(), font.c_str(), size);
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
    
    return obj;
}

SpriteObject* SpriteObject::CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha)
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

