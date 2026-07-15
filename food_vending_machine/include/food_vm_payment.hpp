#ifndef _FOOD_VM_PAYMENT_HPP_
#define _FOOD_VM_PAYMENT_HPP_

#include <vector>
#include <string>

namespace food_vm {

enum class PaymentState {
    CHOOSING_PAYMENT_METHOD,
    WAITING_FOR_PAYMENT,
    PROCESSING_PAYMENT,
    PAYMENT_SUCCESSFUL,
};

class FoodVmPayment {
    public:
        FoodVmPayment();

        std::vector<std::string> list_payment_options();
        std::string request_payment(const long double& amount);

        void set_payment_state(PaymentState state);
        PaymentState get_payment_state() const;
        std::string process_payment(const long double& amount, const long double& money_input);

    private:
        PaymentState m_payment_state;
};

} // food_vm

#endif // _FOOD_VM_PAYMENT_HPP_
