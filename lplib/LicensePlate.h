/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#ifndef __LICENSEPLATE_H__
#define __LICENSEPLATE_H__

#ifdef LPLIB_EXPORTS
#define LPLIB_API __declspec(dllexport)
#else
#define LPLIB_API __declspec(dllimport)
#endif


#include <string>
#include <vector>
#include <numeric>


namespace licenseplate {

class LPLIB_API LicensePlate
{
    std::vector<char> _symbols;
    std::vector<double> _symbolCosts;
    std::vector<unsigned int> _symbolIndices;
    double _cost;
    unsigned int _indexDistance;

public:
    LicensePlate(const std::vector<char> & symbols, const std::vector<double> & symbol_costs, const std::vector<unsigned int> & symbol_indices)
        : _symbols(symbols), _symbolCosts(symbol_costs), _symbolIndices(symbol_indices),
            _cost(std::accumulate(_symbolCosts.begin(), _symbolCosts.end(), 0.0)),
            _indexDistance(std::accumulate(_symbolIndices.begin(), _symbolIndices.end(), 0)) {}

    std::string str() const { return std::string(_symbols.data(), _symbols.size()); }

    std::vector<char> symbols() const { return _symbols; }
    std::vector<double> symbolCosts() const { return _symbolCosts; }
    std::vector<unsigned int> symbolIndices() const { return _symbolIndices; }
    double cost() const { return _cost; }
    unsigned int indexDistance() const { return _indexDistance; }

    // Implement only < operator and us it in other relation operators
    friend bool operator< (const LicensePlate & lhs, const LicensePlate & rhs) { return lhs.cost() < rhs.cost(); }
    friend bool operator> (const LicensePlate & lhs, const LicensePlate & rhs) { return rhs < lhs; }
    friend bool operator<= (const LicensePlate & lhs, const LicensePlate & rhs) { return !(rhs < lhs); }
    friend bool operator>= (const LicensePlate & lhs, const LicensePlate & rhs) { return !(lhs < rhs); }
};

}

#endif // __LICENSEPLATE_H__