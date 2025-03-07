#pragma once

#include "pdptw_data.h"

#include <string>
#include <spdlog/spdlog.h>

namespace parsing
{
    PDPTWData parseJson(std::string filepath);
}

PDPTWData json_to_data(std::string dataName, const json& j);