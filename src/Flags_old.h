#pragma once

#include <array>
#include <expected>
#include <map>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

class BaseFlagData;

template <typename SubCommand>
    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
class Flags
{
public:
    template <typename T>
    static const T* AddFlag(std::string flag, std::string flagParameterName, std::string description, bool mandatory, T defaultValue, SubCommand subCommand = (SubCommand)-1);

private:
    static std::map<std::string, BaseFlagData*> FlagContainerPerFlagPerSubCommand;
    //static std::map<std::string, BaseFlagContainer> FlagDataPerFlag;
};

class BaseFlagData
{
public:
    virtual void SetData(std::string argv) = 0;
};

// TODO: This might have to move inside Flags<SubCommand>, to be able to store FlagInfo in FlagData
template <typename T>
class FlagData : public BaseFlagData
{
public:
    const T* data; //points to data in FlagContainer<T>

    void SetData(std::string argv) override { (void)argv; };
};

template <typename T>
class FlagContainer
{
public:
    static const T* AddFlag(std::string flag, std::string flagParameterName, std::string description, bool mandatory, T defaultValue);

private:
    T data;
    //struct Flag
    //{
    //    T data;
    //};

    static size_t flagDataSize;
    static constexpr size_t MAX_FLAGS{256};

    static std::array<FlagData<T>, MAX_FLAGS> flagData;
};

#include "Flags.tpp"

//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//class Flags
//{
//    struct BaseFlag;
//    template <typename T>
//    class FlagData;
//
//public:
//    template <typename T>
//    struct FlagInfo
//    {
//        std::string flag;
//        std::string flagParameterName;
//        std::string description;
//        SubCommand subCommand = SubCommand(-1);
//        bool mandatory = false;
//        T defaultValue;
//    };
//
//    //Flags<SubCommand>();
//    static std::map<std::string, BaseFlag*> FlagDataPerFlag;
//
//    template <typename T>
//    static const T* AddFlag(const FlagInfo<T>& flagInfo)
//    {
//        //typename FlagData<T>::Flag flag;
//        //flag.info = flagInfo;
//        //flag.data = flagInfo.defaultValue;
//        //FlagData<T>::AddFlag(flag);
//        //FlagDataPerFlag[flagInfo.flag] = flag;
//        //return &(FlagDataPerFlag[flagInfo.flag].data);
//
//        typename FlagData<T>::Flag* flag{FlagData<T>::AddFlag(flagInfo)};
//
//        std::println("Returned flag: {}", flag->info.flag);
//        // TODO: Assert that flag was not added yet;
//        FlagDataPerFlag[flag->info.flag] = flag;
//        return &flag->data;
//    }
//
//    static std::expected<void, std::string_view> ParseFlags(int argc, char* argv[]);
//
//    //const std::string_view* AddString(FlagInfo<std::string> flagInfo);
//
//    //static std::vector<std::string> SubCommands;
//    //static std::unordered_map<std::type_info, Flag> Flags;
//    //static std::map<SubCommand, std::vector<FlagInfo>> flagInfoPerSubCommand;
//    //static std::map<std::string, FlagData> flagInfoPerSubCommand;
//
//    //static std::map<FlagInfo, std::string> strings;
//    //static std::map<FlagInfo, int> ints;
//    //static std::map<FlagInfo, bool> bools;
//
//private:
//    struct BaseFlag
//    {
//    public:
//        virtual ~BaseFlag() = default;
//        virtual void SetData(std::string argv) = 0;
//    };
//
//    template <typename T>
//    class Flag : public BaseFlag
//    {
//    public:
//        Flags::FlagInfo<T> info;
//        T data;
//        virtual void SetData(std::string argv) override
//        {
//            std::println("{}", argv);
//        };
//    };
//
//    //{
//    //virtual void SetData(std::string argv) override
//    //{
//    //    //data = argv;
//    //    std::println("Specialized! {}", argv);
//    //};
//    //};
//    //template <>
//    //class Flag<std::string> : public BaseFlag
//    //{
//    //public:
//    //    Flags::FlagInfo<std::string> info;
//    //    std::string data;
//    //    void SetData(std::string argv) override
//    //    {
//    //        data = argv;
//    //        std::println("Specialized! {}", argv);
//    //    }
//    //};
//
//    template <typename T>
//    class FlagData
//    {
//    public:
//        static size_t flagDataSize;
//        static constexpr size_t MAX_FLAGS{256};
//        //static std::array<Flag, MAX_FLAGS> flagData;
//        //static std::array<float, 10> flagData;
//        static std::array<Flag<T>, MAX_FLAGS> flagData;
//
//        static Flag<T>* AddFlag(const Flags<SubCommand>::FlagInfo<T>& flagInfo)
//        //static void AddFlag(const typename Flags<SubCommand>::FlagInfo<T>::Flag& flag)
//        {
//            std::println("Call to AddFlag. {}", flagDataSize);
//            Flag flag{};
//            flag.info = flagInfo;
//            flag.data = flagInfo.defaultValue;
//            flagData[flagDataSize] = flag;
//            //return &flagData[flagDataSize++].data;
//            int index = flagDataSize;
//            ++flagDataSize;
//            return &flagData[index];
//        }
//    };
//};
//
//#include "Flags.tpp"
//
//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//std::map<std::string, typename Flags<SubCommand>::BaseFlag*> Flags<SubCommand>::FlagDataPerFlag{};
//
//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//template <typename T>
//size_t Flags<SubCommand>::FlagData<T>::flagDataSize{};
//
//template <typename SubCommand>
//    requires std::is_enum_v<SubCommand> && std::is_signed_v<std::underlying_type_t<SubCommand>>
//template <typename T>
//std::array<typename Flags<SubCommand>::template Flag<T>, Flags<SubCommand>::template FlagData<T>::MAX_FLAGS> Flags<SubCommand>::FlagData<T>::flagData{};
