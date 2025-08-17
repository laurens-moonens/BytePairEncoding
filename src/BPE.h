#pragma once

#include <climits>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace BPE
{
    typedef char16_t TOKEN;
    constexpr TOKEN FIRST_TOKEN{CHAR_MAX + 1};
    constexpr int GENERATION_DEFAULT_TOKEN_COUNT{100};

    enum class SomeOtherCommands : int
    {
        Encode,
        Decode,
        Inspect,
        Generate
    };

    enum class SubCommand : int
    {
        NONE = -1,
        Encode,
        Decode,
        Inspect,
        Generate
    };

    enum class GenerationEndCause
    {
        CountReached,
        TerminalTokenReached,
        NoNextTokenFound
    };

    struct BpeEncodingResultInfo
    {
        uint64_t EncodingIterationCount;
        uint64_t EncodedStringInitialLength;
        uint64_t EncodedStringLength;
    };

    struct BpeDecodingResultInfo
    {
        uint64_t EncodedStringLength;
        uint64_t DecodedStringLength;
    };

    struct BpeGenerationResultInfo
    {
        uint TokenCount;
        GenerationEndCause EndCause;
        TOKEN LastToken;
    };

    template <typename charType>
    std::expected<void, std::string> TryWriteBasicStringToFile(const std::basic_string<charType>& dataToWrite, const std::filesystem::path& outputFilePath);

    template <typename ContainerType>
    std::expected<ContainerType, std::string> TryReadFileIntoContainer(const std::filesystem::path& inputFilePath);

    std::expected<void, std::string> TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath);
    bool TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<TOKEN>& encodedString, std::vector<std::pair<TOKEN, TOKEN>>& tokens);

    std::tuple<std::basic_string<TOKEN>, std::basic_string<TOKEN>, BpeEncodingResultInfo> EncodeText(const std::string& input);
    std::tuple<std::string, BpeDecodingResultInfo> DecodeString(const std::basic_string<TOKEN>& input, const std::vector<std::pair<TOKEN, TOKEN>>& tokens);
    void PrintBpeTable(const std::vector<std::pair<TOKEN, TOKEN>>& bpeTable);
    void DecodeToken(TOKEN token, std::string& decodedToken, const std::vector<std::pair<TOKEN, TOKEN>>& bpeTable);

    std::tuple<std::basic_string<TOKEN>, BpeGenerationResultInfo> GenerateTokenString(const std::vector<std::pair<TOKEN, TOKEN>>& bpeTable, uint tokenCount);

    struct PairHash
    {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const
        {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ h2;
        }
    };
} //namespace BPE
