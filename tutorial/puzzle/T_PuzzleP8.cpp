#include "T_PuzzleP8.h"
#include "T_Puzzle.h"

T_PuzzleP8* T_PuzzleP8::CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder, float scale, int type)
{
    T_PuzzleP8* puzzleP8 = new T_PuzzleP8();

    puzzleP8->type = type;

    if (type == -1) // 기본 랜덤 피스 만들기
    {
        puzzleP8->type = rand() % TYPE_COUNT;
        //puzzleP8->type = 3 + rand()%2;
    }
    
    char name[20];
    sprintf(name, "pieces/%d.png", puzzleP8->type);

    puzzleP8->sprite = CCSprite::createWithSpriteFrameName(name);
    puzzleP8->sprite->setAnchorPoint(ap);
    puzzleP8->sprite->setPosition(pos);
    puzzleP8->sprite->setScale(scale*1.02f);
    
    puzzleP8->parent = parent;
    puzzleP8->zOrder = zOrder;
    
    return puzzleP8;
}

void T_PuzzleP8::SetType(int type)
{
    this->type = type;
}

int T_PuzzleP8::GetType()
{
    return type;
}

int T_PuzzleP8:: GetZOrder()
{
    return zOrder;
}

CCSprite* T_PuzzleP8::GetPiece()
{
    return sprite;
}

void T_PuzzleP8::RemovePiece()
{
    sprite = NULL;
}

void T_PuzzleP8::SetPosition(CCPoint pos)
{
    sprite->setPosition(pos);
}

void T_PuzzleP8::SetPiece(CCSprite* sprite)
{
    this->sprite = sprite;
}

void T_PuzzleP8::SetPieceTexture(CCTexture2D* texture)
{
    this->sprite->setTexture(texture);
}

void T_PuzzleP8::SetZOrder(int zOrder)
{
    this->zOrder = zOrder;
}
