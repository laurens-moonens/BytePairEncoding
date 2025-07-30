#include <climits>
#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace BPE
{
    enum class SubCommand
    {
        NONE = -1,
        Encode,
        Decode,
        Print
    };

    typedef char16_t TOKEN;
    const TOKEN FIRST_TOKEN{CHAR_MAX + 1};

    bool TryWriteTextToFile(const std::string& textToWrite, const std::filesystem::path& outputFilePath);
    std::expected<std::string, std::string> TryReadTextFromFile(const std::filesystem::path& inputFilePath);
    std::expected<void, std::string> TryWriteEncodedTextToFile(const std::basic_string<TOKEN>& encodedString, const std::string& outputFilePath);
    bool TryReadEncodedTextFromFile(const std::string& inputFilePath, std::basic_string<TOKEN>& encodedString, std::vector<std::pair<TOKEN, TOKEN>>& tokens);

    std::tuple<std::basic_string<TOKEN>, std::basic_string<TOKEN>> EncodeText(const std::string& input);
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
