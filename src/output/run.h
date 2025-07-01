#pragma once

#include "input/pdptw_data.h"
#include "lns/solution/solution.h"
#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>


std::vector<std::string> getFilesInDirectory(const std::string& directoryPath);

/**
 * Used to run multiple instances located in the same directory.
 */
void runAllInDirectory(const std::string& directoryPath, std::function<void(PDPTWData &, Solution &)> function);