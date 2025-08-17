#pragma once

#include <array>
#include <expected>
#include <map>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
class Flags
{
public:
    struct BaseFlagInfo
    {
    public:
        virtual void SetData(std::string argv) = 0;

        std::string flag;
        SubCommand subCommand;
    };

    template <typename T>
    struct FlagInfo : public BaseFlagInfo
    {
        T data;

        void SetData(std::string argv) override
        {
            data = argv;
        }
    };

    template <typename T, SubCommand S>
    class FlagData
    {
    public:
        static size_t dataSize;
        static std::array<FlagInfo<T>, 256> flagData;
    };

    static std::map<std::string, BaseFlagInfo*> flagDataPerFlag;

    template <typename T, SubCommand S = (SubCommand)-1>
    static const T* AddFlag(std::string flag, T defaultValue)
    {
        FlagInfo<T> flagInfo{};
        flagInfo.flag = flag;
        flagInfo.subCommand = S;
        flagInfo.data = defaultValue;

        FlagData<T, S>::flagData[FlagData<T, S>::dataSize] = flagInfo;
        size_t index = FlagData<T, S>::dataSize;
        FlagData<T, S>::dataSize++;

        flagDataPerFlag[flag] = &FlagData<T, S>::flagData[index];
        return &FlagData<T, S>::flagData[index].data;
    }

    static void ParseFlags(std::string argv1, std::string argv2)
    {
        BaseFlagInfo* baseFlagInfo{flagDataPerFlag.at(argv1)};
        baseFlagInfo->SetData(argv2);
    }
};

#include "Flags.tpp"
