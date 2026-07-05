#include <client_session.hpp>

using namespace food_vm;

ClientSession::ClientSession(asio::ip::tcp::socket socket):
    m_socket{std::move(socket)}
{}

ClientSession::~ClientSession()
{
    stop();
}

void ClientSession::start()
{
    read();
}

void ClientSession::stop() 
{
    m_socket.close();
}

void ClientSession::read()
{
    std::shared_ptr<ClientSession> self = shared_from_this();

    m_message.clear();
    asio::dynamic_string_buffer b = asio::dynamic_buffer(m_message);

    asio::async_read_until(m_socket, b, '\n', [self] (const asio::error_code& e, std::size_t bt) {
            if (e.value()) {
                std::cout << "Error on read: (" << e.value() << ") " << e.message() << "\n";
                return;
            }
            self->write();
        }
    );
}

void ClientSession::write()
{
    std::shared_ptr<ClientSession> self = shared_from_this();
    asio::async_write(m_socket, asio::buffer(m_message), [self](const asio::error_code& e, long unsigned int b) {
            if (e.value()) {
                std::cout << "Error on write: (" << e.value() << ") " << e.message() << "\n";
                return;
            }
            self->read();
        }
    );
}
