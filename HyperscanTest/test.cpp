#include "pch.h"
#include "hs.h"

hs_database_t* database;
hs_scratch_t* scratch = NULL;
bool gIsMatched = false;

char* regexBrowserOpera = "(?:Chrome).*OPR/\\d+\\.\\d+\\.\\d+|Opera/";
char* regexBrowserEdge = "EdgiOS|Edge/\\d+\\.\\d+";
char* regexBrowserChrome = "CriOS|Chromium|Chrome/\\d+\\.\\d+(?:\\.\\d+)?(?:\\.\\d+)?";
char* regexBrowserFirefox = "FxiOS|Firefox/";
char* regexBrowserSafari = "Safari/";
char* regexBrowserIe = "Trident/|[MS]?IE \\d+\\.\\d+";

char* regexOsWindows = "Windows|Win 9x|WinNT|Windows ?CE|Win ?(?:95|98|3.1|NT 4.0|ME|2000)|Windows ?(?:95|98|3.1|NT 4.0|ME|2000)|Win16|Win32";
char* regexOsAndroid = "Android|Silk-Accelerated";
char* regexOsLinux = "[Dd]ebian|Linux|linux|Fedora|Red Hat|PCLinuxOS|Puppy|Ubuntu|Kindle|Bada|Lubuntu|BackTrack|Slackware|(?:Free|Open|Net|\\b)BSD[/ ]|Ubuntu|Kubuntu|Arch Linux|CentOS|Slackware|Gentoo|openSUSE|SUSE|Red Hat|Fedora|PCLinuxOS|Mageia|(?:Free|Open|Net|\b)BSD|freebsd";
char* regexOsIos = "iPhone|iPad|iPod|like Mac OS X";
char* regexOsMacOs = "(?:Mac[ +]?|; )OS[ +]X|Mac OS X|Macintosh|Darwin";

char* regexDeviceSmartTv = "TV|tvOS";
char* regexDeviceTablet = "iPad|Tablet|Silk|Kindle|; ARM; Trident/6\\.0; Touch[\\);]";
char* regexDeviceSmartphone = "iPhone|Android|KAIOS/|Windows Phone OS";

enum BrowserName {kBrowserOpera, kBrowserEdge, kBrowserChrome, kBrowserFirefox, kBrowserSafari, kBrowserIe, kBrowserOther};
enum OsName {kOsWindows, kOsAndroid, kOsLinux, kOsIos, kOsMacOs, kOsOther};
enum DeviceName {kDeviceSmartTv, kDeviceTablet, kDeviceSmartphone, kDeviceOther};

// Hyperscan Example Code
// https://github.com/intel/hyperscan/tree/master/examples
int initPattern(char* pattern) {
    hs_compile_error_t* compile_err;
    if (hs_compile(pattern, HS_FLAG_DOTALL, HS_MODE_BLOCK, NULL, &database, &compile_err) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to compile pattern \"%s\": %s\n", pattern, compile_err->message);
        hs_free_compile_error(compile_err);
        return -1;
    }

    if (hs_alloc_scratch(database, &scratch) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to allocate scratch space. Exiting.\n");
        hs_free_database(database);
        return -1;
    }

    return 0;
}

static int eventHandler(unsigned int id, unsigned long long from,
    unsigned long long to, unsigned int flags, void* ctx) {
    gIsMatched = true;
    printf("Match for pattern \"%s\" at offset %llu\n", (char*)ctx, to);
    return 0;
}

bool isMatched(char* pattern, char* userAgent) {
    gIsMatched = false;

    if (hs_scan(database, userAgent, strlen(userAgent), 0, scratch, eventHandler,
        pattern) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        // free(inputData);
        hs_free_database(database);
        return false;
    }

    return gIsMatched;
}

BrowserName checkBrowser(char* userAgent) {
    if (isMatched(regexBrowserOpera, userAgent)) {
        return kBrowserOpera;
    } else if (isMatched(regexBrowserEdge, userAgent)) {
        return kBrowserEdge;
    } else if (isMatched(regexBrowserChrome, userAgent)) {
        return kBrowserChrome;
    } else if (isMatched(regexBrowserFirefox, userAgent)) {
        return kBrowserFirefox;
    } else if (isMatched(regexBrowserSafari, userAgent)) {
        return kBrowserSafari;
    } else if (isMatched(regexBrowserIe, userAgent)) {
        return kBrowserIe;
    } else {
        return kBrowserOther;
    }
}

OsName checkOs(char* userAgent) {
    if (isMatched(regexOsWindows, userAgent)) {
        return kOsWindows;
    } else if (isMatched(regexOsAndroid, userAgent)) {
        return kOsAndroid;
    } else if (isMatched(regexOsLinux, userAgent)) {
        return kOsLinux;
    } else if (isMatched(regexOsIos, userAgent)) {
        return kOsIos;
    } else if (isMatched(regexOsMacOs, userAgent)) {
        return kOsMacOs;
    } else {
        return kOsOther;
    }
}

DeviceName checkDevice(char* userAgent) {
    if (isMatched(regexDeviceSmartTv, userAgent)) {
        return kDeviceSmartTv;
    } else if (isMatched(regexDeviceTablet, userAgent)) {
        return kDeviceTablet;
    } else if (isMatched(regexDeviceSmartphone, userAgent)) {
        return kDeviceSmartphone;
    } else {
        return kDeviceOther;
    }
}

void testMatch(char* userAgent, bool res) {
    // init(pattern);
    BrowserName browserName = checkBrowser(userAgent);
    OsName osName = checkOs(userAgent);
    DeviceName deviceName = checkDevice(userAgent);
    printf("(%s, %s, %s)\n", browserName, osName, deviceName);
    // EXPECT_EQ(myRes, res);
}

TEST(TestCaseName, TestName) {
    testMatch("Opera/9.80 (Windows NT 5.1) Presto/2.12.388 Version/12.17", true);
}
