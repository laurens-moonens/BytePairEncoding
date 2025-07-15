#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

struct pairHash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;  
    }
};

//std::string input = "aaabdaaabac";
std::string input = "In the above example, the output of the BPE is a vocabulary, which can be used to encode any text that is written with the letters abcd. It will not be able to encode text containing other symbols, such as no. Even giving each of the 26 letters an entry in the vocabulary, since there are many languages in the world using many different scripts, inevitably some symbols would be unencodable by such a vocabulary.";

std::unordered_map<std::pair<uint64_t, uint64_t>, int, pairHash> pairCounts;
std::unordered_map<uint64_t, std::pair<uint64_t, uint64_t>> encodedTokens;
std::vector<uint64_t> encodedString;
uint64_t nextEncodedToken = CHAR_MAX + 1;

int main()
{
    for (int i = 0; i < input.size(); ++i)
    {
        encodedString.push_back(input[i]);
    }

    while(true)
    {
        pairCounts.clear();

        for (int i = 0; i < encodedString.size() - 1; ++i)
        {
            std::pair<uint64_t, uint64_t> pair {encodedString[i], encodedString[i + 1]};
            pairCounts[pair] += 1;
            std::cout << encodedString[i] << ' ';
        }
        std::cout << encodedString[encodedString.size() - 1] << std::endl;

        std::pair<uint64_t, uint64_t> mostFrequentPair;
        int mostFrequentCount {0};

        for (std::pair<std::pair<uint64_t, uint64_t>, int> kvp : pairCounts)
        {
            if (kvp.second > mostFrequentCount) 
            {
                mostFrequentPair = kvp.first;
                mostFrequentCount = kvp.second;
            }
        }

        std::cout << mostFrequentPair.first << ' ' << mostFrequentPair.second << " = " << mostFrequentCount << std::endl;

        if(mostFrequentCount <= 1)
        {
            break;
        }

        size_t encodedStringSize = encodedString.size();
        uint64_t encodedStringCopy[encodedStringSize];
        std::copy(encodedString.begin(), encodedString.end(), encodedStringCopy);
        encodedString.clear();

        for (int i = 0; i < encodedStringSize; ++i)
        {
            if(i == encodedStringSize - 1)
            {
                encodedString.push_back(encodedStringCopy[i]);
                continue;
            }

            std::pair<uint64_t, uint64_t> pair {encodedStringCopy[i], encodedStringCopy[i + 1]};

            if(pair == mostFrequentPair)
            {
                encodedString.push_back(nextEncodedToken);
                ++i;
            }
            else
            {
                encodedString.push_back(encodedStringCopy[i]);
            }
        }

        encodedTokens[nextEncodedToken] = mostFrequentPair;

        ++nextEncodedToken;
    }

    for (const auto& kvp : encodedTokens)
    {
        std::cout << kvp.first << " = " << kvp.second.first  << ' ' << kvp.second.second << std::endl;
    }

    return 0;
}
