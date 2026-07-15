#ifndef _FOOD_VM_PAYMENT_HPP_
#define _FOOD_VM_PAYMENT_HPP_

#include <vector>
#include <string>
#include <asio.hpp>

#include <food_vm_notification.hpp>

namespace food_vm {

enum class PaymentState {
    CHOOSING_PAYMENT_METHOD,
    WAITING_FOR_PAYMENT,
    PROCESSING_PAYMENT,
    PAYMENT_SUCCESSFUL,
};

enum class PaymentStatus {
    SUCCESS,
    INSUFFICIENT_FUNDS,
    PAYMENT_FAILED,
};

struct PaymentEvent {
    PaymentStatus status;
};

class FoodVmPayment : public std::enable_shared_from_this<FoodVmPayment> {
  public:
    FoodVmPayment(asio::any_io_executor executor, NotificationCallback<PaymentEvent> callback);

    std::vector<std::string> list_payment_options();
    std::string request_payment(const long double& amount);

    PaymentState get_payment_state() const;
    void process_payment(const long double& amount, const long double& money_input);

    void reset_payment_state();

  private:
    PaymentState m_payment_state;
    asio::any_io_executor m_executor;
    FoodVmNotification<PaymentEvent> m_controller_notification;
};

} // namespace food_vm

#endif // _FOOD_VM_PAYMENT_HPP_
