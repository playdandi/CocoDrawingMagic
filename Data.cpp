#include "Data.h"
#include <algorithm>

int iGameVersion;
int iBinaryVersion;
class MyInfo* myInfo;
std::vector<class ProfileSprite*> profiles;
std::vector<class Friend*> friendList;
std::vector<class Msg*> msgData;

std::vector<class PriceTopaz*> priceTopaz;
std::vector<class PriceStarCandy*> priceStarCandy;
std::vector<class MagicStaffBuildUpInfo*> magicStaffBuildupInfo;
std::vector<class SkillSlotInfo*> skillSlotInfo;
std::vector<class PrerequisiteInfo*> prerequisiteInfo;
std::vector<class FairyInfo*> fairyInfo;
std::vector<class FairyBuildUpInfo*> fairyBuildUpInfo;
std::vector<class SkillInfo*> skillInfo;
std::vector<class SkillBuildUpInfo*> skillBuildUpInfo;
std::vector<class SkillPropertyInfo*> skillPropertyInfo;

std::vector<class LastWeeklyRank*> lastWeeklyRank;


std::vector<class Depth*> depth;
std::vector<int> inGameSkill;
std::vector<int> todayCandyKakaoId;

bool isInGame;
int savedTime;

int myRank;
int myLastWeekHighScore;
int rewardType;


////////////////////////////////////////////////////////////////////////////////
Depth::Depth(std::string name, int priority)
{
    this->name = name;
    this->priority = priority;
}
const char* Depth::GetParentName()
{
    return depth[depth.size()-2]->name.c_str();
}
const char* Depth::GetCurName()
{
    return depth[depth.size()-1]->name.c_str();
}
int Depth::GetCurPriority()
{
    return depth[depth.size()-1]->priority;
}
void Depth::AddCurDepth(std::string name)
{
    int priority = (int)depth.size() * -1;
    depth.push_back( new Depth(name, priority) );
}
void Depth::RemoveCurDepth()
{
    delete depth[depth.size()-1];
    depth.pop_back();
}
void Depth::ClearDepth()
{
    for (int i = 0 ; i < depth.size() ; i++)
        delete depth[i];
    depth.clear();
}

////////////////////////////////////////////////////////////////////////////////
ProfileSprite::ProfileSprite(std::string profileUrl)
{
    this->profileUrl = profileUrl;
    this->profile = NULL;
}
CCSprite* ProfileSprite::GetProfile(std::string profileUrl)
{
    for (int i = 0 ; i < profiles.size(); i++)
    {
        if (profiles[i]->GetProfileUrl() == profileUrl)
            return profiles[i]->GetProfile();
    }
    return NULL;
}
CCSprite* ProfileSprite::GetProfile()
{
    return profile;
}
std::string ProfileSprite::GetProfileUrl()
{
    return profileUrl;
}
void ProfileSprite::SetSpriteNoImage()
{
    this->profile = CCSprite::createWithSpriteFrameName("background/bg_profile_noimage.png");
    this->profile->retain();
}
void ProfileSprite::SetSprite(CCTexture2D* texture)
{
    this->profile = new CCSprite();
    this->profile->initWithTexture(texture);
}

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

void MyInfo::InitRestInfo(int topaz, int starcandy, int mp, int mpStaffPercent, int mpFairy, int staffLv, int highScore, int weeklyHighScore, int lastWeeklyHighScore, int isWeeklyRankReward, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion, int remainPotionTime, int fire, int water, int land, int master)
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
    this->lastWeeklyHighScore = lastWeeklyHighScore;
    this->isWeeklyRankReward = (isWeeklyRankReward == 1);

    this->certificateType = certificateType;
    this->remainWeeklyRankTime = remainWeeklyRankTime;
    this->item[0] = item1;
    this->item[1] = item2;
    this->item[2] = item3;
    this->item[3] = item4;
    this->item[4] = item5;
    this->potion = potion;
    this->remainPotionTime = remainPotionTime;
    this->propertyFire = (fire == 1);
    this->propertyWater = (water == 1);
    this->propertyLand = (land == 1);
    this->propertyMaster = (master == 1);
}

