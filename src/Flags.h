#pragma once

#include <expected>
#include <optional>
#include <vector>
#include <map>
#include <string>
#include <type_traits>

class BaseFlagInfo
{
public:
    virtual ~BaseFlagInfo() = default;

    virtual void SetData(std::string_view argv) = 0;

    std::string parameterName;
    std::string description;
    bool required;
    bool isDataSet;
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
    Flags(){};
    Flags(const Flags&) = delete;
    void operator=(const Flags&) = delete;

    struct SubCommandInfo
    {
        SubCommand subCommand;
        std::string subCommandString;
        std::string info = "";
    };

    std::string programName{};

    void SetSubCommandInfo(const std::initializer_list<Flags<SubCommand>::SubCommandInfo>& info);

    template <typename T, SubCommand S = (SubCommand)-1>
    const T* AddFlag(const std::string& flag, const std::string& parameterName, const std::string& description, bool required = true, const T& defaultValue = T{});

    //template <SubCommand S = (SubCommand)-1>
    //static const bool* AddFlag(const std::string& flag, bool required = true);

    std::pair<SubCommand, std::optional<std::string>>  ParseFlags(const int argc, char* const argv[]);
    std::string GetUsage(SubCommand subCommand = (SubCommand)-1);

private:

    template <typename T, SubCommand S>
    struct FlagData
    {
        static size_t dataSize;
        static std::array<FlagInfo<T>, 256> flagData;
    };

    std::vector<std::map<std::string, BaseFlagInfo*>> flagInfoPerSubCommandAndFlag;
    std::map<std::string, SubCommand> stringToSubCommand{};
    std::map<SubCommand, SubCommandInfo> subCommandToInfo{};
};

#include "Flags.tpp"
