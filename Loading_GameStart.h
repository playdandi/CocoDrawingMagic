#ifndef __CocoMagic__Loading_GameStart__
#define __CocoMagic__Loading_GameStart__

#include "Common.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
using namespace cocos2d;
using namespace cocos2d::extension;

class Loading_GameStart : public CCLayer
{
public:
    static CCScene* scene();
    
	virtual bool init();
    virtual void onEnter();
    virtual void onExit();
    
    void LoadingSprites();
    //void LoadingSpriteTimer(float f);
    //void Callback(CCNode* sender, void* pointer);
    
    //void onHttpRequestCompleted(CCNode *sender, void *data);
    //void XmlParseGameStart(xml_document *xmlDoc);
    
    void EndScene();
    
    CREATE_FUNC(Loading_GameStart);
    
protected:
    CCSize m_winSize;
    int spriteStatus;
    bool loadingSprites;
    bool timerStop;
    
private:
    CCSprite* pCoco;
    CCSprite* pLoading;
    CCSprite* pLoading2;
    CCSprite* pTip;
    CCLabelTTF* pTipMsg;
};

#endif /* defined(__CocoMagic__Loading_GameStart__) */
