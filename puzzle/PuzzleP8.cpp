#include "PuzzleP8.h"
#include "Puzzle.h"

PuzzleP8* PuzzleP8::CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder, float scale, bool clear, int type)
{
    PuzzleP8* puzzleP8 = new PuzzleP8();

    puzzleP8->type = type;

    if (type == -1)
    {
        if (clear)
            puzzleP8->type = rand() % (TYPE_COUNT-1);
        else
            puzzleP8->type = rand() % TYPE_COUNT;
    }

    char name[20];
    sprintf(name, "pieces/%d.png", puzzleP8->type);
    
    // type : red(0), blue(1), green(2), yellow(3), white(4)
    
    puzzleP8->sprite = CCSprite::createWithSpriteFrameName(name);
    puzzleP8->sprite->setAnchorPoint(ap);
    puzzleP8->sprite->setPosition(pos);
    puzzleP8->sprite->setScale(scale*1.02f);
    
    puzzleP8->parent = parent;
    puzzleP8->zOrder = zOrder;
    
    return puzzleP8;
}

void PuzzleP8::SetType(int type)
{
    this->type = type;
}

int PuzzleP8::GetType()
{
    return type;
}

int PuzzleP8:: GetZOrder()
{
    return zOrder;
}

CCSprite* PuzzleP8::GetPiece()
{
    return sprite;
}

void PuzzleP8::RemovePiece()
{
    sprite = NULL;
}

void PuzzleP8::SetPosition(CCPoint pos)
{
    sprite->setPosition(pos);
}

void PuzzleP8::SetPiece(CCSprite* sprite)
{
    this->sprite = sprite;
}

void PuzzleP8::SetPieceTexture(CCTexture2D* texture)
{
    this->sprite->setTexture(texture);
}

void PuzzleP8::SetZOrder(int zOrder)
{
    this->zOrder = zOrder;
}
