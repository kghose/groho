/*
This file is part of Groho, a simulator for inter-planetary travel and warfare.
Copyright (c) 2017-2020 by Kaushik Ghose. Some rights reserved, see LICENSE.
*/

#include "catch.hpp"

#include "units.hpp"

using namespace groho;

// Truth data obtained from
// https://www.fourmilab.ch/documents/calendar/

TEST_CASE("Calendar date to JD", "[UT to JD]")
{
    // Catch::StringMaker<float>::precision = 100;

    REQUIRE(
        double(JulianDate(GregorianDate{ 0, 1, 1, 0.5 }))
        == Approx(1721060.0).margin(0.001));

    REQUIRE(
        JulianDate(GregorianDate{ 763, 9, 18, 0.5 })
        == Approx(2000000.0).margin(0.001));

    REQUIRE(
        JulianDate(GregorianDate{ 2406, 6, 16, (16 - 12 + 48. / 60) / 24.0 })
        == Approx(2600000.2).margin(0.001));

    REQUIRE(double(JulianDate(GregorianDate{ 2650, 1, 25, 0.0 })) == 2688976.5);
}

TEST_CASE("JD to calendar date", "[JD to UT]")
{
    Catch::StringMaker<double>::precision = 20;

    SECTION("763 AD")
    {
        auto gd = JulianDate(2000000.).as_ut();

        REQUIRE(gd.Y == 763);
        REQUIRE(gd.M == 9);
        REQUIRE(gd.D == 18);
        REQUIRE(gd.H == 0.5);
    }

    SECTION("J2000")
    {
        auto gd = JulianDate(2451546.0).as_ut();

        REQUIRE(gd.Y == 2000);
        REQUIRE(gd.M == 1);
        REQUIRE(gd.D == 2);
        REQUIRE(gd.H == 0.5);
    }

    SECTION("2200 AD")
    {
        auto gd = JulianDate(2524653.2).as_ut();

        REQUIRE(gd.Y == 2200);
        REQUIRE(gd.M == 3);
        REQUIRE(gd.D == 1);
        REQUIRE(gd.H == Approx((16 + 48. / 60) / 24.).margin(0.000001));
    }

    SECTION("2650 AD")
    {
        auto gd = JulianDate(2688976.5).as_ut();

        REQUIRE(gd.Y == 2650);
        REQUIRE(gd.M == 1);
        REQUIRE(gd.D == 25);
        REQUIRE(gd.H == 0.0);
    }
}

TEST_CASE("JD to J2000", "[JD to J2000]")
{
    Catch::StringMaker<double>::precision = 20;

    auto j2k = J2000_s(JulianDate(2451545.0));

    REQUIRE(double(j2k) == 0);

    auto gd = j2k.as_ut();
    REQUIRE(gd.Y == 2000);
    REQUIRE(gd.M == 1);
    REQUIRE(gd.D == 1);
    REQUIRE(gd.H == 0.5);
}

TEST_CASE("Parse Gregorian Date from string", "[ParseDate]")
{
    std::string d   = "2050.02.29:0.5";
    auto[date, err] = as_gregorian_date(d, 1);

    REQUIRE(date.Y == 2050);
    REQUIRE(date.H == 0.5);
    REQUIRE(!err.error);
}

TEST_CASE("Parse Gregorian Date from string with error", "[ParseDate]")
{
    std::string d   = "2050.0229.0.5";
    auto[date, err] = as_gregorian_date(d, 1);

    REQUIRE(err.error);
    REQUIRE(err.line == 1);
    REQUIRE(err.message == "Invalid date");
}