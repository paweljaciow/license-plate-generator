/**
* This file is part of the License Plate Generator project
* modified:    2017-07-23
* author:      Pawe³ Jaciów
* e-mail:      pawel.jaciow@gmail.com
*/

#include "LicensePlateGenerator.h"
//#include <iostream>
//#include <ctime>

namespace licenseplate {

LicensePlateGenerator::LicensePlateGenerator(const std::vector<LicensePlatePattern>& licensePlatePatterns)
    : _licensePlatePatterns(licensePlatePatterns), _countLicensePlates(_licensePlatePatterns.size() == 0 ? 0 : [&]() {
        unsigned long long count = 1;
        for (const auto & pattern : _licensePlatePatterns)
        {
            count *= pattern.symbols().size();
        }
        return count;
    }()) {};

std::vector<LicensePlate> LicensePlateGenerator::GenerateAllLicensePlates(int maxResults)
{
    //clock_t time_start = clock();

    const auto nPatterns = _licensePlatePatterns.size();

    if (maxResults == -1 || maxResults > _countLicensePlates)
    {
        maxResults = _countLicensePlates;
    }

    if (maxResults > _cache.orderedBestPlates.size())
    {
        if (_cache.orderedBestPlates.empty())
        {
            _cache.plateQueue.push([&]() {// create best plate and push it to the queue
                std::vector<char> symbols;
                symbols.reserve(nPatterns);
                std::vector<double> symbolCosts;
                symbolCosts.reserve(nPatterns);
                // Assuming here that symbols and their costs are already sorted from the most probable to the least
                // i.e. the most probable symbols are at indices 0
                std::vector<unsigned int> symbolIndices(nPatterns, 0);
                for (auto i = 0; i < nPatterns; ++i)
                {
                    symbols.push_back(_licensePlatePatterns[i].symbols()[0]);
                    symbolCosts.push_back(_licensePlatePatterns[i].symbolCosts()[0]);
                }
                return LicensePlate{ symbols, symbolCosts, symbolIndices };
            }());
        }

        _cache.orderedBestPlates.reserve(maxResults);

        while (_cache.orderedBestPlates.size() < maxResults)
        {
            // Move first plate from the queue to best plates
            const auto first = _cache.plateQueue.top();
            _cache.plateQueue.pop();
            _cache.orderedBestPlates.push_back(first);
            _cache.alreadyQueued.insert(std::make_pair(first.indexDistance(), std::set<std::vector<unsigned int>>()));// add new key if does not exist

            // Generate all possible plates different by 1 index change from the first
            for (auto i = 0; i < nPatterns; ++i)
            {
                const auto pattern = _licensePlatePatterns.begin() + i;
                if (first.symbolIndices()[i] + 1 < pattern->symbols().size())
                {// if there are any symbols left in current pattern
                    auto newIndices = first.symbolIndices();
                    ++newIndices[i];
                    if (_cache.alreadyQueued[first.indexDistance()].insert(newIndices).second)
                    {// plate has not been queued yet so queue it
                        auto newSymbols = first.symbols();
                        newSymbols[i] = pattern->symbols()[newIndices[i]];
                        auto newCosts = first.symbolCosts();
                        newCosts[i] = pattern->symbolCosts()[newIndices[i]];
                        _cache.plateQueue.push(LicensePlate{ newSymbols, newCosts, newIndices });
                    }
                }
            }
        }
        //clock_t time_stop = clock();
        //std::cout << "elapsed time: " << double(time_stop - time_start) / CLOCKS_PER_SEC << "\n";
        return _cache.orderedBestPlates;
    }
    else
    {
        return std::vector<LicensePlate>(_cache.orderedBestPlates.begin(), _cache.orderedBestPlates.begin() + maxResults);
    }
}

void LicensePlateGenerator::ProcessAllLicensePlates(void * data, std::function<void(void*, const LicensePlate &)> callbackproc, int maxResults)
{
    const auto plates = GenerateAllLicensePlates(maxResults);

    for (const auto & p : plates)
    {
        callbackproc(data, p);
    }
}

std::vector<LicensePlate> LicensePlateGenerator::FindBestLicensePlates(void * data, std::function<double(void*, const LicensePlate &)> callbackscorefun, int maxResults)
{
    const auto plates = GenerateAllLicensePlates(maxResults);

    auto bestScore = DBL_MAX;
    std::vector<LicensePlate> bestPlates;
    for (const auto & p : plates)
    {
        const auto score = callbackscorefun(data, p);
        if (score <= bestScore)
        {
            if (score < bestScore)
            {
                bestScore = score;
                bestPlates.clear();
            }
            bestPlates.push_back(p);
        }
    }

    return bestPlates;
}

}
