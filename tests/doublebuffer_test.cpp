#include "catch.hpp"

#include "threadedbuffer.hpp"

using namespace groho;

TEST_CASE("Test of double buffer", "[BUFFER]")
{
    const size_t test_size = 1 << 5;

    auto path = fs::temp_directory_path() / "test.bin";
    {
        ThreadedBuffer<size_t> buffer(path);
        for (size_t j = 0; j < test_size; j++) {
            buffer.write(j);
        }
    }

    {
        std::ifstream file(path, std::ios::binary | std::ios::in);
        size_t        data_in[test_size];
        file.read((char*)data_in, sizeof(size_t) * test_size);
        bool passed = true;
        for (size_t i = 0; i < test_size; i++) {
            if (data_in[i] != i) {
                passed = false;
                break;
            }
        }
        REQUIRE(passed);
    }
}