#include <print>

#include "BPE.h"
#include "Flags.h"

int main(int argc, char* argv[])
{
    Flags<BPE::SubCommand> flags{};

    flags.SetSubCommandInfo(
        {
            {BPE::SubCommand::Encode, "encode", "Encode the input file using byte pair encoding"},
            {BPE::SubCommand::Decode, "decode", "Decode an encoded file using a BPE table"},
            {BPE::SubCommand::Inspect, "inspect", "Inspect a BPE table"},
            {BPE::SubCommand::Generate, "generate", "Generate new text (gibberish) based on an BPE table"},
        });

    const std::string* encodeInputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Encode>("-i", "path", "Input file to encode")};
    const std::string* encodeBpeOutputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Encode>("-b", "path", "Output file containing the BPE table")};
    const std::string* encodeTokenOutputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Encode>("-t", "path", "Output file containing the encodedtokens", false)};

    const std::string* decodeBpeInputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Decode>("-b", "path", "Input file containing the BPE table")};
    const std::string* decodeTokenInputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Decode>("-t", "path", "Input file containing the encoded tokens")};
    const std::string* decodeOutputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Decode>("-o", "path", "Output file containing the decoded text")};

    const std::string* inspectBpeInputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Inspect>("-b", "path", "Input file containing the BPE table")};

    const std::string* generateBpeInputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Generate>("-b", "path", "Input file containing the BPE table")};
    const std::string* generateOutputFilePath{flags.AddFlag<std::string, BPE::SubCommand::Generate>("-o", "path", "Output file to write the generate text to", false)};
    const int* generateTokenCount{flags.AddFlag<int, BPE::SubCommand::Generate>("-c", "value", "Number of tokens to generate", false, BPE::GENERATION_DEFAULT_TOKEN_COUNT)};

    const auto& [parseStatus, subCommand, error]{flags.ParseFlags(argc, argv)};

    switch (parseStatus)
    {
        case Flags<BPE::SubCommand>::ParseStatus::Error:
        {
            std::println(stderr, "{}", error);

            std::string usage{flags.GetUsage(subCommand)};
            std::print("{}", usage);

            return 1;
        }
        case Flags<BPE::SubCommand>::ParseStatus::Help:
        {
            std::string usage{flags.GetUsage(subCommand)};
            std::print("{}", usage);

            return 0;
        }
        case Flags<BPE::SubCommand>::ParseStatus::Success:
        default:
            break;
    }

    switch (subCommand)
    {
        case BPE::SubCommand::Encode:
        {
            std::expected<std::string, std::string> inputData{BPE::TryReadFileIntoContainer<std::string>(*encodeInputFilePath)};
            if (!inputData.has_value())
            {
                std::println(stderr, "{}", inputData.error());
                return 1;
            }

            const auto& [bpeTable, encodedString, info]{BPE::EncodeText(inputData.value())};

            std::expected<void, std::string> writeBpeTableResult = BPE::TryWriteBasicStringToFile(bpeTable, *encodeBpeOutputFilePath);
            if (!writeBpeTableResult.has_value())
            {
                std::println(stderr, "{}", writeBpeTableResult.error());
                return 1;
            }

            if (!encodeTokenOutputFilePath->empty())
            {
                std::expected<void, std::string> writeTokensResult = BPE::TryWriteBasicStringToFile(encodedString, *encodeTokenOutputFilePath);
                if (!writeTokensResult.has_value())
                {
                    std::println(stderr, "{}", writeTokensResult.error());
                    return 1;
                }
            }

            if (encodeTokenOutputFilePath->empty())
            {
                std::println("Successfully encoded in {} iterations.", info.EncodingIterationCount);
            }
            else
            {
                std::println("Succesfully encoded {} tokens to {} tokens in {} iterations.", info.EncodedStringInitialLength, info.EncodedStringLength, info.EncodingIterationCount);
            }
        }
        break;

        case BPE::SubCommand::Decode:
        {
            std::expected<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>, std::string> bpeTable{BPE::TryReadFileIntoContainer<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>>(*decodeBpeInputFilePath)};
            if (!bpeTable.has_value())
            {
                std::println(stderr, "{}", bpeTable.error());
                return 1;
            }

            std::expected<std::basic_string<BPE::TOKEN>, std::string> tokens{BPE::TryReadFileIntoContainer<std::basic_string<BPE::TOKEN>>(*decodeTokenInputFilePath)};
            if (!tokens.has_value())
            {
                std::println(stderr, "{}", tokens.error());
                return 1;
            }

            auto [decodedString, info]{BPE::DecodeString(tokens.value(), bpeTable.value())};

            std::expected<void, std::string> writeStringResult{BPE::TryWriteBasicStringToFile(decodedString, *decodeOutputFilePath)};
            if (!writeStringResult.has_value())
            {
                std::println(stderr, "{}", writeStringResult.error());
                return 1;
            }

            std::println("Successfully decoded {} tokens to {} tokens.", info.EncodedStringLength, info.DecodedStringLength);
        }
        break;

        case BPE::SubCommand::Inspect:
        {
            std::expected<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>, std::string> bpeTable{BPE::TryReadFileIntoContainer<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>>(*inspectBpeInputFilePath)};
            if (!bpeTable.has_value())
            {
                std::println(stderr, "{}", bpeTable.error());
                return 1;
            }

            BPE::PrintBpeTable(bpeTable.value());
        }
        break;

        case BPE::SubCommand::Generate:
        {
            if (*generateTokenCount <= 0)
            {
                std::println(stderr, "ERROR: Token count should be greater than zero.");
                return 1;
            }

            std::expected<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>, std::string> bpeTable{BPE::TryReadFileIntoContainer<std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>>>(*generateBpeInputFilePath)};
            if (!bpeTable.has_value())
            {
                std::println(stderr, "{}", bpeTable.error());
                return 1;
            }

            auto [generatedTokenString, info]{BPE::GenerateTokenString(bpeTable.value(), *generateTokenCount)};
            auto [decodedString, _]{BPE::DecodeString(generatedTokenString, bpeTable.value())};

            std::string lastTokenDecoded{};
            BPE::DecodeToken(info.LastToken, lastTokenDecoded, bpeTable.value());

            switch (info.EndCause)
            {
                case BPE::GenerationEndCause::CountReached:
                    std::println("Successfully generated {} tokens.", info.TokenCount);
                    break;
                case BPE::GenerationEndCause::NoNextTokenFound:
                    std::println("Generated {} tokens. No next token was found after token |{}| ({})", info.TokenCount, lastTokenDecoded, (uint)info.LastToken);
                    break;
                case BPE::GenerationEndCause::TerminalTokenReached:
                    std::println("Generated {} tokens. Reached terminal token |{}| ({})", info.TokenCount, lastTokenDecoded, (uint)info.LastToken);
                    break;
            }

            if (generateOutputFilePath->empty())
            {
                std::println("{}\n", decodedString);
            }
            else
            {
                std::expected<void, std::string> writeStringResult{BPE::TryWriteBasicStringToFile(decodedString, *generateOutputFilePath)};
                if (!writeStringResult.has_value())
                {
                    std::println(stderr, "{}", writeStringResult.error());
                    return 1;
                }
                else
                {
                    std::println("-> written to \"{}\"", generateOutputFilePath->c_str());
                }
            }
        }
        break;

        case BPE::SubCommand::None:
        default:
            throw std::runtime_error("Subcommand not implemented");
            break;
    }
}
