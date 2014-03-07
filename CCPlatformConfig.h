#ifndef poops8_CCPlatformConfig_h
#define poops8_CCPlatformConfig_h

// define supported target platform macro which CC uses.
#define CC_PLATFORM_UNKNOWN            0
#define CC_PLATFORM_IOS                1
#define CC_PLATFORM_ANDROID            2
#define CC_PLATFORM_WIN32              3
#define CC_PLATFORM_MARMALADE          4
#define CC_PLATFORM_LINUX              5
#define CC_PLATFORM_BADA               6
#define CC_PLATFORM_QNX                7

// Determine tartet platform by compile environment macro.
//#define CC_TARGET_PLATFORM             CC_PLATFORM_UNKNOWN

// iphone
#if ! CC_TARGET_PLATFORM && (defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR))
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_IOS
#define CC_SUPPORT_PVRTC
#endif

// android
#if ! CC_TARGET_PLATFORM && defined(ANDROID)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_ANDROID
#endif

// win32
#if ! CC_TARGET_PLATFORM && (defined(WIN32) && defined(_WINDOWS))
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_WIN32
#endif

// linux
#if ! CC_TARGET_PLATFORM && defined(LINUX)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_LINUX
#endif

// marmalade
#if ! CC_TARGET_PLATFORM && defined(MARMALADE)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_MARMALADE
#endif

// bada
#if ! CC_TARGET_PLATFORM && defined(SHP)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_BADA
#endif

// qnx
#if ! CC_TARGET_PLATFORM && defined(__QNX__)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM     CC_PLATFORM_QNX
#endif

#endif
