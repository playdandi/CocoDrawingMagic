#ifndef __CocoMagic__Data__
#define __CocoMagic__Data__

#include "Common.h"

#define POTION_X 0
#define POTION_SEND 1
#define POTION_REMAIN 2
#define POTION_NOTHING 3

extern int iGameVersion;
extern int iBinaryVersion;
extern class MyInfo* myInfo;
extern std::vector<class ProfileSprite*> profiles;
extern std::vector<class Friend*> friendList;
extern std::vector<class Msg*> msgData;

extern std::vector<class PriceTopaz*> priceTopaz;
extern std::vector<class PriceStarCandy*> priceStarCandy;
extern std::vector<class MagicStaffBuildUpInfo*> magicStaffBuildupInfo;
extern std::vector<class SkillSlotInfo*> skillSlotInfo;
extern std::vector<class PrerequisiteInfo*> prerequisiteInfo;
extern std::vector<class FairyInfo*> fairyInfo;
extern std::vector<class FairyBuildUpInfo*> fairyBuildUpInfo;
extern std::vector<class SkillInfo*> skillInfo;
extern std::vector<class SkillBuildUpInfo*> skillBuildUpInfo;
extern std::vector<class SkillPropertyInfo*> skillPropertyInfo;

extern std::vector<class LastWeeklyRank*> lastWeeklyRank;

extern std::vector<int> inGameSkill;
extern std::vector<class Depth*> depth;
extern std::vector<int> todayCandyKakaoId;

extern bool isRebooting;
extern bool isInGame;
extern bool isInGamePause;
extern int savedTime;

// 주간랭킹 보상 관련 변수
extern int myRank;
extern int myLastWeekHighScore;
extern int rewardType;

// 게임에 필요한 미션 내용
extern int missionType;
extern int missionVal;
extern int missionRefVal;

// 게임결과에 필요한 값들
extern class MyGameResult* myGameResult;


using namespace cocos2d;

class Depth
{
public:
    Depth(std::string name, int priority, void* pointer = NULL);
    static void* GetCurPointer();
    static void* GetParentPointer();
    static const char* GetCurName();
    static const char* GetParentName();
    static std::string GetCurNameString();
    static std::string GetParentNameString();
    static int GetCurPriority();
    static void AddCurDepth(std::string name, void* pointer = NULL);
    static void RemoveCurDepth();
    static void ClearDepth();
    static void DumpDepth();
    
    int priority;
    std::string name;
    void* pointer;
};

class ProfileSprite
{
public:
    ProfileSprite(std::string profileUrl);
    static CCSprite* GetProfile(std::string profileUrl);
    CCSprite* GetProfile();
    std::string GetProfileUrl();
    void SetSpriteNoImage();
    void SetSprite(CCTexture2D* texture);
private:
    CCSprite* profile;
    std::string profileUrl;
};

class MyGameResult
{
public:
    MyGameResult(int topaz, int starcandy, int potion, int mp, int score, int totalscore, int combo, int mission, int newrecord);
    int getTopaz;
    int getStarCandy;
    int getPotion;
    int getMP;
    int score;
    int totalScore;
    int combo;
    bool isMissionSuccess;
    bool isNewRecord;
    std::vector<int> skillNum;
    std::vector<int> skillCnt;
};

class MyInfo
{
public:
    void Init(int kakaoId, int deviceType, int userId, bool kakaoMsg, bool pushNoti, bool potionMsg, int msgCnt);
    void InitRestInfo(int topaz, int starcandy, int mp, int mpStaff, int mpFairy, int staffLv, int highScore, int weeklyHighScore, int lastWeeklyHighScore, int isWeeklyRankReward, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion ,int remainPotionTime, int fire, int water, int land, int master);
    
    std::string GetSessionId();
    void SetSessionId(std::string sessionId);
    static CCSprite* GetProfile();
    static std::string GetName();
    int GetDeviceType();
    int GetKakaoId();
    int GetMsgCnt();
    void SetMsgCnt(int cnt);
    bool GetKakaoMsg();
    bool GetPushNotification();
    bool GetPotionMsg();
    
