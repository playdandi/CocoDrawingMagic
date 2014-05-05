#ifndef __CocoMagic__Loading__
#define __CocoMagic__Loading__

#include "Common.h"

using namespace cocos2d::extension;

class Loading : public CCLayer
{
public:
    ~Loading(void);
	virtual bool init();
    virtual void onEnter();
    virtual void onPause();
    virtual void onExit();
    
	static CCScene* scene();
	CREATE_FUNC(Loading);
    
    void onHttpRequestCompleted(CCNode *sender, void *data);
    void XmlParseGameStart(char* data, int size);
    
    void EndScene();
};

#endif /* defined(__CocoMagic__Loading__) */
