#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <ios>
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

TOKEN nextEncodedToken = CHAR_MAX + 1;

bool TryReadTextFromFile(const std::string& inputFilePath, std::string& result);
bool TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<TOKEN>& encodedString, std::map<TOKEN, std::pair<TOKEN, TOKEN>>& tokens);
bool TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath);
std::basic_string<TOKEN> EncodeText(const std::string& input);
std::string DecodeString(const std::basic_string<TOKEN>& input, const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& tokens);
void PrintTokenTable(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);
void PrintTokenTableDecoded(const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);
void DecodeToken(TOKEN token, std::string& decriptedToken, const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& encodedTokens);

int main(int argc, char** argv)
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
            if (TryReadTextFromFile(inputFile, input))
            {
                std::basic_string<TOKEN> encodedString = EncodeText(input);
                TryWriteEncodedTextToFile(encodedString, outputFile);
            }
        }
        else if (strcmp(arg, "-d") == 0)
        {
            assert(a + 2 < argc);
            std::string inputFile = argv[a + 1];
            std::string outputFile = argv[a + 2];
            ++a;

            std::basic_string<TOKEN> encodedInput;
            std::map<TOKEN, std::pair<TOKEN, TOKEN>> tokens;

            if (TryReadEncodedTextFromFile(inputFile, encodedInput, tokens) == false)
            {
                std::cout << "ERROR: Failed to read raw encoded input from file." << std::endl;
                return -1;
            }

            TryWriteEncodedTextToFile(encodedInput, outputFile);

            //encodedStringStream.getline(encodedInput.data(), rawInput.size());

            //DecodeString(encodedInput, tokens);
        }
    }

    return 0;
}

bool TryReadTextFromFile(const std::string& inputFilePath, std::string& result)
{
    std::ifstream file(inputFilePath);

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

bool TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath)
{
    std::ofstream outputFile(outputFilePath, std::ios::binary);

    if (outputFile.is_open() == false)
    {
        std::cout << "ERROR: Unable to open or create output file at path " << outputFilePath << std::endl;
        return false;
    }

    outputFile.write(reinterpret_cast<const char*>(encodedString.data()), encodedString.size() * sizeof(TOKEN));
    outputFile.close();

    return true;
}

bool TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<TOKEN>& encodedString, std::map<TOKEN, std::pair<TOKEN, TOKEN>>& tokens)
{
    auto vat = tokens;

    std::ifstream file(inputFilePath, std::ios::binary);

    if (file.is_open() == false)
    {
        std::cout << "ERROR: Unable to open file at path " << inputFilePath << std::endl;
        return false;
    }

    file.seekg(0, file.end);
    std::streamsize fileLength = file.tellg();
    file.seekg(0, file.beg);

    if (fileLength % (sizeof(TOKEN) / sizeof(char)))
    {
        std::cout << "ERROR: Bad UTF-16 format (odd number of bytes)" << std::endl;
    }

    TOKEN token;
    while (file.read(reinterpret_cast<char*>(&token), sizeof(TOKEN)) && token > 0)
    {
        encodedString.push_back(token);
    }

    file.close();

    return true;
}

std::basic_string<TOKEN> EncodeText(const std::string& input)
{
    std::basic_string<TOKEN> result;
    result.reserve(input.size());

    for (const char& i : input)
    {
        result.push_back(i);
    }

    std::basic_string<TOKEN> encodedStringCopy;
    encodedStringCopy.reserve(input.size());

    std::unordered_map<std::pair<TOKEN, TOKEN>, int, pairHash> pairCounts;
    std::map<TOKEN, std::pair<TOKEN, TOKEN>> encodedTokens;

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
        encodedStringCopy = result;
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

    return result;
}

std::string DecodeString(const std::basic_string<TOKEN>& input, const std::map<TOKEN, std::pair<TOKEN, TOKEN>>& tokens)
{
    std::string result;
    result.reserve(input.size() * 2);
    auto a = tokens;

    //for (const TOKEN& token : input)
    {
        //DecodeToken(token);
    }

    return result;
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
