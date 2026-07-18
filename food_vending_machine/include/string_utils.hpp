#ifndef FVM_STRING_UTILS_HPP
#define FVM_STRING_UTILS_HPP

#include <string>
#include <vector>

namespace food_vm::str_util {

std::vector<std::string_view> tokenize_input(std::string_view input);
std::string to_uppercase(const std::string& s);
bool is_digit(std::string_view s);

} // namespace food_vm::str_util

#endif // FVM_STRING_UTILS_HPP
