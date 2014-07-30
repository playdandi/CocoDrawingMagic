#ifndef __CocoMagic__Data__
#define __CocoMagic__Data__

#include "Common.h"
#include "Kakao/Common/KakaoLocalUser.h"
#include "Kakao/Common/KakaoFriends.h"

#include "openssl/rsa.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/objects.h"
#include "openssl/x509.h"
#include "openssl/rand.h"
#include "openssl/pem.h"

#define POTION_X 0
#define POTION_SEND 1
#define POTION_REMAIN 2
#define POTION_NOTHING 3

// 서버 점검중일 경우 나오는 메시지
extern std::string serverCheckMsg;

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
//extern std::vector<class PrerequisiteInfo*> prerequisiteInfo;
extern std::vector<class FairyInfo*> fairyInfo;
extern std::vector<class FairyBuildUpInfo*> fairyBuildUpInfo;
extern std::vector<class SkillInfo*> skillInfo;
extern std::vector<class SkillBuildupMPInfo*> skillBuildupMPInfo;
extern std::vector<class SkillBuildUpInfo*> skillBuildUpInfo;
extern std::vector<class SkillPropertyInfo*> skillPropertyInfo;

extern std::vector<class LastWeeklyRank*> lastWeeklyRank;
extern std::vector<class TipContent*> tipContent;
extern std::vector<class ProfileTitle*> profileTitle;
extern std::vector<class NoticeList*> noticeList;

// 친구초대 리스트
extern std::vector<class InviteList*> inviteList;
extern int todayCnt, monthCnt, totalCnt;
extern bool isInviteListGathered;

extern std::vector<int> inGameSkill;
extern std::vector<class Depth*> depth;
extern std::vector<std::string> todayCandyKakaoId;

// 인게임 관련
extern bool isRebooting;
extern bool isInGame;
extern bool isInGamePause;
extern bool isInGameTutorial;
extern bool isStartGameEnd;
extern int savedTime;
extern int savedTime2;

// item type&cost
extern int itemType[5];
extern int itemCost[5];

// 환경설정 메뉴
extern int menuInSetting;

// 주간랭킹 보상 관련 변수
extern int myRank;
extern int myLastWeekHighScore;
extern int rewardType;
extern int certificateType;

// 게임에 필요한 미션 내용
extern int missionType;
extern int missionVal;
extern int missionRefVal;

// 게임결과에 필요한 값들
extern class MyGameResult* myGameResult;

// 바이너리 버전
extern int binaryVersion_current;

// 초보유저 보상 관련
extern bool isStartUser;

extern bool isPossibleBuyFairyShown;


// rsa 관련
extern RSA* rsa;
extern std::string publicKey;
extern int publicKeyIndex;
extern std::string obfuscationKey[30];
extern std::string basicKey;

// 결제용 key
extern std::string gcmKey;

//
extern cocos2d::CCNode* verifyStatusScene;

extern bool isTutorial;
extern int tutorialState;


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
    ProfileSprite(std::string profileUrl, bool preload);
    static ProfileSprite* GetObj(std::string profileUrl);
    static CCSprite* GetProfile(std::string profileUrl);
    bool IsPreload();
    bool IsLoadingDone();
    bool IsLoadingStarted();
    bool IsLoadingDoneForRanking();
    void SetLoadingDoneForRanking(bool flag);
    void SetLoadingDone(bool flag);
    void SetLoadingStarted(bool flag);
    CCSprite* GetProfile();
    std::string GetProfileUrl();
    void SetSpriteNoImage();
    void SetSprite(CCTexture2D* texture);
private:
    CCSprite* profile;
    std::string profileUrl;
    bool preload;
    bool isLoadingStarted;
    bool isLoadingDone;
    bool isLoadingDoneForRanking;
};

class MyGameResult
{
public:
    MyGameResult(int topaz, int starcandy, int potion, int mp, float addedPercent, int score, int totalscore, int combo, int bestcombo, int mission, int newrecord, std::string text);
    int getTopaz;
    int getStarCandy;
    int getPotion;
    int getMP;
    float addedMPPercent;
    int score;
    int totalScore;
    int combo;
    int bestCombo;
    bool isMissionSuccess;
    bool isNewRecord;
    std::string content;
    std::vector<int> skillNum;
    std::vector<int> skillCnt;
};

class MyInfo
{
public:
    void Init(std::string kakaoId, int deviceType, int userId, bool kakaoMsg, bool pushNoti, bool potionMsg, int msgCnt, std::string sessionId);
    void InitRestInfo(int topaz, int starcandy, int mp, int mpStaff, int mpFairy, int staffLv, int staffFailPoint, int highScore, int weeklyHighScore, int lastWeeklyHighScore, int isWeeklyRankReward, int certificateType, int remainWeeklyRankTime, int item1, int item2, int item3, int item4, int item5, int potion ,int remainPotionTime, int fire, int water, int land, int master, int fireByTopaz, int waterByTopaz, int landByTopaz);
    
    std::string GetSessionId();
    int GetKeyValue();
    int GetUserId();
    
