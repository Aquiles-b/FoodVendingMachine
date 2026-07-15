#include <cctype>

#include <asio.hpp>
#include <iomanip>
#include <sstream>
#include <string_utils.hpp>

#include <food_vm_controller.hpp>

using namespace food_vm;

FoodVmController::FoodVmController(FoodVmDatabase& food_vm_db, asio::any_io_executor executor,
                                   NotificationCallback<ControllerEvent> client_notification)
    : m_food_vm_db{food_vm_db}, m_executor{executor}, m_client_notification{client_notification},
      m_controller_state{FoodVmControllerState::MENU}, m_order{food_vm_db}
{
    m_food_vm_payment = std::make_shared<FoodVmPayment>(
        m_executor, [this](PaymentEvent e) { payment_notification_callback(e); });
}

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
        return confirm_order();
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
        switch (m_food_vm_payment->get_payment_state()) {
        case PaymentState::CHOOSING_PAYMENT_METHOD:
            out << "\nInvalid option!\n" << "Available options:\n" << get_payment_options_message();
            break;
        case PaymentState::WAITING_FOR_PAYMENT:
            out << "Error reading the card. Please try again:\n"
                << m_food_vm_payment->request_payment(m_order.get_order_info().total_price);
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
        m_food_vm_payment->reset_payment_state();
        return "Payment canceled. Returning to menu.\n";
    }

    double money_input;
    std::vector<std::string> payment_options = m_food_vm_payment->list_payment_options();

    switch (m_food_vm_payment->get_payment_state()) {
    case PaymentState::CHOOSING_PAYMENT_METHOD:
        if (option > payment_options.size()) {
            out << "\nInvalid option!\n" << "Available options:\n" << get_payment_options_message();
            break;
        }
        out << "Option selected: " << payment_options[option - 1] << "\n";
        out << m_food_vm_payment->request_payment(m_order.get_order_info().total_price);
        break;
    case PaymentState::WAITING_FOR_PAYMENT:
        money_input = std::atof(response.data());
        out << "Processing payment...\n";
        m_food_vm_payment->process_payment(m_order.get_order_info().total_price, money_input);
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

void FoodVmController::payment_notification_callback(PaymentEvent e)
{
    ControllerEvent event;
    std::ostringstream out;

    switch (e.status) {
    case PaymentStatus::SUCCESS:
        m_order.register_event(FoodVmOrderStatus::PAYMENT_CONFIRMED);
        m_controller_state = FoodVmControllerState::MENU;
        m_food_vm_payment->reset_payment_state();

        out << "Payment successful! Thank you for your purchase.\n";
        out << "Order #" << m_order.get_order_info().id << " is being prepared.\n";
        m_order.register_event(FoodVmOrderStatus::PREPARATION_STARTED);
        m_order.register_order_products();

        event.message = out.str();
        m_client_notification.notify(event);
        break;
    case PaymentStatus::INSUFFICIENT_FUNDS:
        m_order.register_event(FoodVmOrderStatus::PAYMENT_FAILED);
        out << "Insufficient funds. Please try again:\n"
            << m_food_vm_payment->request_payment(m_order.get_order_info().total_price);
        event.message = out.str();
        m_client_notification.notify(event);
        break;
    case PaymentStatus::PAYMENT_FAILED:
        m_order.register_event(FoodVmOrderStatus::PAYMENT_FAILED);
        m_controller_state = FoodVmControllerState::MENU;
        m_food_vm_payment->reset_payment_state();

        event.message = "Payment failed. Returning to menu.\n";
        m_client_notification.notify(event);
        break;
    }
}

std::string FoodVmController::confirm_order()
{
    std::string error_msg;
    if (!m_order.separate_order(error_msg)) {
        return error_msg;
    }

    m_controller_state = FoodVmControllerState::PAYMENT;

    std::ostringstream out;

    out << "\nConfirming order...\n"
        << m_order.get_selected_products_table_message() << "Please select a payment option:\n"
        << get_payment_options_message();

    return out.str();
}

std::string FoodVmController::get_payment_options_message()
{
    std::vector<std::string> payment_options = m_food_vm_payment->list_payment_options();
    std::ostringstream out;
    for (size_t i = 0; i < payment_options.size(); i++) {
        out << i + 1 << "- " << payment_options[i] << "\n";
    }
    out << "0- Cancel\n";
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

    m_order.add_product(prods_to_add);

    out << m_order.get_selected_products_table_message();

    return out.str();
}

std::string FoodVmController::create_menu_table_message()
{
    std::ostringstream out;
    std::vector<ProductInfo> products = m_food_vm_db.get_product_info();

    out << std::string(35, '-') << "\n"
        << std::left << std::setw(4) << "ID" << std::setw(20) << "Name" << std::right
        << std::setw(10) << "Price"
        << "\n"
        << std::string(35, '-') << "\n";

    out << std::fixed << std::setprecision(2);

    for (const auto& prod : products) {
        if (!prod.enabled) {
            continue;
        }
        out << std::left << std::setw(4) << prod.id << std::setw(20) << prod.name << std::right
            << std::setw(10) << prod.price << '\n';
    }
    out << std::string(35, '-') << "\n\n";

    return out.str();
}
