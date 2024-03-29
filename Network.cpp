#include "Network.h"
#include <sstream>

void Network::replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if (from.empty())
        return;
    
    size_t start_pos = 0;
    while ( (start_pos = str.find(from, start_pos)) != std::string::npos )
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void Network::HttpPost(std::string data, std::string url, void* pointer, SEL_HttpResponse hr, std::string tag, std::string etc, bool isbasic)
{
    //CCLog("HttpPost : url = %s", url.c_str());
    //CCLog("param length = %d", (int)data.size());
    //CCLog("param = %s", (unsigned char*)(data.c_str()));
    
    std::string encoded_a = Encrypt_a(data);
    std::string encoded_ps = Encrypt_PS(isbasic);
    
    // 이제 파라미터를 만든다. (필요 파라미터 : PS, a)
    std::string postData = "";
    char temp[1024*40];
    
    // PS (encrypted data)
    sprintf(temp, "PS=%s&", encoded_ps.c_str());
    postData += temp;
    
    // a (encrypted data)
    sprintf(temp, "a=%s", encoded_a.c_str());
    postData += temp;
     
    CCHttpRequest* req = new CCHttpRequest();
    req->setUrl(url.c_str());
    req->setRequestData(postData.c_str(), postData.size());
    req->setRequestType(CCHttpRequest::kHttpPost);
    if (pointer != NULL)
        req->setResponseCallback((CCObject*)pointer, hr);
    if (tag != "")
    {
        if (etc != "") // (현재는 send_topaz에 대한 내용뿐이다)
            req->setTag((tag+"@"+etc).c_str());
        else
            req->setTag(tag.c_str());
    }
    CCHttpClient::getInstance()->send(req);
    req->release();
}

std::string Network::Encrypt_PS(bool isBasic)
{
    unsigned char encrypted[1024];
    int result;

    char ps_param[100];
    if (isBasic)
        //sprintf(ps_param, "0|1061|%d", binaryVersion_current);
        sprintf(ps_param, "0|%d|%d", CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion"), binaryVersion_current);
    else
        //sprintf(ps_param, "%d|1061|%d", myInfo->GetUserId(), binaryVersion_current);
        sprintf(ps_param, "%d|%d|%d", myInfo->GetUserId(), CCUserDefault::sharedUserDefault()->getIntegerForKey("gameVersion"), binaryVersion_current);
    std::string ps_param_s = ps_param;
    
    //CCLog("%s", ps_param_s.c_str());
    
    result = RSA_public_encrypt((int)ps_param_s.size(), (unsigned char*)(ps_param_s.c_str()), encrypted, rsa, RSA_PKCS1_PADDING);
    if (result == -1)
    {
        char* err;
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), err);
        //CCLog("RSA ERROR = %s", err);
        exit(0);
    }
    
    std::string encoded_ps = Common::base64_encode(encrypted, result);
    for (int i = 0 ; i < encoded_ps.size() ; i++)
        if (encoded_ps[i] == '+')
            encoded_ps[i] = '-';
    
    char temp[5];
    if (isBasic)
        sprintf(temp, "10");
    else
        sprintf(temp, "%d", publicKeyIndex);
    std::string number = temp;
    
    return number + encoded_ps;
}

std::string Network::Encrypt_a(std::string data, int size)
{
    // & -> || 형태로 바꿔 놓자.
    Network::replaceAll(data, "&", "||");
    
    if (size == -1) // 사이즈를 지정하지 않았다면 맞춰준다.
        size = (int)data.size();
    
    int unitLength = RSA_BIT/8 - 11; // substring 한 개의 단위길이
    int numOfSubstr = size / unitLength; // substring 개수
    if (size % unitLength > 0)
        numOfSubstr++;
    
    std::string totalEncryptedString = "";
    int totalEncryptedLength = 0;
    
    unsigned char encrypted[1024];
    int result;
    std::string subs;
    
    // data => 다음 3단계를 거쳐 암호화한다.
    // 1) RSA_public_encrypt (512 bit씩 쪼개서 암호화)
    // 2) base64_encode
    // 3) replacing '+' to '-'
    
    // 1) RSA_public_encrypt (512 bit씩 쪼개서 암호화)
    //CCLog("numOfSubstr = %d", numOfSubstr);
    for (int i = 0 ; i < numOfSubstr ; i++)
    {
        //CCLog("%d  /  %d %d", (int)data.size(), i*unitLength, unitLength);
        subs = data.substr(i*unitLength, unitLength);
        result = RSA_public_encrypt((int)subs.size(), (unsigned char*)(subs.c_str()), encrypted, rsa, RSA_PKCS1_PADDING);
        subs.clear();

        if (result == -1)
        {
            char* err;
            ERR_load_crypto_strings();
            ERR_error_string(ERR_get_error(), err);
            //CCLog("RSA ERROR = %s", err);
            exit(0);
        }
        
        for (int i = 0 ; i < result ; i++)
            totalEncryptedString.push_back(encrypted[i]);
        totalEncryptedLength += result;
        //CCLog("len len = %d", totalEncryptedLength);
    }
    //CCLog("done");
    
    // 2) base64_encode + 3) replacing '+' to '-'
    std::string encoded = Common::base64_encode((unsigned char*)totalEncryptedString.c_str(), totalEncryptedLength);
    for (int i = 0 ; i < encoded.size() ; i++)
        if (encoded[i] == '+')
            encoded[i] = '-';
    
    return encoded;
}

