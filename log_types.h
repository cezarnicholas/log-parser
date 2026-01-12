#pragma once
#include <cstddef>
#include <span>
#include <string_view>
#include <utility>

using kv_t = std::pair< std::string_view, std::string_view >;
struct log_entry_t
{
    std::string_view timestamp;
    std::string_view level;
    std::string_view service;
    int process_id{ };
    std::string_view message;
    std::span< const kv_t > metadata;
};

struct meta_range_t
{
    size_t start{ };
    size_t end{ };
};