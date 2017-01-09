#include <catch/catch.hpp>

#include <proto11/encoder/varint.hpp>

#include <limits>
#include <random>


TEST_CASE("Encode zero") {
    std::string buf;
    proto11::write_varint(0u, back_inserter(buf));
    CHECK(buf.size() == 1);
    CHECK(buf[0] == '\0');  // Zero is a null byte
}

TEST_CASE("Decode zero") {
    std::string buf{ "\0", 1 };
    REQUIRE(buf.size() == 1);
    const auto value = proto11::read_varint(buf.begin(), buf.end());
}

TEST_CASE("Encode/decode values") {
    std::string buf;

    proto11::write_varint(300u, back_inserter(buf));
    CHECK(proto11::read_varint(buf.begin(), buf.end()).first == 300);

    std::mt19937 mt;
    std::uniform_int_distribution<std::uint64_t> dist{ 0, std::numeric_limits<std::uint64_t>::max() };

    for (int i = 0; i < 1000; ++i) {
        const auto num = dist(mt);
        buf.clear();
        proto11::write_varint(num, back_inserter(buf));
        REQUIRE(buf.size() > 0);
        const auto decoded = proto11::read_varint(buf.begin(), buf.end()).first;
        REQUIRE(num == decoded);
    }
}

TEST_CASE("ZigZag encode/decode") {

    CHECK(proto11::zigzag_encode(0) == 0);
    CHECK(proto11::zigzag_encode(-1) == 1);
    CHECK(proto11::zigzag_encode(1) == 2);
    CHECK(proto11::zigzag_encode(-2) == 3);

    CHECK(proto11::zigzag_decode(0) == 0);
    CHECK(proto11::zigzag_decode(1) == -1);
    CHECK(proto11::zigzag_decode(2) == 1);
    CHECK(proto11::zigzag_decode(3) == -2);

    CHECK(proto11::zigzag_decode(4294967294) == 2147483647);

    std::mt19937 mt;
    std::uniform_int_distribution<std::int64_t> dist{ std::numeric_limits<std::int64_t>::min(),
                                                      std::numeric_limits<std::int64_t>::max() };

    for (int i = 0; i < 1000; ++i) {
        const auto num = dist(mt);
        const auto encoded = proto11::zigzag_encode(num);
        CHECK(encoded != num);
        REQUIRE(proto11::zigzag_decode(encoded) == num);
    }
}