#include <tests/basic.proto.hpp>

#include <catch/catch.hpp>

#include <proto11/encoder/encoder.hpp>

#include <boost/optional/optional_io.hpp>

#include <iostream>

TEST_CASE("Encode/decode message") {
    basic::User u;
    u.name = "Johnny";
    u.realName = "Johnathan";
    u.nickName = "JJ";
    u.what = basic::MyEnumeration::Something;
    u.meow = 44;
    std::string buf;
    proto11::write(u, std::back_inserter(buf));

    CHECK(buf.size() != 0);

    // Should be able to decode our messages
    auto decoded = proto11::read<basic::User>(buf.begin(), buf.end()).first;
    CHECK(decoded.name == u.name);
    CHECK(decoded.realName == u.realName);
    CHECK(decoded.nickName == u.nickName);
    CHECK((decoded.what == u.what));
    CHECK(decoded.meow == u.meow);
}