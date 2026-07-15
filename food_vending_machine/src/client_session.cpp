#include <client_session.hpp>
#include <sstream>

using namespace food_vm;

ClientSession::ClientSession(asio::ip::tcp::socket socket)
    : m_socket{std::move(socket)},
      m_food_vm_db{"tcp://127.0.0.1:3306", "my_user", "my_password", "my_database"},
      m_controller{m_food_vm_db, m_socket.get_executor(),
                   [this](ControllerEvent e) { controller_notification_callback(e); }}
{
}

ClientSession::~ClientSession()
{
    stop();
}

void ClientSession::controller_notification_callback(ControllerEvent e)
{
    std::shared_ptr<ClientSession> self = shared_from_this();
    std::string message = e.message;
    asio::async_write(m_socket, asio::buffer(message),
                      [self, message](const asio::error_code& e, size_t b) {
                          if (e.value()) {
                              std::cout << "Error on notification write: (" << e.value() << ") "
                                        << e.message() << "bytes: " << b << "\n";
                              return;
                          }
                      });
}

void ClientSession::start()
{
    auto endpoint = m_socket.remote_endpoint();
    std::cout << "\"" << endpoint.address() << ":" << endpoint.port() << "\" Connected\n";

    std::ostringstream out;
    out << "+" << std::string(52, '-') << "+\n"
        << "| Welcome to FoodVM! Type \"help\" to see the commands |\n"
        << "+" << std::string(52, '-') << "+\n";
    m_message = out.str();

    std::shared_ptr<ClientSession> self = shared_from_this();
    write(self);
}

void ClientSession::stop()
{
    auto endpoint = m_socket.remote_endpoint();
    std::cout << "\"" << endpoint.address() << ":" << endpoint.port() << "\" Disconnected\n";

    m_socket.close();
}

void ClientSession::read(std::shared_ptr<ClientSession> self)
{
    m_message.clear();
    asio::dynamic_string_buffer b = asio::dynamic_buffer(m_message);

    asio::async_read_until(m_socket, b, '\n', [self](const asio::error_code& e, size_t b) {
        if (e.value()) {
            std::cout << "Error on read: (" << e.value() << ") " << e.message() << "bytes: " << b
                      << "\n";
            return;
        }
        self->m_message = self->m_controller.parse_user_input(self->m_message);
        self->write(self);
    });
}

void ClientSession::write(std::shared_ptr<ClientSession> self)
{
    asio::async_write(m_socket, asio::buffer(m_message),
                      [self](const asio::error_code& e, size_t b) {
                          if (e.value()) {
                              std::cout << "Error on write: (" << e.value() << ") " << e.message()
                                        << "bytes: " << b << "\n";
                              return;
                          }
                          if (self->m_message == "Bye!\n") {
                              return;
                          }
                          self->read(self);
                      });
}
