#include "BPE.h"

#include <cassert>
#include <expected>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>

template std::expected<void, std::string> BPE::TryWriteBasicStringToFile<BPE::TOKEN>(const std::basic_string<BPE::TOKEN>& textToWrite, const std::filesystem::path& outputFilePath);
template <typename charType>
std::expected<void, std::string> BPE::TryWriteBasicStringToFile(const std::basic_string<charType>& dataToWrite, const std::filesystem::path& outputFilePath)
{
    std::ofstream outputFile{outputFilePath, std::ios::binary};

    if (outputFile.is_open() == false)
    {
        return std::unexpected(std::format("ERROR: Unable to open or create output file at path \"{}\"", outputFilePath.c_str()));
    }

    outputFile.write(reinterpret_cast<const char*>(dataToWrite.data()), dataToWrite.size() * sizeof(charType));
    outputFile.close();

    return {};
}

std::expected<std::string, std::string> BPE::TryReadTextFromFile(const std::filesystem::path& inputFilePath)
{
    std::ifstream file{inputFilePath};

    if (file.is_open() == false)
    {
        return std::unexpected{std::format("ERROR: Unable to open file at path \"{}\"", inputFilePath.c_str())};
    }

    uintmax_t fileSize{std::filesystem::file_size(inputFilePath)};
    std::string content(fileSize, '\0');
    file.read(&content[0], fileSize);

    return content;
}

std::expected<void, std::string> BPE::TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath)
{
    std::ofstream outputFile{outputFilePath, std::ios::binary};

    if (outputFile.is_open() == false)
    {
        return std::unexpected{std::format("ERROR: Unable to open or create output file at path \"{}\"", outputFilePath)};
    }

    outputFile.write(reinterpret_cast<const char*>(encodedString.data()), encodedString.size() * sizeof(BPE::TOKEN));
    outputFile.close();

    return {};
}

bool BPE::TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<BPE::TOKEN>& encodedString, std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& tokens)
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

    if (fileLength % (sizeof(BPE::TOKEN) / sizeof(char)) != 0)
    {
        std::cout << "ERROR: Bad UTF-16 format (odd number of bytes)" << std::endl;
    }

    BPE::TOKEN token;
    while (file.read(reinterpret_cast<char*>(&token), sizeof(BPE::TOKEN)) && token > 0)
    {
        encodedString.push_back(token);
    }

    std::pair<BPE::TOKEN, BPE::TOKEN> tokenPair;
    while (file.read(reinterpret_cast<char*>(&tokenPair.first), sizeof(BPE::TOKEN)) && file.read(reinterpret_cast<char*>(&tokenPair.second), sizeof(BPE::TOKEN)))
    {
        tokens.push_back(tokenPair);
    }

    file.close();

    return true;
}

std::tuple<std::basic_string<BPE::TOKEN>, std::basic_string<BPE::TOKEN>> BPE::EncodeText(const std::string& input)
{
    std::basic_string<BPE::TOKEN> encodedString{};
    encodedString.reserve(input.size());

    for (const char& i : input)
    {
        encodedString.push_back(i);
    }

    std::basic_string<BPE::TOKEN> encodedStringCopy{};
    encodedStringCopy.reserve(input.size());

    std::unordered_map<std::pair<BPE::TOKEN, BPE::TOKEN>, int, BPE::PairHash> pairCounts;
    std::basic_string<BPE::TOKEN> bpeTable;

    BPE::TOKEN nextEncodedToken{FIRST_TOKEN};

    while (true)
    {
        pairCounts.clear();

        for (size_t i = 0; i < encodedString.size() - 1; ++i)
        {
            std::pair<BPE::TOKEN, BPE::TOKEN> pair{encodedString[i], encodedString[i + 1]};
            pairCounts[pair] += 1;
        }

        std::pair<BPE::TOKEN, BPE::TOKEN> mostFrequentPair{};
        int mostFrequentCount{0};

        for (std::pair<std::pair<BPE::TOKEN, BPE::TOKEN>, int> kvp : pairCounts)
        {
            if (kvp.second > mostFrequentCount)
            {
                mostFrequentPair = kvp.first;
                mostFrequentCount = kvp.second;
            }
        }

        if (mostFrequentCount <= 1)
        {
            break;
        }

        size_t encodedStringSize{encodedString.size()};
        encodedStringCopy = encodedString;
        encodedString.clear();

        for (size_t i = 0; i < encodedStringCopy.size(); ++i)
        {
            if (i == encodedStringSize - 1)
            {
                encodedString.push_back(encodedStringCopy[i]);
                continue;
            }

            std::pair<BPE::TOKEN, BPE::TOKEN> pair{encodedStringCopy[i], encodedStringCopy[i + 1]};

            if (pair == mostFrequentPair)
            {
                encodedString.push_back(nextEncodedToken);
                ++i;
            }
            else
            {
                encodedString.push_back(encodedStringCopy[i]);
            }
        }

        assert((int)(bpeTable.size()) == (nextEncodedToken - FIRST_TOKEN) * 2);
        bpeTable.push_back(mostFrequentPair.first);
        bpeTable.push_back(mostFrequentPair.second);

        ++nextEncodedToken;
    }

    return {bpeTable, encodedString};
}

std::string BPE::DecodeString(const std::basic_string<BPE::TOKEN>& input, const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& tokens)
{
    std::string result;
    result.reserve(input.size() * 2);

    for (const BPE::TOKEN& token : input)
    {
        DecodeToken(token, result, tokens);
    }

    return result;
}

void BPE::PrintTokenTable(const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& encodedTokens)
{
    for (BPE::TOKEN token = 0; token < encodedTokens.size(); ++token)
    {
        std::pair<BPE::TOKEN, BPE::TOKEN> tokenPair = encodedTokens.at(token);

        std::string decriptedToken;

        DecodeToken(tokenPair.first, decriptedToken, encodedTokens);
        DecodeToken(tokenPair.second, decriptedToken, encodedTokens);

        std::cout << std::dec << token + FIRST_TOKEN << " = |";

        for (size_t i = 0; i < decriptedToken.size(); ++i)
        {
            if (std::isprint(decriptedToken[i]))
            {
                std::cout << decriptedToken[i];
            }
            else
            {
                std::cout << "\\x" << std::setfill('0') << std::setw(2) << std::hex << (uint)decriptedToken[i];
            }
        }

        std::cout << '|' << std::endl;
    }
}

void BPE::DecodeToken(BPE::TOKEN token, std::string& decodedToken, const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& encodedTokens)
{
    if (token < FIRST_TOKEN)
    {
        decodedToken.push_back(token);
        return;
    }

    BPE::TOKEN tokenPairIndex = token - FIRST_TOKEN;
    std::pair<BPE::TOKEN, BPE::TOKEN> pair = encodedTokens.at(tokenPairIndex);

    DecodeToken(pair.first, decodedToken, encodedTokens);
    DecodeToken(pair.second, decodedToken, encodedTokens);
}
