#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>

struct pairHash
{
    template <class T1, class T2> std::size_t operator()(const std::pair<T1, T2>& p) const
    {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

typedef char16_t TOKEN;

std::unordered_map<std::pair<TOKEN, TOKEN>, int, pairHash> pairCounts;
TOKEN nextEncodedToken = CHAR_MAX + 1;

bool TryReadTextFromFile(const std::string& inputFilePath, std::string& result);
bool TryWriteEncodedTextToFile(std::basic_string<TOKEN>& encodedString, std::string& outputFilePath);
void EncodeText(const std::string& input, std::basic_string<TOKEN>& result);
void PrintTokenTable(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);
void PrintTokenTableDecoded(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);
void DecodeToken(TOKEN token, std::string& decriptedToken, const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);

int main(int argc, char* argv[])
{
    for (int a = 0; a < argc; ++a)
    {
        char* arg = argv[a];

        if (strcmp(arg, "-e") == 0)
        {
            assert(a + 2 < argc);
            std::string inputFile = argv[a + 1];
            std::string outputFile = argv[a + 2];
            ++a;

            std::string input;
            std::basic_string<TOKEN> encodedString;
            if (TryReadTextFromFile(inputFile, input))
            {
                EncodeText(input, encodedString);
                TryWriteEncodedTextToFile(encodedString, outputFile);
            }
        }
    }

    return 0;
}

bool TryReadTextFromFile(const std::string& inputFilePath, std::string& result)
{
    std::ifstream file = std::ifstream(inputFilePath);

    if (file.is_open() == false)
    {
        std::cout << "ERROR: Unable to open file at path " << inputFilePath << std::endl;
        return false;
    }

    std::ostringstream stringStream;
    stringStream << file.rdbuf();
    file.close();

    result = stringStream.str();

    return true;
}

bool TryWriteEncodedTextToFile(std::basic_string<TOKEN>& encodedString, std::string& outputFilePath)
{
    std::ofstream outputFile = std::ofstream(outputFilePath, std::ios::binary);

    if (outputFile.is_open() == false)
    {
        std::cout << "ERROR: Unable to open or create output file at path " << outputFilePath << std::endl;
        return false;
    }

    outputFile.write(reinterpret_cast<char*>(encodedString.data()), encodedString.size() * sizeof(TOKEN));
    outputFile.close();

    return true;
}

void EncodeText(const std::string& input, std::basic_string<TOKEN>& result)
{
    std::map<TOKEN, std::pair<TOKEN, TOKEN>> encodedTokens;

    for (size_t i = 0; i < input.size(); ++i)
    {
        result.push_back(input[i]);
    }

    while (true)
    {
        pairCounts.clear();

        for (size_t i = 0; i < result.size() - 1; ++i)
        {
            std::pair<TOKEN, TOKEN> pair{result[i], result[i + 1]};
            pairCounts[pair] += 1;
        }

        std::pair<TOKEN, TOKEN> mostFrequentPair;
        int mostFrequentCount{0};

        for (std::pair<std::pair<TOKEN, TOKEN>, int> kvp : pairCounts)
        {
            if (kvp.second > mostFrequentCount)
            {
                mostFrequentPair = kvp.first;
                mostFrequentCount = kvp.second;
            }
        }

        // std::cout << mostFrequentPair.first << ' ' << mostFrequentPair.second <<
        // " = " << mostFrequentCount << std::endl;

        if (mostFrequentCount <= 1)
        {
            break;
        }

        size_t encodedStringSize = result.size();
        TOKEN encodedStringCopy[encodedStringSize];
        std::copy(result.begin(), result.end(), encodedStringCopy);
        result.clear();

        for (size_t i = 0; i < encodedStringSize; ++i)
        {
            if (i == encodedStringSize - 1)
            {
                result.push_back(encodedStringCopy[i]);
                continue;
            }

            std::pair<TOKEN, TOKEN> pair{encodedStringCopy[i], encodedStringCopy[i + 1]};

            if (pair == mostFrequentPair)
            {
                result.push_back(nextEncodedToken);
                ++i;
            }
            else
            {
                result.push_back(encodedStringCopy[i]);
            }
        }

        encodedTokens[nextEncodedToken] = mostFrequentPair;

        ++nextEncodedToken;
    }

    //return;

    result.push_back(0);

    for (const std::pair<TOKEN, std::pair<TOKEN, TOKEN>> kvp : encodedTokens)
    {
        result.push_back(kvp.first);
        result.push_back(kvp.second.first);
        result.push_back(kvp.second.second);
    }
}

void PrintTokenTable(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens)
{
    for (const auto& kvp : encodedTokens)
    {
        if (kvp.first < CHAR_MAX)
        {
            std::cout << (char)kvp.first;
        }
        else
        {
            std::cout << kvp.first;
        }

        std::cout << " = ";

        if (kvp.second.first < CHAR_MAX)
        {
            std::cout << (char)kvp.second.first;
        }
        else
        {
            std::cout << kvp.second.first;
        }

        std::cout << ' ';

        if (kvp.second.second < CHAR_MAX)
        {
            std::cout << (char)kvp.second.second;
        }
        else
        {
            std::cout << kvp.second.second;
        }

        std::cout << std::endl;
    }
}

void PrintTokenTableDecoded(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens)
{
    for (const auto& kvp : encodedTokens)
    {
        std::string decriptedToken;
        DecodeToken(kvp.first, decriptedToken, encodedTokens);
        std::cout << decriptedToken << std::endl;
    }
}

void DecodeToken(TOKEN token, std::string& decriptedToken, const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens)
{
    assert(token > CHAR_MAX);

    std::pair<TOKEN, TOKEN> pair = encodedTokens.at(token);

    if (pair.first > CHAR_MAX)
    {
        DecodeToken(pair.first, decriptedToken, encodedTokens);
    }
    else
    {
        decriptedToken.push_back(pair.first);
    }

    if (pair.second > CHAR_MAX)
    {
        DecodeToken(pair.second, decriptedToken, encodedTokens);
    }
    else
    {
        decriptedToken.push_back(pair.second);
    }
}
