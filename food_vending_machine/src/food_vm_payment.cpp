#include <food_vm_payment.hpp>
#include <iomanip>
#include <sstream>

using namespace food_vm;

FoodVmPayment::FoodVmPayment()
    : m_payment_state(PaymentState::CHOOSING_PAYMENT_METHOD)
{
}

void FoodVmPayment::set_payment_state(PaymentState state)
{
    m_payment_state = state;
}

PaymentState FoodVmPayment::get_payment_state() const
{
    return m_payment_state;
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
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);
    out << "\n$" << amount << "\nTap or insert the card:\n";
    return out.str();
}

std::string FoodVmPayment::process_payment(const long double& amount, const long double& money_input)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2);

    if (money_input < amount) {
        out << "Insufficient funds. Please try again:\n";
        return out.str();
    }

    m_payment_state = PaymentState::PROCESSING_PAYMENT;
    out << "Processing...\n";

    return out.str();
}
