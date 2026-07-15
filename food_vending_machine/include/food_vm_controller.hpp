#ifndef _FOOD_VM_CONTROLLER_
#define _FOOD_VM_CONTROLLER_

#include <unordered_map>
#include <vector>
#include <set>

#include <food_vm_database.hpp>
#include <food_vm_payment.hpp>

namespace food_vm {

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
        FoodVmController(FoodVmDatabase& food_vm_database);

        std::string parse_user_input(std::string user_input);

    private:
        FoodVmDatabase& m_food_vm_db;
        FoodVmControllerState m_controller_state;
        FoodVmPayment m_food_vm_payment;
        Order m_order;

        std::string select_products(const std::vector<std::string_view>& selected_prods);
        std::string set_payment_state();
        std::string handle_menu_state(const std::vector<std::string_view>& tokens);
        std::string handle_payment_state(const std::vector<std::string_view>& tokens);

        std::string create_menu_table_message();
        std::string get_selected_products_table_message(
            const std::unordered_map<size_t, ProductInfo> &prod_id_mapped);
        std::string get_payment_options_message();
};

}

#endif // _FOOD_VM_CONTROLLER_
