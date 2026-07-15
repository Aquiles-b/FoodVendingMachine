#include <food_vm_payment.hpp>
#include <iomanip>
#include <sstream>
#include <iostream>

using namespace food_vm;

FoodVmPayment::FoodVmPayment(asio::any_io_executor executor, NotificationCallback<PaymentEvent> controller_callback)
    : m_payment_state(PaymentState::CHOOSING_PAYMENT_METHOD),
      m_executor(executor),
      m_controller_notification(controller_callback)
{
}

PaymentState FoodVmPayment::get_payment_state() const
{
    return m_payment_state;
}

void FoodVmPayment::reset_payment_state()
{
    m_payment_state = PaymentState::CHOOSING_PAYMENT_METHOD;
}

std::vector<std::string> FoodVmPayment::list_payment_options()
{
    return std::vector<std::string> {
        "Debit",
        "Credit"
    };

}

std::string FoodVmPayment::request_payment(const long double& amount)
{
    m_payment_state = PaymentState::WAITING_FOR_PAYMENT;

    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << "\n$" << amount << "\nTap or insert the card:\n0- Cancel\n";
    return out.str();
}

void FoodVmPayment::process_payment(const long double& amount, const long double& money_input)
{
    std::shared_ptr<FoodVmPayment> self = shared_from_this();
    self->m_payment_state = PaymentState::PROCESSING_PAYMENT;

    std::shared_ptr<asio::steady_timer> timer = std::make_shared<asio::steady_timer>(self->m_executor, std::chrono::seconds(1));

    timer->async_wait([self, timer, amount, money_input](const asio::error_code& ec) {
        PaymentEvent event;
        if (ec.value()) {
            std::cerr << "Error processing payment: " << ec.message() << std::endl;
            event.status = PaymentStatus::PAYMENT_FAILED;
            self->m_controller_notification.notify(event);
            return;
        }

        if (money_input < amount) {
            event.status = PaymentStatus::INSUFFICIENT_FUNDS;
            self->m_controller_notification.notify(event);
            return;
        }

        std::shared_ptr<asio::steady_timer> t =
            std::make_shared<asio::steady_timer>(
                self->m_executor, std::chrono::seconds(rand() % 5 + 1));

        t->async_wait([self, t] (const asio::error_code& ec) {
            PaymentEvent event;
            if (ec.value()) {
                std::cerr << "Error processing payment: " << ec.message() << std::endl;
                event.status = PaymentStatus::PAYMENT_FAILED;
                self->m_controller_notification.notify(event);
                return;
            }

            self->m_payment_state = PaymentState::PAYMENT_SUCCESSFUL;
            event.status = PaymentStatus::SUCCESS;
            self->m_controller_notification.notify(event);
        });
    });
}
