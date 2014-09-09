#ifndef PuzzleGame_Common
#define PuzzleGame_Common

#include <algorithm>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCPlatformConfig.h"
#include "Network.h"
#include "Sound.h"
#include "Data.h"
#include "Fairy.h"
#include "Loading.h"
#include "Loading_GameStart.h"
#include "pugixml/pugixml.hpp"
#include "platform/android/jni/JniHelper.h"

USING_NS_CC;

#define ROW_COUNT 7
#define COLUMN_COUNT 7

#define TYPE_COUNT 5
#define PIECE_RED 0
#define PIECE_BLUE 1
#define PIECE_GREEN 2
#define PIECE_YELLOW 3
#define PIECE_WHITE 4

// 피스의 아이템
#define ITEM_PAINT_RED 10
#define ITEM_PAINT_BLUE 11
#define ITEM_PAINT_GREEN 12
#define ITEM_PAINT_YELLOW 13
#define ITEM_PAINT_WHITE 14
#define ITEM_STAFF 20
#define ITEM_PAINT_RED_SPECIAL 103
#define ITEM_PAINT_BLUE_SPECIAL 113
#define ITEM_PAINT_GREEN_SPECIAL 123

#define INGAME_ITEM_3PAINT 1

// 인게임 기본 구성 변수
#define PUZZLE_TIME 60
#define FEVER_TIME 9
#define FEVER_START_COMBO 12
#define MAX_COMBO_TIME 2000

#define MAX_NUM_OF_INVITE_FRIEND 30

#define QUEUE_CNT 5

#define BUFFER_SIZE 80000
#define IMAGE_BUFFER_SIZE 120*120*8

//#define NUMOFSKILL 46 // 11~17, 21~27, 31~37, (41~45) 까지 현재 사용중 (skill-common-id)
#define NUMOFSKILL 32

#define COUPON_LENGTH 16

#define MAX_COMMON_ERROR_CODE 9
// 팝업창에서 버튼 개수를 구분하기 위한 predefined type
#define BTN_1 0
#define BTN_2 1

