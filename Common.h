#ifndef PuzzleGame_Common
#define PuzzleGame_Common

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCPlatformConfig.h"
#include "Sound.h"
#include "Data.h"

USING_NS_CC;

#define DESIGN_WIDTH 768.0f
#define DESIGN_HEIGHT 1024.0f

#define ROW_COUNT 7
#define COLUMN_COUNT 7

#define TYPE_COUNT 5
#define PIECE_RED 0
#define PIECE_BLUE 1
#define PIECE_GREEN 2
#define TYPE_DIA_COUNT 3
#define TYPE_SP_COUNT 3

#define PIECE8_WIDTH  152//150
#define PIECE8_HEIGHT 152//150
#define PIECE8_FRAME_WIDTH 154
#define PIECE8_FRAME_HEIGHT 154
#define PIECE4_WIDTH 39  //64/2
#define PIECE4_HEIGHT 39 //64/2

#define QUEUE_CNT 5

#define LARGEST_MASS_LOWER_BOUND 7

#define BUFFER_SIZE 1024*10
#define IMAGE_BUFFER_SIZE 120*120*8

#define NUMOFSKILL 32


#define BTN_1 0
#define BTN_2 1
#define POPUP_STARCANDY_0 0
#define POPUP_STARCANDY_1 1
#define POPUP_STARCANDY_2 2
#define POPUP_EXIT 3

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
    
    static std::string InsertComma(std::string number);
    static CCLayer* MakeImageNumberLayer(std::string number);
    static void ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced, int etc = -1);
    static void ShowPopup(void* obj, std::string from, std::string to, bool isReplaced, int popupType, int btnType, std::vector<int> data);
};


class SpriteObject
{
public:
    static SpriteObject* Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255);
    static SpriteObject* CreateFromSprite(int spriteType, CCSprite* spr, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255);
    static SpriteObject* CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int priority = 0, int alpha = 255);
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
    
    std::vector<SpriteObject*> spriteObj;
};



#endif