CCSprite* MyInfo::GetProfile()
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            return friendList[i]->GetProfile();
    }
    return NULL;
}
std::string MyInfo::GetName()
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            return friendList[i]->GetNickname();
    }
    return "";
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
int MyInfo::GetHighScore()
{
    return highScore;
}
int MyInfo::GetWeeklyHighScore()
{
    return weeklyHighScore;
}
int MyInfo::GetLastWeeklyHighScore()
{
    return lastWeeklyHighScore;
}
bool MyInfo::IsWeeklyRankReward()
{
    return isWeeklyRankReward;
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

int MyInfo::GetRemainPotionTimeNumber()
{
    return remainPotionTime;
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
    if (remainPotionTime != -1)
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
void MyInfo::SetItem(std::vector<int> items)
{
    for (int i = 0 ; i < items.size() ; i++)
        this->item[i] = items[i];
}
void MyInfo::SetProperties(int fire, int water, int land, int master)
{
    this->propertyFire = (fire == 1) ? true : false;
    this->propertyWater = (water == 1) ? true : false;
    this->propertyLand = (land == 1) ? true : false;
    this->propertyMaster = (master == 1) ? true : false;
}
void MyInfo::SetScore(int highScore, int weeklyHighScore, int certificateType, int remainWeeklyRankTime)
{
    this->highScore = highScore;
    this->weeklyHighScore = weeklyHighScore;
    this->certificateType = certificateType;
    this->remainWeeklyRankTime = remainWeeklyRankTime;
}
void MyInfo::SetRemainWeeklyRankTime(int time)
{
    this->remainWeeklyRankTime = time;
}

void MyInfo::SetProfileSkill(int id, int level)
{
    this->profileSkillId = id;
    this->profileSkillLv = level;
}
int MyInfo::GetProfileSkillId()
{
    return profileSkillId;
}
int MyInfo::GetProfileSkillLv()
{
    return profileSkillLv;
}
void MyInfo::SetPracticeSkill(int id, int level)
{
    this->practiceSkillId = id;
    this->practiceSkillLv = level;
}
int MyInfo::GetPracticeSkillId()
{
    return practiceSkillId;
}
int MyInfo::GetPracticeSkillLv()
{
    return practiceSkillLv;
}

void MyInfo::AddSkillSlot(int id, int csi, int usi)
{
    mySkillSlot.push_back( new MySkillSlot(id, csi, usi) );
}
void MyInfo::AddFairy(int cfi, int ufi, int level, int isUse)
{
    myFairy.push_back( new MyFairy(cfi, ufi, level, isUse) );
}
void MyInfo::AddSkill(int csi, int usi, int level, int exp)
{
    mySkill.push_back( new MySkill(csi, usi, level, exp) );
}

int MyInfo::GetActiveFairyId() // 현재 사용중인 요정의 common id
{
    for (int i = 0 ; i < myFairy.size() ; i++)
    {
        if (myFairy[i]->IsUse())
            return myFairy[i]->GetId();
    }
    return -1;
}
int MyInfo::GetActiveFairyUserId() // 현재 사용중인 요정의 user id
{
    for (int i = 0 ; i < myFairy.size() ; i++)
    {
        if (myFairy[i]->IsUse())
            return myFairy[i]->GetUserId();
    }
    return -1;
}
int MyInfo::GetActiveFairyLevel() // 현재 사용중인 요정의 레벨
{
    for (int i = 0 ; i < myFairy.size() ; i++)
    {
        if (myFairy[i]->IsUse())
            return myFairy[i]->GetLevel();
    }
    return -1;
}
std::vector<class MyFairy*> MyInfo::GetFairyList() // 내가 산 요정 리스트를 모두 get
{
    return myFairy;
}
std::vector<class MySkill*> MyInfo::GetSkillList() // 내가 산 스킬 모두 get
{
    return mySkill;
}
bool compare3(MySkill* ms1, MySkill* ms2)
{
    return ms1->GetCommonId() < ms2->GetCommonId();
}
void MyInfo::SortMySkillByCommonId() // 내가 산 스킬 common-id로 오름차순 정렬
{
    std::sort(mySkill.begin(), mySkill.end(), compare3);
}

std::vector<class MySkillSlot*> MyInfo::GetSlot() // 내가 산 슬롯 get
{
    return mySkillSlot;
}
void MyInfo::ClearFairyList() // 내 요정 리스트 갱신할 때 clear하기 위한 용도
{
    for (int i = 0 ; i < myFairy.size() ; i++)
        delete myFairy[i];
    myFairy.clear();
}
void MyInfo::ClearSkillList() // 내 스킬 리스트 갱신할 때 clear하기 위한 용도
{
    for (int i = 0 ; i < mySkill.size() ; i++)
        delete mySkill[i];
    mySkill.clear();
}
void MyInfo::ClearSkillSlot() // 내 스킬 슬롯 갱신할 때 clear하기 위한 용도
{
    for (int i = 0 ; i < mySkillSlot.size() ; i++)
        delete mySkillSlot[i];
    mySkillSlot.clear();
}


MySkillSlot::MySkillSlot(int id, int csi, int usi)
{
    this->id = id;
    this->common_skill_id = csi;
    this->user_skill_id = usi;
}
int MySkillSlot::GetId()
{
    return id;
}
int MySkillSlot::GetCommonId()
{
    return common_skill_id;
}
int MySkillSlot::GetUserId()
{
    return user_skill_id;
}
void MySkillSlot::InsertSkill(int scid, int suid)
{
    this->common_skill_id = scid;
    this->user_skill_id = suid;
}


MyFairy::MyFairy(int cfi, int ufi, int level, int isUse)
{
    this->common_fairy_id = cfi;
    this->user_fairy_id = ufi;
    this->level = level;
    this->isUse = isUse;
}
bool MyFairy::IsUse()
{
    return (isUse == 1);
}
int MyFairy::GetId()
{
    return common_fairy_id;
}
int MyFairy::GetUserId()
{
    return user_fairy_id;
}
int MyFairy::GetLevel()
{
    return level;
}

MySkill::MySkill(int csi, int usi, int level, int exp)
{
    this->common_skill_id = csi;
    this->user_skill_id = usi;
    this->level = level;
    this->exp = exp;
}
MySkill* MySkill::GetObj(int scid)
{
    MySkill* ms;
    for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
    {
        ms = myInfo->GetSkillList()[i];
        if (ms->GetCommonId() == scid)
            return ms;
    }
    return NULL;
}
int MySkill::GetCommonId()
{
    return common_skill_id;
}
int MySkill::GetUserId()
{
    return user_skill_id;
}
int MySkill::GetLevel()
{
    return level;
}
int MySkill::GetExp()
{
    return exp;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

LastWeeklyRank::LastWeeklyRank(std::string nickname, std::string profileUrl, int rank, int score, int isFriend)
{
    this->nickname = nickname;
    this->profileUrl = profileUrl;
    this->rank = rank;
    this->score = score;
    this->isFriend = (isFriend == 1);
}
std::string LastWeeklyRank::GetNickname()
{
    return nickname;
}
std::string LastWeeklyRank::GetProfileUrl()
{
    return profileUrl;
}
bool LastWeeklyRank::IsFriend()
{
    return isFriend;
}
int LastWeeklyRank::GetRank()
{
    return rank;
}
int LastWeeklyRank::GetScore()
{
    return score;
}
int compare4(LastWeeklyRank* l1, LastWeeklyRank* l2)
{
    return l1->GetRank() < l2->GetRank();
}
void LastWeeklyRank::SortByRank()
{
    std::sort(lastWeeklyRank.begin(), lastWeeklyRank.end(), compare4);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

Friend::Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int remainRequestPotionTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillId, int skillLevel)
{
    // constructor
    this->kakaoId = kakaoId;
    this->nickname = nickname;
    this->nicknameLabel = new CCLabelTTF();
    this->nicknameLabel->initWithString(nickname.c_str(), fontList[0].c_str(), 48);
    this->imageUrl = imageUrl;
    this->potionMsgStatus = potionMsgStatus;
    this->remainPotionTime = remainPotionTime;
    this->remainRequestPotionTime = remainRequestPotionTime;
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
    this->skillLevel = skillLevel;
}

Friend* Friend::GetObj(int kakaoId)
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == kakaoId)
            return friendList[i];
    }
    return NULL;
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
void Friend::ChangeMyFairyInfo()
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
        {
            friendList[i]->fairyId = myInfo->GetActiveFairyId();
            friendList[i]->fairyLevel = myInfo->GetActiveFairyLevel();
        }
    }
}
void Friend::SetScore(int highScore, int weeklyHighScore, int certificateType)
{
    this->highScore = highScore;
    this->weeklyHighScore = weeklyHighScore;
    this->certificateType = certificateType;
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
int Friend::GetRemainPotionTime(int kakaoId)
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == kakaoId)
            return friendList[i]->GetRemainPotionTime();
    }
    return -1;
}
int Friend::GetRemainPotionTime()
{
    return remainPotionTime;
}
int Friend::GetRemainRequestPotionTime()
{
    return remainRequestPotionTime;
}
void Friend::SetRemainPotionTime(int time)
{
    remainPotionTime = time;
}
void Friend::SetRemainRequestPotionTime(int time)
{
    remainRequestPotionTime = time;
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

int Friend::GetFairyId()
{
    return fairyId;
}
int Friend::GetFairyLv()
{
    return fairyLevel;
}
int Friend::GetSkillId()
{
    return skillId;
}
int Friend::GetSkillLv()
{
    return skillLevel;
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
bool compare2(MagicStaffBuildUpInfo* m1, MagicStaffBuildUpInfo* m2)
{
    return m1->GetLevel() < m2->GetLevel();
}

void DataProcess::SortFriendListByScore()
{
    std::sort(friendList.begin(), friendList.end(), compare);
}
void DataProcess::SortMagicStaffBuildUpInfo()
{
    std::sort(magicStaffBuildupInfo.begin(), magicStaffBuildupInfo.end(), compare2);
}
//void DataProcess::SortMySkillByCommonId(std::vector<MySkill*> mySkill)
/*void DataProcess::SortMySkillByCommonId()
{
    std::vector<MySkill*> ms = (myInfo->GetSkillList());
    std::sort(mySkill.begin(), mySkill.end(), compare3);
}*/
std::string DataProcess::FindSkillNameById(int skillId)
{
    for (int i = 0 ; i < skillInfo.size() ; i++)
    {
        if (skillInfo[i]->GetId() == skillId)
            return skillInfo[i]->GetName();
    }
    return "";
}


////////////////////////////////////////////////////////////////////////////////////
Msg::Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl, int friendKakaoId)
{
    this->id = id;
    this->type = type;
    this->rewardCount = rewardCount;
    this->content = content;
    this->profileUrl = profileUrl;
    this->noticeUrl = noticeUrl;
    this->friendKakaoId = friendKakaoId;
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
std::string Msg::GetProfileUrl()
{
    return profileUrl;
}
std::string Msg::GetNoticeUrl()
{
    return noticeUrl;
}
int Msg::GetFriendKakaoId()
{
    return friendKakaoId;
}

////////////////////////////////////////////////////////////////////////////////////
PriceTopaz::PriceTopaz(int id, int count, int KRW, int USD, int bonus)
{
    this->nId = id;
    this->nCount = count;
    this->nPriceKRW = KRW;
    this->nPriceUSD = USD;
    this->nBonus = bonus;
}
int PriceTopaz::GetId()
{
    return nId;
}
int PriceTopaz::GetCount()
{
    return nCount;
}
int PriceTopaz::GetPrice(int deviceType)
{
    if (deviceType == 1)
        return nPriceKRW;
    return nPriceUSD;
}
int PriceTopaz::GetBonus()
{
    return nBonus;
}

PriceStarCandy::PriceStarCandy(int id, int count, int price, int bonus)
{
    this->nId = id;
    this->nCount = count;
    this->nPrice = price;
    this->nBonus = bonus;
}
int PriceStarCandy::GetId()
{
    return nId;
}
int PriceStarCandy::GetCount()
{
    return nCount;
}
int PriceStarCandy::GetPrice()
{
    return nPrice;
}
int PriceStarCandy::GetBonus()
{
    return nBonus;
}

//////////////////////////////////////////////////////////////////////////////////////////

MagicStaffBuildUpInfo::MagicStaffBuildUpInfo(int level, int bonusMP, int cs, int ct)
{
    this->nLevel = level;
    this->nBonusMP_percent = bonusMP;
    this->nCost_starcandy = cs;
    this->nCost_topaz = ct;
}
int MagicStaffBuildUpInfo::GetCost_StarCandy()
{
    return nCost_starcandy;
}
int MagicStaffBuildUpInfo::GetCost_Topaz()
{
    return nCost_topaz;
}
int MagicStaffBuildUpInfo::GetLevel()
{
    return nLevel;
}
int MagicStaffBuildUpInfo::GetBonusMPPercent()
{
    return nBonusMP_percent;
}

//////////////////////////////////////////////////////////////////////////////////////////


SkillSlotInfo::SkillSlotInfo(int id, int costType, int cost)
{
    this->nId = id;
    this->nCostType = costType;
    this->nCost = cost;
}
int SkillSlotInfo::GetCostType(int id)
{
    for (int i = 0 ; i < skillSlotInfo.size() ; i++)
        if (skillSlotInfo[i]->nId == id)
            return skillSlotInfo[i]->nCostType;
    return -1;
}
int SkillSlotInfo::GetCost(int id)
{
    for (int i = 0 ; i < skillSlotInfo.size() ; i++)
        if (skillSlotInfo[i]->nId == id)
            return skillSlotInfo[i]->nCost;
    return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////

PrerequisiteInfo::PrerequisiteInfo(int id, int category, int type, int value1, int value2)
{
    this->nId = id;
    this->nCategory = category;
    this->nType = type;
    this->nValue1 = value1;
    this->nValue2 = value2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FairyInfo::FairyInfo(int id, int type, int grade, int cs, int ct, int pid)
{
    this->nId = id;
    this->nType = type;
    this->sName = MakeName(this->nId);
    this->nGrade = grade;
    this->nCost_starcandy = cs;
    this->nCost_topaz = ct;
    this->nPid = pid;
}
std::string FairyInfo::MakeName(int id)
{
    switch (id)
    {
        case 1: return "꽃등신"; break;
        case 2: return "은근해"; break;
    }
    return "노네임";
}
std::string FairyInfo::GetAbilityName(FairyInfo* f, int level) // 추가속성 문구
{
    int ability = FairyBuildUpInfo::GetAbility(f->nId, level);
    char temp[30];
    
    switch (f->nType)
    {
        case 1: sprintf(temp, "연결 + %d%%", ability); break;
        case 4: sprintf(temp, "점수 + %d%%", ability); break;
        case 5: sprintf(temp, "시간 + %d초", ability); break;
        case 8: sprintf(temp, "MP + %d", ability); break;
        default: sprintf(temp, ""); break;
    }
    
    std::string res = temp;
    return res;
}
std::string FairyInfo::GetAbilityDesc(int type) // 특수능력
{
    switch (type)
    {
        case 1: return "연결피스 더 많이!"; break;
        case 2: return "미션 추가 경험치"; break;
        case 3: return "착용스킬 능력향상"; break;
        case 4: return "보너스 점수"; break;
        case 5: return "보너스 시간"; break;
        case 6: return "아이템 더 쓴다!"; break;
        case 7: return "지팡이 강화 좋게!"; break;
        case 8: return "MP 증가"; break;
    }
    return "";
}
std::string FairyInfo::GetDescription()
{
    switch (this->nId)
    {
        case 1: return "나같이 꽃같은 등신봤어~?"; break;
        case 2: return "(은근은근) (은근은근)"; break;
    }
    return "할 말이 없네요";
}
int FairyInfo::GetType()
{
    return nType;
}
int FairyInfo::GetId()
{
    return nId;
}
int FairyInfo::GetGrade()
{
    return nGrade;
}
int FairyInfo::GetCostTopaz()
{
    return nCost_topaz;
}
int FairyInfo::GetCostStarCandy()
{
    return nCost_starcandy;
}
std::string FairyInfo::GetName()
{
    return sName;
}
FairyInfo* FairyInfo::GetObj(int id)
{
    for (int i = 0 ; i < fairyInfo.size() ; i++)
    {
        if (fairyInfo[i]->GetId() == id)
            return fairyInfo[i];
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FairyBuildUpInfo::FairyBuildUpInfo(int id, int level, int ability, int refId, int cs, int ct)
{
    this->nId = id;
    this->nLevel = level;
    this->nAbility = ability;
    this->nRefId = refId;
    this->nCost_starcandy = cs;
    this->nCost_topaz = ct;
}
int FairyBuildUpInfo::GetAbility(int id, int level) // 한 요정(id)이 어떤 레벨일 때 능력치가 얼마인가?
{
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++)
    {
        if (fairyBuildUpInfo[i]->nId == id && fairyBuildUpInfo[i]->nLevel == level)
            return fairyBuildUpInfo[i]->nAbility;
    }
    return -1;
}
int FairyBuildUpInfo::GetCostTopaz(int id, int level)
{
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++)
    {
        if (fairyBuildUpInfo[i]->nId == id && fairyBuildUpInfo[i]->nLevel == level)
            return fairyBuildUpInfo[i]->nCost_topaz;
    }
    return -1;
}
int FairyBuildUpInfo::GetCostStarCandy(int id, int level)
{
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++)
    {
        if (fairyBuildUpInfo[i]->nId == id && fairyBuildUpInfo[i]->nLevel == level)
            return fairyBuildUpInfo[i]->nCost_starcandy;
    }
    return -1;
}
int FairyBuildUpInfo::GetMaxLevel(int id)
{
    int ret = -1;
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++)
    {
        if (fairyBuildUpInfo[i]->nId == id && fairyBuildUpInfo[i]->nLevel > ret)
            ret = fairyBuildUpInfo[i]->nLevel;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SkillInfo::SkillInfo(int id, std::string name, int type, int maxLevel, int mp, int staffLv, int skillId, int skillLv, int isActive)
{
    this->nId = id;
    this->sName = name;
    this->nType = type;
    this->nMaxLevel = maxLevel;
    this->nRequiredMP = mp;
    this->nRequiredStaffLv = staffLv;
    this->nRequiredSkillId = skillId;
    this->nRequiredSkillLv = skillLv;
    this->bIsActive = (isActive == 1);
}
SkillInfo* SkillInfo::GetSkillInfo(int sid)
{
    for (int i = 0 ; i < skillInfo.size() ; i++)
    {
        if (skillInfo[i]->nId == sid)
            return skillInfo[i];
    }
    return NULL;
}
std::string SkillInfo::GetShortDesc(int sid)
{
    switch (sid)
    {
        case 21: return "빨간구슬을 터뜨리면 추가점수"; break;
        case 22: return "빨간구슬 사이클로 추가폭발"; break;
        case 23: return "10개이상 제거 시 추가점수"; break;
        case 24: return "매직타임의 마법진 폭발 2회"; break;
        case 25: return "불의 정령 : 빨간구슬 모으기"; break;
        case 26: return "6개이상 제거 시 두 번 폭발"; break;
        case 27: return "코코의 자동 한붓그리기"; break;
        case 28: return "10개이상 제거 시 용의 출현"; break;
            
        case 31: return "초록구슬을 터뜨리면 추가점수"; break;
        case 32: return "초록구슬 사이클로 추가폭발"; break;
        case 33: return "지팡이 레벨에 따른 추가별사탕"; break;
        case 34: return "10개이상 제거 시 추가별사탕"; break;
        case 35: return "땅의 정령 : 초록구슬 복사"; break;
        case 36: return "6개이상 제거 시 두 번 폭발"; break;
        case 37: return "확률적으로 포션 1개 획득"; break;
        case 38: return "확률적으로 고대나무의 출현"; break;
            
        case 11: return "푸른구슬을 터뜨리면 추가점수"; break;
        case 12: return "푸른구슬 사이클로 추가폭발"; break;
    }
    return "";
}
int SkillInfo::GetId()
{
    return nId;
}
std::string SkillInfo::GetName()
{
    return sName;
}
bool SkillInfo::IsActive()
{
    return bIsActive;
}
int SkillInfo::GetMaxLevel()
{
    return nMaxLevel;
}
int SkillInfo::GetRequiredSkillId()
{
    return nRequiredSkillId;
}
int SkillInfo::GetRequiredSkillLv()
{
    return nRequiredSkillLv;
}
int SkillInfo::GetRequiredStaffLv()
{
    return nRequiredStaffLv;
}
int SkillInfo::GetRequiredMP()
{
    return nRequiredMP;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SkillBuildUpInfo::SkillBuildUpInfo(int id, std::string name, int skillLv, int maxExp, int ability1, int ability2, int prob, int cs)
{
    this->nId = id;
    this->sName = name;
    this->nSkillLv = skillLv;
    this->nSkillMaxExp = maxExp;
    this->nAbility1 = ability1;
    this->nAbility2 = ability2;
    this->nProbability = prob;
    this->nCost_starcandy = cs;
}
int SkillBuildUpInfo::GetMaxExp(int sid, int level)
{
    SkillBuildUpInfo* sbi;
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++)
    {
        sbi = skillBuildUpInfo[i];
        if (sbi->nId == sid && sbi->nSkillLv == level)
            return sbi->nSkillMaxExp;
    }
    return -1;
}
int SkillBuildUpInfo::GetCost(int sid, int level)
{
    SkillBuildUpInfo* sbi;
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++)
    {
        sbi = skillBuildUpInfo[i];
        if (sbi->nId == sid && sbi->nSkillLv == level)
            return sbi->nCost_starcandy;
    }
    return -1;
}
bool SkillBuildUpInfo::IsMastered(int sid, int level)
{
    SkillBuildUpInfo* sbi;
    int maxLevel = -1;
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++)
    {
        sbi = skillBuildUpInfo[i];
        if (sbi->nId == sid)
            maxLevel = std::max(maxLevel, sbi->nSkillLv);
    }
    if (maxLevel == level)
        return true;
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SkillPropertyInfo::SkillPropertyInfo(int id, int cost)
{
    this->nId = id;
    this->nCost_topaz = cost;
}
int SkillPropertyInfo::GetCost(int id)
{
    for (int i = 0 ; i < skillPropertyInfo.size() ; i++)
    {
        if (skillPropertyInfo[i]->nId == id)
            return skillPropertyInfo[i]->nCost_topaz;
    }
    return -1;
}
