/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#ifndef __LICENSEPLATEGENERATOR_H__
#define __LICENSEPLATEGENERATOR_H__

#ifdef LPLIB_EXPORTS
    #define LPLIB_API __declspec(dllexport)
#else
    #define LPLIB_API __declspec(dllimport)
#endif


#include "LicensePlate.h"
#include "LicensePlatePattern.h"
#include <vector>
#include <functional>
#include <queue>
#include <set>
#include <map>


namespace licenseplate {

class LPLIB_API LicensePlateGenerator
{
    /* A cache mechanism prevents generating the same plates again; 
     * also when requested more plates, only the new will be generated
     */
    struct GeneratorCache
    {
        // Container for picked best plates, sorted from the most probable to the least
        std::vector<LicensePlate> orderedBestPlates;

        // Priority queue enables quick access to the most probable plate
        // Using std::greater<T> will cause the smallest cost (the greatest probability) plate to appear as the top()
        std::priority_queue<LicensePlate, std::vector<LicensePlate>, std::greater<LicensePlate>> plateQueue;

        // Container for already queued combinations kept to avoid duplicates
        // key: level of searching new plates
        // value: set of used pattern symbol indices vectors 
        std::map<unsigned int, std::set<std::vector<unsigned int>>> alreadyQueued;
    } _cache;

    const std::vector<LicensePlatePattern> _licensePlatePatterns;
    const unsigned long long _countLicensePlates;

public:
    LicensePlateGenerator::LicensePlateGenerator(const std::vector<LicensePlatePattern>& licensePlatePatterns);

    /* Returns a list of the LicensePlates given by the cartesian product of the patterns */
    std::vector<LicensePlate> GenerateAllLicensePlates(int maxResults);

    /* Invokes callbackproc for every LicensePlate given by the cartesian product of the patterns */
    void ProcessAllLicensePlates(void* data, std::function<void(void*, const LicensePlate &)> callbackproc, int maxResults);

    /* Invokes callbackscorefun for every LicensePlate objects given by the cartesian product of the patterns, and returns the B "best" license plates with the same lowest score */
    std::vector<LicensePlate> FindBestLicensePlates(void* data, std::function<double(void*, const LicensePlate &)> callbackscorefun, int maxResults);
    
    /* Returns number of all possible LicensePlates given by the cartesian product of the patterns */
    unsigned long long CountLicensePlates() const { return _countLicensePlates; }
};

}

#endif // __LICENSEPLATEGENERATOR_H__