#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>

template <typename SubCommand>
    requires std::is_enum_v<SubCommand>
class Flags
{
public:
    struct FlagInfo
    {
        std::string flag;
        std::string flagParameterName;
        std::string description;
        SubCommand subCommand;
        bool mandatory;
    };

    //Flags<SubCommand>();

    static void AddSubCommand(std::string name);

    //const std::string_view* AddString(FlagInfo<std::string> flagInfo);

    //static std::vector<std::string> SubCommands;
    //static std::unordered_map<std::type_info, Flag> Flags;
    //static std::map<SubCommand, std::vector<FlagInfo>> flagInfoPerSubCommand;
    //static std::map<std::string, FlagData> flagInfoPerSubCommand;

    //static std::map<FlagInfo, std::string> strings;
    //static std::map<FlagInfo, int> ints;
    //static std::map<FlagInfo, bool> bools;

    template <typename T>
    class FlagData
    {
    public:

        struct Flag
        {
            Flags::FlagInfo info;
            T data;
        };

        static size_t flagDataSize;
        static const T* AddFlag(const Flags<SubCommand>::FlagInfo& flagInfo)
        {
            Flag flag{};
            flag.info = flagInfo;
            flagData[flagDataSize] = flag;
            ++flagDataSize;
            //return &flag.data;
            return NULL;
        }

    private:

        static const size_t MAX_FLAGS{256};

        static std::array<Flag, MAX_FLAGS> flagData;
    };

};

template <typename SubCommand>
requires std::is_enum_v<SubCommand>
template <typename T>
size_t Flags<SubCommand>::FlagData<T>::flagDataSize{};

template <typename SubCommand>
requires std::is_enum_v<SubCommand>
template <typename T>
std::array<Flags<SubCommand>::FlagData<T>::Flag, Flags<SubCommand>::FlagData<T>::MAX_FLAGS> Flags<SubCommand>::FlagData<T>::flagData{};
