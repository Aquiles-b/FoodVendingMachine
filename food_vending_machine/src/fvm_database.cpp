#include <fvm_database.hpp>
#include <sstream>
#include <string_utils.hpp>

using namespace food_vm;

FvmDatabase::FvmDatabase(const std::string& host, const std::string& user,
                               const std::string& pass, const std::string& database)
{
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    m_connection = std::unique_ptr<sql::Connection>(driver->connect(host, user, pass));

    m_connection->setSchema(database);
}

FvmDatabase::~FvmDatabase() {}

std::vector<ProductInfo> FvmDatabase::get_product_info()
{
    std::unique_ptr<sql::Statement> statement(m_connection->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery("SELECT * FROM products"));

    std::vector<ProductInfo> products;

    while (result->next()) {
        products.emplace_back(ProductInfo{.id = result->getUInt64("id"),
                                          .name = result->getString("name"),
                                          .price = result->getDouble("price"),
                                          .preparation_time = result->getUInt64("preparation_time"),
                                          .enabled = result->getBoolean("enabled")});
    }

    return products;
}

std::unordered_map<std::string, ProductInfo> FvmDatabase::get_product_info_name_mapped()
{
    std::vector<ProductInfo> prods = get_product_info();
    std::unordered_map<std::string, ProductInfo> prod_info_name_mapped;

    for (const auto& p : prods) {
        std::string k = str_util::to_uppercase(p.name);
        prod_info_name_mapped[k] = p;
    }

    return prod_info_name_mapped;
}

std::unordered_map<size_t, ProductInfo> FvmDatabase::get_product_info_id_mapped()
{
    std::vector<ProductInfo> prods = get_product_info();
    std::unordered_map<size_t, ProductInfo> prod_info_id_mapped;

    for (const auto& p : prods) {
        prod_info_id_mapped[p.id] = p;
    }

    return prod_info_id_mapped;
}

OrderInfo FvmDatabase::create_order(std::string initial_state)
{
    std::unique_ptr<sql::Statement> statement(m_connection->createStatement());

    statement->execute("INSERT INTO orders (state, total_price) VALUES ('CREATED', 0.0)");

    std::unique_ptr<sql::ResultSet> result(
        statement->executeQuery("SELECT LAST_INSERT_ID() AS id"));

    if (result->next()) {
        size_t order_id = result->getUInt64("id");
        return OrderInfo{.id = order_id,
                         .product_ids = std::set<size_t>(),
                         .state = initial_state,
                         .total_price = 0.0};
    }
    throw std::runtime_error("Error creating order: could not retrieve last insert ID.");
}

void FvmDatabase::register_order_event(std::string_view event, OrderInfo& order_info)
{
    std::unique_ptr<sql::Statement> statement(m_connection->createStatement());

    std::string event_str(event);
    std::string query = "INSERT INTO order_events (order_id, event, time) VALUES (" +
                        std::to_string(order_info.id) + ", '" + event_str + "', NOW())";

    statement->execute(query);

    // Update the order state in the orders table
    std::string update_query =
        "UPDATE orders SET state = '" + event_str + "' WHERE id = " + std::to_string(order_info.id);
    statement->execute(update_query);

    // Update the order_info state
    order_info.state = event_str;
}

bool FvmDatabase::separate_order(OrderInfo& order_info, std::string& error_msg)
{
    if (order_info.product_ids.empty()) {
        error_msg = "Order has no products to separate.";
        return false;
    }

    auto statement = std::unique_ptr<sql::Statement>(m_connection->createStatement());
    std::unordered_map<size_t, ProductInfo> prod_id_mapped = get_product_info_id_mapped();

    std::ostringstream out;

    try {
        m_connection->setAutoCommit(false);

        for (auto it = order_info.product_ids.begin(); it != order_info.product_ids.end();) {
            size_t product_id = *it;

            std::string check_query =
                "SELECT quantity FROM inventory WHERE product_id = " + std::to_string(product_id);

            auto result = std::unique_ptr<sql::ResultSet>(statement->executeQuery(check_query));

            if (!result->next()) {
                out << "Product " << prod_id_mapped[product_id].name
                    << " was not found in inventory.\n";

                it = order_info.product_ids.erase(it);
                continue;
            }

            int quantity = result->getInt("quantity");

            if (quantity <= 0) {
                out << "Product " << prod_id_mapped[product_id].name << " is out of stock.\n";
                it = order_info.product_ids.erase(it);
                continue;
            }

            std::string update_query = "UPDATE inventory "
                                       "SET quantity = quantity - 1 "
                                       "WHERE product_id = " +
                                       std::to_string(product_id);

            statement->execute(update_query);
            ++it;
        }
        m_connection->commit();
    }
    catch (const sql::SQLException& e) {
        m_connection->rollback();
        m_connection->setAutoCommit(true);

        error_msg = e.what();
        return false;
    }

    m_connection->setAutoCommit(true);

    if (!out.str().empty()) {
        error_msg = out.str();
        return false;
    }

    return !order_info.product_ids.empty();
}

void FvmDatabase::register_order_products(OrderInfo& order_info)
{
    if (order_info.product_ids.empty()) {
        return;
    }

    std::unordered_map<size_t, ProductInfo> prod_id_mapped = get_product_info_id_mapped();

    auto statement = std::unique_ptr<sql::Statement>(m_connection->createStatement());

    for (const size_t& product_id : order_info.product_ids) {
        std::string query = "INSERT INTO order_items (order_id, product_id, unit_price) VALUES (" +
                            std::to_string(order_info.id) + ", " + std::to_string(product_id) +
                            ", " + std::to_string(prod_id_mapped[product_id].price) + ")";

        statement->execute(query);
    }
}
