#include "PuzzleP8.h"
#include "Puzzle.h"

PuzzleP8* PuzzleP8::CreateP8(CCPoint ap, CCPoint pos, void* parent, int zOrder, float scale, bool clear, int type)
{
    PuzzleP8* puzzleP8 = new PuzzleP8();

    puzzleP8->type = type;
    
    // "3줄 페인트" 아이템을 만들어야 하는 경우 (1회성 : 게임 시작 직전) -> 미리 CreatePiece에서 만들어져 들어옴.

    if (type == -1) // 기본 랜덤 피스 만들기
    {
        if (clear) // 한종류 색깔 없애는 아이템 썼을 경우 (흰색이 나오지 않도록)
            puzzleP8->type = rand() % (TYPE_COUNT-1);
        else // 기본 5가지 색
            puzzleP8->type = rand() % TYPE_COUNT;
    }
    else if (type == -10) // "페인트" 아이템을 만들어야 하는 경우
    {
        if (clear) // 한종류 색깔 없애는 아이템 썼을 경우 (흰색이 나오지 않도록)
            puzzleP8->type = (rand() % (TYPE_COUNT-1)) + ITEM_PAINT_RED;
        else
            puzzleP8->type = (rand() % TYPE_COUNT) + ITEM_PAINT_RED;
    }
    
    char name[20];
    sprintf(name, "pieces/%d.png", puzzleP8->type);
    
    // type : red(0), blue(1), green(2), yellow(3), white(4)
    // item - paint : red(10), blue(11), green(12), yellow(13), white(14)
    // item - staff : 20
    // item - paint(special) : red(103), blue(113), green(123)
    
    puzzleP8->sprite = CCSprite::createWithSpriteFrameName(name);
    puzzleP8->sprite->setAnchorPoint(ap);
    puzzleP8->sprite->setPosition(pos);
    puzzleP8->sprite->setScale(scale*1.02f);
    
    puzzleP8->effect = NULL;
    if (type > 100)
    {
        float ofs = puzzleP8->sprite->getContentSize().width / 2;
        puzzleP8->effect = CCSprite::createWithSpriteFrameName("effect/item_paint_special.png");
        //puzzleP8->effect->setAnchorPoint(ccp(0.5, 0.5));
        puzzleP8->effect->setPosition(ccp(25+ofs, 25+ofs));
        puzzleP8->effect->setOpacity(0);
    }
    
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

CCSprite* PuzzleP8::GetEffect()
{
    return effect;
}

void PuzzleP8::RemovePiece()
{
    if (effect != NULL)
        effect->removeFromParentAndCleanup(true);
    effect = NULL;
    
    sprite->removeFromParentAndCleanup(true);
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
