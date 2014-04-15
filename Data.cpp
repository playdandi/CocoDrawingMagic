#include "Data.h"
#include <algorithm>

class MyInfo* myInfo;
std::vector<class Friend*> friendList;
std::vector<class PriceTopaz*> priceTopaz;
std::vector<class PriceStarCandy*> priceStarCandy;
std::vector<class Msg*> msgData;

////////////////////////////////////////////////////////////////////////////////
void MyInfo::Init(int kakaoId, int deviceType, int userId, bool kakaoMsg, bool pushNoti, bool potionMsg, int msgCnt)
{
    this->kakaoId = kakaoId;
    this->deviceType = deviceType;
    this->userId = userId;
    this->settingKakaoMsg = kakaoMsg;
    this->settingPushNoti = pushNoti;
    this->settingPotionMsg = potionMsg;
    this->msgCnt = msgCnt;
}

void MyInfo::InitRestInfo(int topaz, int starcandy, int mp, int mpStaffPercent, int mpFairy, int staffLv, int highScore, int weeklyHighScore, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion, int remainPotionTime, int fire, int water, int land, int master)
{
    this->topaz = topaz;
    this->starcandy = starcandy;
    this->mp = mp;
    this->mpStaffPercent = mpStaffPercent;
    this->mpStaff = (int)(floor((double)(mp*mpStaffPercent)/(double)100 + 0.50));
    this->mpFairy = mpFairy;
    this->staffLv = staffLv;
    this->highScore = highScore;
    this->weeklyHighScore = weeklyHighScore;
    //this->scoreUpdateTime = scoreUpdateTime;
    this->certificateType = certificateType;
    this->remainWeeklyRankTime = remainWeeklyRankTime;
    this->item[0] = item1;
    this->item[1] = item2;
    this->item[2] = item3;
    this->item[3] = item4;
    this->item[4] = item5;
    this->potion = potion;
    this->remainPotionTime = remainPotionTime;
    this->propertyFire = (fire == 1) ? true : false;
    this->propertyWater = (water == 1) ? true : false;
    this->propertyLand = (land == 1) ? true : false;
    this->propertyMaster = (master == 1) ? true : false;
}

int MyInfo::GetDeviceType()
{
    return deviceType;
}
int MyInfo::GetKakaoId()
{
    return kakaoId;
}
int MyInfo::GetMsgCnt()
{
    return msgCnt;
}
void MyInfo::SetMsgCnt(int cnt)
{
    this->msgCnt = cnt;
}
bool MyInfo::GetKakaoMsg()
{
    return settingKakaoMsg;
}
bool MyInfo::GetPushNotification()
{
    return settingPushNoti;
}
bool MyInfo::GetPotionMsg()
{
    return settingPotionMsg;
}

int MyInfo::GetTopaz()
{
    return topaz;
}
int MyInfo::GetStarCandy()
{
    return starcandy;
}

int MyInfo::GetMPTotal()
{
    return mp + mpStaff + mpFairy;
}
int MyInfo::GetMP()
{
    return mp;
}
int MyInfo::GetMPStaffPercent()
{
    return mpStaffPercent;
}
int MyInfo::GetMPStaff()
{
    return mpStaff;
}
int MyInfo::GetMPFairy()
{
    return mpFairy;
}
int MyInfo::GetStaffLv()
{
    return staffLv;
}
int MyInfo::GetStaffLvNext()
{
    return staffLvNext;
}
int MyInfo::GetMPStaffPercentNext()
{
    return mpStaffPercentNext;
}
int MyInfo::GetMPNextCostStarcandy()
{
    return mpNextCostStarcandy;
}
int MyInfo::GetMPNextCostTopaz()
{
    return mpNextCostTopaz;
}

int MyInfo::GetHighScore()
{
    return highScore;
}
int MyInfo::GetWeeklyHighScore()
{
    return weeklyHighScore;
}
int MyInfo::GetCertificateType()
{
    return certificateType;
}