// 어떤 팝업창인지 구분하기 위한 predefined type
#define YOU_WERE_BLOCKED -100
#define FUCKING_APP_DETECTED -99
#define ROOTING_DETECTED -98
#define SERVER_CHECK -8
#define WILL_BE_UPDATED -7
#define ERROR_IN_APP_BILLING -6
#define NEED_TO_UPDATE -5
#define NEED_TO_REBOOT -4
#define NEED_TO_BUY_POTION -3
#define NEED_TO_BUY_TOPAZ -2
#define NEED_TO_BUY_STARCANDY -1
#define BUY_STARCANDY_TRY 0
#define BUY_STARCANDY_OK 1
#define BUY_STARCANDY_FAIL 2
#define POPUP_EXIT 3
#define BUYPOTION_1 4
#define BUYPOTION_OK 5
#define BUYPOTION_FAIL 6
#define POTION_SEND_TRY 7
#define POTION_SEND_OK 8
#define POTION_SEND_REJECT 9
#define POTION_SEND_NO_FRIEND 10
#define POTION_SEND_EARLY 11
#define MESSAGE_NOTICE 12
#define MESSAGE_OK_STARCANDY 13
#define MESSAGE_OK_TOPAZ 14
#define MESSAGE_OK_POTION 15
#define MESSAGE_EMPTY 16
#define MESSAGE_ALL_TRY 17
#define MESSAGE_ALL_OK 18
#define SEND_TOPAZ_TRY 19
#define SEND_TOPAZ_OK 20
#define SEND_TOPAZ_FAIL 21
#define BUY_TOPAZ_TRY 22
#define BUY_TOPAZ_OK 23
#define NETWORK_FAIL 24
#define UPGRADE_STAFF_BY_TOPAZ_TRY 25
#define UPGRADE_STAFF_BY_STARCANDY_TRY 26
#define UPGRADE_STAFF_OK 27
#define UPGRADE_STAFF_FAIL 28
#define UPGRADE_STAFF_INSUFFICIENT_MP 29
#define UPGRADE_STAFF_FULL_LEVEL 31
#define BUY_FAIRY_BY_TOPAZ_TRY 32
#define BUY_FAIRY_BY_STARCANDY_TRY 33
#define BUY_FAIRY_OK 34
#define BUY_FAIRY_FAIL 35
#define BUY_FAIRY_BY_TOPAZ_NOMONEY 36
#define BUY_FAIRY_BY_STARCANDY_NOMONEY 37
#define UPGRADE_FAIRY_BY_TOPAZ_TRY 38
#define UPGRADE_FAIRY_BY_STARCANDY_TRY 39
#define UPGRADE_FAIRY_OK 40
#define UPGRADE_FAIRY_FAIL 41
#define UPGRADE_FAIRY_FULL_LEVEL 42
#define UPGRADE_SKILL_OK 44
#define UPGRADE_SKILL_FAIL 45
#define PURCHASE_SKILL_OK 46
#define PURCHASE_SKILL_FAIL 47
#define BUY_PROPERTY_TRY 48
#define BUY_PROPERTY_OK 49
#define BUY_PROPERTY_FAIL 50
#define BUY_SKILLSLOT_BY_STARCANDY_TRY 51
#define BUY_SKILLSLOT_BY_TOPAZ_TRY 52
#define BUY_SKILLSLOT_OK 53
#define BUY_SKILLSLOT_FAIL 54
#define BUY_SKILLSLOT_FULL 55
#define INVITE_FRIEND_OK 56
#define INVITE_FRIEND_MONTH_OVER_30 57
#define INVITE_FRIEND_DAY_OVER_20 58
#define INVITE_FRIEND_ALREADY_DID 59
#define INVITE_FRIEND_10 60
#define INVITE_FRIEND_20 61
#define INVITE_FRIEND_30 62
#define REQUEST_POTION_OK 63
#define REQUEST_POTION_REJECT 64
#define REQUEST_POTION_EARLY 65
#define MESSAGE_OK_POTION_REQUEST 66
#define REQUEST_TOPAZ_OK 67
#define REQUEST_TOPAZ_NO_FRIEND 68
#define REQUEST_TOPAZ_EARLY 69
#define TODAYCANDY_RESULT_WIN 70
#define TODAYCANDY_RESULT_LOSE 71
#define TODAYCANDY_ALREADY_DONE 72
#define TODAYCANDY_NOTENOUGH_FRIENDS 73
#define PRACTICE_SKILL_FULL_EXP 74
#define USING_FAIRY 75
#define SELECT_PROPERTY_TRY 76
#define SELECT_PROPERTY_OK 77
#define GET_DEGREE 78
#define COUPON_OK 79
#define COUPON_ALREADY_USED 80
#define COUPON_EXPIRED 81
#define COUPON_NOT_EXIST 82
#define BUY_PROPERTY_FREE_MSG 83
#define KAKAO_LOGOUT 84
#define KAKAO_UNREGISTER 85
#define SERVICE 86
#define KAKAO_TOKEN_ERROR 87
#define INVITE_FRIEND_TRY 88
#define INVITE_FRIEND_MAX_PER_DAY 89
#define INVITE_FRIEND_BLOCKED 90
#define INVITE_FRIEND_NOT_SUPPORTED_DEVICE 91
#define INVITE_FRIEND_UNREGISTERED 92
#define INVITE_FRIEND_NO_MORE 93
#define SEND_TOPAZ_OK_NOKAKAOMSG 94
#define MP_REWARD_50 95
#define MP_REWARD_100 96
#define RANKUP_BOAST 97
#define RANKUP_BOAST_REJECTED 98
#define KAKAOTALK_UNKNOWN 99
#define KAKAO_MSG_BLOCKED 100
#define CREDIT 101
#define TUTORIAL_START 102
#define COUPON_SAME_TYPE 103
#define POTION_REWARD 104
#define POSSIBLE_BUY_FAIRY 105
#define POPUP_NOTICE 106
#define PURCHASE_SKILL_BY_TOPAZ_TRY 107
#define HINT_BUY_PROPERTY 108
#define GUEST_LOGIN 109

#define LOADING_PUZZLEEND -2
#define LOADING_MESSAGE 0

// 터치 관련
#define BTN_MENU_GAMEREADY 0
#define BTN_MENU_GAMESTART 1
#define BTN_MENU_TUTORIAL 2
#define BTN_MENU_COUPON 3
#define BTN_MENU_CONFIRM 4
#define BTN_MENU_UPGRADE_1 5
#define BTN_MENU_UPGRADE_2 6
#define BTN_MENU_LOGOUT 7
#define BTN_MENU_UNREGISTER 8
#define BTN_MENU_SERVICE 9
#define BTN_MENU_BOAST 10
#define BTN_MENU_CREDIT 11
#define BTN_MENU_KAKAOBTN 12
#define BTN_MENU_RESUMEGAME 13
#define BTN_MENU_ENDGAME 14
#define BTN_MENU_CAFE 15
#define BTN_MENU_GUESTBTN 16

