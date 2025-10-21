#include "Flags.h"

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