int Network::GetHttpResponseData(CCHttpResponse* res, char* data, bool isDeObfuscate)
{
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        CCLog("%d", res->getResponseCode());
        return NULL;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    std::string dumpData = "";
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData.push_back( (*buffer)[i] );

    int bufferSize = Network::DeObfuscation(dumpData, data);
    
    return bufferSize;
}

void Network::GetXMLFromResponseData(CCHttpResponse* res, xml_document &xmlDoc)
{
    if (!res || !res->isSucceed())
    {
        CCLog("res failed. error buffer: %s", res->getErrorBuffer());
        CCLog("%d", res->getResponseCode());
        return;
    }
    
    // dump data
    std::vector<char> *buffer = res->getResponseData();
    std::string dumpData = "";
    for (unsigned int i = 0 ; i < buffer->size() ; i++)
        dumpData.push_back( (*buffer)[i] );

    char decryptedData[BUFFER_SIZE];
    int bufferSize = Network::DeObfuscation(dumpData, decryptedData);
   
    //CCLog("%s", decryptedData);
    
    // xml parsing
    xml_parse_result result = xmlDoc.load_buffer(decryptedData, bufferSize);
    if (!result)
    {
        //CCLog("error description: %s", result.description());
        //CCLog("error offset: %d", result.offset);
    }
}

std::string Network::GetXMLFromResponseDataForAndroid(const char* resData)
{
    char decryptedData[BUFFER_SIZE];
    Network::DeObfuscation(resData, decryptedData);
    //CCLog("FOR ANDROID : %s", decryptedData);

    std::string result = decryptedData;
    return result;
}

int Network::DeObfuscation(std::string obfuscatedStr, char* data)
{
    int obfKey = atoi(obfuscatedStr.substr(0, 2).c_str()) - 10; // 앞 두자리는 key값이므로 분리한다.
    obfuscatedStr = obfuscatedStr.substr(2);
    //CCLog("%d %s", obfKey, obfuscatedStr.c_str());
    
    // replacing '-' to '+'
    Network::replaceAll(obfuscatedStr, "-", "+");
    // base64_decode
    obfuscatedStr = Common::base64_decode(obfuscatedStr);
    // xor operation with a key
    int keyLen = obfuscationKey[obfKey].size();
    std::string decryptedKey = "";
    for (int i = 0 ; i < obfuscatedStr.size() ; i++)
    {
        decryptedKey += obfuscatedStr[i] ^ obfuscationKey[obfKey][i%keyLen];
    }
    
    sprintf(data, "%s", decryptedKey.c_str());
    
    return (int)decryptedKey.size();
}


void Network::ShowCommonError(int code)
{
    //CCLog("xml code 공통에러 : %d", code);
    //CCLog("현재 위치 : %s", Depth::GetCurNameString().c_str());
    
    int popupType;
    switch (code)
    {
        case 1:
        case 2:
            popupType = NETWORK_FAIL; break;
        case -3:
        case -2:
        case -1:
        case 5:
            popupType = NEED_TO_REBOOT; break;
        case 4:
            popupType = NEED_TO_UPDATE; break;
        
        default:
            popupType = NEED_TO_REBOOT; break;
    }
    
    std::vector<int> nullData;
    Common::ShowPopup(Depth::GetCurPointer(), Depth::GetCurNameString(), "NoImage", false, popupType, BTN_1, nullData);
    
    /*
     코드 -1 : 디비 syntax 에러 (재부팅 필요)
     코드 -2 : 디비 insertion 에러 (재부팅 필요)
     코드 -3 : 10분 지나 세션 종료 (재부팅 필요)
     코드 1 : 파라미터 에러
     코드 2 : userId 없음
     코드 3 : 돈이 모지람.
     코드 4 : 바이너리 업데이트로 인한 앱 업데이트 필요
     코드 5 : 게임버전 업데이트로 인한 재부팅 필요 (재부팅 필요)
    */
}






