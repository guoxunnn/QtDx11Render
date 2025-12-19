#pragma once
#include <iostream>

#define ERROR_CHECK(condition) \
    (condition)          \
        ? (void)0        \
        : std::cout << "[ERROR " << __FUNCTION__ << "]: "

#define LOGW std::cout
#define LOGE std::cout
