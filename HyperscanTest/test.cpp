#include "pch.h"
#include "uarecognizer.h"

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
