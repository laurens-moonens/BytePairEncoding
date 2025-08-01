#include <array>
#include <climits>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace BPE
{
    typedef char16_t TOKEN;
    const TOKEN FIRST_TOKEN{CHAR_MAX + 1};

    enum class SubCommand
    {
        NONE = -1,
        Encode,
        Decode,
        Print
    };

    struct BpeEncodingResultInfo
    {
        uint64_t EncodingIterationCount;
        uint64_t EncodedStringInitialLength;
        uint64_t EncodedStringLength;
    };

    template <typename charType>
    std::expected<void, std::string> TryWriteBasicStringToFile(const std::basic_string<charType>& dataToWrite, const std::filesystem::path& outputFilePath);

    template <typename ContainerType>
    std::expected<ContainerType, std::string> TryReadFileIntoContainer(const std::filesystem::path& inputFilePath);

    std::expected<void, std::string> TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath);
    bool TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<TOKEN>& encodedString, std::vector<std::pair<TOKEN, TOKEN>>& tokens);

    std::tuple<std::basic_string<TOKEN>, std::basic_string<TOKEN>, BpeEncodingResultInfo> EncodeText(const std::string& input);
    std::string DecodeString(const std::basic_string<TOKEN>& input, const std::vector<std::pair<TOKEN, TOKEN>>& tokens);
    void PrintTokenTable(const std::vector<std::pair<TOKEN, TOKEN>>& encodedTokens);
    void DecodeToken(TOKEN token, std::string& decodedToken, const std::vector<std::pair<TOKEN, TOKEN>>& encodedTokens);

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
