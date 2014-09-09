#ifndef __CocoMagic__Network__
#define __CocoMagic__Network__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Common.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d::extension;

#define RSA_BIT 512
#define TEST_IP "14.63.212.106"
#define REAL_IP "14.63.200.217"

#define TEST 1

#if TEST
// pre-defined URLs
#define URL_MT                      "http://14.63.212.106/cogma/game/mt.php?"
#define URL_JOIN_GUEST              "http://14.63.212.106/cogma/game/join_guest.php?"
#define URL_JOIN                    "http://14.63.212.106/cogma/game/join.php?"
#define URL_VERSION                 "http://14.63.212.106/cogma/game/get_version.php"
#define URL_LOGIN                   "http://14.63.212.106/cogma/game/login.php"
#define URL_NOTICE                  "http://14.63.212.106/cogma/game/notice.php"
#define URL_USERINFO                "http://14.63.212.106/cogma/game/user_info.php?"
#define URL_WEEKLYRANK              "http://14.63.212.106/cogma/game/reward_weekly_rank.php?"
#define URL_FRIENDLIST              "http://14.63.212.106/cogma/game/get_friendslist.php?"
#define URL_GAMESTART               "http://14.63.212.106/cogma/game/game_start.php?"
#define URL_PRACTICE_SKILL          "http://14.63.212.106/cogma/game/practice_skill.php?"
#define URL_UPGRADE_SKILL           "http://14.63.212.106/cogma/game/upgrade_skill.php?"
#define URL_USING_SKILL             "http://14.63.212.106/cogma/game/using_skill.php?"
#define URL_PURCHASE_SKILL          "http://14.63.212.106/cogma/game/purchase_skill.php?"
#define URL_TODAY_STARCANDY         "http://14.63.212.106/cogma/game/today_starcandy.php?"
#define URL_USING_FAIRY             "http://14.63.212.106/cogma/game/using_fairy.php?"
#define URL_PURCHASE_FAIRY_LIST     "http://14.63.212.106/cogma/game/get_purchase_fairy_list.php?"
#define URL_MESSAGE_LIST            "http://14.63.212.106/cogma/game/get_messagelist.php?"
#define URL_MESSAGE_ONE             "http://14.63.212.106/cogma/game/receive_message_one.php?"
#define URL_MESSAGE_ALL             "http://14.63.212.106/cogma/game/receive_message_all_potion.php?"
#define URL_INVITE_FRIEND_LIST      "http://14.63.212.106/cogma/game/friend_invite_list.php?"
#define URL_INVITE_FRIEND           "http://14.63.212.106/cogma/game/friend_invite.php?"
#define URL_SETTING                 "http://14.63.212.106/cogma/game/setting.php?"
#define URL_REQUEST_POTION          "http://14.63.212.106/cogma/game/request_potion.php?"
#define URL_REQUEST_TOPAZ           "http://14.63.212.106/cogma/game/request_topaz.php?"
#define URL_GAMEEND                 "http://14.63.212.106/cogma/game/game_end.php"
#define URL_PURCHASE_STARCANDY      "http://14.63.212.106/cogma/game/purchase_starcandy.php?"
#define URL_PURCHASE_POTION         "http://14.63.212.106/cogma/game/purchase_potion.php?"
#define URL_SEND_POTION             "http://14.63.212.106/cogma/game/send_potion.php?"
#define URL_UPGRADE_STAFF           "http://14.63.212.106/cogma/game/upgrade_staff.php?"
#define URL_UPGRADE_FAIRY           "http://14.63.212.106/cogma/game/upgrade_fairy.php?"
#define URL_PURCHASE_FAIRY          "http://14.63.212.106/cogma/game/purchase_fairy.php?"
#define URL_UPGRADE_SKILLSLOT       "http://14.63.212.106/cogma/game/upgrade_skill_slot.php?"
#define URL_PURCHASE_SKILL_PROPERTY "http://14.63.212.106/cogma/game/purchase_skill_type.php?"
#define URL_SESSION_CHECK           "http://14.63.212.106/cogma/game/update_session.php?"
#define URL_COUPON                  "http://14.63.212.106/cogma/game/receive_coupon_reward.php?"
#define URL_NONCONSUMED_GETFRIENDID "http://14.63.212.106/cogma/game/send_topaz_google_get_friend.php?"
#define URL_FRIENDADD               "http://14.63.212.106/cogma/game/friend_add.php?"
#define URL_FRIENDDEL               "http://14.63.212.106/cogma/game/friend_del.php?"
#define URL_QUIT                    "http://14.63.212.106/cogma/game/quit_user.php?"
#define URL_GET_POTION_STATUS       "http://14.63.212.106/cogma/game/get_user_potion_setting.php?"
#define URL_UPDATE_NAME_TITLE       "http://14.63.212.106/cogma/game/update_name_title.php?"
#define URL_GOOGLE_PAYLOAD          "http://14.63.212.106/cogma/game/get_payload_google.php"
#define URL_APPLE_UPDATE            "http://14.63.212.106/cogma/game/update_friend_apple.php?"
#define URL_PURCHASE_TOPAZ_GOOGLE   "http://14.63.212.106/cogma/game/purchase_topaz_google.php?"
#define URL_SEND_TOPAZ_GOOGLE       "http://14.63.212.106/cogma/game/send_topaz_google.php?"
#define URL_PURCHASE_TOPAZ_APPLE    "http://14.63.212.106/cogma/game/purchase_topaz_apple.php?"
#define URL_SEND_TOPAZ_APPLE        "http://14.63.212.106/cogma/game/send_topaz_apple.php?"
#define URL_PURCHASE_INGAME_ITEM    "http://14.63.212.106/cogma/game/purchase_ingame_item.php?"
#define URL_TERM_SERVICE            "http://14.63.212.106/cogma/game/update/rule_service.html"
#define URL_TERM_PRIVATE            "http://14.63.212.106/cogma/game/update/rule_private.html"