// 카카오톡 메시지 관련
#define KAKAO_MSG_TEMPLATE_SENDPOTION   "1766"
#define KAKAO_MSG_TEMPLATE_SENDTOPAZ    "1765"
#define KAKAO_MSG_TEMPLATE_INVITEFRIEND "1767"
#define KAKAO_MSG_TEMPLATE_BOAST        "1763"
#define KAKAO_MSG_TEMPLATE_TODAYCANDY   "1764"

// 아이템 타입 관련
#define ITEM_STARCANDY 1
#define ITEM_TOPAZ 2

// 공식카페 주소
#define URL_PUBLIC_CAFE "http://cafe.naver.com/cocomagic2014/"


using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class Sound;
class Ranking;
class BuyTopaz;

extern Sound* sound;
extern std::string fontList[];
extern std::string resFileList[];

class Common
{
public:
    //static CCRenderTexture* CreateStroke( CCSprite* label, int size, ccColor3B color, GLubyte opacity );
    static void MakeBadgeToZeroFunc();
    
    static void AddSpriteFramesWithFile(std::string plist, std::string png);
    static std::string GetResFilename(std::string filename);
    static CCTexture2D* GetResFileImg(std::string filename);
    
    static std::string GetTip();
    
    static std::string MakeComma(int number);
    static std::string InsertComma(std::string number);
    static CCLayer* MakeCombo(int num);
    static CCLayer* MakeScoreLayer(int num);
    static CCLayer* MakeImageNumberLayer(std::string number, int type);
    static CCLayer* MakeItemNumberLayer(std::string number, float scale = 1.0f);
    static void ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc = -1, int etc2 = -1, int etc3 = -1);
    static void ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data, int etc = -1, int priority = -1);
    static void RebootSystem(void* p);

    static std::string SubstrNickname(std::string nickname);
    
    static std::string GetMissionContent(int type, int val, int refVal);
    static void UpdateProfileTitle();
    static std::string GetProfileTitle(int idx);
    static std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
    static std::string base64_decode(std::string const& encoded_string);
    
    static std::string GetVerifyParams(int type, int topazId, const char* kakaoId, const char* friendkakaoId, const char* purchaseData, const char* dataSignature, int consumeIdx);
    static void XmlParseVerifyPurchaseResult(const char* data, int size, int consumeIdx, const char* fkid);

    static void BackToCocos2dxFromApple();
    static std::string GetVerifyParamsApple(int type, int topazId, std::string friendKakaoId, const char* receipt);
    static void XmlParseVerifyPurchaseResultApple(const char* data, int size);
};


class SpriteObject
{
public:
    static SpriteObject* Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, int tag = -1);
    
    static SpriteObject* CreateFromSprite(int spriteType, CCSprite* spr, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, float scale = 1.0f, int tag = -1);
    
    static SpriteObject* CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, int tag = -1);
    
    static SpriteObject* CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, int tag = -1);

    int type;
    CCSprite* sprite;
    CCScale9Sprite* sprite9;
    CCLabelTTF* label;
    std::string name;
    
    std::string parentName;
    std::string parentType; // 0 : 이 안의 sprite, 1 : 이 안의 sprite9
    void* parent;
    int zOrder;
    int priority;
};

class SpriteClass
{
public:
    void SetOpacity(int idx, int alpha);
    void AddChild(int idx);
    int FindParentPriority(int idx, std::string parentName);
    void* FindParentSprite(int idx, std::string parentName);
    void* FindParentSprite9(int idx, std::string parentName);
    void* FindSpriteByName(std::string name);
    CCPoint FindParentCenterPos(std::string parentName);
    CCSize GetContentSizeByName(std::string name);
    void* FindLabelByTag(int tag);
    void* FindSpriteByTag(int tag);
    //void ChangeSprite(int tag, CCTexture2D* texture);
    void ChangeSprite(int tag, CCSprite* sp);

    void RemoveAllObjects();
    
    std::vector<SpriteObject*> spriteObj;
    std::vector<CCLayer*> layers;
};



#endif

