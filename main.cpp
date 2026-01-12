#include "log_parser.h"
#include "mapped_file.h"
#include "utils.h"
#include <cerrno>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{
    ensure( argc >= 2, "error! usage: {} log_file.log", argv[ 0 ] );

    c_mapped_file mapped_file( argv[ 1 ] );

    parsed_logs_t logs = parse_logs( mapped_file.contents( ) );

    // do whatever
    for ( log_entry_t &log_entry : logs.entries )
    {
        // clang-format off
        std::println( 
            "{} | {} | {} | {} | {} | {}", 
            log_entry.timestamp, 
            log_entry.level, 
            log_entry.service, 
            log_entry.process_id, 
            log_entry.message,
            log_entry.metadata 
        );
        // clang-format on
    }
}