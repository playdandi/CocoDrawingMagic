#ifndef __CocoMagic__Data__
#define __CocoMagic__Data__

#include "Common.h"

#define POTION_X 0
#define POTION_SEND 1
#define POTION_REMAIN 2
#define POTION_NOTHING 3

extern class MyInfo* myInfo;
extern std::vector<class Friend*> friendList;

using namespace cocos2d;

class MyInfo
{
public:
    MyInfo(int kakaoId, int deviceType);
    
    int GetKakaoId();
    
private:
    int kakaoId;
    int deviceType;
};

////////////////////////////////////////////////////////////////////////////////
class Friend
{
public:
    Friend(int kakaoId, std::string nickname, std::string imageUrl, int potionMsgStatus, int remainPotionTime, int weeklyHighScore, int highScore, int certificateType, int fire, int water, int land, int master, int fairyId, int fairyLevel, int skillid, int skillLevel);
    
    void SetSprite(CCTexture2D* texture);
    
    int GetKakaoId();
    CCSprite* GetProfile();
    std::string GetNickname();
    CCLabelTTF* GetNicknameLabel();
    int GetPotionMsgStatus();
    int GetWeeklyHighScore();
    int GetHighScore();
    int GetRemainPotionTime();
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
    int weeklyHighScore;
    int highScore;
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

#endif /* defined(__CocoMagic__Data__) */