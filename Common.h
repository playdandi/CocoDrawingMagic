#ifndef PuzzleGame_Common
#define PuzzleGame_Common

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCPlatformConfig.h"
#include "Sound.h"
#include "Data.h"
#include "Fairy.h"

USING_NS_CC;

#define DESIGN_WIDTH 768.0f
#define DESIGN_HEIGHT 1024.0f

#define ROW_COUNT 7
#define COLUMN_COUNT 7

#define TYPE_COUNT 5
#define PIECE_RED 0
#define PIECE_BLUE 1
#define PIECE_GREEN 2
#define PIECE_WHITE 3
#define PIECE_YELLOW 4

#define TYPE_DIA_COUNT 3
#define TYPE_SP_COUNT 3

/*#define PIECE8_WIDTH  128//152
#define PIECE8_HEIGHT 128//152
#define PIECE8_FRAME_WIDTH 130//154
#define PIECE8_FRAME_HEIGHT 130//154
#define PIECE4_WIDTH 39  //64/2
#define PIECE4_HEIGHT 39 //64/2
*/
#define QUEUE_CNT 5

#define LARGEST_MASS_LOWER_BOUND 7

#define BUFFER_SIZE 40000
#define IMAGE_BUFFER_SIZE 120*120*8

#define NUMOFSKILL 32

// 팝업창에서 버튼 개수를 구분하기 위한 predefined type
#define BTN_1 0
#define BTN_2 1
// 어떤 팝업창인지 구분하기 위한 predefined type
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
#define UPGRADE_STAFF_BY_TOPAZ_NOMONEY 29
#define UPGRADE_STAFF_BY_STARCANDY_NOMONEY 30
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
#define UPGRADE_FAIRY_BY_TOPAZ_NOMONEY 42
#define UPGRADE_FAIRY_BY_STARCANDY_NOMONEY 43


using namespace cocos2d;
using namespace cocos2d::extension;


extern std::string fontList[];

class Sound;
class Ranking;
class BuyTopaz;

extern Sound* sound;


class Common
{
public:
    static CCRenderTexture* CreateStroke( CCSprite* label, int size, ccColor3B color, GLubyte opacity );
    
    static std::string MakeComma(int number);
    static std::string InsertComma(std::string number);
    static CCLayer* MakeImageNumberLayer(std::string number, int type);
    static CCLayer* MakeItemNumberLayer(std::string number);
    static void ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc = -1);
    static void ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data);
};


class SpriteObject
{
public:
    static SpriteObject* Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, int tag = -1);
    static SpriteObject* CreateFromSprite(int spriteType, CCSprite* spr, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, float scale = 1.0f);
    static SpriteObject* CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255, int tag = -1);
    static SpriteObject* CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255);

    int type;
    CCSprite* sprite;
    CCScale9Sprite* sprite9;
    CCLabelTTF* label;
    std::string name;
    
    std::string parentName;
    std::string parentType; // -1 : 현재 scene, 0 : 이 안의 sprite, 1 : 이 안의 sprite9, 2 : 다른 곳의 layer
    void* parent;
    int zOrder;
    int priority;
};

class SpriteClass
{
public:
    void AddChild(int idx);
    void* FindParentSprite(int idx, std::string parentName);
    void* FindParentSprite9(int idx, std::string parentName);
    void* FindSpriteByName(std::string name);
    CCPoint FindParentCenterPos(std::string parentName);
    CCSize GetContentSizeByName(std::string name);
    void* FindLabelByTag(int tag);
    
    void RemoveAllObjects();
    
    std::vector<SpriteObject*> spriteObj;
    std::vector<CCLayer*> layers;
};



#endif

