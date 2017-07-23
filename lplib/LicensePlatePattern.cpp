/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#include "LicensePlatePattern.h"
#include <sstream>
#include <numeric>
#include <iostream>

namespace licenseplate {

/* Splits given string by specified delimiter */
std::vector<std::string> splitByDelimiter(const std::string & toSplit, char delimiter)
{
    std::vector<std::string> split;
    std::stringstream ss;
    ss.str(toSplit);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        split.push_back(token);
    }
    return split;
}

/* Returns strue if character is a pattern symbol: digit or uppercase letter */
bool isSymbol(char c)
{
    return isdigit(c) || isupper(c);
}

LicensePlatePattern::LicensePlatePattern(const std::string & pattern)
{
    constexpr auto alternativeDelimiter = ',';
    constexpr auto intervalDelimiter = '-';

    const auto alternatives(splitByDelimiter(pattern, alternativeDelimiter)); // could use `boost::split`
    for (const auto & alternative : alternatives)
    {
        if (alternative.size() == 1 && (isSymbol(alternative[0]) || alternative[0] == '?'))
        {// single symbol
            _symbols.push_back(alternative[0]);
        }
        else if (alternative.size() == 3 && alternative[1] == intervalDelimiter && isSymbol(alternative[0]) && isSymbol(alternative[2]))
        {// interval
            for (auto c = alternative[0]; c <= alternative[2]; ++c)
            {
                if (isSymbol(c)) // skip characters between 9 and A
                {
                    _symbols.push_back(c);
                }
            }
        }
        else
        {// invalid pattern
            std::cout << "Warning! Skipping invalid pattern: " << alternative << "\n";
        }
    }

    // Provide symbol costs (the higher cost, the less probability)
    _symbolCosts.resize(_symbols.size());
    std::iota(_symbolCosts.begin(), _symbolCosts.end(), 0.0); // the most probable (the least cost) symbols are in the left part of the pattern
    // Note: could provide any costs here but keep symbols and their costs sorted from the most probable to the least
}

}
