#ifndef _CLIENT_SESSION_HPP_
#define _CLIENT_SESSION_HPP_

#include <asio.hpp>
#include <iostream>
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
        std::string m_message;
        FoodVmController m_controller;

};

}

#endif  // _CLIENT_SESSION_HPP_
