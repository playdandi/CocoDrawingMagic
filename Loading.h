#ifndef __CocoMagic__Loading__
#define __CocoMagic__Loading__

#include "Common.h"
#include "pugixml/pugixml.hpp"

using namespace pugi;
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
    
    void LoadingSprites();
    void Callback(CCNode* sender, void* pointer);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseGameStart(xml_document *xmlDoc);
    
    void EndScene();
    
protected:
    CCSize m_winSize;

private:
    CCSprite* pBlack;
    CCSprite* pCoco;
    CCSprite* pLoading;
    CCSprite* pLoading2;
};

#endif /* defined(__CocoMagic__Loading__) */