    int GetTopaz();
    int GetStarCandy();
    int GetMP();
    int GetMPStaffPercent();
    int GetMPStaff();
    int GetMPFairy();
    int GetMPTotal();

    int GetStaffLv();
    int GetHighScore();
    int GetWeeklyHighScore();
    int GetLastWeeklyHighScore();
    bool IsWeeklyRankReward();
    
    int GetCertificateType();
    int GetRemainWeeklyRankTimeInt();
    std::string GetRemainWeeklyRankTime();
    
    int GetItem(int idx);
    int GetPotion();
    int GetRemainPotionTimeNumber();
    std::string GetRemainPotionTime();
    bool IsFire();
    bool IsWater();
    bool IsLand();
    bool IsMaster();
    
    void SetSettingVariables(bool kakaoMsgReserved, bool pushNotiReserved, bool potionMsgReserved);
    void SetMoney(int topaz, int starcandy);
    void SetPotion(int potion, int remainPotionTime);
    void SetCoco(int mp, int mpStaff, int mpFairy, int staffLv);
    void SetItem(std::vector<int> items);
    void SetProperties(int fire, int water, int land, int master);
    void SetScore(int highScore, int weeklyHighScore, int certificateType, int remainWeeklyRankTime);
    void SetRemainWeeklyRankTime(int time);
    
    void SetProfileSkill(int id, int level);
    int GetProfileSkillId();
    int GetProfileSkillLv();
    void SetPracticeSkill(int id, int level);
    int GetPracticeSkillId();
    int GetPracticeSkillLv();
    
    void SetTodayCandy(int todayCandyType, int todayCandyValueChoice, int todayCandyValueMiss, int istodayCandyUsed);
    void SetTodayCandy(int isTodayCandyUsed);
    int GetTodayCandyType();
    int GetTodayCandyValueChoice();
    int GetTodayCandyValueMiss();
    bool IsTodayCandyUsed();
    
    void AddSkillSlot(int id, int csi, int usi);
    void AddFairy(int cfi, int ufi, int level, int isUse);
    void AddSkill(int csi, int usi, int level, int exp);
    void SortMySkillByCommonId();
    
    int GetActiveFairyId();
    int GetActiveFairyUserId();
    int GetActiveFairyLevel();
    
    std::vector<class MyFairy*> GetFairyList();
    std::vector<class MySkill*> GetSkillList();
    std::vector<class MySkillSlot*> GetSlot();
    void ClearFairyList();
    void ClearSkillList();
    void ClearSkillSlot();

private:
    std::string mySessionId;
    
    bool settingKakaoMsg;
    bool settingPushNoti;
    bool settingPotionMsg;
    
    int kakaoId;
    int deviceType;
    int userId;
    int msgCnt;
    
    int topaz;
    int starcandy;
    
    int mp;
    int mpStaffPercent;
    int mpStaff;
    int mpFairy;
    int staffLv;
    
    int highScore;
    int weeklyHighScore;
    int lastWeeklyHighScore;
    bool isWeeklyRankReward;
    
    int certificateType;
    int remainWeeklyRankTime;
    int item[5];
    int potion;
    int remainPotionTime;
    bool propertyFire;
    bool propertyWater;
    bool propertyLand;
    bool propertyMaster;
    
    int profileSkillId;
    int profileSkillLv;
    int practiceSkillId;
    int practiceSkillLv;
    
    int todayCandyType;
    int todayCandyValueChoice;
    int todayCandyValueMiss;
    bool istodayCandyUsed;
    
    std::vector<class MySkillSlot*> mySkillSlot;
    std::vector<class MyFairy*> myFairy;
    std::vector<class MySkill*> mySkill;
};

