#pragma once

#include "pdptw_data.h"

#include <string>
#include <spdlog/spdlog.h>

namespace parsing
{
    PDPTWData parseJson(std::string const &filepath);
}

PDPTWData json_to_data(std::string const &dataName, json const &j);
PDPTWData json_to_data_with_elevation(std::string const &dataName, json const &j);