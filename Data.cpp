#include "Data.h"

class MyInfo* myInfo;
std::vector<class Friend*> friendList;


MyInfo::MyInfo(int kakaoId, int deviceType)
{
    this->kakaoId = kakaoId;
    this->deviceType = deviceType;
}

int MyInfo::GetKakaoId()
{
    return kakaoId;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
Friend::Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int weeklyHighScore, int highScore, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillId, int skillLevel)
{
    // constructor
    this->kakaoId = kakaoId;
    this->nickname = nickname;
    this->nicknameLabel = new CCLabelTTF();
    this->nicknameLabel->initWithString(nickname.c_str(), fontList[0].c_str(), 48);
    this->imageUrl = imageUrl;
    this->potionMsgStatus = potionMsgStatus;
    this->remainPotionTime = remainPotionTime;
    this->weeklyHighScore = weeklyHighScore;
    this->highScore = highScore;
    this->certificateType = certificateType;
    this->propertyFire = (fire == 1) ? true : false;
    this->propertyWater = (water == 1) ? true : false;
    this->propertyLand = (land == 1) ? true : false;
    this->propertyMaster = (master == 1) ? true : false;
    this->fairyId = fairyId;
    this->fairyLevel = fairyLevel;
    this->skillId = skillId;
    this->skilllevel = skillLevel;
}

void Friend::SetSprite(CCTexture2D* texture)
{
    this->profile = new CCSprite();
    this->profile->initWithTexture(texture);
}

int Friend::GetKakaoId()
{
    return kakaoId;
}
CCSprite* Friend::GetProfile()
{
    return profile;
}
std::string Friend::GetNickname()
{
    return nickname;
}
CCLabelTTF* Friend::GetNicknameLabel()
{
    return nicknameLabel;
}
int Friend::GetPotionMsgStatus()
{
    return potionMsgStatus;
}
int Friend::GetWeeklyHighScore()
{
    return weeklyHighScore;
}
int Friend::GetHighScore()
{
    return highScore;
}
int Friend::GetRemainPotionTime()
{
    return remainPotionTime;
}

bool Friend::IsFire()
{
    return propertyFire;
}
bool Friend::IsWater()
{
    return propertyWater;
}
bool Friend::IsLand()
{
    return propertyLand;
}
bool Friend::IsMaster()
{
    return propertyMaster;
}
