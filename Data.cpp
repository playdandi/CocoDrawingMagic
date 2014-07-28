#include "Data.h"
#include "Kakao/Plugins/KakaoNativeExtension.h"
#include <algorithm>

// 서버 점검중일 경우 나오는 메시지
std::string serverCheckMsg;

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
//std::vector<class PrerequisiteInfo*> prerequisiteInfo;
std::vector<class FairyInfo*> fairyInfo;
std::vector<class FairyBuildUpInfo*> fairyBuildUpInfo;
std::vector<class SkillInfo*> skillInfo;
std::vector<class SkillBuildupMPInfo*> skillBuildupMPInfo;
std::vector<class SkillBuildUpInfo*> skillBuildUpInfo;
std::vector<class SkillPropertyInfo*> skillPropertyInfo;

std::vector<class LastWeeklyRank*> lastWeeklyRank;
std::vector<class TipContent*> tipContent;
std::vector<class NoticeList*> noticeList;

// 친구초대리스트
std::vector<class InviteList*> inviteList;
int todayCnt, monthCnt, totalCnt;
bool isInviteListGathered;

std::vector<class Depth*> depth;
std::vector<int> inGameSkill;
std::vector<std::string> todayCandyKakaoId;

bool isRebooting; // 시스템 재부팅 중일 시 true
bool isInGamePause; // 인게임 중에 pause되었는지 여부
bool isInGame; // 인게임 중이면 true
bool isInGameTutorial; // 인게임의 튜토리얼 중이면 true
bool isStartGameEnd = false; // 인게임 오버하고, game_end.php 를 시작했는지에 대한 flag
int savedTime;  // background로 가거나, 인게임 시작할 때 저장해 놓은 시간(시점)
int savedTime2; // 똑같은데, 친구관계에서 필요한 시간(시점)

// item type&cost (type = 1(별사탕), 2(토파즈))
int itemType[5];
int itemCost[5];

// 환경설정 메뉴
int menuInSetting = -1;

// 주간랭킹 보상 관련 변수
int myRank;
int myLastWeekHighScore;
int rewardType;
int certificateType;

// 게임에 필요한 미션 내용
int missionType;
int missionVal;
int missionRefVal;

// 바이너리 버전
int binaryVersion_current;

// 게임결과에 필요한 값들
class MyGameResult* myGameResult;

// 초보유저 보상 관련
bool isStartUser = false;

bool isPossibleBuyFairyShown = false;


// rsa 관련
RSA* rsa;
std::string publicKey;
int publicKeyIndex;

// 결제용 key
std::string gcmKey;

//
cocos2d::CCNode* verifyStatusScene;

// 튜토리얼에 필요한 값들
bool isTutorial = false;
int tutorialState = -1;

////////////////////////////////////////////////////////////////////////////////
Depth::Depth(std::string name, int priority, void* pointer)
{
    this->name = name;
    this->priority = priority;
    this->pointer = pointer;
}
const char* Depth::GetParentName()
{
    return depth[depth.size()-2]->name.c_str();
}
const char* Depth::GetCurName()
{
    return depth[depth.size()-1]->name.c_str();
}
std::string Depth::GetParentNameString()
{
    return depth[depth.size()-2]->name;
}
std::string Depth::GetCurNameString()
{
    return depth[depth.size()-1]->name;
}

