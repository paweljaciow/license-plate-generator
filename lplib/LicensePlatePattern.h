/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#ifndef __LICENSEPLATEPATTERN_H__
#define __LICENSEPLATEPATTERN_H__

#ifdef LPLIB_EXPORTS
#define LPLIB_API __declspec(dllexport)
#else
#define LPLIB_API __declspec(dllimport)
#endif


#include <string>
#include <vector>


namespace licenseplate {

class LPLIB_API LicensePlatePattern
{
    std::vector<char> _symbols;         // Pattern symbols sorted from the most probable to the least
    std::vector<double> _symbolCosts;   // Costs of respective pattern symbols

public:
    LicensePlatePattern(const std::string & pattern);
    ~LicensePlatePattern() {};

    std::vector<char> symbols() const { return _symbols; }
    std::vector<double> symbolCosts() const { return _symbolCosts; }
};

}

#endif // __LICENSEPLATEPATTERN_H__