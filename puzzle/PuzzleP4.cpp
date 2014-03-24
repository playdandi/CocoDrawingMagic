#include "PuzzleP4.h"
#include "Puzzle.h"

PuzzleP4::PuzzleP4()
: m_pGameLayer(NULL)
{
}

PuzzleP4::~PuzzleP4()
{
}


PuzzleP4* PuzzleP4::Create(Puzzle* layer, int x, int y)
{
    PuzzleP4* pPuzzleP4 = new PuzzleP4();
    pPuzzleP4->SetGameLayer(layer);
    
    int r = rand()%100;
    int type = (r < 50) ? BLOCKED : CONNECTED; // 연결 diamond 확률은 50%

    pPuzzleP4->SetType(type);
    
    pPuzzleP4->CreateSprites(layer, x, y);
    
	return pPuzzleP4;
}

void PuzzleP4::InitSprites()
{
    /*
    if (m_basic == NULL)
    {
        m_basic = new CCSprite();
        m_basic->initWithTexture(,
                             CCRectMake(, , , ));
        m_basic->autorelease();
    }
     */
    
    m_leftup = NULL;
    m_rightup = NULL;
    m_leftdown = NULL;
    m_rightdown = NULL;
    //m_special = NULL;
}

void PuzzleP4::CreateSprites(Puzzle* layer, int x, int y)
{
    InitSprites();
    
    if (m_type != BLOCKED)
    {
        // 4각형의 4방향에 있는 8각형들의 type을 구해서 삼각형을 어떻게 만들지 결정한다.
        int lu = (x == 1 && y == ROW_COUNT-1) ? -1 : layer->GetBoard(x-1, y)->GetType();
        int rd = (x == COLUMN_COUNT-1 && y == 1) ? -2 : layer->GetBoard(x, y-1)->GetType();
        int ru = (x == COLUMN_COUNT-1 && y == ROW_COUNT-1) ? -3 : layer->GetBoard(x, y)->GetType();
        int ld = (x == 1 && y == 1) ? -4 : layer->GetBoard(x-1, y-1)->GetType();
        
        if (lu == rd)
        {
            CCLog(" type : %d", lu);
            char name[20];
            sprintf(name, "pieces/%d_link.png", lu);
            m_leftup = CCSprite::createWithSpriteFrameName(name);
            m_rightdown = CCSprite::createWithSpriteFrameName(name);

            /*m_leftup->initWithTexture(pPuzzlePiece4,
                                      CCRectMake(lu*PIECE4_WIDTH+3*lu+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
            m_rightdown->initWithTexture(pPuzzlePiece4,
                                      CCRectMake(rd*PIECE4_WIDTH+3*rd+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
            m_leftup->autorelease();
            m_rightdown->autorelease();*/
            
            if (ru != ld)
            {
                //m_rightup = new CCSprite();
                //m_leftdown = new CCSprite();
                m_rightup = CCSprite::createWithSpriteFrameName(name);
                m_leftdown = CCSprite::createWithSpriteFrameName(name);
                /*
                m_rightup->initWithTexture(pPuzzlePiece4,
                                    CCRectMake(lu*PIECE4_WIDTH+3*lu+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
                m_leftdown->initWithTexture(pPuzzlePiece4,
                                    CCRectMake(lu*PIECE4_WIDTH+3*lu+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
                m_rightup->autorelease();
                m_leftdown->autorelease();
                 */
            }
        }
        if (ru == ld)
        {
            char name[20];
            sprintf(name, "pieces/%d_link.png", ru);
            m_rightup = CCSprite::createWithSpriteFrameName(name);
            m_leftdown = CCSprite::createWithSpriteFrameName(name);
            /*
            m_rightup = new CCSprite();
            m_leftdown = new CCSprite();
            m_rightup->initWithTexture(pPuzzlePiece4,
                                CCRectMake(ru*PIECE4_WIDTH+3*ru+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
            m_leftdown->initWithTexture(pPuzzlePiece4,
                                CCRectMake(ld*PIECE4_WIDTH+3*ld+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
            m_rightup->autorelease();
            m_leftdown->autorelease();
             */
            
            if (lu != rd)
            {
                m_leftup = CCSprite::createWithSpriteFrameName(name);
                m_rightdown = CCSprite::createWithSpriteFrameName(name);
                /*
                m_leftup->initWithTexture(pPuzzlePiece4,
                                    CCRectMake(ru*PIECE4_WIDTH+3*ru+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
                m_rightdown->initWithTexture(pPuzzlePiece4,
                                    CCRectMake(ru*PIECE4_WIDTH+3*ru+1, 0, PIECE4_WIDTH, PIECE4_HEIGHT));
                m_leftup->autorelease();
                m_rightdown->autorelease();
                */
            }
        }
    }
    
    // special은 나중에...
}

