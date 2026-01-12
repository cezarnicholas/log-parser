#include "mapped_file.h"
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

c_mapped_file::c_mapped_file( std::filesystem::path file_name )
{
    int fd = open( file_name.c_str( ), O_RDONLY );
    ensure( fd != -1, "file didn't open properly" );

    struct stat st;
    ensure( fstat( fd, &st ) != -1 && st.st_size != 0, "fstat failed / empty file" );

    ptr_ = mmap( nullptr, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
    ensure( ptr_ != MAP_FAILED, "memory mapping failed" );
    size_ = st.st_size;
    close( fd );

    text_ = { static_cast< const char * >( ptr_ ), size_ };
}

c_mapped_file::~c_mapped_file( )
{
    if ( ptr_ && size_ )
        munmap( ptr_, size_ );
}

// clang-format off
c_mapped_file::c_mapped_file( c_mapped_file &&other ) noexcept : 
    ptr_( other.ptr_ ), size_( other.size_ ), text_( other.text_ ) 
{
    other.ptr_ = nullptr;
    other.size_ = 0;
    other.text_ = {};
}
// clang-format on

c_mapped_file &c_mapped_file::operator=( c_mapped_file &&other ) noexcept
{
    if ( this != &other )
    {
        munmap( ptr_, size_ );

        ptr_ = other.ptr_;
        size_ = other.size_;
        text_ = other.text_;

        other.ptr_ = nullptr;
        other.size_ = 0;
        other.text_ = { };
    }

    return *this;
}