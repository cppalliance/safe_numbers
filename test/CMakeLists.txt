# Copyright 2018, 2019 Peter Dimov
# Copyright 2023 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTestJamfile OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(HAVE_BOOST_TEST)

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64|i386|i686|x86" AND NOT MINGW)
        boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::safe_numbers Boost::charconv Boost::core Boost::random Boost::safe_numerics Boost::multiprecision Boost::exception Boost::stacktrace_from_exception)
    else()
        boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::safe_numbers Boost::charconv Boost::core Boost::random Boost::safe_numerics Boost::multiprecision Boost::exception)
    endif()

endif()
