#pragma once

#include <exception>
#include <string>

#include "Flags.h"

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
size_t Flags<SubCommand>::FlagData<T, S>::dataSize{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
template <typename T, SubCommand S>
std::array<typename Flags<SubCommand>::template FlagInfo<T>, 256> Flags<SubCommand>::FlagData<T, S>::flagData{};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
std::map<std::string, typename Flags<SubCommand>::BaseFlagInfo*> Flags<SubCommand>::flagDataPerFlag{};

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
