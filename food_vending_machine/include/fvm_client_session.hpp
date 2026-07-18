#ifndef FVM_CLIENT_SESSION_HPP
#define FVM_CLIENT_SESSION_HPP

#include <asio.hpp>

#include <fvm_database.hpp>
#include <fvm_controller.hpp>
#include <fvm_notification.hpp>

namespace food_vm {

class FvmClientSession : public std::enable_shared_from_this<FvmClientSession> {
  public:
    FvmClientSession(asio::ip::tcp::socket socket);
    ~FvmClientSession();

    void start();
    void stop();
    void read(std::shared_ptr<FvmClientSession> self);
    void write(std::shared_ptr<FvmClientSession> self);

  private:
    asio::ip::tcp::socket m_socket;
    FvmDatabase m_fvm_db;
    FvmController m_fvm_controller;
    std::string m_message;

    void controller_notification_callback(ControllerEvent e);
};

} // namespace food_vm

#endif // FVM_CLIENT_SESSION_HPP
