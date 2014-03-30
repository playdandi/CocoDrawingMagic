#ifndef __poops8__PuzzleP4__
#define __poops8__PuzzleP4__

#include "Common.h"

#define BLOCKED 0
#define CONNECTED 1
#define ITEM 2

/*
class Puzzle;

class PuzzleP4
{
public:
	PuzzleP4();
	~PuzzleP4();
    
public:
	static PuzzleP4* Create(Puzzle* layer, int x, int y);
	int GetType();
	void SetType(int type);
    int GetTypeSP();
    void SetTypeSP(int type_sp);
    
	void SetGameLayer(Puzzle* layer);
    Puzzle* GetGameLayer();
    
    void InitSprites();
    void CreateSprites(Puzzle* layer, int x, int y);
    
    void SetScales(float scale);
    void SetOpacities(int alpha);
    void SetPositions(CCPoint pos);
    void AddChildren(int zOrder);
    void RemoveChildren();
    
private:
    int m_type; // 0 : blocked, 1 : connected , 2 : special
    //int m_type_sp; // special type -> 0 : 4*4 bomb , ......
    
    //CCSprite* m_basic;
    CCSprite* m_leftup;
    CCSprite* m_rightup;
    CCSprite* m_leftdown;
    CCSprite* m_rightdown;
    CCSprite* m_special;

	Puzzle* m_pGameLayer;
};
*/

class PuzzleP4
{
public:
    static PuzzleP4* CreateP4(void* parent, int zOrder, int type = -1);
    void CreateSprites(int x, int y, int lu, int ru, int ld, int rd, CCPoint ap, CCPoint pos);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    void InitChild();
    CCSprite* GetLeftUp();
    CCSprite* GetRightUp();
    CCSprite* GetLeftDown();
    CCSprite* GetRightDown();
    //CCSprite* GetPiece();
    //void SetPosition(CCPoint pos);
    
private:
    CCSprite* leftup;
    CCSprite* leftdown;
    CCSprite* rightup;
    CCSprite* rightdown;
    int type;
    void* parent;
    int zOrder;
};

#endif