#else

#define URL_MT                      "http://14.63.200.217/cogma/game/mt.php?"
#define URL_JOIN_GUEST              "http://14.63.200.217/cogma/game/join_guest.php?"
#define URL_JOIN                    "http://14.63.200.217/cogma/game/join.php?"
#define URL_VERSION                 "http://14.63.200.217/cogma/game/get_version.php"
#define URL_LOGIN                   "http://14.63.200.217/cogma/game/login.php"
#define URL_NOTICE                  "http://14.63.200.217/cogma/game/notice.php"
#define URL_USERINFO                "http://14.63.200.217/cogma/game/user_info.php?"
#define URL_WEEKLYRANK              "http://14.63.200.217/cogma/game/reward_weekly_rank.php?"
#define URL_FRIENDLIST              "http://14.63.200.217/cogma/game/get_friendslist.php?"
#define URL_GAMESTART               "http://14.63.200.217/cogma/game/game_start.php?"
#define URL_PRACTICE_SKILL          "http://14.63.200.217/cogma/game/practice_skill.php?"
#define URL_UPGRADE_SKILL           "http://14.63.200.217/cogma/game/upgrade_skill.php?"
#define URL_USING_SKILL             "http://14.63.200.217/cogma/game/using_skill.php?"
#define URL_PURCHASE_SKILL          "http://14.63.200.217/cogma/game/purchase_skill.php?"
#define URL_TODAY_STARCANDY         "http://14.63.200.217/cogma/game/today_starcandy.php?"
#define URL_USING_FAIRY             "http://14.63.200.217/cogma/game/using_fairy.php?"
#define URL_PURCHASE_FAIRY_LIST     "http://14.63.200.217/cogma/game/get_purchase_fairy_list.php?"
#define URL_MESSAGE_LIST            "http://14.63.200.217/cogma/game/get_messagelist.php?"
#define URL_MESSAGE_ONE             "http://14.63.200.217/cogma/game/receive_message_one.php?"
#define URL_MESSAGE_ALL             "http://14.63.200.217/cogma/game/receive_message_all_potion.php?"
#define URL_INVITE_FRIEND_LIST      "http://14.63.200.217/cogma/game/friend_invite_list.php?"
#define URL_INVITE_FRIEND           "http://14.63.200.217/cogma/game/friend_invite.php?"
#define URL_SETTING                 "http://14.63.200.217/cogma/game/setting.php?"
#define URL_REQUEST_POTION          "http://14.63.200.217/cogma/game/request_potion.php?"
#define URL_REQUEST_TOPAZ           "http://14.63.200.217/cogma/game/request_topaz.php?"
#define URL_GAMEEND                 "http://14.63.200.217/cogma/game/game_end.php"
#define URL_PURCHASE_STARCANDY      "http://14.63.200.217/cogma/game/purchase_starcandy.php?"
#define URL_PURCHASE_POTION         "http://14.63.200.217/cogma/game/purchase_potion.php?"
#define URL_SEND_POTION             "http://14.63.200.217/cogma/game/send_potion.php?"
#define URL_UPGRADE_STAFF           "http://14.63.200.217/cogma/game/upgrade_staff.php?"
#define URL_UPGRADE_FAIRY           "http://14.63.200.217/cogma/game/upgrade_fairy.php?"
#define URL_PURCHASE_FAIRY          "http://14.63.200.217/cogma/game/purchase_fairy.php?"
#define URL_UPGRADE_SKILLSLOT       "http://14.63.200.217/cogma/game/upgrade_skill_slot.php?"
#define URL_PURCHASE_SKILL_PROPERTY "http://14.63.200.217/cogma/game/purchase_skill_type.php?"
#define URL_SESSION_CHECK           "http://14.63.200.217/cogma/game/update_session.php?"
#define URL_COUPON                  "http://14.63.200.217/cogma/game/receive_coupon_reward.php?"
#define URL_NONCONSUMED_GETFRIENDID "http://14.63.200.217/cogma/game/send_topaz_google_get_friend.php?"
#define URL_FRIENDADD               "http://14.63.200.217/cogma/game/friend_add.php?"
#define URL_FRIENDDEL               "http://14.63.200.217/cogma/game/friend_del.php?"
#define URL_QUIT                    "http://14.63.200.217/cogma/game/quit_user.php?"
#define URL_GET_POTION_STATUS       "http://14.63.200.217/cogma/game/get_user_potion_setting.php?"
#define URL_UPDATE_NAME_TITLE       "http://14.63.200.217/cogma/game/update_name_title.php?"
//#define URL_BUY_TOPAZ_TEST          "http://14.63.200.217/cogma/game/purchase_topaz.php?"
//#define URL_SEND_TOPAZ_TEST         "http://14.63.200.217/cogma/game/send_topaz.php?"
#define URL_GOOGLE_PAYLOAD          "http://14.63.200.217/cogma/game/get_payload_google.php"
#define URL_APPLE_UPDATE            "http://14.63.200.217/cogma/game/update_friend_apple.php?"
#define URL_PURCHASE_TOPAZ_GOOGLE   "http://14.63.200.217/cogma/game/purchase_topaz_google.php?"
#define URL_SEND_TOPAZ_GOOGLE       "http://14.63.200.217/cogma/game/send_topaz_google.php?"
#define URL_PURCHASE_TOPAZ_APPLE    "http://14.63.200.217/cogma/game/purchase_topaz_apple.php?"
#define URL_SEND_TOPAZ_APPLE        "http://14.63.200.217/cogma/game/send_topaz_apple.php?"
#define URL_PURCHASE_INGAME_ITEM    "http://14.63.200.217/cogma/game/purchase_ingame_item.php?"
#define URL_TERM_SERVICE            "http://14.63.200.217/cogma/game/update/rule_service.html"
#define URL_TERM_PRIVATE            "http://14.63.200.217/cogma/game/update/rule_private.html"

#endif



class Network
{
public:
    static void HttpPost(std::string data, std::string url, void* pointer, SEL_HttpResponse hr, std::string tag = "", std::string etc = "", bool isBasic = false);
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
    static std::string Encrypt_PS(bool isBasic = false);
    static std::string Encrypt_a(std::string data, bool isBasic = false);
    
    static int GetHttpResponseData(CCHttpResponse* res, char* data, bool isDeObfuscate = true);
    static int DeObfuscation(std::string obfuscatedStr, char* data);
    
    static void GetXMLFromResponseData(CCHttpResponse* res, xml_document &xmlDoc);
    static std::string GetXMLFromResponseDataForAndroid(const char* resData);
    static void ShowCommonError(int code);
};

#endif /* defined(__CocoMagic__Network__) */
