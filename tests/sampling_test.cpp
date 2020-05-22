#include "catch.hpp"

#include "serialize.hpp"

using namespace groho;

TEST_CASE("Serializer", "[SAMPLING]")
{
    auto                  path    = fs::temp_directory_path();
    std::vector<NAIFbody> objects = { 0, 2, 5 };
    Serialize(path, objects);
}