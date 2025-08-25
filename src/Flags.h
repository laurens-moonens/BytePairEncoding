#pragma once

#include <array>
#include <expected>
#include <map>
#include <string>
#include <type_traits>

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
class Flags
{
public:
    static void SetSubCommandMapping(std::map<std::string_view, SubCommand> mapping);

    template <typename T, SubCommand S = (SubCommand)-1>
    static const T* AddFlag(std::string flag, T defaultValue);

    static std::expected<void, std::string> ParseFlags(const int argc, char* const argv[]);
    static void GetUsage();

private:
    class BaseFlagInfo
    {
    public:
        virtual ~BaseFlagInfo() = default;

        virtual void SetData(std::string_view argv) = 0;

        std::string flag;
        SubCommand subCommand;
    };

    template <typename T>
    class FlagInfo : public BaseFlagInfo
    {
    public:
        T data;

        void SetData(std::string_view argv) override
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

    static std::map<std::pair<SubCommand, std::string_view>, BaseFlagInfo*> flagInfoPerSubCommandAndFlag;
    static std::map<std::string_view, SubCommand> subCommandMapping;
};

#include "Flags.tpp"
