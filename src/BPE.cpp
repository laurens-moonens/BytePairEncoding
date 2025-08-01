#include "BPE.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <print>
#include <unordered_map>

template std::expected<void, std::string> BPE::TryWriteBasicStringToFile<std::string::value_type>(const std::basic_string<std::string::value_type>& textToWrite, const std::filesystem::path& outputFilePath);
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

template std::expected<std::string, std::string> BPE::TryReadFileIntoContainer(const std::filesystem::path& inputFilePath);
template std::expected<std::basic_string<BPE::TOKEN>, std::string> BPE::TryReadFileIntoContainer(const std::filesystem::path& inputFilePath);
template std::expected<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>, std::string> BPE::TryReadFileIntoContainer(const std::filesystem::path& inputFilePath);
template <typename ContainerType>
std::expected<ContainerType, std::string> BPE::TryReadFileIntoContainer(const std::filesystem::path& inputFilePath)
{
    std::ifstream file{inputFilePath};

    if (file.is_open() == false)
    {
        return std::unexpected{std::format("ERROR: Unable to open file at path \"{}\"", inputFilePath.c_str())};
    }

    uintmax_t fileSize{std::filesystem::file_size(inputFilePath)};

    if (fileSize % (sizeof(typename ContainerType::value_type)) != 0)
    {
        return std::unexpected{std::format("ERROR: Input file size (\"{}\" with size {}) was not divisible by the templated data type with size {}", inputFilePath.c_str(), fileSize, sizeof(typename ContainerType::value_type))};
    }

    ContainerType content{};
    content.resize(fileSize / sizeof(typename ContainerType::value_type));
    file.read(reinterpret_cast<char*>(content.data()), fileSize);

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

std::tuple<std::basic_string<BPE::TOKEN>, std::basic_string<BPE::TOKEN>, BPE::BpeEncodingResultInfo> BPE::EncodeText(const std::string& input)
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

    BPE::BpeEncodingResultInfo encodingInfo{};
    encodingInfo.EncodedStringInitialLength = input.size();

    for (;; encodingInfo.EncodingIterationCount++)
    {
        pairCounts.clear();

        for (size_t i{0}; i < encodedString.size() - 1; ++i)
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

        for (size_t i{0}; i < encodedStringCopy.size(); ++i)
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

    encodingInfo.EncodedStringLength = encodedString.size();

    return {bpeTable, encodedString, encodingInfo};
}

std::tuple<std::string, BPE::BpeDecodingResultInfo> BPE::DecodeString(const std::basic_string<BPE::TOKEN>& input, const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& bpeTable)
{
    std::string result;
    result.reserve(input.size() * 2);

    for (const BPE::TOKEN& token : input)
    {
        DecodeToken(token, result, bpeTable);
    }

    BpeDecodingResultInfo info{input.size(), result.size()};

    return {result, info};
}

void BPE::DecodeToken(BPE::TOKEN token, std::string& decodedToken, const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& bpeTable)
{
    if (token < FIRST_TOKEN)
    {
        decodedToken.push_back(token);
        return;
    }

    BPE::TOKEN tokenPairIndex{BPE::TOKEN(token - FIRST_TOKEN)};
    std::pair<BPE::TOKEN, BPE::TOKEN> pair{bpeTable.at(tokenPairIndex)};

    DecodeToken(pair.first, decodedToken, bpeTable);
    DecodeToken(pair.second, decodedToken, bpeTable);
}

void BPE::PrintBpeTable(const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& bpeTable)
{
    for (BPE::TOKEN token{0}; token < bpeTable.size(); ++token)
    {
        std::pair<BPE::TOKEN, BPE::TOKEN> tokenPair{bpeTable.at(token)};

        std::string decriptedToken;

        DecodeToken(tokenPair.first, decriptedToken, bpeTable);
        DecodeToken(tokenPair.second, decriptedToken, bpeTable);

        std::print("{} = |", token + FIRST_TOKEN);

        for (size_t i{0}; i < decriptedToken.size(); ++i)
        {
            if (std::isprint(decriptedToken[i]))
            {
                std::print("{}", decriptedToken[i]);
            }
            else
            {
                std::print("\\{:#04X}", decriptedToken[i]);
            }
        }

        std::println("|");
    }
}

std::basic_string<BPE::TOKEN> BPE::GenerateTokenString(const std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>& bpeTable, uint tokenCount)
{
    srand(time(0));
    std::basic_string<BPE::TOKEN> result{};

    auto asd = bpeTable;

    BPE::TOKEN currentToken{bpeTable.at(rand() % (int)bpeTable.size()).second};
    result.push_back(currentToken);

    for (uint i{1}; i < tokenCount; ++i)
    {
        std::string decodedString1{};
        BPE::DecodeToken(currentToken, decodedString1, bpeTable);
        std::println("TOKEN {}: |{}|", i, decodedString1);

        std::vector<BPE::TOKEN> possibleNextTokens{};

        while (possibleNextTokens.size() == 0)
        {
            for (std::pair<BPE::TOKEN, BPE::TOKEN> tokenPair : bpeTable)
            {
                if (tokenPair.first == currentToken)
                {
                    possibleNextTokens.push_back(tokenPair.second);
                }
            }

            if (possibleNextTokens.size() == 0)
            {
                if (currentToken < FIRST_TOKEN)
                {
                    std::println("GAVE UP: Reached terminal token {}", (char)currentToken);
                    break;
                }

                std::string decodedString2{};
                BPE::DecodeToken(currentToken, decodedString2, bpeTable);
                std::println("Could not find next token after |{}|", decodedString2);
                currentToken = bpeTable.at(currentToken - FIRST_TOKEN).second;

                decodedString2 = std::string{};
                BPE::DecodeToken(currentToken, decodedString2, bpeTable);
                std::println("Checking token |{}| instead (value {})", decodedString2, (uint16_t)currentToken);
            }
        }

        if (possibleNextTokens.size() == 0)
        {
            std::string decodedString3{};
            BPE::DecodeToken(result.at(result.size() - 1), decodedString3, bpeTable);
            std::println("GAVE UP: Could not find next token after |{}|", decodedString3);
            return result;
        }

        std::println("Found {} possible next tokens", possibleNextTokens.size());
        for (size_t j{0}; j < possibleNextTokens.size(); ++j)
        {
            std::string decodedString4{};
            BPE::DecodeToken(possibleNextTokens.at(j), decodedString4, bpeTable);
            std::println("|{}| ({})", decodedString4, (uint16_t)possibleNextTokens.at(j));
        }

        currentToken = possibleNextTokens.at(rand() % possibleNextTokens.size());
        result.push_back(currentToken);
        std::println();
    }

    return result;
}
