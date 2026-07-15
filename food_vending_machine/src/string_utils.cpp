#include <algorithm>
#include <string_utils.hpp>

namespace food_vm::str_util {

std::vector<std::string_view> tokenize_input(std::string_view input)
{
    std::vector<std::string_view> tokens;
    std::string_view separators = " \r\n\t";
    std::string_view::size_type begin, end;

    while (!input.empty()) {
        begin = input.find_first_not_of(separators);
        if (begin == std::string_view::npos) {
            break;
        }

        input.remove_prefix(begin);

        end = input.find_first_of(separators);

        tokens.push_back(input.substr(0, end));

        if (end == std::string_view::npos) {
            break;
        }

        input.remove_prefix(end);
    }

    return tokens;
}

std::string to_uppercase(const std::string& s)
{
    std::string result(s);

    std::transform(s.begin(), s.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    return result;
}

bool is_digit(std::string_view s)
{
    return !s.empty() && s.find_first_not_of("1234567890") == std::string_view::npos;
}

} // namespace food_vm::str_util
