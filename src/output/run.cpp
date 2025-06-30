#include "run.h"

#include "config.h"
#include "input/load_dependent.h"
#include "input/json_parser.h"
#include "lns/lns.h"

namespace fs = std::filesystem;

std::vector<std::string> getFilesInDirectory(std::string const &directoryPath)
{
    std::vector<std::string> filePaths;
    try
    {
        for (auto const &entry: fs::directory_iterator(directoryPath))
        {
            if (entry.is_regular_file())
            {
                filePaths.push_back(entry.path().string());
            }
        }
    } catch (std::filesystem::filesystem_error const &e)
    {
        std::cerr << "Error : " << e.what() << std::endl;
    }
    return filePaths;
}

void runAllInDirectory(std::string const &directoryPath, std::function<void(PDPTWData &, Solution &)> function)
{
    std::vector<std::string> files = getFilesInDirectory(directoryPath);

    for (std::string file: files)
    {
        PDPTWData data = parsing::parseJson(file);
        if (ELEVATION)
        {
            ltt::preCalculation(data);
        }
        Solution startingSolution = Solution::emptySolution(data);
        function(data, startingSolution);
    }
}
