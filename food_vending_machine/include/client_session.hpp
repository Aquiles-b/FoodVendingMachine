#ifndef _CLIENT_SESSION_HPP_
#define _CLIENT_SESSION_HPP_

#include <asio.hpp>

#include <food_vm_database.hpp>
#include <food_vm_controller.hpp>

namespace food_vm {

class ClientSession : public std::enable_shared_from_this<ClientSession> {
    public:
        ClientSession(asio::ip::tcp::socket socket);
        ~ClientSession();

        void start();
        void stop();
        void read();
        void write();

    private:
        asio::ip::tcp::socket m_socket;
        FoodVmDatabase m_food_vm_db;
        FoodVmController m_controller;
        std::string m_message;
};

}

#endif  // _CLIENT_SESSION_HPP_
