#ifndef PuzzleGame_PuzzleP8
#define PuzzleGame_PuzzleP8

#include "Common.h"

class PuzzleP8
{
public:
    static PuzzleP8* CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder, float scale, int type = -1);
    void SetType(int type);
    int GetType();
    int GetZOrder();
    CCSprite* GetPiece();
    void RemovePiece();
    
    void SetPosition(CCPoint pos);
    void SetPiece(CCSprite* sprite);
    void SetPieceTexture(CCTexture2D* texture);
    void SetZOrder(int zOrder);

private:
    CCSprite* sprite;
    int type;
    void* parent;
    int zOrder;
};

#endif