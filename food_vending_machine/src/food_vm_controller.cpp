#include <cctype>

#include <food_vm_controller.hpp>
#include <iomanip>
#include <sstream>
#include <string_utils.hpp>

using namespace food_vm;

FoodVmController::FoodVmController(FoodVmDatabase& food_vm_db)
    : m_food_vm_db{food_vm_db}, m_controller_state{FoodVmControllerState::MENU}
{}

std::string FoodVmController::parse_user_input(std::string user_input) 
{
    user_input = str_util::to_uppercase(user_input);
    std::vector<std::string_view> tokens = str_util::tokenize_input(user_input);

    if (tokens.empty()) {
        return "";
    }

    switch (m_controller_state) {
        case FoodVmControllerState::MENU:
            return handle_menu_state(tokens);
        case FoodVmControllerState::PAYMENT:
            return handle_payment_state(tokens);
    };

    assert(false && "Invalid controller state");
}

std::string FoodVmController::handle_menu_state(const std::vector<std::string_view>& tokens)
{
    std::string cmd(tokens[0]);

    if (cmd == "LIST") {
        return create_menu_table_message();
    }
    else if (cmd == "SELECT") {
        return select_products(tokens);
    }
    else if (cmd == "CONFIRM") {
        return set_payment_state();
    }
    else if (cmd == "QUIT") {
        return "Bye!\n";
    }

    return "Unknown command. Type help to see the available commands.\n";
}

std::string FoodVmController::handle_payment_state(const std::vector<std::string_view>& tokens)
{
    std::string response(tokens[0]);

    std::ostringstream out;

    if (!str_util::is_digit(response)) {
        switch (m_food_vm_payment.get_payment_state()) {
            case PaymentState::CHOOSING_PAYMENT_METHOD:
                out << "Invalid option!\n" << "Available options:\n" << get_payment_options_message();
                break;
            case PaymentState::WAITING_FOR_PAYMENT:
                out << "Error reading the card. Please try again:\n";
                break;
            case PaymentState::PROCESSING_PAYMENT:
                out << "Processing payment...\n";
                break;
            case PaymentState::PAYMENT_SUCCESSFUL:
                out << "Payment successful! Thank you for your purchase.\n";
                break;
        }
        return out.str();
    }

    size_t option = std::atoi(response.data());

    if (option == 0) {
        m_controller_state = FoodVmControllerState::MENU;
        m_food_vm_payment.set_payment_state(PaymentState::CHOOSING_PAYMENT_METHOD);
        return "Payment canceled. Returning to menu.\n";
    }

    double money_input;
    std::vector<std::string> payment_options = m_food_vm_payment.list_payment_options();

    switch (m_food_vm_payment.get_payment_state()) {
        case PaymentState::CHOOSING_PAYMENT_METHOD:
            if (option >= payment_options.size()) {
                out << "Invalid option!\n" << "Available options:\n" << get_payment_options_message();
                break;
            }
            out << "Option selected: " << payment_options[option] << "\n";
            out << m_food_vm_payment.request_payment(m_order.total_price)
                << "\n0 - Cancel\n";
            m_food_vm_payment.set_payment_state(PaymentState::WAITING_FOR_PAYMENT);
            break;
        case PaymentState::WAITING_FOR_PAYMENT:
            money_input = std::atof(response.data());
            out << m_food_vm_payment.process_payment(m_order.total_price, money_input) << "\n";
            break;
        case PaymentState::PROCESSING_PAYMENT:
            out << "Processing payment...\n";
            break;
        case PaymentState::PAYMENT_SUCCESSFUL:
            out << "Payment successful! Thank you for your purchase.\n";
            break;
    }

    return out.str();
}

std::string FoodVmController::set_payment_state() 
{
    m_controller_state = FoodVmControllerState::PAYMENT;
    std::ostringstream out;
    out << get_payment_options_message()
        << "0 - Cancel\n";

    return out.str();
}

std::string FoodVmController::get_payment_options_message()
{
    std::vector<std::string> payment_options = m_food_vm_payment.list_payment_options();
    std::ostringstream out;
    for (size_t i = 0; i < payment_options.size(); i++) {
        out << i + 1 << "- " << payment_options[i] << "\n";
    }
    out << "\n";

    return out.str();
}

std::string FoodVmController::select_products(const std::vector<std::string_view>& selected_prods)
{
    std::unordered_map<std::string, ProductInfo> prod_name_mapped =
        m_food_vm_db.get_product_info_name_mapped();
    std::unordered_map<size_t, ProductInfo> prod_id_mapped =
        m_food_vm_db.get_product_info_id_mapped();

    std::vector<size_t> prods_to_add;
    std::vector<std::string_view> unknown_prods;

    std::vector<std::string_view>::const_iterator it = selected_prods.begin();

    // Skip command
    it++;

    while (it != selected_prods.end()) {
        if (str_util::is_digit(*it)) {
            auto prod = prod_id_mapped.find(std::atoi(it->data()));
            if (prod == prod_id_mapped.end() || !prod->second.enabled) {
                unknown_prods.emplace_back(*it);
            }
            else {
                prods_to_add.emplace_back(prod->second.id);
            }
        }
        else {
            auto prod = prod_name_mapped.find(std::string(*it));
            if (prod == prod_name_mapped.end() || !prod->second.enabled) {
                unknown_prods.emplace_back(*it);
            }
            else {
                prods_to_add.emplace_back(prod->second.id);
            }
        }
        it++;
    }

    std::ostringstream out;

    if (!unknown_prods.empty()) {
        out << "Unknown products: ";
        for (const std::string_view& p : unknown_prods) {
            out << "\"" << p << "\" ";
        }
        out << "\n";
    }

    for (const size_t& id : prods_to_add) {
        m_order.product_ids.insert(id);
    }

    out << get_selected_products_table_message(prod_id_mapped);

    return out.str();
}

std::string FoodVmController::get_selected_products_table_message(
    const std::unordered_map<size_t, ProductInfo>& prod_id_mapped) 
{
    std::ostringstream out;

    out << std::string(19, '-') << "\n"
        << "Selected products\n"
        << std::string(19, '-') << "\n";


    long double total_price = 0;
    out << std::fixed << std::setprecision(2);

    for (const size_t& p : m_order.product_ids) {
        auto prod = prod_id_mapped.find(p);
        if (prod != prod_id_mapped.end()) {
            out << std::left << std::setw(10) << prod->second.name
                << std::right << std::setw(8) << prod->second.price
                << "\n";
            total_price += prod->second.price;
        }
    }
    out << std::string(19, '-') << "\n";
    out << "Total:" << std::right << std::setw(12) << total_price << "\n";
    out << std::string(19, '-') << "\n";

    return out.str();
}

std::string FoodVmController::create_menu_table_message() 
{
    std::ostringstream out;
    std::vector<ProductInfo> products = m_food_vm_db.get_product_info();

    out << std::string(35, '-') << "\n" 
        << std::left << std::setw(4) << "ID"
        << std::setw(20) << "Name"
        << std::right << std::setw(10) << "Price"
        << "\n"
        << std::string(35, '-') << "\n";

    out << std::fixed << std::setprecision(2);

    for (const auto& prod : products) {
        if (!prod.enabled) {
            continue;
        }
        out << std::left << std::setw(4) << prod.id
            << std::setw(20) << prod.name 
            << std::right << std::setw(10) << prod.price 
            << '\n';
    }
    out << std::string(35, '-') << "\n";

    return out.str();
}
