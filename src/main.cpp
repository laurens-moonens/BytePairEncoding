#include <cassert>
#include <cstring>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <print>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "BPE.h"

void PrintUsage(std::string_view programName, BPE::SubCommand subCommand = BPE::SubCommand::NONE)
{
    if (subCommand == BPE::SubCommand::NONE)
    {
        std::println();
        std::println("Usage: {} <command> [options]", programName);
        std::println();
        std::println("Commands:");
        std::println("\tencode\t Encode the input file using byte pair encoding");
        std::println();
        std::println("Options:");
    }
    else
    {
        switch (subCommand)
        {
            case BPE::SubCommand::Encode:
                std::println();
                std::println("Usage: {} encode -i <input> -b <bpe-output> [-t <token-output>]", programName);
                std::println();
                std::println("Options:");
                std::println("\t-i <file>\t Input file to encode (REQUIRED)");
                std::println("\t-b <file>\t Output file containing the BPE table (REQUIRED)");
                std::println("\t-t <file>\t Output file containing the encoded tokens (optional)");
                std::println();
                break;
            default:
                throw std::runtime_error("Subcommand not implemented");
                break;
        }
    }

    std::println("\t-h, --help\t Print this help message");
    std::println();
}

int main(int argc, char* argv[])
{
    BPE::SubCommand subCommand{BPE::SubCommand::NONE};

    std::string_view programName{argv[0]};

    std::queue<std::string_view> args{argv + 1, argv + argc};

    if (args.size() <= 0)
    {
        std::println(stderr, "ERROR: Missing command");
        PrintUsage(programName);
        return 1;
    }

    std::string_view subCommandArg{args.front()};
    args.pop();

    if (subCommandArg == "encode") subCommand = BPE::SubCommand::Encode;
    else if (subCommandArg == "decode") subCommand = BPE::SubCommand::Decode;
    else if (subCommandArg == "-h" || subCommandArg == "--help")
    {
        PrintUsage(programName);
        return 0;
    }
    else
    {
        std::println(stderr, "ERROR: Unknown command {}", subCommandArg);
        PrintUsage(programName);
        return 1;
    }

    if (args.size() > 0 && (args.front() == "-h" || args.front() == "--help"))
    {
        PrintUsage(programName, subCommand);
        return 0;
    }

    switch (subCommand)
    {
        case BPE::SubCommand::Encode:
        {
            std::filesystem::path inputFilePath{};
            std::filesystem::path bpeFilePath{};
            std::filesystem::path tokenFilePath{};

            while (args.size() > 0)
            {
                std::string_view arg{args.front()};
                args.pop();

                if (arg == "-i")
                {
                    inputFilePath = args.front();
                    args.pop();
                }
                else if (arg == "-b")
                {
                    bpeFilePath = args.front();
                    args.pop();
                }
                else if (arg == "-t")
                {
                    tokenFilePath = args.front();
                    args.pop();
                }
                else
                {
                    std::println("ERROR: Unknown option '{}'", arg);
                    PrintUsage(programName, subCommand);
                    return 1;
                }
            }

            if (inputFilePath.empty())
            {
                std::println(stderr, "ERROR: Missing option '-i <file>'");
                PrintUsage(programName, subCommand);
                return 1;
            }

            if (bpeFilePath.empty())
            {
                std::println(stderr, "ERROR: Missing option '-b <file>'");
                PrintUsage(programName, subCommand);
                return 1;
            }

            std::expected<std::string, std::string> inputData{BPE::TryReadTextFromFile(inputFilePath)};
            if (!inputData.has_value())
            {
                std::println(stderr, "{}", inputData.error());
                return 1;
            }

            auto [bpeTable, encodedString] = BPE::EncodeText(inputData.value());

            //bpe.TryWriteEncodedTextToFile(const)

            break;
        }
        case BPE::SubCommand::NONE:
        default:
            throw std::runtime_error("Subcommand not implemented");
            break;
    }

    return 0;

    /*
    for (int a = 0; a < argc; ++a)
    {
        char* arg = argv[a];

        if (strcmp(arg, "-e") == 0)
        {
            assert(a + 2 < argc);
            std::string inputFile = argv[a + 1];
            std::string outputFile = argv[a + 2];
            a += 2;

            std::string input;
            //if (!bpe.TryReadTextFromFile(inputFile, input))
            //{
            //    std::print(stderr, "ERROR: Failed to read text from file {}", inputFile);
            //    continue;
            //}

            //std::basic_string<BPE::TOKEN> encodedString = bpe.EncodeText(input);
            //bpe.TryWriteEncodedTextToFile(encodedString, outputFile);
        }
        else if (strcmp(arg, "-d") == 0)
        {
            assert(a + 2 < argc);
            std::string inputFile = argv[a + 1];
            std::string outputFile = argv[a + 2];
            a += 2;

            std::basic_string<BPE::TOKEN> encodedInput;
            std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>> tokens;

            if (bpe.TryReadEncodedTextFromFile(inputFile, encodedInput, tokens) == false)
            {
                std::cout << "ERROR: Failed to read raw encoded input from file." << std::endl;
                return -1;
            }

            std::string decodedText = bpe.DecodeString(encodedInput, tokens);

            if (bpe.TryWriteTextToFile(decodedText, outputFile) == false)
            {
                std::cout << "ERROR: Failed to write decoded text to file." << std::endl;
                return -1;
            }
        }
        else if (strcmp(arg, "-p") == 0)
        {
            assert(a + 1 < argc);
            std::string inputFile = argv[a + 1];
            ++a;

            std::basic_string<BPE::TOKEN> encodedInput;
            std::vector<std::pair<BPE::TOKEN, BPE::TOKEN>> tokens;

            if (bpe.TryReadEncodedTextFromFile(inputFile, encodedInput, tokens) == false)
            {
                std::cout << "ERROR: Failed to read raw encoded input from file." << std::endl;
                return -1;
            }

            bpe.PrintTokenTable(tokens);
        }
    }*/

    return 0;
}
