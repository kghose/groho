#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include "catch.hpp"

#define LOGURU_IMPLEMENTATION 1 // Otherwise compile error
#define LOGURU_THREADNAME_WIDTH 0
#define LOGURU_FILENAME_WIDTH 14
#include "loguru.hpp"