void PuzzleP4::SetScales(float scale)
{
    if (m_leftup != NULL)
        m_leftup->setScale(scale);
    if (m_rightup != NULL)
        m_rightup->setScale(scale);
    if (m_leftdown != NULL)
        m_leftdown->setScale(scale);
    if (m_rightdown != NULL)
        m_rightdown->setScale(scale);
}

void PuzzleP4::SetOpacities(int alpha)
{
    if (m_leftup != NULL)
        m_leftup->setOpacity(alpha);
    if (m_rightup != NULL)
        m_rightup->setOpacity(alpha);
    if (m_leftdown != NULL)
        m_leftdown->setOpacity(alpha);
    if (m_rightdown != NULL)
        m_rightdown->setOpacity(alpha);
}

void PuzzleP4::SetPositions(CCPoint pos)
{
    //m_basic->setPosition(pos);

    if (m_leftup != NULL)
    {
        m_leftup->setAnchorPoint(ccp(1, 0));
        m_leftup->setPosition(pos);
    }
    if (m_rightup != NULL)
    {
        m_rightup->setAnchorPoint(ccp(1, 0));
        m_rightup->setRotation(90);
        m_rightup->setPosition(pos);
    }
    if (m_leftdown != NULL)
    {
        m_leftdown->setAnchorPoint(ccp(1, 0));
        m_leftdown->setRotation(-90);
        m_leftdown->setPosition(pos);
    }
    if (m_rightdown != NULL)
    {
        m_rightdown->setAnchorPoint(ccp(1, 0));
        m_rightdown->setRotation(180);
        m_rightdown->setPosition(pos);
    }
    
    // special은 나중에...
    //CCLog("set position");
}

void PuzzleP4::AddChildren(int zOrder)
{
    //CCLog("add children");
    //GetGameLayer()->GetPieceLayer()->addChild(m_basic, 0);
    /*
    if (m_leftup != NULL)
        GetGameLayer()->GetPieceLayer()->addChild(m_leftup, zOrder);
    if (m_rightup != NULL)
        GetGameLayer()->GetPieceLayer()->addChild(m_rightup, zOrder);
    if (m_leftdown != NULL)
        GetGameLayer()->GetPieceLayer()->addChild(m_leftdown, zOrder);
    if (m_rightdown != NULL)
        GetGameLayer()->GetPieceLayer()->addChild(m_rightdown, zOrder);
    */
    if (m_leftup != NULL)    GetGameLayer()->addChild(m_leftup, zOrder);
    if (m_rightup != NULL)   GetGameLayer()->addChild(m_rightup, zOrder);
    if (m_leftdown != NULL)  GetGameLayer()->addChild(m_leftdown, zOrder);
    if (m_rightdown != NULL) GetGameLayer()->addChild(m_rightdown, zOrder);
    // special은 나중에...
    //CCLog("add children done");
}

void PuzzleP4::RemoveChildren()
{
    //GetGameLayer()->GetPieceLayer()->removeChild(m_basic, true);
    if (m_leftup != NULL)
        GetGameLayer()->GetPieceLayer()->removeChild(m_leftup, true);
    if (m_rightup != NULL)
        GetGameLayer()->GetPieceLayer()->removeChild(m_rightup, true);
    if (m_leftdown != NULL)
        GetGameLayer()->GetPieceLayer()->removeChild(m_leftdown, true);
    if (m_rightdown != NULL)
        GetGameLayer()->GetPieceLayer()->removeChild(m_rightdown, true);
    
    InitSprites();
    // special은 나중에...
}


int PuzzleP4::GetType()
{
	return m_type;
}

void PuzzleP4::SetType(int type)
{
	m_type = type;
}

/*
int PuzzleP4::GetTypeSP()
{
    return m_type_sp;
}

void PuzzleP4::SetTypeSP(int type_sp)
{
    m_type_sp = type_sp;
}
*/

void PuzzleP4::SetGameLayer(Puzzle* layer)
{
    m_pGameLayer = layer;
}

Puzzle* PuzzleP4::GetGameLayer()
{
    return m_pGameLayer;
}
