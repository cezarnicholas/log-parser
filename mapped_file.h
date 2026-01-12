#pragma once
#include <filesystem>
#include <string_view>

class c_mapped_file
{
private:
    void *ptr_;
    size_t size_;
    std::string_view text;

public:
    inline std::string_view contents( ) const noexcept
    {
        return text;
    }
    c_mapped_file( std::filesystem::path file_name );
    ~c_mapped_file( );

    c_mapped_file( const c_mapped_file & ) = delete;
    c_mapped_file &operator=( const c_mapped_file & ) = delete;

    c_mapped_file( c_mapped_file && ) noexcept;
    c_mapped_file &operator=( c_mapped_file && ) noexcept;
};