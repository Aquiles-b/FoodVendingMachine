#ifndef _FOOD_VM_CONTROLLER_
#define _FOOD_VM_CONTROLLER_

#include <iostream>
#include <unordered_map>
#include <vector>

#include <food_vm_database.hpp>

namespace food_vm {

class FoodVmController {
    public:
        FoodVmController();

        std::string parse_user_input(std::string_view user_input);

    private:
        int state;
        std::unique_ptr<FoodVmDatabase> m_food_vm_db;

        std::vector<std::string_view> tokenize_input(std::string_view input);
};

}

#endif // _FOOD_VM_CONTROLLER_
