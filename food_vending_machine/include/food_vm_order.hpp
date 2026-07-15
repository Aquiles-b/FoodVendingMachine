#ifndef _FOOD_VM_ORDER_HPP_
#define _FOOD_VM_ORDER_HPP_

#include <vector>

#include <food_vm_database.hpp>

namespace food_vm {

enum class FoodVmOrderStatus {
    CREATED,
    CONFIRMED,
    PAYMENT_CONFIRMED,
    PAYMENT_FAILED,
    PREPARATION_STARTED,
    READY,
    PICKED_UP,
    CANCELLED
};

class FoodVmOrder {
  public:
    FoodVmOrder(FoodVmDatabase& food_vm_db);

    const OrderInfo& get_order_info() const;
    void clear_order();
    void add_product(std::vector<size_t> product_ids);
    void register_event(FoodVmOrderStatus event);
    bool separate_order(std::string& error_msg);
    void register_order_products();

    const std::string& get_selected_products_table_message() const;

  private:
    OrderInfo m_order;
    FoodVmDatabase& m_food_vm_db;
    std::string m_selected_products_table_message;
    bool m_processing_order;

    void
    update_selected_products_table_message(std::unordered_map<size_t, ProductInfo>& prod_id_mapped);
    void create_order();

    static std::string order_event_to_string(FoodVmOrderStatus event)
    {
        switch (event) {
        case FoodVmOrderStatus::CREATED:
            return "CREATED";
        case FoodVmOrderStatus::CONFIRMED:
            return "CONFIRMED";
        case FoodVmOrderStatus::PAYMENT_CONFIRMED:
            return "PAYMENT_CONFIRMED";
        case FoodVmOrderStatus::PAYMENT_FAILED:
            return "PAYMENT_FAILED";
        case FoodVmOrderStatus::PREPARATION_STARTED:
            return "PREPARATION_STARTED";
        case FoodVmOrderStatus::READY:
            return "READY";
        case FoodVmOrderStatus::PICKED_UP:
            return "PICKED_UP";
        case FoodVmOrderStatus::CANCELLED:
            return "CANCELLED";
        }
        return "UNKNOWN";
    }
};

} // namespace food_vm

#endif // _FOOD_VM_ORDER_HPP_
