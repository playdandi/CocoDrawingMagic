#ifndef __CocoMagic__Data__
#define __CocoMagic__Data__

#include "Common.h"

#define POTION_X 0
#define POTION_SEND 1
#define POTION_REMAIN 2
#define POTION_NOTHING 3

extern class MyInfo* myInfo;
extern std::vector<class Friend*> friendList;
extern std::vector<class PriceTopaz*> priceTopaz;
extern std::vector<class PriceStarCandy*> priceStarCandy;
extern std::vector<class Msg*> msgData;

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
    int GetMPStaffPercentNext();
    int GetMPNextCostStarcandy();
    int GetMPNextCostTopaz();
    
    int GetStaffLv();
    int GetStaffLvNext();
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
    void SetNextStaff(int staffLvNext, int mpNextCostStarcandy, int mpNextCostTopaz, int staffNextPercent);
    
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
    
    int mpStaffPercentNext;
    int staffLvNext;
    int mpNextCostStarcandy;
    int mpNextCostTopaz;
    
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
    int GetWeeklyHighScore();
    int GetHighScore();
    int GetScoreUpdateTime();
    int GetRemainPotionTime();
    void SetRemainPotionTime(int time);
    bool IsFire();
    bool IsWater();
    bool IsLand();
    bool IsMaster();
    
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
    int skilllevel;
};

////////////////////////////////////////////////////////////////////////////////
class DataProcess
{
public:
    static void SortFriendListByScore();
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
    PriceTopaz(int id, int count, int price, int bonus);
    int GetId();
    int GetCount();
    int GetPrice();
    int GetBonus();
private:
    int id;
    int count;
    int price;
    int bonus;
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
    int id;
    int count;
    int price;
    int bonus;
};


#endif /* defined(__CocoMagic__Data__) */