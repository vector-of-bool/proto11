#include <catch/catch.hpp>

#include <proto11/encoder/message.hpp>

using namespace proto11;

TEST_CASE("Encode/decode field header") {
    detail::field_header header;

    header.field = 12;
    header.type = wire_type::fixed64;

    std::string buffer;
    detail::write_field_header(header, back_inserter(buffer));
    REQUIRE(buffer.size() != 0);

    auto decoded_header = detail::read_field_header(buffer.begin(), buffer.end()).first;
    CHECK(header.field == 12);
    CHECK(header.type == wire_type::fixed64);
}