#pragma once

#include <array>
#include <expected>
#include <map>
#include <print>
#include <string>
#include <type_traits>

class BaseFlagInfo
{
public:
    virtual ~BaseFlagInfo() = default;

    virtual void SetData(std::string_view argv) = 0;

    std::string flag;
    std::string parameterName;
    bool mandatory;
};

template <typename T>
class FlagInfo : public BaseFlagInfo
{
public:
    T data;
    void SetData(std::string_view argv);
};

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
class Flags
{
public:
    struct SubCommandInfo
    {
        SubCommand subCommand;
        std::string subCommandString;
        std::string info;
    };

    static std::string programName;

    static void SetSubCommandMapping(const std::initializer_list<Flags<SubCommand>::SubCommandInfo> mapping);

    template <typename T, SubCommand S = (SubCommand)-1>
    static const T* AddFlag(const std::string& flag, const std::string& parameterName, bool mandatory = true, const T& defaultValue = T{});

    template <SubCommand S = (SubCommand)-1>
    static const bool* AddFlag(const std::string& flag, bool mandatory = true);

    static std::expected<SubCommand, std::string> ParseFlags(const int argc, char* const argv[]);
    static std::string GetUsage(SubCommand subCommand = (SubCommand)-1);

private:
    template <typename T, SubCommand S>
    struct FlagData
    {
        static size_t dataSize;
        static std::array<FlagInfo<T>, 256> flagData;
    };

    static std::map<std::pair<SubCommand, std::string>, BaseFlagInfo*> flagInfoPerSubCommandAndFlag;
    static std::map<std::string_view, SubCommand> stringToSubCommand;
    static std::map<SubCommand, std::string_view> subCommandToString;
};

#include "Flags.tpp"
