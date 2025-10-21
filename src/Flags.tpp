#pragma once

#include <algorithm>
#include <cstdarg>
#include <format>
#include <optional>
#include <print>
#include <string>

#include "Flags.h"

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
size_t Flags<SubCommand>::FlagData<T, S>::dataSize{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
std::array<FlagInfo<T>, 256> Flags<SubCommand>::FlagData<T, S>::flagData{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
void Flags<SubCommand>::SetSubCommandInfo(const std::initializer_list<Flags<SubCommand>::SubCommandInfo>& info)
{
    for (SubCommandInfo info : info)
    {
        subCommandToInfo[info.subCommand] = info;
        stringToSubCommand[info.subCommandString] = info.subCommand;
    }

    flagInfoPerSubCommandAndFlag.resize(info.size());
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
const T* Flags<SubCommand>::AddFlag(const std::string& flag, const std::string& parameterName, const std::string& desciption, bool required, const T& defaultValue)
{
    FlagInfo<T> flagInfo{};
    flagInfo.data = defaultValue;
    flagInfo.parameterName = parameterName;
    flagInfo.description = desciption;
    flagInfo.required = required;

    if (flagInfoPerSubCommandAndFlag[(int)S].contains(flag))
    {
        throw std::runtime_error(std::format("Flag \"{}\" was already added for command {}", flag, subCommandToInfo.at(S).subCommandString));
    }

    FlagData<T, S>::flagData[FlagData<T, S>::dataSize] = flagInfo;
    size_t index = FlagData<T, S>::dataSize;
    FlagData<T, S>::dataSize++;

    flagInfoPerSubCommandAndFlag[(int)S][flag] = &FlagData<T, S>::flagData[index];
    return &FlagData<T, S>::flagData[index].data;
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::tuple<typename Flags<SubCommand>::ParseStatus, SubCommand, std::string> Flags<SubCommand>::ParseFlags(const int argc, char* const argv[])
{
    Flags::programName = argv[0];

    SubCommand subCommand{(SubCommand)-1};

    if (argc > 1)
    {
        std::string subCommandString{argv[1]};
        if (stringToSubCommand.contains(subCommandString))
        {
            subCommand = stringToSubCommand.at(subCommandString);
        }
        else if (subCommandString == "-h")
        {
            return {ParseStatus::Help, subCommand, ""};
        }
        else
        {
            return {ParseStatus::Error, subCommand, std::format("ERROR: Unknown command {}", subCommandString)};
        }
    }
    else
    {
        return {ParseStatus::Error, subCommand, std::format("ERROR: Missing command")};
    }

    std::map<std::string, BaseFlagInfo*> flagsForSelectedSubCommand{flagInfoPerSubCommandAndFlag.at((int)subCommand)};

    for (int i{2}; i < argc; ++i)
    {
        std::string arg{argv[i]};

        if (arg == "-h")
        {
            return {ParseStatus::Help, subCommand, ""};
        }
        else if (!flagsForSelectedSubCommand.contains(arg))
        {
            return {ParseStatus::Error, subCommand, std::format("ERROR: Unknown option {}", arg, (int)subCommand)};
        }

        ++i;

        if (i >= argc)
        {
            return {ParseStatus::Error, subCommand, std::format("ERROR: Missing value for option {}", arg)};
        }

        std::string_view optionArg{argv[i]};

        BaseFlagInfo* baseFlagInfo{flagsForSelectedSubCommand.at(arg)};
        baseFlagInfo->SetData(optionArg);
        baseFlagInfo->isDataSet = true;
    }

    for (std::pair<std::string, BaseFlagInfo*> kvp : flagsForSelectedSubCommand)
    {
        std::string flag = kvp.first;
        BaseFlagInfo* baseFlagInfo = kvp.second;
        if (baseFlagInfo->isDataSet == false && baseFlagInfo->required)
        {
            return {ParseStatus::Error, subCommand, std::format("ERROR: Missing option {}", flag)};
        }
    }

    return {ParseStatus::Success, subCommand, ""};
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::string Flags<SubCommand>::GetUsage(SubCommand subCommand)
{
    std::string result{std::string(1024, '\0')};

    if (subCommand == (SubCommand)-1)
    {
        result.append(std::format("\nUsage: {} <command> [options]\n\nCommands:\n", programName));

        for (const auto& [subCommand, subCommandInfo] : Flags<SubCommand>::subCommandToInfo)
        {
            result.append(std::format("\t{}\t\t {}\n", subCommandInfo.subCommandString, subCommandInfo.info));
        }

        result.append("\nOptions:\n\t-h\tPrint this help message\n\n");
        //TODO: Print options without subcommand here

        return result;
    }

    SubCommandInfo subCommandInfo{subCommandToInfo.at(subCommand)};
    result.append(std::format("\nUsage: {} {} ", programName, subCommandInfo.subCommandString));

    const std::map<std::string, BaseFlagInfo*>& flagsForSubCommand{flagInfoPerSubCommandAndFlag.at((int)subCommand)};

    if (flagsForSubCommand.empty())
    {
        result.append("\n\n");
        return result;
    }

    std::vector<std::pair<std::string, BaseFlagInfo*>> optionalFlags{};
    optionalFlags.reserve(flagsForSubCommand.size());

    for (const auto& [flag, flagInfo] : flagsForSubCommand)
    {
        if (flagInfo->required)
        {
            result.append(std::format("{} <{}> ", flag, flagInfo->parameterName));
        }
        else
        {
            //TODO: Print default value
            optionalFlags.push_back({flag, flagInfo});
        }
    }

    if (optionalFlags.size() > 0)
    {
        for (const auto& [flag, flagInfo] : optionalFlags)
        {
            result.append(std::format("[{} <{}>] ", flag, flagInfo->parameterName));
        }
    }

    result.append("\n\nOptions:\n");

    for (const auto& [flag, flagInfo] : flagsForSubCommand)
    {
        result.append(std::format("\t{} <{}>\t{}", flag, flagInfo->parameterName, flagInfo->description));
        if (flagInfo->required)
        {
            result.append(" (REQUIRED)");
        }
        else
        {
            result.append(" (optional)");
        }
        result.append("\n");
    }

    result.append("\n\t-h\t\tPrint this help message\n\n");

    return result;
}