int Depth::GetCurPriority()
{
    return depth[depth.size()-1]->priority;
}
void* Depth::GetParentPointer()
{
    return depth[depth.size()-2]->pointer;
}
void* Depth::GetCurPointer()
{
    return depth[depth.size()-1]->pointer;
}
void Depth::AddCurDepth(std::string name, void* pointer)
{
    int priority = (int)depth.size() * -1;
    depth.push_back( new Depth(name, priority, pointer) );
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
void Depth::DumpDepth()
{
    for (int i = 0 ; i < depth.size() ; i++)
        CCLog("%s (%d)", depth[i]->name.c_str(), depth[i]->priority);
}


////////////////////////////////////////////////////////////////////////////////
ProfileSprite::ProfileSprite(std::string profileUrl, bool preload)
{
    this->profileUrl = profileUrl;
    this->SetSpriteNoImage();
    this->preload = preload;
    this->isLoadingStarted = (profileUrl == "") ? true : false;
    this->isLoadingDone = (profileUrl == "") ? true : false;

    this->isLoadingDoneForRanking = preload;
    if (profileUrl == "")
        this->isLoadingDoneForRanking = true;
}
ProfileSprite* ProfileSprite::GetObj(std::string profileUrl)
{
    for (int i = 0 ; i < profiles.size(); i++)
    {
        if (profiles[i]->GetProfileUrl() == profileUrl)
            return profiles[i];
    }
    return NULL;
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
bool ProfileSprite::IsPreload()
{
    return preload;
}
bool ProfileSprite::IsLoadingStarted()
{
    return isLoadingStarted;
}
void ProfileSprite::SetLoadingStarted(bool flag)
{
    isLoadingStarted = flag;
}
bool ProfileSprite::IsLoadingDone()
{
    return isLoadingDone;
}
void ProfileSprite::SetLoadingDone(bool flag)
{
    isLoadingDone = flag;
}
bool ProfileSprite::IsLoadingDoneForRanking()
{
    return isLoadingDoneForRanking;
}
void ProfileSprite::SetLoadingDoneForRanking(bool flag)
{
    isLoadingDoneForRanking = flag;
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
MyGameResult::MyGameResult(int topaz, int starcandy, int potion, int mp, float addedPercent, int score, int totalscore, int combo, int bestcombo, int mission, int newrecord, std::string text)
{
    this->getTopaz = topaz;
    this->getStarCandy = starcandy;
    this->getPotion = potion;
    this->getMP = mp;
    this->addedMPPercent = addedPercent;
    this->score = score;
    this->totalScore = totalscore;
    this->combo = combo;
    this->bestCombo = bestcombo;
    this->isMissionSuccess = (mission == 1);
    this->isNewRecord = (newrecord == 1);
    this->content = text;
}

////////////////////////////////////////////////////////////////////////////////
void MyInfo::Init(std::string kakaoId, int deviceType, int userId, bool kakaoMsg, bool pushNoti, bool potionMsg, int msgCnt, std::string sessionId)
{
    this->kakaoId = kakaoId;
    this->hashedTalkUserId = KakaoLocalUser::getInstance()->hashedTalkUserId;
    this->countryIso = KakaoLocalUser::getInstance()->countryIso;
    this->messageBlocked = KakaoLocalUser::getInstance()->messageBlocked;
    this->verified =KakaoLocalUser::getInstance()->verified;
    this->deviceType = deviceType;
    this->userId = userId;
    this->settingKakaoMsg = kakaoMsg;
    this->settingPushNoti = pushNoti;
    this->settingPotionMsg = potionMsg;
    this->msgCnt = msgCnt;
    
    this->mySessionId = sessionId;
    int pos = sessionId.find("|");
    this->keyValue = atoi(sessionId.substr(0, pos).c_str());
    this->userId = atoi(sessionId.substr(pos+1, sessionId.size()-1).c_str());
    
    this->isPotionMax = 0;
    this->addedTopaz = 0;
}

void MyInfo::InitRestInfo(int topaz, int starcandy, int mp, int mpStaffPercent, int mpFairy, int staffLv, int highScore, int weeklyHighScore, int lastWeeklyHighScore, int isWeeklyRankReward, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion, int remainPotionTime, int fire, int water, int land, int master, int fireByTopaz, int waterByTopaz, int landByTopaz)
{
    this->topaz = topaz * 3 + 1892;
    this->starcandy = starcandy * 2 + 179805;
    this->mp = mp * 4 + 34890;
    this->mpStaffPercent = mpStaffPercent + 189;
    this->mpStaff = (int)((float)(GetMP() * GetMPStaffPercent()) / (float)100) + 718933 ;
    this->mpFairy = mpFairy * 2 + 22902;
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
    this->potion = potion * 5 + 11789;
    this->remainPotionTime = remainPotionTime;
    
    this->propertyFire = (fire == 1);
    this->propertyWater = (water == 1);
    this->propertyLand = (land == 1);
    this->propertyMaster = (master == 1);
    this->isPropertyFireByTopaz = (fireByTopaz == 1);
    this->isPropertyWaterByTopaz = (waterByTopaz == 1);
    this->isPropertyLandByTopaz = (landByTopaz == 1);
}

int MyInfo::GetKeyValue()
{
    return keyValue;
}
int MyInfo::GetUserId()
{
    return userId;
}
std::string MyInfo::GetSessionId()
{
    return mySessionId;
}
CCSprite* MyInfo::GetProfile()
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == myInfo->GetKakaoId())
            return ProfileSprite::GetProfile(friendList[i]->GetImageUrl());
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
std::string MyInfo::GetKakaoId()
{
    return kakaoId;
}
std::string MyInfo::GetHashedTalkUserId()
{
    return hashedTalkUserId;
}
std::string MyInfo::GetCountryIso()
{
    return countryIso;
}
bool MyInfo::IsMessageBlocked()
{
    return messageBlocked;
}
bool MyInfo::isVerified()
{
    return verified;
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
    return (topaz-1892)/3;
}
int MyInfo::GetStarCandy()
{
    return (starcandy-179805)/2;
}

int MyInfo::GetMPTotal()
{
    return GetMP() + GetMPStaff() + GetMPFairy();
}
int MyInfo::GetMP()
{
    return (mp-34890)/4;
}
int MyInfo::GetMPStaffPercent()
{
    return mpStaffPercent-189;
}
int MyInfo::GetMPStaff()
{
    return mpStaff-718933;
}
int MyInfo::GetMPFairy()
{
    return (mpFairy-22902)/2;
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
    return (potion-11789)/5;
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
    
    if (GetPotion() >= 5)
    {
        res = "총";
        sprintf(min, "%d", GetPotion());
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
bool MyInfo::IsFireByTopaz()
{
    return isPropertyFireByTopaz;
}
bool MyInfo::IsWaterByTopaz()
{
    return isPropertyWaterByTopaz;
}
bool MyInfo::IsLandByTopaz()
{
    return isPropertyLandByTopaz;
}
bool MyInfo::HasNoProperty()
{
    return (!IsFire() && !IsWater() && !IsLand() && !IsMaster());
}
bool MyInfo::IsTimeToFreelyBuyProperty()
{
    int checkProperty[5] = {0,};
    int numOfSkillsInProperty[5] = {0,};
    int idx;
    
    for (int i = 0 ; i < myInfo->GetSkillList().size() ; i++)
    {
        idx = myInfo->GetSkillList()[i]->GetCommonId() / 10;
        numOfSkillsInProperty[idx]++;
        if (myInfo->GetSkillList()[i]->GetLevel() >= 5)
            checkProperty[idx]++;
    }
    
    int count = 0;
    bool ret = true;
    for (int i = 1 ; i <= 4; i++) // i = {물,불,땅,마스터}
    {
        if (numOfSkillsInProperty[i] == 0) // 아예 스킬이 없으면 (배우지 않은 속성) 패스.
            continue;
        if (numOfSkillsInProperty[i] > 0 && numOfSkillsInProperty[i] < 7) // 일단 개수가 0과 7사이면 다 배운 게 아니므로 false
            ret = false;
        else if (numOfSkillsInProperty[i] == 7 && (checkProperty[i] < 7)) // 스킬 7개는 다 배웠지만, 5레벨 미만인 게 있는 경우도 false
            ret = false;
        else
            count++; // 조건달성한 속성의 개수
    }
    if (count >= 3) // 물/불/땅 모두 조건이 만족되면, 더 이상 열 속성이 없으므로 return false를 한다.
        ret = false;
    
    return ret;
}

void MyInfo::SetSettingVariables(bool kakaoMsgReserved, bool pushNotiReserved, bool potionMsgReserved)
{
    this->settingKakaoMsg = kakaoMsgReserved;
    this->settingPushNoti = pushNotiReserved;
    this->settingPotionMsg = potionMsgReserved;
}

void MyInfo::SetMoney(int topaz, int starcandy)
{
    this->topaz = topaz * 3 + 1892;
    this->starcandy = starcandy * 2 + 179805;
}
void MyInfo::SetPotion(int potion, int remainPotionTime)
{
    this->potion = potion * 5 + 11789;
    if (remainPotionTime != -1)
        this->remainPotionTime = remainPotionTime;
}
void MyInfo::SetCoco(int mp, int mpStaffPercent, int mpFairy, int staffLv)
{
    this->mp = mp * 4 + 34890;
    this->mpStaffPercent = mpStaffPercent + 189;
    this->mpStaff = (int)((float)(GetMP()*GetMPStaffPercent())/(float)100) + 718933;
    this->mpFairy = mpFairy * 2 + 22902;
    this->staffLv = staffLv;
}
void MyInfo::SetItem(std::vector<int> items)
{
    for (int i = 0 ; i < items.size() ; i++)
        this->item[i] = items[i];
}
void MyInfo::ChangeItem(int idx, int value)
{
    item[idx] += value;
}
void MyInfo::SetProperties(int fire, int water, int land, int master, int fireByTopaz, int waterByTopaz, int landByTopaz)
{
    this->propertyFire = (fire == 1);
    this->propertyWater = (water == 1);
    this->propertyLand = (land == 1);
    this->propertyMaster = (master == 1);
    this->isPropertyFireByTopaz = (fireByTopaz == 1);
    this->isPropertyWaterByTopaz = (waterByTopaz == 1);
    this->isPropertyLandByTopaz = (landByTopaz == 1);
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

void MyInfo::SetTodayCandy(int todayCandyTypeChoice, int todayCandyValueChoice, int todayCandyTypeMiss, int todayCandyValueMiss, int istodayCandyUsed)
{
    this->todayCandyTypeChoice = todayCandyTypeChoice;
    this->todayCandyValueChoice = todayCandyValueChoice;
    this->todayCandyTypeMiss = todayCandyTypeMiss;
    this->todayCandyValueMiss = todayCandyValueMiss;
    this->istodayCandyUsed = (istodayCandyUsed == 1);
}
void MyInfo::SetTodayCandy(int isTodayCandyUsed)
{
    this->istodayCandyUsed = (isTodayCandyUsed == 1);
}
int MyInfo::GetTodayCandyTypeChoice()
{
    return todayCandyTypeChoice;
}
int MyInfo::GetTodayCandyValueChoice()
{
    return todayCandyValueChoice;
}
int MyInfo::GetTodayCandyTypeMiss()
{
    return todayCandyTypeMiss;
}
int MyInfo::GetTodayCandyValueMiss()
{
    return todayCandyValueMiss;
}
bool MyInfo::IsTodayCandyUsed()
{
    return istodayCandyUsed;
}

void MyInfo::AddSkillSlot(int id, int csi, int usi)
{
    mySkillSlot.push_back( new MySkillSlot(id, csi, usi) );
}
void MyInfo::AddFairy(int cfi, int ufi, int level, int isUse)
{
    myFairy.push_back( new MyFairy(cfi, ufi, level, isUse) );
}
void MyInfo::AddSkill(int csi, int usi, int level, int exp, int learntime)
{
    mySkill.push_back( new MySkill(csi, usi, level, exp, learntime) );
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

void MyInfo::SetReward(int potion, int topaz)
{
    isPotionMax = potion;
    addedTopaz = topaz;
}
bool MyInfo::IsRewardPotion()
{
    return (isPotionMax == 1);
}
int MyInfo::GetRewardTopaz()
{
    return addedTopaz;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

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
MyFairy* MyFairy::GetObj(int cfi)
{
    for (int i = 0 ; i < myInfo->GetFairyList().size() ; i++)
    {
        MyFairy* mf = myInfo->GetFairyList()[i];
        if (mf->GetId() == cfi)
            return mf;
    }
    return NULL;
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

MySkill::MySkill(int csi, int usi, int level, int exp, int learntime)
{
    this->common_skill_id = csi;
    this->user_skill_id = usi;
    this->level = level;
    this->exp = exp;
    this->learnTime = learntime;
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
int MySkill::GetLearnTime()
{
    return learnTime;
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

Friend::Friend(std::string kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int remainRequestPotionTime, int remainRequestTopazTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillId, int skillLevel)
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
    this->remainRequestTopazTime = remainRequestTopazTime;
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

Friend* Friend::GetObj(std::string kakaoId)
{
    for (int i = 0 ; i < friendList.size() ; i++)
    {
        if (friendList[i]->GetKakaoId() == kakaoId)
            return friendList[i];
    }
    return NULL;
}
void Friend::SetProfile(CCSprite* sp)
{
    this->profile = sp;
}
void Friend::SetProfileUrl(std::string url)
{
    this->imageUrl = url;
}

void Friend::SetProperties(int fire, int water, int land, int master)
{
    this->propertyFire = (fire == 1) ? true : false;
    this->propertyWater = (water == 1) ? true : false;
    this->propertyLand = (land == 1) ? true : false;
    this->propertyMaster = (master == 1) ? true : false;
}

void Friend::SetPotionSprite()
{
    // potion state (내 프로필에는 당연히 포션 그림 나오면 안 된다.)
    if (kakaoId != myInfo->GetKakaoId())
    {
        CCSprite* potion;
        //if (potionMsgStatus == POTION_SEND)
        //{
            if (remainPotionTime == 0)
                potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_send.png");
            else
                potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_remain.png");
        //}
        //else
        //    potion = CCSprite::createWithSpriteFrameName("icon/icon_potion_x.png");
        
        potion->setAnchorPoint(ccp(0, 0));
        potion->setPosition(ccp(724, 24));
        potion->retain();
        
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

std::string Friend::GetKakaoId()
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
int Friend::GetRemainPotionTime(std::string kakaoId)
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
int Friend::GetRemainRequestTopazTime()
{
    return remainRequestTopazTime;
}
void Friend::SetRemainPotionTime(int time)
{
    remainPotionTime = time;
}
void Friend::SetRemainRequestPotionTime(int time)
{
    remainRequestPotionTime = time;
}
void Friend::SetRemainRequestTopazTime(int time)
{
    remainRequestTopazTime = time;
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

void Friend::SetKakaoVariables(std::string name, std::string purl, std::string htuid, bool msgblocked, bool supporteddevice)
{
    this->nickname = name;
    this->nicknameLabel->setString(name.c_str());
    //this->imageUrl = purl;
    this->hashedTalkUserId = htuid;
    this->messageBlocked = msgblocked;
    this->supportedDevice = supporteddevice;
}
std::string Friend::GetHashedTalkUserId()
{
    return hashedTalkUserId;
}
bool Friend::IsMessageBlocked()
{
    return messageBlocked;
}
bool Friend::IsSupportedDevice()
{
    return supportedDevice;
}

////////////////////////////////////////////////////////////////////////

bool compare(Friend *f1, Friend *f2)
{
    if (f1->GetWeeklyHighScore() == f2->GetWeeklyHighScore())
    {
        if (f2->GetWeeklyHighScore() == -1 && f2->GetKakaoId() == myInfo->GetKakaoId())
            return false;
        if (f1->GetWeeklyHighScore() == -1 && f1->GetKakaoId() == myInfo->GetKakaoId())
            return true;
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
Msg::Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl, std::string friendKakaoId)
{
    this->id = id;
    this->type = type;
    this->rewardCount = rewardCount;
    this->content = content;
    this->profileUrl = profileUrl;
    this->noticeUrl = noticeUrl;
    this->friendKakaoId = friendKakaoId;
    this->profile = NULL;
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
std::string Msg::GetFriendKakaoId()
{
    return friendKakaoId;
}
CCSprite* Msg::GetProfile()
{
    return profile;
}
void Msg::SetProfile(CCSprite* sp)
{
    profile = sp;
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

MagicStaffBuildUpInfo::MagicStaffBuildUpInfo(int level, int bonusMPPercent, int bonusMPPlus, int cs, int ct)
{
    this->nLevel = level;
    this->nBonusMP_percent = bonusMPPercent;
    this->nBonusMP_plus = bonusMPPlus;
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
int MagicStaffBuildUpInfo::GetBonusMPPlus()
{
    return nBonusMP_plus;
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
/*
PrerequisiteInfo::PrerequisiteInfo(int id, int category, int type, int value1, int value2)
{
    this->nId = id;
    this->nCategory = category;
    this->nType = type;
    this->nValue1 = value1;
    this->nValue2 = value2;
}
*/
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
        case 3: return "구르미"; break;
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
std::string FairyInfo::GetAbilityDesc(int type, bool newline) // 특수능력
{
    if (newline)
    {
        switch (type)
        {
            case 1: return "연결피스\n확률증가"; break;
            case 9: return "피버타임\n시간증가"; break;
            case 10: return "별사탕\n추가획득"; break;
            /*
        case 4: return "보너스 점수"; break;
        case 5: return "보너스 시간"; break;
        case 6: return "아이템 더 쓴다!"; break;
        case 7: return "지팡이 강화 좋게!"; break;
        case 8: return "MP 증가"; break;
             */
        }
    }
    else
    {
        switch (type)
        {
            case 1: return "연결피스 확률증가"; break;
            case 9: return "피버타임 시간증가"; break;
            case 10: return "별사탕 추가획득"; break;
        }
    }
    return "";
}
std::string FairyInfo::GetDescription()
{
    switch (this->nId)
    {
        case 1: return "나같이 꽃같은 등신봤어~?"; break;
        case 2: return "(은근은근) (은근은근)"; break;
        case 3: return "뭉게뭉게 무웅게무웅게"; break;
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
int FairyBuildUpInfo::GetTotalMP(int id, int level)
{
    int mp = 0;
    for (int i = 0 ; i < fairyBuildUpInfo.size() ; i++)
    {
        if (fairyBuildUpInfo[i]->nId == id && fairyBuildUpInfo[i]->nLevel <= level)
            mp += fairyBuildUpInfo[i]->nAbility;
    }
    return mp;
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
std::string SkillInfo::GetShortDesc(int sid) // 스케치북에 나오는 설명
{
    switch (sid)
    {
        case 21: return "빨간피스 그리면 추가점수"; break;
        case 22: return "싸이클로 그리면 추가피스 제거"; break;
        case 23: return "하트로 빨간피스 생성"; break;
        case 24: return "8개 이상 그리면 추가점수"; break;
        case 25: return "6개 이상 그리면 두배 제거"; break;
        case 26: return "확률로 Fever Time 모드 전환"; break;
        case 27: return "10개 이상 그리면 붉은 용 소환"; break;
            
        case 11: return "파란피스 그리면 추가점수"; break;
        case 12: return "싸이클로 그리면 파도 발생"; break;
        case 13: return "흰색,노란색 피스를 파란피스로"; break;
        case 14: return "10 Combo 마다 추가점수"; break;
        case 15: return "6개 이상 그리면 두배 제거"; break;
        case 16: return "시간을 얼려 5초간 시간 정지"; break;
        case 17: return "10개 이상 그리면 여신 소환"; break;
            
        case 31: return "초록피스 그리면 추가점수"; break;
        case 32: return "싸이클로 그리면 추가피스 제거"; break;
        case 33: return "마법잎사귀가 초록피스를 모음"; break;
        case 34: return "보너스타임 때 추가별사탕 획득"; break;
        case 35: return "6개 이상 그리면 두배 제거"; break;
        case 36: return "확률적으로 포션 1개 획득"; break;
        case 37: return "10개 이상 그리면 고대나무 소환"; break;
    }
    return "";
}
std::string SkillInfo::GetFullDesc(int sid)
{
    switch (sid)
    {
        case 21: return "빨간 피스를 그리면 추가 점수를 획득해요."; break;
        case 22: return "빨간 피스를 사이클로 그리면 주변 피스를 터뜨릴 수 있어요."; break;
        case 23: return "사랑의 불꽃으로 빨간 피스를 여러 개 만들어요."; break;
        case 24: return "빨간 피스를 한 번에 8개 이상 그리면 추가 점수를 획득해요."; break;
        case 25: return "빨간 피스를 한 번에 6개 이상 그리면 불꽃으로 한 번 더 터뜨려요."; break;
        case 26: return "가끔 코코가 스스로 피스들을 터뜨려요."; break;
        case 27: return "빨간 피스를 한 번에 10개 이상 그리면 드래곤을 소환해요!"; break;
            
        case 11: return "파란 피스를 그리면 추가 점수를 획득해요."; break;
        case 12: return "파란 피스를 사이클로 그리면 파도가 피스를 터뜨려요."; break;
        case 13: return "파란 나비가 날아와 파란 피스를 만들어요."; break;
        case 14: return "10콤보마다 추가 점수를 획득해요."; break;
        case 15: return "파란 피스를 한 번에 6개 이상 그리면 얼음비가 한 번 더 터뜨려요."; break;
        case 16: return "코코가 시간을 얼려 잠시 동안 시간이 가지 않아요."; break;
        case 17: return "파란 피스를 한 번에 10개 이상 그리면 여신을 소환해요!"; break;
            
        case 31: return "초록 피스를 그리면 추가 점수를 획득해요."; break;
        case 32: return "초록 피스를 사이클로 그리면 무작위로 피스를 터뜨려요."; break;
        case 33: return "마법 잎사귀가 초록 피스를 모두 모아줘요."; break;
        case 34: return "지팡이의 레벨만큼 별사탕을 추가로 획득해요."; break;
        case 35: return "초록 피스를 한 번에 6개 이상 그리면 상쾌한 바람으로 한 번 더 터뜨려요."; break;
        case 36: return "가끔씩 포션 1개를 얻을 수 있어요."; break;
        case 37: return "초록 피스를 한 번에 10개 이상 그리면 고대나무를 소환해요!"; break;
    }
    return "";
}
int SkillInfo::Converted(int id) // 인게임에서 사용되는 스킬 id로 conversion
{
    switch(id)
    {
        case 11: return 8; break;
        case 12: return 9; break;
        case 13: return 12; break;
        case 14: return 10; break;
        case 15: return 13; break;
        case 16: return 14; break;
        case 17: return 15; break;
            
        case 21: return 0; break;
        case 22: return 1; break;
        case 23: return 4; break;
        case 24: return 2; break;
        case 25: return 5; break;
        case 26: return 6; break;
        case 27: return 7; break;
            
        case 31: return 16; break;
        case 32: return 17; break;
        case 33: return 20; break;
        case 34: return 18; break;
        case 35: return 21; break;
        case 36: return 22; break;
        case 37: return 23; break;
    }
    return -1;
}
int SkillInfo::ConvertedToOriginal(int skillId) // 인게임의 스킬id를 다시 원래 id로 conversion
{
    switch(skillId)
    {
        case 8: return 11; break;
        case 9: return 12; break;
        case 12: return 13; break;
        case 10: return 14; break;
        case 13: return 15; break;
        case 14: return 16; break;
        case 15: return 17; break;
            
        case 0: return 21; break;
        case 1: return 22; break;
        case 4: return 23; break;
        case 2: return 24; break;
        case 5: return 25; break;
        case 6: return 26; break;
        case 7: return 27; break;
            
        case 16: return 31; break;
        case 17: return 32; break;
        case 20: return 33; break;
        case 18: return 34; break;
        case 21: return 35; break;
        case 22: return 36; break;
        case 23: return 37; break;
    }
    return -1;
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

SkillBuildupMPInfo::SkillBuildupMPInfo(int skillCount, int requireMP, int discount1, int discount2)
{
    this->nSkillCount = skillCount;
    this->nRequireMP = requireMP;
    this->nDiscountOne = discount1;
    this->nDiscountTwo = discount2;
}
SkillBuildupMPInfo* SkillBuildupMPInfo::GetObj(int skillCount)
{
    for (int i = 0 ; i < skillBuildupMPInfo.size() ; i++)
    {
        if (skillBuildupMPInfo[i]->GetSkillCount() == skillCount)
            return skillBuildupMPInfo[i];
    }
    return NULL;
}
int SkillBuildupMPInfo::GetOrder(std::vector<MySkill*> sList, int scid)
{
    // '?' 직전 스킬의 common-id가 뭔지 찾기.
    int p;
    for (int i = 0 ; i < sList.size() ; i++)
    {
        if (sList[i]->GetCommonId() == scid-1)
            p = i;
    }
    
    // [처음 배운 스킬 ~ '?' 직전 스킬]을 속성별로 몇 개씩인지 분류한다.
    // (단, 속성별 첫 번째 스킬은 직접 배우는 것이 아니므로 개념상 제외한다)
    int eachPropertyCnt[5] = {0,};
    for (int i = 0 ; i <= p ; i++)
        //if (sList[i]->GetCommonId() % 10 != 1)
        eachPropertyCnt[ sList[i]->GetCommonId() / 10 ]++;
    
    // 각 속성마다 그것+1개만큼 개수를 계산한다. (+1 하는 이유는, 예를 들어 불 속성이 2개 있다면 필연적으로 불의 3번째 '?' 스킬이 등장했기 때문)
    int orderNumber = 0;
    for (int i = 1 ; i <= 3 ; i++) // 1(물), 2(불), 3(땅)
        orderNumber += (eachPropertyCnt[i]) - (eachPropertyCnt[i] == 7);
    
    return orderNumber;
}
int SkillBuildupMPInfo::RequiredMP(std::vector<MySkill*> sList, int scid)
{
    // '?' 스킬이 몇 번째로 등장한 것인지 알아내자.
    
    int orderNumber = SkillBuildupMPInfo::GetOrder(sList, scid);
    
    // finally, {orderNumber}번째 스킬 (우리가 원하는 '?' 스킬) 의 요구 MP를 구한다.
    SkillBuildupMPInfo* sInfoMP = SkillBuildupMPInfo::GetObj( orderNumber );
    float requiredMP = (float)sInfoMP->GetRequireMP();
    
    // 할인 적용을 위한 내용
    int numOfPropertiesByTopaz = 0;
    if (myInfo->IsFireByTopaz()) numOfPropertiesByTopaz++;
    if (myInfo->IsWaterByTopaz()) numOfPropertiesByTopaz++;
    if (myInfo->IsLandByTopaz()) numOfPropertiesByTopaz++;
    
    if (numOfPropertiesByTopaz == 1)
        requiredMP = (float)requiredMP * (float)(100 - sInfoMP->GetDiscount1()) / (float)100;
    else if (numOfPropertiesByTopaz >= 2)
        requiredMP = (float)requiredMP * (float)(100 - sInfoMP->GetDiscount2()) / (float)100;
    
    return (int)requiredMP;
}
int SkillBuildupMPInfo::GetSkillCount()
{
    return nSkillCount;
}
int SkillBuildupMPInfo::GetRequireMP()
{
    return nRequireMP;
}
int SkillBuildupMPInfo::GetDiscount1()
{
    return nDiscountOne;
}
int SkillBuildupMPInfo::GetDiscount2()
{
    return nDiscountTwo;
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
SkillBuildUpInfo* SkillBuildUpInfo::GetObj(int sid, int level)
{
    for (int i = 0 ; i < skillBuildUpInfo.size() ; i++)
        if (skillBuildUpInfo[i]->GetId() == sid && skillBuildUpInfo[i]->GetLevel() == level)
            return skillBuildUpInfo[i];
    return NULL;
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
int SkillBuildUpInfo::GetId()
{
    return nId;
}
int SkillBuildUpInfo::GetAbility1()
{
    return nAbility1;
}
int SkillBuildUpInfo::GetAbility2()
{
    return nAbility2;
}
int SkillBuildUpInfo::GetProb()
{
    return nProbability;
}
int SkillBuildUpInfo::GetLevel()
{
    return nSkillLv;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TipContent::TipContent(int id, int type, std::string script)
{
    this->nId = id;
    this->nCategory = type;
    this->content = script;
}
int TipContent::GetId()
{
    return nId;
}
int TipContent::GetCategory()
{
    return nCategory;
}
std::string TipContent::GetContent()
{
    return content;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InviteList::InviteList(std::string userid, std::string name, std::string purl, std::string htuid, bool msgblocked, bool supporteddevice, bool wi)
{
    this->userId = userid;
    this->nickname = name;
    this->profileUrl = purl;
    this->hashedTalkUserId = htuid;
    this->messageBlocked = msgblocked;
    this->supportedDevice = supporteddevice;
    this->wasInvited = wi;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

NoticeList::NoticeList(int i, int pf, std::string t, std::string m, std::string l)
{
    this->id = i;
    this->platform = pf;
    this->title = t;
    this->message = m;
    this->link = l;
    this->isShown = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 난독화 public keys
std::string obfuscationKey[30] = {
"3898bj3483",
"cjke38v3gu",
"nmcyw27gec",
"kj98gjdfh3",
"v83jgj3jf7",
"zzm398gj3f",
"bn39bj3ak3",
"03498bk1cd",
"qoibmx9384",
"vm38vw38vc",
 "al289vybnx",
 "vmf3487ghj",
 "7vhj4g873f",
 "vncyuw7uvw",
 "cnvjf7g3fs",
 "vjkvue7vcs",
 "kvue7vh742",
 "bvnbwe7fyu",
 "ljkbvjueh2",
 "mvnju37vje",
 "vnvn38734e",
 "vmeuv73hjd",
 "nv93jg7230",
 "nm389g73hf",
 "8923jg6y33",
 "jv8237vj3w",
 "1j2389f83f",
 "8vj3ejvc73",
 "mvjc82387d",
 "nn27d72g4h",
};

std::string basicKey = "-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBANTPl77S5NRHJlyVXiZIv36bpkX8m+5A\na0NM3S+BliP3l8LntujYQtT/uelabrfDGwgbbhcaqfU06AwbYA7R4jECAwEAAQ==\n-----END PUBLIC KEY-----";
