#include <tests/tut1.proto.hpp>

#include <catch/catch.hpp>


TEST_CASE("Basic tutorial message") {
    tutorial::Person person;
    person.name = "John Smith";
    person.id = 42;
    person.email = "john@example.com";

    person.phone.emplace_back();
    person.phone.back().number = "1-800-555-5555";
    person.phone.back().type = tutorial::Person::PhoneType::WORK;
    person.phone.emplace_back();
    person.phone.back().number = "123-555-5555";
    person.phone.back().type = tutorial::Person::PhoneType::MOBILE;

    std::string buf;
    proto11::write(person, std::back_inserter(buf));

    auto decoded = proto11::read<tutorial::Person>(buf.begin(), buf.end()).first;
}