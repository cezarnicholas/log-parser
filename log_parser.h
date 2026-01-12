#pragma once
#include "log_types.h"
#include <vector>

struct parsed_logs_t
{
    std::vector< log_entry_t > entries;
    std::vector< kv_t > metadata_pool;
    std::vector< meta_range_t > metadata_ranges;
};

parsed_logs_t parse_logs( std::string_view text );