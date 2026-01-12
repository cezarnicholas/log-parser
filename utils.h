#pragma once
#include <format>
#include <print>
#include <string_view>

template < class... Args >
inline void ensure( const bool condition, std::format_string< Args... > fmt, Args &&...args )
{
    if ( !condition )
    {
        std::println( fmt, std::forward< Args >( args )... );
        std::fflush( stdout );
        std::exit( 1 );
    }
}

static inline std::string_view trim( std::string_view s )
{
    const size_t ffno = s.find_first_not_of( " \t" );
    if ( ffno == std::string_view::npos )
        return { };

    const size_t flno = s.find_last_not_of( " \t" );
    return s.substr( ffno, flno - ffno + 1 );
}