#include "log_parser.h"
#include "log_types.h"
#include "utils.h"
#include <charconv>
#include <string_view>

namespace
{
    std::vector< std::string_view > split_file_into_lines( std::string_view text )
    {
        if ( text.empty( ) )
            return { };

        std::vector< std::string_view > lines;
        size_t start = 0;
        for ( size_t i = 0; i < text.size( ); ++i )
        {
            if ( text[ i ] == '\n' )
            {
                std::string_view line = text.substr( start, i - start );
                if ( !line.empty( ) && line.back( ) == '\r' )
                    line.remove_suffix( 1 );

                lines.push_back( line );
                start = i + 1;
            }
        }

        if ( text.back( ) != '\n' )
        {
            std::string_view line = text.substr( start, text.size( ) - start );
            if ( !line.empty( ) && line.back( ) == '\r' )
                line.remove_suffix( 1 );

            lines.push_back( line );
        }

        return lines;
    }

    void tokenize_line( std::string_view line, std::vector< std::string_view > &tokens )
    {
        size_t start = 0;
        for ( size_t i = 0; i < line.size( ); ++i )
        {
            if ( line[ i ] == '|' )
            {
                tokens.push_back( trim( line.substr( start, i - start ) ) );
                start = i + 1;
            }
        }

        tokens.push_back( trim( line.substr( start ) ) );
    }

    void parse_metadata( std::string_view metadata_kv, parsed_logs_t &logs )
    {
        size_t kvs_start = logs.metadata_pool.size( );

        size_t start = 0;
        for ( size_t i = 0; i < metadata_kv.size( ); ++i )
        {
            if ( metadata_kv[ i ] == ',' )
            {
                std::string_view kv = metadata_kv.substr( start, i - start );
                size_t eq = kv.find( '=' );
                if ( eq == std::string_view::npos )
                {
                    std::println( "malformed metadata" );
                    start = i + 1;
                    continue;
                }

                std::string_view key = kv.substr( 0, eq ), value = kv.substr( eq + 1 );
                const kv_t kvp = kv_t{ key, value };
                logs.metadata_pool.push_back( kvp );
                start = i + 1;
            }
        }

        std::string_view kv = metadata_kv.substr( start );
        if ( kv.empty( ) )
        {
            size_t kvs_end = logs.metadata_pool.size( );
            logs.metadata_ranges.push_back( { kvs_start, kvs_end } );
            return;
        }

        size_t eq = kv.find( '=' );
        if ( eq == std::string_view::npos )
        {
            size_t kvs_end = logs.metadata_pool.size( );
            logs.metadata_ranges.push_back( { kvs_start, kvs_end } );
            std::println( "malformed metadata" );
            return;
        }

        std::string_view key = kv.substr( 0, eq ), value = kv.substr( eq + 1 );
        const kv_t kvp = make_pair( key, value );
        logs.metadata_pool.push_back( kvp );

        size_t kvs_end = logs.metadata_pool.size( );
        logs.metadata_ranges.push_back( { kvs_start, kvs_end } );
    }

    log_entry_t tokens_to_log( const std::vector< std::string_view > &tokens, parsed_logs_t &logs )
    {
        if ( tokens.size( ) < 6 )
        {
            const size_t kvs_start = logs.metadata_pool.size( );
            logs.metadata_ranges.push_back( { kvs_start, kvs_start } );
            std::println( "malformed line" );
            return { };
        }

        std::string_view timestamp = tokens[ 0 ];
        std::string_view level = tokens[ 1 ];
        std::string_view service = tokens[ 2 ];
        int process_id;
        {
            // clang-format off
            auto [ ptr, ec ] = std::from_chars( 
                tokens[ 3 ].begin( ), 
                tokens[ 3 ].end( ), 
                process_id );
            // clang-format on
            ensure( ec == std::errc{ }, "parse failed" );
            ensure( ptr == tokens[ 3 ].end( ), "garbage after number" );
        }
        std::string_view message = tokens[ 4 ];
        parse_metadata( tokens[ 5 ], logs );

        // clang-format off
        log_entry_t log_entry = ( log_entry_t ){ 
            timestamp, 
            level, 
            service, 
            process_id, 
            message, 
            {}
        };
        // clang-format on

        return log_entry;
    }
} // namespace

parsed_logs_t parse_logs( std::string_view text )
{
    parsed_logs_t logs;

    std::vector< std::string_view > lines = split_file_into_lines( text );
    logs.entries.resize( lines.size( ) );

    std::vector< std::string_view > tokens;
    tokens.reserve( 6 );

    for ( size_t i = 0; i < lines.size( ); ++i )
    {
        tokens.clear( );
        tokenize_line( lines[ i ], tokens );
        logs.entries[ i ] = tokens_to_log( tokens, logs );
    }

    ensure( logs.metadata_ranges.size( ) == logs.entries.size( ), "ranges desynced" );
    for ( size_t i = 0; i < logs.entries.size( ); ++i )
    {
        log_entry_t &cur_entry = logs.entries[ i ];
        const meta_range_t &range = logs.metadata_ranges[ i ];
        cur_entry.metadata = std::span( logs.metadata_pool ).subspan( range.start, range.end - range.start );
    }

    return logs;
}