    std::string GetHashedTalkUserId();
    std::string GetCountryIso();
    bool IsMessageBlocked();
    bool isVerified();
    
    //void SetSessionId(std::string sessionId);
    static CCSprite* GetProfile();
    static std::string GetName();
    int GetDeviceType();
    std::string GetKakaoId();
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
    int GetStaffFailPoint();

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
    bool IsFireByTopaz();
    bool IsWaterByTopaz();
    bool IsLandByTopaz();
    bool HasNoProperty();
    int GetNumOfProperties();
    bool IsTimeToFreelyBuyProperty();
    
    void SetSettingVariables(bool kakaoMsgReserved, bool pushNotiReserved, bool potionMsgReserved);
    void SetMoney(int topaz, int starcandy);
    void SetPotion(int potion, int remainPotionTime);
    void SetCoco(int mp, int mpStaff, int mpFairy, int staffLv, int staffFailPoint);
    void SetItem(std::vector<int> items);
    void ChangeItem(int idx, int value);
    void SetProperties(int fire, int water, int land, int master, int fireByTopaz, int waterByTopaz, int landByTopaz);
    void SetScore(int highScore, int weeklyHighScore, int certificateType, int remainWeeklyRankTime);
    void SetRemainWeeklyRankTime(int time);
    
    void SetProfileSkill(int id, int level);
    int GetProfileSkillId();
    int GetProfileSkillLv();
    void SetPracticeSkill(int id, int level);
    int GetPracticeSkillId();
    int GetPracticeSkillLv();
    
    void SetTodayCandy(int todayCandyTypeChoice, int todayCandyValueChoice, int todayCandyTypeMiss, int todayCandyValueMiss, int istodayCandyUsed);
    void SetTodayCandy(int isTodayCandyUsed);
    int GetTodayCandyTypeChoice();
    int GetTodayCandyValueChoice();
    int GetTodayCandyTypeMiss();
    int GetTodayCandyValueMiss();
    bool IsTodayCandyUsed();
    
    void AddSkillSlot(int id, int csi, int usi);
    void AddFairy(int cfi, int ufi, int level, int isUse, int failPoint);
    void AddSkill(int csi, int usi, int level, int exp, int learntime);
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
    
    void SetReward(int potion, int topaz);
    bool IsRewardPotion();
    int GetRewardTopaz();

private:
    int keyValue;
    std::string mySessionId;
    
    bool settingKakaoMsg;
    bool settingPushNoti;
    bool settingPotionMsg;
    
    std::string kakaoId;
    std::string hashedTalkUserId;
    std::string countryIso;
    bool messageBlocked;
    bool verified;
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
    int staffFailPoint;
    
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
    bool isPropertyFireByTopaz;
    bool isPropertyWaterByTopaz;
    bool isPropertyLandByTopaz;
    
    int profileSkillId;
    int profileSkillLv;
    int practiceSkillId;
    int practiceSkillLv;
    
    int todayCandyTypeChoice;
    int todayCandyValueChoice;
    int todayCandyTypeMiss;
    int todayCandyValueMiss;
    bool istodayCandyUsed;
    
    std::vector<class MySkillSlot*> mySkillSlot;
    std::vector<class MyFairy*> myFairy;
    std::vector<class MySkill*> mySkill;
    
    int isPotionMax;
    int addedTopaz;
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
    MyFairy(int cfi, int ufi, int level, int isUse, int failPoint);
    static MyFairy* GetObj(int cfi);
    bool IsUse();
    int GetId();
    int GetUserId();
    int GetLevel();
    int GetFailPoint();
private:
    int common_fairy_id;
    int user_fairy_id;
    int level;
    int isUse;
    int failPoint;
};
class MySkill
{
public:
    MySkill(int csi, int usi, int level, int exp, int learntime);
    static MySkill* GetObj(int scid);
    int GetCommonId();
    int GetUserId();
    int GetLevel();
    int GetExp();
    int GetLearnTime();
private:
    int common_skill_id;
    int user_skill_id;
    int level;
    int exp;
    int learnTime;
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
    Friend(std::string kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int remainRequestPotionTime, int remainRequestTopazTime, int weeklyHighScore, int highScore, int scoreUpdateTime, int certificateType, int profileTitleId, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillid, int skillLevel);
    
    void SetProfile(CCSprite* sp);
    void SetProfileUrl(std::string url);
    void SetPotionSprite();
    void SetScore(int highScore, int weeklyHighScore, int certificateType);
    
    static Friend* GetObj(std::string kakaoId);
    static int GetRemainPotionTime(std::string kakaoId);
    
    std::string GetKakaoId();
    CCSprite* GetProfile();
    std::string GetImageUrl();
    std::string GetNickname();
    CCLabelTTF* GetNicknameLabel();
    int GetPotionMsgStatus();
    void SetPotionMsgStatus(int status);
    CCSprite* GetPotionSprite();
    CCLabelTTF* GetPotionLabelMin();
    CCLabelTTF* GetPotionLabelSec();
    
    int GetCertificateType();
    int GetProfileTitleId();
    void SetProfileTitleId(int id);

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
    