int MyInfo::GetItem(int idx)
{
    return item[idx];
}
int MyInfo::GetPotion()
{
    return potion;
}


int MyInfo::GetRemainWeeklyRankTimeInt()
{
    return remainWeeklyRankTime;
}
std::string MyInfo::GetRemainWeeklyRankTime()
{
    std::string res;
    char number[3];
    if (remainWeeklyRankTime / 86400 > 0)
    {
        res = "일";
        sprintf(number, "%d", remainWeeklyRankTime/86400);
    }
    else
    {
        res = "시간";
        sprintf(number, "%d", remainWeeklyRankTime/3600);
    }
    res = number + res;
    return res;
}
std::string MyInfo::GetRemainPotionTime()
{
    std::string res = ":";
    char min[3], sec[3];
    
    if (potion >= 5)
    {
        res = "총";
        sprintf(min, "%d", potion);
        sprintf(sec, "개");
        res = res + min + sec;
    }
    else
    {
        sprintf(min, "%d", remainPotionTime/60);
        if (remainPotionTime%60 < 10)
            sprintf(sec, "0%d", remainPotionTime%60);
        else
            sprintf(sec, "%d", remainPotionTime%60);
        res = min + res + sec;
    }
    
    return res;
}

bool MyInfo::IsFire()
{
    return propertyFire;
}
bool MyInfo::IsWater()
{
    return propertyWater;
}
bool MyInfo::IsLand()
{
    return propertyLand;
}
bool MyInfo::IsMaster()
{
    return propertyMaster;
}

void MyInfo::SetSettingVariables(bool kakaoMsgReserved, bool pushNotiReserved, bool potionMsgReserved)
{
    this->settingKakaoMsg = kakaoMsgReserved;
    this->settingPushNoti = pushNotiReserved;
    this->settingPotionMsg = potionMsgReserved;
}

