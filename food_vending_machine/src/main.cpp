#include <iostream>
#include <asio.hpp>

#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

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

class ClientSession {
    public:
        ClientSession(asio::ip::tcp::socket socket):
            m_socket{std::move(socket)} {
                m_message = "Hello!\n";
                send();
            }

        void send() {
            asio::async_write(m_socket, asio::buffer(m_message), [](const asio::error_code& e, long unsigned int b) {
                    std::cout << "Write finished!\n";
                }
            );
        }

    private:
        asio::ip::tcp::socket m_socket;
        std::string m_message;
};

void start_server(asio::io_context& io, asio::ip::tcp::acceptor& acp) {
    acp.async_accept([&io, &acp](const asio::error_code& ec, asio::ip::tcp::socket socket) {
            ClientSession* cs = new ClientSession(std::move(socket));
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
