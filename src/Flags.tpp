#pragma once

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
//std::array<typename Flags<SubCommand>::template FlagInfo<T>, 256> Flags<SubCommand>::FlagData<T, S>::flagData{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::map<std::pair<SubCommand, std::string>, BaseFlagInfo*> Flags<SubCommand>::flagInfoPerSubCommandAndFlag{};
//std::map<std::pair<SubCommand, std::string>, typename Flags<SubCommand>::BaseFlagInfo*> Flags<SubCommand>::flagInfoPerSubCommandAndFlag{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::map<std::string_view, SubCommand> Flags<SubCommand>::stringToSubCommand{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::map<SubCommand, std::string_view> Flags<SubCommand>::subCommandToString{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
void Flags<SubCommand>::SetSubCommandMapping(const std::map<std::string_view, SubCommand>& mapping)
{
    Flags<SubCommand>::stringToSubCommand = mapping;

    for (std::pair<std::string_view, SubCommand> kvp : mapping)
    {
        Flags<SubCommand>::subCommandToString[kvp.second] = kvp.first;
    }
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
const T* Flags<SubCommand>::AddFlag(const std::string& flag, const std::string& parameterName, bool mandatory, const T& defaultValue)
{
    FlagInfo<T> flagInfo{};
    flagInfo.data = defaultValue;
    flagInfo.parameterName = parameterName;
    flagInfo.mandatory = mandatory;

    FlagData<T, S>::flagData[FlagData<T, S>::dataSize] = flagInfo;
    size_t index = FlagData<T, S>::dataSize;
    FlagData<T, S>::dataSize++;

    flagInfoPerSubCommandAndFlag[{S, flag}] = &FlagData<T, S>::flagData[index];
    return &FlagData<T, S>::flagData[index].data;
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <SubCommand S>
const bool* Flags<SubCommand>::AddFlag(const std::string& flag, bool mandatory)
{
    return AddFlag<bool, S>(flag, "", mandatory, false);
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::expected<SubCommand, std::string> Flags<SubCommand>::ParseFlags(const int argc, char* const argv[])
{
    std::println("Program name: {}", argv[0]);

    SubCommand subCommand{(SubCommand)-1};

    if (argc > 1)
    {
        std::string_view subCommandString{argv[1]};
        if (Flags<SubCommand>::stringToSubCommand.contains(subCommandString))
        {
            subCommand = Flags<SubCommand>::stringToSubCommand.at(subCommandString);
        }
    }

    for (int i{2}; i < argc; ++i)
    {
        std::string arg{argv[i]};
        if (!flagInfoPerSubCommandAndFlag.contains({subCommand, arg}))
        {
            return std::unexpected{std::format("ERROR: Unknown option {} for subcommand {}", arg, (int)subCommand)};
        }

        ++i;

        if (i >= argc)
        {
            return std::unexpected{std::format("ERROR: I need an argument after option {}", arg)};
        }

        std::string_view optionArg{argv[i]};

        BaseFlagInfo* baseFlagInfo{flagInfoPerSubCommandAndFlag.at({subCommand, arg})};
        baseFlagInfo->SetData(optionArg);
    }

    return subCommand;
}

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::string Flags<SubCommand>::GetUsage()
{
    std::string result{std::string(1024, '\0')};
    //for (std::tuple<std::string_view, SubCommand> subCommand : Flags<SubCommand>::subCommandMapping)
    //{
    for (std::pair<std::pair<SubCommand, std::string_view>, BaseFlagInfo*> kvp : Flags<SubCommand>::flagInfoPerSubCommandAndFlag)
    {
        result.append(std::format("{} | {}", Flags<SubCommand>::subCommandToString.at(kvp.first.first), kvp.first.second));
        if (kvp.second->mandatory)
        {
            result.append(" | mandatory");
        }
        result.append("\n");
    }
    //}

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
