/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#include "LicensePlateGenerator.h"
#include <iostream>
#include <string>


/* A structure for basic configuration */
struct Config
{
    static constexpr auto generateAllLicensePlatesAction = "A";
    static constexpr auto processAllLicensePlatesAction = "B";
    static constexpr auto findBestLicensePlatesAction = "C";

    bool valid = false;
    int maxResults = -1;
    std::string action;
    std::vector<std::string> patterns;

    Config(bool valid_, int maxResults_, const std::string & action_, const std::vector<std::string> & patterns_) :
        valid(valid_), maxResults(maxResults_), action(action_), patterns(patterns_) {}
};

/* Returns true if val matches any of allowed elements */
template <typename T>
bool isAnyOf(const T & val , const std::vector<T> & allowed)
{
    return std::find(allowed.begin(), allowed.end(), val) != allowed.end();
}

/* Parses command line arguments and returns them as a Config object */
Config parseArgs(char** begin, char** end)
{
    const std::string actionOption("--action=");			// mandatory
    const std::string maxResultsOption("--maxResults=");	// optional

    bool valid = true;
    int maxResults = -1;
    std::string action;
    std::vector<std::string> patterns;

    for (auto it = begin; it != end; ++it)
    {
        const std::string arg(*it);

        // look for action
        if (!arg.compare(0, actionOption.size(), actionOption))
        {
            action = arg.substr(actionOption.size());
            continue;
        }

        // look for maxResults
        if (!arg.compare(0, maxResultsOption.size(), maxResultsOption))
        {
            try
            {
                const auto val = std::stoi(arg.substr(maxResultsOption.size()).c_str());
                if (val > 0)
                {// accept maxResults only if it is positive
                    maxResults = val;
                }
            }
            catch (const std::exception & e)
            {
                std::cerr << "Invalid argument for: " << maxResultsOption << "\n" << e.what();
            }
            continue;
        }

        // if none of the above, assume it is a pattern; would consider reading patterns from a file, one pattern per line
        patterns.push_back(arg);
    }

    // check mandatory parameter
    if (action.empty() || !isAnyOf(action, {
            Config::generateAllLicensePlatesAction, 
            Config::processAllLicensePlatesAction,
            Config::findBestLicensePlatesAction}))
    {
        valid = false;
        std::cerr << "Mandatory parameter invalid: " << actionOption << "\n";
    }

    return { valid, maxResults, action, patterns };
}

int main(int argc, char* argv[])
{
    // Simple argument parsing; would consider `boost::program_options`.
    const Config config = parseArgs(argv + 1, argv + argc);
    if (!config.valid)
    {
        std::cerr << "Bad usage!\nUsage:   " << argv[0] << " \"--action={""A|B|C}\" [\"--maxResults=N\"] \"pattern1\" \"pattern2\" ... \"patternM\"\n";
        return 1;
    }

    using namespace licenseplate;

    // Generate LicensePlatePattern objects from patterns read from command line
    const auto licensePlatePatterns = [&]()
    {
        std::vector<LicensePlatePattern> patternsVec;
        for (const auto & p : config.patterns )
        {
            const LicensePlatePattern pattern(p);
            if (!pattern.symbols().empty())
            {
                patternsVec.push_back(pattern);
            }
        }
        return patternsVec;
    }();

    // Create LicensePlateGenerator with these patterns
    LicensePlateGenerator licensePlateGenerator(licensePlatePatterns);

    // Perform action respective to command line request
    if (config.action == Config::generateAllLicensePlatesAction)
    {// invoke GenerateAllLicensePlates() and print all the LicensePlates on standard output, one per line
        const auto licensePlates = licensePlateGenerator.GenerateAllLicensePlates(config.maxResults);
        for (const auto & lp : licensePlates)
        {
            std::cout << lp.str() << "\n";
        }
    }
    else if (config.action == Config::processAllLicensePlatesAction)
    {// invoke ProcessAllLicensePlates() using a callback that takes argument licensePlate and prints it on stdout enclosed in square brackets
        licensePlateGenerator.ProcessAllLicensePlates(nullptr, [](void* data, const LicensePlate & licensePlate) {
            std::cout << "[" << licensePlate.str() << "]\n";
        }, config.maxResults);
    }
    else if (config.action == Config::findBestLicensePlatesAction)
    {// invoke FindBestLicensePlates() using a score function that "sums" all the symbols of the license plate : normal sum if it's a digit, 1-26 if it's a letter
        const auto bestPlates = licensePlateGenerator.FindBestLicensePlates(nullptr, [](void* data, const licenseplate::LicensePlate & licensePlate) {
            constexpr auto refAlpha = 'A' + 1; // 'A' is 1
            constexpr auto refDigit = '0';		// '0' is 0
            double score = 0;
            for (auto c : licensePlate.str())
            {
                if (isdigit(c))
                {
                    score += c - refDigit;
                }
                else if (isupper(c))
                {
                    score += c - refAlpha;
                }
            }
            return score;
        }, config.maxResults);
        // print the LicensePlates on standard output, one per line
        for (const auto & bp : bestPlates)
        {
            std::cout << bp.str() << "\n";
        }
    }
    else
    {
        std::cerr << "Unrecognized action: " << config.action << "\n";
        return 1;
    }

    return 0;
}
