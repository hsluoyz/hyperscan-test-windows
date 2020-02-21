#include "pch.h"
#include "hs.h"

hs_database_t* database = NULL;
hs_scratch_t* scratch = NULL;
bool gIsMatched = false;

char* regexBrowserArray[] = {
    "(?:Chrome).*OPR/\\d+\\.\\d+\\.\\d+|Opera/",
    "EdgiOS|Edge/\\d+\\.\\d+",
    "CriOS|Chromium|Chrome/\\d+\\.\\d+(?:\\.\\d+)?(?:\\.\\d+)?",
    "FxiOS|Firefox/",
    "Safari/",
    "Trident/|[MS]?IE \\d+\\.\\d+"
};

char* regexOsArray[] = {
    "Windows|Win 9x|WinNT|Windows ?CE|Win ?(?:95|98|3.1|NT 4.0|ME|2000)|Windows ?(?:95|98|3.1|NT 4.0|ME|2000)|Win16|Win32",
    "Android|Silk-Accelerated",
    "[Dd]ebian|Linux|linux|Fedora|Red Hat|PCLinuxOS|Puppy|Ubuntu|Kindle|Bada|Lubuntu|BackTrack|Slackware|(?:Free|Open|Net|\\b)BSD[/ ]|Ubuntu|Kubuntu|Arch Linux|CentOS|Slackware|Gentoo|openSUSE|SUSE|Red Hat|Fedora|PCLinuxOS|Mageia|(?:Free|Open|Net|\b)BSD|freebsd",
    "iPhone|iPad|iPod|like Mac OS X",
    "(?:Mac[ +]?|; )OS[ +]X|Mac OS X|Macintosh|Darwin"
};

char* regexDeviceArray[] = {
    "TV|tvOS",
    "iPad|Tablet|Silk|Kindle|; ARM; Trident/6\\.0; Touch[\\);]",
    "iPhone|Android|KAIOS/|Windows Phone OS"
};

enum BrowserName {kBrowserOpera, kBrowserEdge, kBrowserChrome, kBrowserFirefox, kBrowserSafari, kBrowserIe, kBrowserOther};
char* browserNames[] = {"Opera", "Edge", "Chrome", "Firefox", "Safari", "IE", "Other"};

enum OsName {kOsWindows, kOsAndroid, kOsLinux, kOsIos, kOsMacOs, kOsOther};
char* osNames[] = { "Windows", "Android", "Linux", "iOS", "macOS", "Other" };

enum DeviceName {kDeviceSmartTv, kDeviceTablet, kDeviceSmartphone, kDeviceOther};
char* deviceNames[] = { "SmartTV", "Tablet", "Smartphone", "Other" };

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

void releasePattern() {
    hs_free_scratch(scratch);
    scratch = NULL;
    hs_free_database(database);
    database = NULL;
}

// void initAllPatterns() {
//     for (int i = 0; i < kBrowserOther; i++) {
//         initPattern(regexBrowserArray[i]);
//     }
//
//     for (int i = 0; i < kOsOther; i++) {
//         initPattern(regexOsArray[i]);
//     }
//
//     for (int i = 0; i < kDeviceOther; i++) {
//         initPattern(regexDeviceArray[i]);
//     }
// }

static int eventHandler(unsigned int id, unsigned long long from,
    unsigned long long to, unsigned int flags, void* ctx) {
    gIsMatched = true;
    // printf("Match for pattern \"%s\" at offset %llu\n", (char*)ctx, to);
    return 0;
}

bool isMatched(char* pattern, char* userAgent) {
    gIsMatched = false;

    initPattern(pattern);
    if (hs_scan(database, userAgent, strlen(userAgent), 0, scratch, eventHandler,
        pattern) != HS_SUCCESS) {
        fprintf(stderr, "ERROR: Unable to scan input buffer. Exiting.\n");
        hs_free_scratch(scratch);
        // free(inputData);
        hs_free_database(database);
        return false;
    }
    releasePattern();

    return gIsMatched;
}

BrowserName checkBrowser(char* userAgent) {
    for (int i = 0; i < kBrowserOther; i++) {
        if (isMatched(regexBrowserArray[i], userAgent)) {
            return (BrowserName) i;
        }
    }

    return kBrowserOther;
}

OsName checkOs(char* userAgent) {
    for (int i = 0; i < kOsOther; i++) {
        if (isMatched(regexOsArray[i], userAgent)) {
            return (OsName)i;
        }
    }

    return kOsOther;
}

DeviceName checkDevice(char* userAgent) {
    for (int i = 0; i < kDeviceOther; i++) {
        if (isMatched(regexDeviceArray[i], userAgent)) {
            return (DeviceName)i;
        }
    }

    return kDeviceOther;
}

void testMatch(char* userAgent, bool res) {
    // initAllPatterns();

    BrowserName browserName = checkBrowser(userAgent);
    OsName osName = checkOs(userAgent);
    DeviceName deviceName = checkDevice(userAgent);

    printf("(%s, %s, %s)\n", browserNames[browserName], osNames[osName], deviceNames[deviceName]);

    // EXPECT_EQ(myRes, res);
}

TEST(TestCaseName, TestName) {
    testMatch("Opera/9.80 (Windows NT 5.1) Presto/2.12.388 Version/12.17", true);
}
