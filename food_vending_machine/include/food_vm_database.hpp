#ifndef _FOOD_VM_DATABASE_
#define _FOOD_VM_DATABASE_

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

namespace food_vm {


struct ProductInfo {
    size_t id;
    std::string name;
    long double price;
    size_t preparation_time;
    bool enabled;
};

class FoodVmDatabase {
    public:
        FoodVmDatabase(const std::string &host, const std::string &user,
                const std::string &pass, const std::string &database);
        ~FoodVmDatabase();

        std::vector<ProductInfo> get_product_info();

    private:
        std::unique_ptr<sql::Connection> m_connection;
};

}

#endif // _FOOD_VM_DATABASE_