class MySkillSlot
{
public:
    MySkillSlot(int id, int csi, int usi);
    void InsertSkill(int scid, int suid);
    int GetId();
    int GetCommonId();
    int GetUserId();
private:
    int id;
    int common_skill_id;
    int user_skill_id;
};
class MyFairy
{
public:
    MyFairy(int cfi, int ufi, int level, int isUse);
    static MyFairy* GetObj(int cfi);
    bool IsUse();
    int GetId();
    int GetUserId();
    int GetLevel();
private:
    int common_fairy_id;
    int user_fairy_id;
    int level;
    int isUse;
};
class MySkill
{
public:
    MySkill(int csi, int usi, int level, int exp);
    static MySkill* GetObj(int scid);
    int GetCommonId();
    int GetUserId();
    int GetLevel();
    int GetExp();
private:
    int common_skill_id;
    int user_skill_id;
    int level;
    int exp;
};

/////////////////////////////////////////////////////////////////////////////////
class LastWeeklyRank
{
public:
    LastWeeklyRank(std::string nickname, std::string profileUrl, int rank, int score, int isFriend);
    std::string GetNickname();
    std::string GetProfileUrl();
    int GetRank();
    int GetScore();
    bool IsFriend();
    static void SortByRank();
private:
    std::string nickname;
    std::string profileUrl;
    int rank;
    int score;
    bool isFriend;
};

////////////////////////////////////////////////////////////////////////////////
class Friend
{
public:
    Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int remainRequestPotionTime, int remainRequestTopazTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillid, int skillLevel);
    
    void SetProfile(CCSprite* sp);
    void SetPotionSprite();
    void SetScore(int highScore, int weeklyHighScore, int certificateType);
    
    static Friend* GetObj(int kakaoId);
    static int GetRemainPotionTime(int kakaoId);
    
    int GetKakaoId();
    CCSprite* GetProfile();
    std::string GetImageUrl();
    std::string GetNickname();
    CCLabelTTF* GetNicknameLabel();
    int GetPotionMsgStatus();
    void SetPotionMsgStatus(int status);
    CCSprite* GetPotionSprite();
    CCLabelTTF* GetPotionLabelMin();
    CCLabelTTF* GetPotionLabelSec();

    static void ChangeMyFairyInfo();
    
    int GetWeeklyHighScore();
    int GetHighScore();
    int GetScoreUpdateTime();
    int GetRemainPotionTime();
    int GetRemainRequestPotionTime();
    int GetRemainRequestTopazTime();
    void SetRemainPotionTime(int time);
    void SetRemainRequestPotionTime(int time);
    void SetRemainRequestTopazTime(int time);
    void SetProperties(int fire, int water, int land, int master);
    bool IsFire();
    bool IsWater();
    bool IsLand();
    bool IsMaster();
    int GetFairyId();
    int GetFairyLv();
    int GetSkillId();
    int GetSkillLv();
    
private:
    int kakaoId;
    std::string nickname;
    CCLabelTTF* nicknameLabel;
    std::string imageUrl;
    CCSprite* profile;
    int potionMsgStatus;
    int remainPotionTime;
    int remainRequestPotionTime;
    int remainRequestTopazTime;
    CCSprite* potionSprite;
    CCLabelTTF* potionRemainTimeMin;
    CCLabelTTF* potionRemainTimeSec;
    int highScore;
    int weeklyHighScore;
    int scoreUpdateTime;
    int certificateType;
    bool propertyFire;
    bool propertyWater;
    bool propertyLand;
    bool propertyMaster;
    int fairyId;
    int fairyLevel;
    int skillId;
    int skillLevel;
};

////////////////////////////////////////////////////////////////////////////////
class DataProcess
{
public:
    static void SortFriendListByScore();
    static void SortMagicStaffBuildUpInfo();
    //static void SortMySkillByCommonId(std::vector<MySkill*> mySkill);
   // static void SortMySkillByCommonId();
    static std::string FindSkillNameById(int skillId);
};


////////////////////////////////////////////////////////////////////////////////
class Msg
{
public:
    Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl, int friendKakaoId);
    int GetId();
    int GetType();
    int GetRewardCount();
    std::string GetContent();
    std::string GetProfileUrl();
    std::string GetNoticeUrl();
    int GetFriendKakaoId();

private:
    int id;
    int type;
    int rewardCount;
    std::string content;
    std::string profileUrl;
    CCSprite* profile;
    std::string noticeUrl;
    int friendKakaoId;
};


