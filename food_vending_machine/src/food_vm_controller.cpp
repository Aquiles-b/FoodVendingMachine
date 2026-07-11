#include <food_vm_controller.hpp>

using namespace food_vm;

FoodVmController::FoodVmController()
{
    m_food_vm_db = std::make_unique<FoodVmDatabase>("tcp://127.0.0.1:3306", "my_user", "my_password", "my_database");
}

std::string FoodVmController::parse_user_input(std::string_view user_input) 
{
    std::vector<std::string_view> tokens = tokenize_input(user_input);

    if (tokens.empty()) {
        return "\n";
    }

    std::string_view cmd = tokens[0];
    if (cmd == "LIST") {
        std::vector<ProductInfo> products = m_food_vm_db->get_product_info();

        std::string result = "ID | Name | Price | Preparation time | Enabled\n";

        for (auto prod : products) {
            result += std::to_string(prod.id) + " | "
                      + prod.name + " | "
                      + std::to_string(prod.price) + " | "
                      + std::to_string(prod.preparation_time) + " | " 
                      + (prod.enabled ? "Yes" : "No") + "\n";
        }


        return result;
    }


    return "Unknown command. Type help to see the available commands.\n";
}

std::vector<std::string_view> FoodVmController::tokenize_input(std::string_view input) {
    std::vector<std::string_view> tokens;
    std::string_view separators = " \r\n\t";
    std::string_view::size_type begin, end;

    while (!input.empty())
    {
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