void MyInfo::SetMoney(int topaz, int starcandy)
{
    this->topaz = topaz;
    this->starcandy = starcandy;
}
void MyInfo::SetPotion(int potion, int remainPotionTime)
{
    this->potion = potion;
    this->remainPotionTime = remainPotionTime;
}
void MyInfo::SetCoco(int mp, int mpStaffPercent, int mpFairy, int staffLv)
{
    this->mp = mp;
    this->mpStaffPercent = mpStaffPercent;
    this->mpStaff = (int)(floor((double)(mp*mpStaffPercent)/(double)100 + 0.50));
    this->mpFairy = mpFairy;
    this->staffLv = staffLv;
}
void MyInfo::SetNextStaff(int staffLvNext, int mpNextCostStarcandy, int mpNextCostTopaz, int staffNextPercent)
{
    this->staffLvNext = staffLvNext;
    this->mpNextCostStarcandy = mpNextCostStarcandy;
    this->mpNextCostTopaz = mpNextCostTopaz;
    this->mpStaffPercentNext = staffNextPercent;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
Friend::Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillId, int skillLevel)
{
    // constructor
    this->kakaoId = kakaoId;
    this->nickname = nickname;
    this->nicknameLabel = new CCLabelTTF();
    this->nicknameLabel->initWithString(nickname.c_str(), fontList[0].c_str(), 48);
    this->imageUrl = imageUrl;
    this->potionMsgStatus = potionMsgStatus;
    this->remainPotionTime = remainPotionTime;
    this->potionSprite = NULL;
    this->potionRemainTimeMin = new CCLabelTTF();
    this->potionRemainTimeMin->initWithString("", fontList[0].c_str(), 28);
    this->potionRemainTimeSec = new CCLabelTTF();
    this->potionRemainTimeSec->initWithString("", fontList[0].c_str(), 28);
    this->weeklyHighScore = weeklyHighScore;
    this->highScore = highScore;
    this->scoreUpdateTime = scoreUpdateTime;
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

void Friend::SetSprite()
{
    this->profile = CCSprite::createWithSpriteFrameName("background/bg_profile_noimage.png");
    this->profile->retain();
}

void Friend::SetPotionSprite()
{
    // potion state (내 프로필에는 당연히 포션 그림 나오면 안 된다.)
    if (kakaoId != myInfo->GetKakaoId())
    {
        CCSprite* potion;
        if (potionMsgStatus == POTION_SEND)
        {
            if (remainPotionTime == 0)
                potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_send.png");
            else
                potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_remain.png");
        }
        else
            potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_x.png");
        
        potion->setAnchorPoint(ccp(0, 0));
        potion->setPosition(ccp(724, 24));
        potion->retain();
        //potion->setTag(i);
        //profileLayer->addChild(potion, 5);
        
        if (potionSprite != NULL)
        {
            CCNode* parent = potionSprite->getParent();
            potionSprite->removeFromParentAndCleanup(true);
            potionSprite = potion;
            parent->addChild(potionSprite, 5);
        }
        else
        {
            potionSprite = potion;
        }
        
        
    }
}
CCLabelTTF* Friend::GetPotionLabelMin()
{
    return potionRemainTimeMin;
}
CCLabelTTF* Friend::GetPotionLabelSec()
{
    return potionRemainTimeSec;
}
CCSprite* Friend::GetPotionSprite()
{
    return potionSprite;
}

int Friend::GetKakaoId()
{
    return kakaoId;
}
std::string Friend::GetImageUrl()
{
    return imageUrl;
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
void Friend::SetPotionMsgStatus(int status)
{
    potionMsgStatus = status;
}
int Friend::GetWeeklyHighScore()
{
    return weeklyHighScore;
}
int Friend::GetHighScore()
{
    return highScore;
}
int Friend::GetScoreUpdateTime()
{
    return scoreUpdateTime;
}
int Friend::GetRemainPotionTime()
{
    return remainPotionTime;
}
void Friend::SetRemainPotionTime(int time)
{
    remainPotionTime = time;
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

////////////////////////////////////////////////////////////////////////
bool compare(Friend *f1, Friend *f2)
{
    if (f1->GetWeeklyHighScore() == f2->GetWeeklyHighScore())
    {
        if (f1->GetWeeklyHighScore() == -1)
        {
            if (f1->GetKakaoId() == myInfo->GetKakaoId())
                return true;
            return f1->GetScoreUpdateTime() < f2->GetScoreUpdateTime();
        }
        return f1->GetScoreUpdateTime() < f2->GetScoreUpdateTime();
    }
    
    return f1->GetWeeklyHighScore() > f2->GetWeeklyHighScore();
}

void DataProcess::SortFriendListByScore()
{
    std::sort(friendList.begin(), friendList.end(), compare);
}


////////////////////////////////////////////////////////////////////////////////////
Msg::Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl)
{
    this->id = id;
    this->type = type;
    this->rewardCount = rewardCount;
    this->content = content;
    this->profileUrl = profileUrl;
    this->noticeUrl = noticeUrl;
}
int Msg::GetId()
{
    return id;
}
int Msg::GetType()
{
    return type;
}
int Msg::GetRewardCount()
{
    return rewardCount;
}
std::string Msg::GetContent()
{
    return content;
}


////////////////////////////////////////////////////////////////////////////////////
PriceTopaz::PriceTopaz(int id, int count, int price, int bonus)
{
    this->id = id;
    this->count = count;
    this->price = price;
    this->bonus = bonus;
}
int PriceTopaz::GetId()
{
    return id;
}
int PriceTopaz::GetCount()
{
    return count;
}
int PriceTopaz::GetPrice()
{
    return price;
}
int PriceTopaz::GetBonus()
{
    return bonus;
}

PriceStarCandy::PriceStarCandy(int id, int count, int price, int bonus)
{
    this->id = id;
    this->count = count;
    this->price = price;
    this->bonus = bonus;
}
int PriceStarCandy::GetId()
{
    return id;
}
int PriceStarCandy::GetCount()
{
    return count;
}
int PriceStarCandy::GetPrice()
{
    return price;
}
int PriceStarCandy::GetBonus()
{
    return bonus;
}