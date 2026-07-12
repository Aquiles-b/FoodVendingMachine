#include <food_vm_controller.hpp>
#include <iomanip>
#include <sstream>

using namespace food_vm;

// Aux functions 
static std::vector<std::string_view> tokenize_input(std::string_view input);
static std::string to_uppercase(const std::string& str);

FoodVmController::FoodVmController(FoodVmDatabase& food_vm_db):
    m_food_vm_db{food_vm_db}
{}

std::string FoodVmController::parse_user_input(std::string user_input) 
{
    user_input = to_uppercase(user_input);
    std::vector<std::string_view> tokens = tokenize_input(user_input);

    if (tokens.empty()) {
        return "\n";
    }

    std::string cmd(tokens[0]);

    if (cmd == "LIST") {
        return create_menu_table_message();
    }
    else if (cmd == "BUY") {
        return "Buying...\n";
    }
    else if (cmd == "QUIT") {
        return "Bye!\n";
    }

    return "Unknown command. Type help to see the available commands.\n";
}

std::string FoodVmController::create_menu_table_message() {
    std::ostringstream out;
    std::vector<ProductInfo> products = m_food_vm_db.get_product_info();

       
    out << std::string(31, '-') << "\n" 
        << std::left << std::setw(20) << "Name"
        << std::right << std::setw(10) << "Price"
        << "\n"
        << std::string(31, '-') << "\n";

    out << std::fixed << std::setprecision(2);

    for (const auto& prod : products) {
        out << std::left << std::setw(20) << prod.name 
            << std::right << std::setw(10) << prod.price 
            << '\n';
    }
    out << std::string(31, '-') << "\n";

    return out.str();
}


// Aux functions -----------------------------------------------------

static std::vector<std::string_view> tokenize_input(std::string_view input) {
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

static std::string to_uppercase(const std::string& str)
{
    std::string result(str);

    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) {
            return std::toupper(c);
        });

    return result;
}
