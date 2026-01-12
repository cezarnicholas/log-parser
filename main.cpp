#include "log_parser.h"
#include "utils.h"
#include <cerrno>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void *mmap_file( std::filesystem::path file_name, size_t &length )
{
    int fd = open( file_name.c_str( ), O_RDONLY );
    ensure( fd != -1, "file didn't open properly" );

    struct stat st;
    ensure( fstat( fd, &st ) != -1 && st.st_size != 0, "fstat failed / empty file" );

    void *ptr = mmap( nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
    ensure( ptr != MAP_FAILED, "memory mapping failed" );
    length = st.st_size;

    close( fd );
    return ptr;
}

int main( int argc, char *argv[] )
{
    ensure( argc >= 2, "error! usage: {} log_file.log", argv[ 0 ] );
    size_t length;
    void *file = mmap_file( argv[ 1 ], length );

    std::string_view text = { static_cast< const char * >( file ), length };

    parsed_logs_t logs = parse_logs( text );

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
            log_entry.metadata );
        // clang-format on
    }

    munmap( file, length );
}