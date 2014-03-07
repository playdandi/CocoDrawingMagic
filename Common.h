#ifndef PuzzleGame_Common
#define PuzzleGame_Common

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCPlatformConfig.h"
#include "Ranking.h"
#include "Sound.h"

USING_NS_CC;

#define DESIGN_WIDTH 768.0f
#define DESIGN_HEIGHT 1024.0f

#define ROW_COUNT 7
#define COLUMN_COUNT 7

#define TYPE_COUNT 5
#define PIECE_GREEN 0
#define PIECE_RED 2
#define PIECE_BLUE 3
#define TYPE_DIA_COUNT 3
#define TYPE_SP_COUNT 3

#define PIECE8_WIDTH 108
#define PIECE8_HEIGHT 108
#define PIECE4_WIDTH 64/2
#define PIECE4_HEIGHT 64/2

#define QUEUE_CNT 5

#define LARGEST_MASS_LOWER_BOUND 7

#define BUFFER_SIZE 1024

#define NUMOFSKILL 32

using namespace cocos2d;
using namespace cocos2d::extension;


struct friendScore
{
	char name[16];
	int score;
};

extern int iCash;
extern int iGold;
extern int iRemainingHeartTime;
extern int iRemainingHeartNum;
extern int iRemainingObjectTime;
extern int iAge;
extern int iType;
extern int iWeight;
extern int iMaxScore;
extern std::string sUsername;
extern std::vector<int> vEnabledMaterial;
extern std::vector<int> vStoredMaterial;
extern std::vector<struct friendScore> vScoreList;

extern std::string fontList[];

class Sound;
class Ranking;
class BuyTopaz;

extern Sound* sound;


class Common
{
public:
/*
	static float ComputeX(float x);
	static float ComputeY(float y);
	static CCPoint ComputeXY(float x, float y);
	static int ComputeBoardX(float x);
	static int ComputeBoardY(float y);
*/
    //static CCPoint GetPosition(CCDictionary* dic);
    //static CCPoint GetAnchorPoint(CCDictionary* dic);
    
    static bool XmlParseMoneyRaisePuzzle(char* data, int size, bool hasMoney);

	static bool XmlParsePuzzleStart(std::vector<char>* buffer);
	static bool XmlParseRanking(std::vector<char>* buffer);

    static bool XmlParsePuzzleEnd(char* data, int size);
    
    static CCRenderTexture* CreateStroke( CCSprite* label, int size, ccColor3B color, GLubyte opacity );
    
    static CCLayer* MakeImageNumberLayer(std::string number);
    static void ShowNextScene(void* obj, std::string from, std::string to, bool isReplaced);
};


class SpriteObject
{
public:
    static SpriteObject* Create(int spriteType, std::string name, CCPoint ap, CCPoint pos, CCSize size, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha = 255);
    static SpriteObject* CreateLabel(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha = 255);
    static SpriteObject* CreateLabelArea(std::string text, std::string font, int size, CCPoint ap, CCPoint pos, ccColor3B color, CCSize range, CCTextAlignment align, CCVerticalTextAlignment align_vertical, std::string parentName, std::string parentType, void* parent, int zOrder, int alpha = 255);


    int type;
    CCSprite* sprite;
    CCScale9Sprite* sprite9;
    CCLabelTTF* label;
    std::string name;
    
    std::string parentName;
    std::string parentType; // -1 : 현재 scene, 0 : 이 안의 sprite, 1 : 이 안의 sprite9, 2 : 다른 곳의 layer
    void* parent;
    int zOrder;
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

