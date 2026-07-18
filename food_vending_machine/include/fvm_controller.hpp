#ifndef FVM_CONTROLLER_HPP
#define FVM_CONTROLLER_HPP

#include <asio/any_io_executor.hpp>
#include <vector>

#include <fvm_database.hpp>
#include <fvm_payment.hpp>
#include <fvm_notification.hpp>
#include <fvm_order.hpp>

namespace food_vm {

struct ControllerEvent {
    std::string message;
};

enum class FvmControllerState { MENU, PAYMENT };

class FvmController {
  public:
    FvmController(FvmDatabase& food_vm_database, asio::any_io_executor executor,
                     NotificationCallback<ControllerEvent> client_notification);

    std::string parse_user_input(std::string user_input);

  private:
    FvmDatabase& m_fvm_db;
    asio::any_io_executor m_executor;
    FvmNotification<ControllerEvent> m_client_notification;
    FvmControllerState m_controller_state;
    std::shared_ptr<FvmPayment> m_fvm_payment;
    FvmOrder m_order;

    std::string select_products(const std::vector<std::string_view>& selected_prods);
    std::string confirm_order();
    std::string handle_menu_state(const std::vector<std::string_view>& tokens);
    std::string handle_payment_state(const std::vector<std::string_view>& tokens);

    std::string create_menu_table_message();
    std::string get_payment_options_message();

    void payment_notification_callback(PaymentEvent e);
};

} // namespace food_vm

#endif // FVM_CONTROLLER_HPP
