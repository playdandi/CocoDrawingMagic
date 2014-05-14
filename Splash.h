#ifndef __poops8__Splash__
#define __poops8__Splash__

#include "Common.h"
#include "Sound.h"
#include "cocos-ext.h"

#define HTTP_VERSION 0
#define HTTP_LOGIN 1
#define HTTP_MYINFO 2
#define HTTP_FRIENDS 3
#define HTTP_PROFILE_IMAGE 4

using namespace cocos2d::extension;

USING_NS_CC_EXT;

class Splash : public CCLayer,
                public CCIMEDelegate
                //public cocos2d::extension::CCEditBoxDelegate
{
public:
    ~Splash(void);
	virtual bool init();
    virtual void onEnter();
    virtual void onPause();
    virtual void onExit();
    virtual void keyBackClicked();
    
    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
    
	static CCScene* scene();
	CREATE_FUNC(Splash);
    
    void LogoLoadingCompleted();
    
    //virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    //virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    //virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text);
    //virtual void editBoxReturn(CCEditBox* editBox);
    void keyboardWillShow(CCIMEKeyboardNotificationInfo &info);
    void keyboardWillHide(CCIMEKeyboardNotificationInfo &info);
    
    void XMLParseGameData();
    void WriteResFile(char* data, int size);
    void XmlParseVersion(char* data, int size);
    void XmlParseLogin(char* data, int size);
    void XmlParseMyInfo(char* data, int size);
    void XmlParseFriends(char* data, int size);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    void GetTodayCandyFriend();
    void LastActionStart();
    void LastActionCallback(CCNode* sender, void *data);
    void LastActionCallback2(CCNode* sender, void *data);
    void EndScene();
    
    
protected:
    CCSize winSize;
    bool isLoading;
    bool isStarting;
    bool isKeyBackClicked;
    
    int gameVersion; // 새로 받은 게임버전
    
    //CCSprite* m_pEditNameSprite;
    CCTextFieldTTF* m_pEditName;
    //CCEditBox* m_pEditName;
    //CCLabelTTF* m_pLoadLabel;
    
    int mKakaoId;
    int mDeviceType;
    int httpStatus;
    int profileCntTotal;
    int profileCnt;
    
private:
    CCSprite* m_pBackground;
    CCSprite* m_pTitle;
    CCLabelTTF* m_pMsgLabel;
    CCSprite* m_pStartBtn;
    CCSprite* m_pStartLetter;
};


#endif /* defined(__poops8__Splash__) */


