#ifndef __CocoMagic__Loading__
#define __CocoMagic__Loading__

#include "Common.h"

using namespace cocos2d::extension;

class Loading : public CCLayer
{
public:
    ~Loading(void);
    static CCScene* scene(int status);
    
	virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
	CREATE_FUNC(Loading);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseGameStart(char* data, int size);
    
    void EndScene();
    
protected:
    CCSize m_winSize;

private:
    CCSprite* pBlack;
    CCSprite* pCoco;
    CCSprite* pLoading;
};

#endif /* defined(__CocoMagic__Loading__) */
