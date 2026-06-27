#include <iostream>
#include <asio.hpp>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

int main()
{
//     asio::io_context io;
//     asio::steady_timer timer(io, asio::chrono::seconds(3));

//     std::cout << "Waiting..\n";
//     timer.async_wait([](const asio::error_code& e) {
//             std::cout << "Done: e= " << e.message() << "\n";
//     });

//     io.run();

    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();

    sql::Connection* connection = driver->connect("tcp://127.0.0.1:3306", "my_user", "my_password");

    connection->setSchema("my_database");

    sql::Statement* statement = connection->createStatement();

    sql::ResultSet* result = statement->executeQuery("SELECT * FROM products");

    while(result->next())
    {
        std::cout << result->getInt("id") << " " << result->getString("name") << " " << result->getInt("price") << '\n';
    }

    // delete result;
    delete statement;
    delete connection;
}
