#ifndef _FOOD_VM_CONTROLLER_
#define _FOOD_VM_CONTROLLER_

#include <iostream>
#include <unordered_map>
#include <vector>

#include <food_vm_database.hpp>

namespace food_vm {

class FoodVmController {
    public:
        FoodVmController(FoodVmDatabase& food_vm_database);

        std::string parse_user_input(std::string user_input);

    private:
        FoodVmDatabase& m_food_vm_db;

        std::string create_menu_table_message();
};

}

#endif // _FOOD_VM_CONTROLLER_
