#ifndef FVM_DATABASE_HPP
#define FVM_DATABASE_HPP

#include <set>
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

struct OrderInfo {
    size_t id;
    std::set<size_t> product_ids;
    std::string state;
    long double total_price;
};

class FvmDatabase {
  public:
    FvmDatabase(const std::string& host, const std::string& user, const std::string& pass,
                   const std::string& database);
    ~FvmDatabase();

    std::vector<ProductInfo> get_product_info();
    std::unordered_map<std::string, ProductInfo> get_product_info_name_mapped();
    std::unordered_map<size_t, ProductInfo> get_product_info_id_mapped();

    OrderInfo create_order(std::string initial_state);
    void register_order_event(std::string_view event, OrderInfo& order_info);
    bool separate_order(OrderInfo& order_info, std::string& error_msg);
    void register_order_products(OrderInfo& order_info);

  private:
    std::unique_ptr<sql::Connection> m_connection;
};

} // namespace food_vm

#endif // FVM_DATABASE_HPP
