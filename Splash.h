#ifndef __poops8__Splash__
#define __poops8__Splash__

#include "Common.h"
#include "Sound.h"
#include "cocos-ext.h"

#define HTTP_VERSION 0
#define HTTP_LOGIN 1
#define HTTP_MYINFO 2
#define HTTP_PRICE 3
#define HTTP_FRIENDS 4
#define HTTP_PROFILE_IMAGE 5

using namespace cocos2d::extension;

class Splash : public CCLayer
                //public CCIMEDelegate
                //public cocos2d::extension::CCEditBoxDelegate
{
public:
	virtual bool init();
    virtual void keyBackClicked();
    
	void ccTouchesBegan(CCSet* pTouches, CCEvent* pEvent);
	void ccTouchesMoved(CCSet* pTouches, CCEvent* pEvent);
	void ccTouchesEnded(CCSet* pTouches, CCEvent* pEvent);
    
	static CCScene* scene();
	CREATE_FUNC(Splash);
    
    void LogoLoadingCompleted();
    
    //virtual void editBoxEditingDidBegin(CCEditBox* editBox);
    //virtual void editBoxEditingDidEnd(CCEditBox* editBox);
    //virtual void editBoxEditing(CCEditBox* editBox);
    //virtual void editBoxReturn(CCEditBox* editBox);
//    void keyboardWillShow(CCIMEKeyboardNotificationInfo &info);
//    void keyboardWillHide(CCIMEKeyboardNotificationInfo &info);
    
    void XmlParseVersion(char* data, int size);
    void XmlParseLogin(char* data, int size);
    void XmlParseMyInfo(char* data, int size);
    void XmlParsePrice(char* data, int size);
    void XmlParseFriends(char* data, int size);
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
    void EndScene();
    
protected:
    CCSize winSize;
    
    //CCSprite* m_pEditNameSprite;
    //CCTextFieldTTF* m_pEditName;
    //CCEditBox* m_pEditName;
    //CCLabelTTF* m_pLoadLabel;
    
private:
    int mKakaoId;
    int mDeviceType;
    
    int httpStatus;
    
    bool isKeyBackClicked;
    CCSprite* m_pBackground;
    CCSprite* m_pTitle;
    CCLabelTTF* m_pMsgLabel;
    CCSprite* m_pStartBtn;
    CCSprite* m_pStartLetter;
    
    int profileCntTotal;
    int profileCnt;
};


#endif /* defined(__poops8__Splash__) */


