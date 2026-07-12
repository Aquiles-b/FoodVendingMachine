#include <food_vm_database.hpp>

using namespace food_vm;

FoodVmDatabase::FoodVmDatabase(const std::string &host, const std::string &user,
        const std::string &pass, const std::string &database)
{
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    m_connection = std::unique_ptr<sql::Connection>(driver->connect(host, user, pass));

    m_connection->setSchema(database);
}

FoodVmDatabase::~FoodVmDatabase()
{
}

std::vector<ProductInfo> FoodVmDatabase::get_product_info()
{
    std::unique_ptr<sql::Statement> statement(m_connection->createStatement());
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery("SELECT * FROM products"));

    std::vector<ProductInfo> products;

    while(result->next()) {
        products.emplace_back(
            ProductInfo{
                .id = result->getUInt64("id"),
                .name = result->getString("name"),
                .price = result->getDouble("price"),
                .preparation_time = result->getUInt64("preparation_time"),
                .enabled = result->getBoolean("enabled")
            }
        );
    }

    return products;
}
