#ifndef _FOOD_VM_CONTROLLER_
#define _FOOD_VM_CONTROLLER_

#include <asio/any_io_executor.hpp>
#include <unordered_map>
#include <vector>
#include <set>

#include <food_vm_database.hpp>
#include <food_vm_payment.hpp>
#include <food_vm_notification.hpp>

namespace food_vm {

struct ControllerEvent {
    std::string message;
};

enum class FoodVmControllerState {
    MENU,
    PAYMENT
};

struct Order {
    std::set<size_t> product_ids;
    long double total_price;
};

class FoodVmController {
    public:
        FoodVmController(FoodVmDatabase& food_vm_database, asio::any_io_executor executor,
                NotificationCallback<ControllerEvent> client_notification);

        std::string parse_user_input(std::string user_input);

    private:
        FoodVmDatabase& m_food_vm_db;
        asio::any_io_executor m_executor;
        FoodVmNotification<ControllerEvent> m_client_notification;
        FoodVmControllerState m_controller_state;
        std::shared_ptr<FoodVmPayment> m_food_vm_payment;
        Order m_order;

        std::string select_products(const std::vector<std::string_view>& selected_prods);
        std::string confirm_order();
        std::string handle_menu_state(const std::vector<std::string_view>& tokens);
        std::string handle_payment_state(const std::vector<std::string_view>& tokens);

        std::string create_menu_table_message();
        std::string get_selected_products_table_message(
            const std::unordered_map<size_t, ProductInfo> &prod_id_mapped);
        std::string get_payment_options_message();

        void payment_notification_callback(PaymentEvent e);
};

}

#endif // _FOOD_VM_CONTROLLER_
