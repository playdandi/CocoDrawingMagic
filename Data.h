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

using namespace cocos2d;



class MyInfo
{
public:
    void Init(int kakaoId, int deviceType, int userId, bool kakaoMsg, bool pushNoti, bool potionMsg, int msgCnt);
    void InitRestInfo(int topaz, int starcandy, int mp, int mpStaff, int mpFairy, int staffLv, int highScore, int weeklyHighScore, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion ,int remainPotionTime, int fire, int water, int land, int master);
    
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
    int GetCertificateType();
    int GetRemainWeeklyRankTimeInt();
    std::string GetRemainWeeklyRankTime();
    
    int GetItem(int idx);
    int GetPotion();
    std::string GetRemainPotionTime();
    bool IsFire();
    bool IsWater();
    bool IsLand();
    bool IsMaster();
    
    void SetSettingVariables(bool kakaoMsgReserved, bool pushNotiReserved, bool potionMsgReserved);
    void SetMoney(int topaz, int starcandy);
    void SetPotion(int potion, int remainPotionTime);
    void SetCoco(int mp, int mpStaff, int mpFairy, int staffLv);
    //void SetNextStaff(int staffLvNext, int mpNextCostStarcandy, int mpNextCostTopaz, int staffNextPercent);
    
    void SetProfileSkill(int id, int level);
    int GetProfileSkillId();
    int GetProfileSkillLv();
    void SetPracticeSkill(int id, int level);
    int GetPracticeSkillId();
    int GetPracticeSkillLv();
    
    void AddSkillSlot(int id, int csi, int usi);
    void AddFairy(int cfi, int ufi, int level, int isUse);
    void AddSkill(int csi, int usi, int level, int exp);
    
    int GetActiveFairyId();
    int GetActiveFairyUserId();
    int GetActiveFairyLevel();
    
    std::vector<class MyFairy*> GetFairyList();
    std::vector<class MySkill*> GetSkillList();
    std::vector<class MySkillSlot*> GetSlot();
    void ClearFairyList();

private:
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
    
    std::vector<class MySkillSlot*> mySkillSlot;
    std::vector<class MyFairy*> myFairy;
    std::vector<class MySkill*> mySkill;
};

class MySkillSlot
{
public:
    MySkillSlot(int id, int csi, int usi);
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
private:
    int common_skill_id;
    int user_skill_id;
    int level;
    int exp;
};


////////////////////////////////////////////////////////////////////////////////
class Friend
{
public:
    Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillid, int skillLevel);
    
    void SetSprite(CCTexture2D* texture);
    void SetSprite();
    void SetPotionSprite();
    
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
    void SetRemainPotionTime(int time);
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
    static std::string FindSkillNameById(int skillId);
};


////////////////////////////////////////////////////////////////////////////////
class Msg
{
public:
    Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl);
    int GetId();
    int GetType();
    int GetRewardCount();
    std::string GetContent();

private:
    int id;
    int type;
    int rewardCount;
    std::string content;
    std::string profileUrl;
    CCSprite* profile;
    std::string noticeUrl;
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
    SkillInfo(int id, std::string name, int type, int maxLevel, int mp, int staffLv, int skillId, int skillLv);
    int GetId();
    std::string GetName();
private:
    int nId;
    std::string sName;
    int nType; // 속성
    int nMaxLevel;
    int nRequiredMP;
    int nRequiredStaffLv;
    int nRequiredSkillId;
    int nRequiredSkillLv;
};

class SkillBuildUpInfo
{
public:
    SkillBuildUpInfo(int id, std::string name, int skillLv, int maxExp, int ability1, int ability2, int prob, int cs);
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
private:
    int nId;
    int nCost_topaz;
};

#endif /* defined(__CocoMagic__Data__) */