////////////////////////////////////////////////////////////////////////////////
class PriceTopaz
{
public:
    PriceTopaz(int id, int count, int KRW, int USD, int bonus);
    int GetId();
    int GetCount();
    int GetPrice(int deviceType);
    int GetBonus();
private:
    int nId;
    int nCount;
    int nPriceKRW;
    int nPriceUSD;
    int nBonus;
};
class PriceStarCandy
{
public:
    PriceStarCandy(int id, int count, int price, int bonus);
    int GetId();
    int GetCount();
    int GetPrice();
    int GetBonus();
private:
    int nId;
    int nCount;
    int nPrice;
    int nBonus;
};

class MagicStaffBuildUpInfo
{
public:
    MagicStaffBuildUpInfo(int level, int bonusMP, int cs, int ct);
    int GetCost_StarCandy();
    int GetCost_Topaz();
    int GetLevel();
    int GetBonusMPPercent();
private:
    int nLevel;
    int nBonusMP_percent;
    int nCost_starcandy;
    int nCost_topaz;
};

class SkillSlotInfo
{
public:
    SkillSlotInfo(int id, int costType, int cost);
    static int GetCostType(int id);
    static int GetCost(int id);
private:
    int nId;
    int nCostType;
    int nCost;
};

class PrerequisiteInfo
{
public:
    PrerequisiteInfo(int id, int category, int type, int value1, int value2);
private:
    int nId;
    int nCategory;
    int nType;
    int nValue1;
    int nValue2;
};


class FairyInfo
{
public:
    FairyInfo(int id, int type, int grade, int cs, int ct, int pid);
    std::string MakeName(int id);
    static std::string GetAbilityName(FairyInfo* f, int level);
    static std::string GetAbilityDesc(int type);
    std::string GetDescription();
    int GetId();
    int GetGrade();
    int GetType();
    int GetCostTopaz();
    int GetCostStarCandy();
    std::string GetName();
    static FairyInfo* GetObj(int id);
private:
    int nId;
    int nType;
    std::string sName;
    int nGrade;
    int nCost_starcandy;
    int nCost_topaz;
    int nPid;
};

class FairyBuildUpInfo
{
public:
    FairyBuildUpInfo(int id, int level, int ability, int refId, int cs, int ct);
    static int GetAbility(int id, int level);
    static int GetMaxLevel(int id);
    static int GetCostTopaz(int id, int level);
    static int GetCostStarCandy(int id, int level);
private:
    int nId;
    int nLevel;
    int nAbility;
    int nRefId;
    int nCost_starcandy;
    int nCost_topaz;
};

class SkillInfo
{
public:
    SkillInfo(int id, std::string name, int type, int maxLevel, int mp, int staffLv, int skillId, int skillLv, int isActive);
    int GetId();
    std::string GetName();
    static SkillInfo* GetSkillInfo(int sid);
    static std::string GetShortDesc(int sid);
    static std::string GetFullDesc(int sid);
    int GetMaxLevel();
    int GetRequiredSkillId();
    int GetRequiredSkillLv();
    int GetRequiredStaffLv();
    int GetRequiredMP();
    bool IsActive();
private:
    int nId;
    std::string sName;
    int nType; // 속성
    int nMaxLevel;
    int nRequiredMP;
    int nRequiredStaffLv;
    int nRequiredSkillId;
    int nRequiredSkillLv;
    bool bIsActive;
};

class SkillBuildUpInfo
{
public:
    SkillBuildUpInfo(int id, std::string name, int skillLv, int maxExp, int ability1, int ability2, int prob, int cs);
    static bool IsMastered(int sid, int level);
    static int GetMaxExp(int sid, int level);
    static int GetCost(int sid, int level);
private:
    int nId;
    std::string sName;
    int nSkillLv;
    int nSkillMaxExp;
    int nAbility1;
    int nAbility2;
    int nProbability;
    int nCost_starcandy;
};

class SkillPropertyInfo
{
public:
    SkillPropertyInfo(int id, int cost);
    static int GetCost(int id);
private:
    int nId;
    int nCost_topaz;
};

#endif /* defined(__CocoMagic__Data__) */