#include <iostream>
#include <asio.hpp>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

#include <client_session.hpp>

using namespace food_vm;

void mysql_conn_example()
{
    sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
    sql::Connection* connection = driver->connect("tcp://127.0.0.1:3306", "my_user", "my_password");

    connection->setSchema("my_database");

    sql::Statement* statement = connection->createStatement();
    sql::ResultSet* result = statement->executeQuery("SELECT * FROM products");

    while(result->next()) {
        std::cout << result->getInt("id") << " " << result->getString("name") << " " << result->getInt("price") << '\n';
    }

    delete result;
    delete statement;
    delete connection;
}

void start_server(asio::io_context& io, asio::ip::tcp::acceptor& acp)
{
    acp.async_accept([&io, &acp](const asio::error_code& e, asio::ip::tcp::socket socket) {
            if (e.value()) {
                std::cout << "Error on accept: (" << e.value() << ") " << e.message() << "\n";
                return;
            }
            std::shared_ptr<ClientSession> cs = std::make_shared<ClientSession>(std::move(socket));
            cs->start();
            start_server(io, acp);
    });
}

int main()
{
    asio::io_context io;
    asio::ip::tcp::acceptor acp(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 6666));
    start_server(io, acp);
    std::cout << "Server started!\n";
    io.run();
}
