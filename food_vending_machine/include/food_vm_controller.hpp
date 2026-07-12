#ifndef _FOOD_VM_CONTROLLER_
#define _FOOD_VM_CONTROLLER_

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>

#include <food_vm_database.hpp>

namespace food_vm {

class FoodVmController {
    public:
        FoodVmController(FoodVmDatabase& food_vm_database);

        std::string parse_user_input(std::string user_input);

    private:
        FoodVmDatabase& m_food_vm_db;
        std::set<size_t> m_product_selection;

        std::string create_menu_table_message();
        std::string select_products(const std::vector<std::string_view>& selected_prods);
        std::string get_selected_products_table_message(
            const std::unordered_map<size_t, ProductInfo> &prod_id_mapped);
};

}

#endif // _FOOD_VM_CONTROLLER_
