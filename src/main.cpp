#include <iostream>
#include <map>

std::string input = "aaabdaaabac";
//std::string input = "In the above example, the output of the BPE is a vocabulary, which can be used to encode any text that is written with the letters abcd. It will not be able to encode text containing other symbols, such as no. Even giving each of the 26 letters an entry in the vocabulary, since there are many languages in the world using many different scripts, inevitably some symbols would be unencodable by such a vocabulary.";

std::map<std::tuple<char, char>, int> pairCounts = std::map<std::tuple<char, char>, int>();

int main()
{
    for(int i = 0; i < input.length() - 1; ++i)
    {
        std::tuple<char, char> pair {input[i], input[i + 1]};
        pairCounts[pair] += 1;

        std::cout << std::get<0>(pair) << std::get<1>(pair) << " = " << pairCounts[pair] << std::endl;
    }

    return 0;
}
