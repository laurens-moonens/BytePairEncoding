#pragma once

#include <cstdarg>
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
    //TODO: Error if flag was already added (in case of -h for example)

    FlagInfo<T> flagInfo{};
    flagInfo.data = defaultValue;
    flagInfo.parameterName = parameterName;
    flagInfo.description = desciption;
    flagInfo.required = required;

    FlagData<T, S>::flagData[FlagData<T, S>::dataSize] = flagInfo;
    size_t index = FlagData<T, S>::dataSize;
    FlagData<T, S>::dataSize++;

    flagInfoPerSubCommandAndFlag[(int)S][flag] = &FlagData<T, S>::flagData[index];
    return &FlagData<T, S>::flagData[index].data;
}

//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//template <SubCommand S>
//const bool* Flags<SubCommand>::AddFlag(const std::string& flag, bool required)
//{
//    return AddFlag<bool, S>(flag, "", required, false);
//}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::pair<SubCommand, std::optional<std::string>> Flags<SubCommand>::ParseFlags(const int argc, char* const argv[])
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
        else
        {
            return {subCommand, std::format("ERROR: Unknown command {}", subCommandString)};
        }
    }
    else
    {
        return {subCommand, std::format("ERROR: Missing command")};
    }

    //std::vector<std::pair<SubCommand, std::string>> parsedOptions{};
    std::map<std::string, BaseFlagInfo*> flagsForSelectedSubCommand{flagInfoPerSubCommandAndFlag.at((int)subCommand)};

    for (int i{2}; i < argc; ++i)
    {
        std::string arg{argv[i]};

        if (!flagsForSelectedSubCommand.contains(arg))
        {
            return {subCommand, std::format("ERROR: Unknown option {}", arg, (int)subCommand)};
        }

        ++i;

        if (i >= argc)
        {
            return {subCommand, std::format("ERROR: Missing value for option {}", arg)};
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
            return {subCommand, std::format("ERROR: Missing option {}", flag)};
        }
    }

    return {subCommand, std::nullopt};
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::string Flags<SubCommand>::GetUsage(SubCommand subCommand)
{
    std::string result{std::string(1024, '\0')};

    if (subCommand == (SubCommand)-1)
    {
        result.append(std::format("\nUsage: {} <command> [options]\nCommands:\n", programName));

        for (const auto& [subCommand, subCommandInfo] : Flags<SubCommand>::subCommandToInfo)
        {
            result.append(std::format("\t{}\t\t {}\n", subCommandInfo.subCommandString, subCommandInfo.info));
        }

        //TODO: Only print this if there are any options without a subcommand
        result.append("\nOptions:\n");

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
        result.append("[");
        for (const auto& [flag, flagInfo] : optionalFlags)
        {
            result.append(std::format("{} <{}> ", flag, flagInfo->parameterName));
        }
        result.pop_back();
        result.append("]");
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

    result.append("\n");
    return result;
}

/*
template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::map<std::string, BaseFlagData*> Flags<SubCommand>::FlagContainerPerFlagPerSubCommand{};
//std::map<std::tuple<SubCommand, std::string>, std::vector<BaseFlagData*>> Flags<SubCommand>::FlagContainerPerFlagPerSubCommand{};

template <typename T>
size_t FlagContainer<T>::flagDataSize{};

template <typename T>
std::array<FlagData<T>, FlagContainer<T>::MAX_FLAGS> FlagContainer<T>::flagData{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T>
const T* Flags<SubCommand>::AddFlag(std::string flag, std::string flagParameterName, std::string description, bool mandatory, T defaultValue, SubCommand subCommand)
{
    (void)subCommand;
    //typename FlagData<T>::Flag flag;
    //flag.info = flagInfo;
    //flag.data = flagInfo.defaultValue;
    //FlagData<T>::AddFlag(flag);
    //FlagDataPerFlag[flagInfo.flag] = flag;
    //return &(FlagDataPerFlag[flagInfo.flag].data);

    //typename FlagData<T>::Flag* flag{FlagData<T>::AddFlag(flagInfo)};

    //std::tuple<SubCommand, std::string> key{subCommand, flag};
    const T* flagData{FlagContainer<T>::AddFlag(flag, flagParameterName, description, mandatory, defaultValue)};
    FlagData<T> F{};
    F.data = flagData;
    Flags<SubCommand>::FlagContainerPerFlagPerSubCommand[flag] = &F;

    //std::println("Returned flag: {}", flag->info.flag);
    //// TODO: Assert that flag was not added yet;
    //FlagDataPerFlag[flag->info.flag] = flag;
    //return &flag->data;
}

template <typename T>
const T* FlagContainer<T>::AddFlag(std::string flag, std::string flagParameterName, std::string description, bool mandatory, T defaultValue)
{
    (void)flag;
    (void)flagParameterName;
    (void)description;
    (void)mandatory;
    (void)defaultValue;

    T flagData{};
    //            flag.info = flagInfo;
    flagData = defaultValue;
    //FlagData<T>::flagData[FlagData<T>::flagDataSize] = flagData;
    //size_t index = FlagData<T>::flagDataSize;
    FlagContainer<T>::flagDataSize++;

    //return &FlagData<T>::flagData[index].data;
    return NULL;
    //            //return &flagData[flagDataSize++].data;
    //            int index = flagDataSize;
    //            ++flagDataSize;
    //            return &flagData[index];
}

//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//std::expected<void, std::string_view> Flags<SubCommand>::ParseFlags(int argc, char* argv[])
//{
//    for (int i{0}; i < argc; ++i)
//    {
//        const char* arg{argv[i]};
//
//        std::println("Arg to find = {}", arg);
//
//        if (FlagDataPerFlag.contains(arg) == false)
//        {
//            std::println("ERROR: Unknown option '{}'", arg);
//            //return std::unexpected(std::format("ERROR: Unknown option '{}'", argv));
//        }
//        else
//        {
//            std::println("Found flag {}", arg);
//            BaseFlag* flag{FlagDataPerFlag.at(arg)};
//            flag->SetData(arg);
//        }
//    }
//
//    return {};
//}
*/
