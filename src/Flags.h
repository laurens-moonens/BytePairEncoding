#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <vector>

template <typename SubCommand>
    requires std::is_enum_v<SubCommand>
class Flags
{
public:
    struct Flag
    {
        std::string shortFlag;
        std::string longFlag;
        std::string flagParameterName;
        std::string description;
        SubCommand subCommand;
        bool mandatory;
    };

    //Flags<SubCommand>();

    static void AddSubCommand(std::string name);

    template <typename T>
    void AddString(std::string name, SubCommand subCommand, bool mandatory);
    void AddString(Flag flag);

    static std::vector<std::string> SubCommands;
    //static std::unordered_map<std::type_info, Flag> Flags;

private:
    //std::vector<>
};