    void SetKakaoVariables(std::string name, std::string purl, std::string htuid, bool msgblocked, bool supporteddevice);
    std::string GetHashedTalkUserId();
    bool IsMessageBlocked();
    bool IsSupportedDevice();
    
private:
    std::string kakaoId;
    std::string nickname;
    std::string imageUrl;
    std::string hashedTalkUserId;
    bool messageBlocked;
    bool supportedDevice;
    
    CCLabelTTF* nicknameLabel;
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
    int profileTitleId;
    
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
    Msg(int id, int type, int rewardCount, std::string content, std::string profileUrl, std::string noticeUrl, std::string friendKakaoId);
    int GetId();
    int GetType();
    int GetRewardCount();
    std::string GetContent();
    std::string GetProfileUrl();
    std::string GetNoticeUrl();
    std::string GetFriendKakaoId();
    CCSprite* GetProfile();
    void SetProfile(CCSprite* sp);

private:
    int id;
    int type;
    int rewardCount;
    std::string content;
    std::string profileUrl;
    std::string noticeUrl;
    std::string friendKakaoId;
    CCSprite* profile;
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
    MagicStaffBuildUpInfo(int level, int bonusMPPercent, int bonusMPPlus, int cs, int ct);
    int GetCost_StarCandy();
    int GetCost_Topaz();
    int GetLevel();
    int GetBonusMPPercent();
    int GetBonusMPPlus();
private:
    int nLevel;
    int nBonusMP_percent;
    int nBonusMP_plus;
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

/*
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
*/


class FairyInfo
{
public:
    FairyInfo(int id, int type, int grade, int cs, int ct, int pid);
    std::string MakeName(int id);
    static std::string GetAbilityName(FairyInfo* f, int level);
    static std::string GetAbilityDesc(int type, bool newline);
    std::string GetDescription();
    int GetId();
    int GetGrade();
    int GetType();
    int GetCostTopaz();
    int GetCostStarCandy();
    int GetRefVal();
    std::string GetName();
    static FairyInfo* GetObj(int id);
private:
    int nId;
    int nType;
    std::string sName;
    int nGrade;
    int nCost_starcandy;
    int nCost_topaz;
    int nRefVal;
};

class FairyBuildUpInfo
{
public:
    FairyBuildUpInfo(int id, int level, int ability, int refId, int cs, int ct);
    static int GetAbility(int id, int level);
    static int GetMaxLevel(int id);
    static int GetCostTopaz(int id, int level);
    static int GetCostStarCandy(int id, int level);
    static int GetTotalMP(int id, int level);
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
    static int Converted(int id);
    static int ConvertedToOriginal(int skillId);
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

class SkillBuildupMPInfo
{
public:
    SkillBuildupMPInfo(int skillCount, int requireMP, int discount1, int discount2, int topaz);
    static SkillBuildupMPInfo* GetObj(int skillCount);
    static int GetOrderForTopaz(std::vector<MySkill*> sList, int scid);
    static int GetRealOrder(std::vector<MySkill*> sList, int scid);
    static int GetOrder(std::vector<MySkill*> sList, int scid);
    static int RequiredMP(std::vector<MySkill*> sList, int scid);
    int GetSkillCount();
    int GetRequireMP();
    int GetDiscount1();
    int GetDiscount2();
    int GetTopazCostValue();
private:
    int nSkillCount;
    int nRequireMP;
    int nDiscountOne;
    int nDiscountTwo;
    int nTopazCostValue;
};

class SkillBuildUpInfo
{
public:
    SkillBuildUpInfo(int id, std::string name, int skillLv, int maxExp, int ability1, int ability2, int prob, int cs);
    static bool IsMastered(int sid, int level);
    static int GetMaxExp(int sid, int level);
    static int GetCost(int sid, int level);
    static SkillBuildUpInfo* GetObj(int sid, int level);
    int GetId();
    int GetAbility1();
    int GetAbility2();
    int GetProb();
    int GetLevel();
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

class TipContent
{
public:
    TipContent(int id, int type, std::string script);
    int GetId();
    int GetCategory();
    std::string GetContent();
private:
    int nId;
    int nCategory;
    std::string content;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class ProfileTitle
{
public:
    ProfileTitle(int id, int ct, int pt, std::string t);
    int GetId();
    int GetCertificateType();
    int GetPropertyType();
    std::string GetTitle();
private:
    int nId;
    int nCertificateType;
    int nPropertyType;
    std::string title;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class InviteList
{
public:
    InviteList(std::string userid, std::string name, std::string purl, std::string htuid, bool msgblocked, bool supporteddevice, bool wi);
    std::string userId;
    std::string nickname;
    std::string profileUrl;
    std::string hashedTalkUserId;
    bool messageBlocked;
    bool supportedDevice;
    bool wasInvited;
    CCSprite* profile;
};

/////////////////////////////////////////////////////////////////////////////////////////////

class NoticeList
{
public:
    NoticeList(int i, int pf, std::string t, std::string m, std::string l, int onetime);
    int id;
    int platform;
    std::string title;
    std::string message;
    std::string link;
    int oneTime;
    bool isShown;
};


#endif /* defined(__CocoMagic__Data__) */








