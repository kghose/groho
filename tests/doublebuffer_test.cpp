#include "catch.hpp"

#include "threadedbuffer.hpp"

#define LOGURU_WITH_STREAMS 1
#include "loguru.hpp"

using namespace groho;

bool data_ok(const fs::path& path, const size_t test_size)
{
    std::ifstream file(path, std::ios::binary | std::ios::in);
    size_t*       data_in = new size_t[test_size];
    file.read((char*)data_in, sizeof(size_t) * test_size);
    bool passed = true;
    for (size_t i = 0; i < test_size; i++) {
        if (data_in[i] != i) {
            passed = false;
            LOG_S(ERROR) << path << ": "
                         << "data_in[i] (" << data_in[i] << ") != " << i;
            break;
        }
    }
    delete[] data_in;
    return passed;
}

TEST_CASE("Double buffer short", "[BUFFER]")
{
    const size_t test_size = 1 << 5;

    auto path = fs::temp_directory_path() / "test.bin";
    {
        ThreadedBuffer<size_t> buffer(path);
        for (size_t j = 0; j < test_size; j++) {
            buffer.write(j);
        }
    }

    REQUIRE(data_ok(path, test_size));
}

TEST_CASE("Double buffer long", "[BUFFER]")
{
    const size_t test_size = 1 << 20;

    auto path = fs::temp_directory_path() / "test.bin";
    {
        ThreadedBuffer<size_t> buffer(path);
        for (size_t j = 0; j < test_size; j++) {
            buffer.write(j);
        }
    }

    REQUIRE(data_ok(path, test_size));
}