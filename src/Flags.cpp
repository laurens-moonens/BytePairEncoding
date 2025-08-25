#include "Flags.h"

//template <>
//std::string FlagParser<std::string>::ParseFlag(std::string_view argv)
//{
//    return std::string{argv};
//}
//
//template <>
//int FlagParser<int>::ParseFlag(std::string_view argv)
//{
//}

template <>
void FlagInfo<std::string>::SetData(std::string_view argv)
{
    data = std::string{argv};
}

template <>
void FlagInfo<int>::SetData(std::string_view argv)
{
    data = std::stoi(std::string{argv});
}

template <>
void FlagInfo<bool>::SetData(std::string_view argv)
{
    (void)argv;
    // TODO:
    data = false;
}
