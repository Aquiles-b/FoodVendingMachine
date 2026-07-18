#ifndef FVM_ORDER_HPP
#define FVM_ORDER_HPP

#include <vector>

#include <fvm_database.hpp>

namespace food_vm {

enum class FvmOrderStatus {
    CREATED,
    CONFIRMED,
    PAYMENT_CONFIRMED,
    PAYMENT_FAILED,
    PREPARATION_STARTED,
    READY,
    PICKED_UP,
    CANCELLED
};

class FvmOrder {
  public:
    FvmOrder(FvmDatabase& fvm_db);

    const OrderInfo& get_order_info() const;
    void clear_order();
    void add_product(std::vector<size_t> product_ids);
    void register_event(FvmOrderStatus event);
    bool separate_order(std::string& error_msg);
    void register_order_products();

    const std::string& get_selected_products_table_message() const;

  private:
    OrderInfo m_order;
    FvmDatabase& m_fvm_db;
    std::string m_selected_products_table_message;
    bool m_processing_order;

    void
    update_selected_products_table_message(std::unordered_map<size_t, ProductInfo>& prod_id_mapped);
    void create_order();

    static std::string order_event_to_string(FvmOrderStatus event)
    {
        switch (event) {
        case FvmOrderStatus::CREATED:
            return "CREATED";
        case FvmOrderStatus::CONFIRMED:
            return "CONFIRMED";
        case FvmOrderStatus::PAYMENT_CONFIRMED:
            return "PAYMENT_CONFIRMED";
        case FvmOrderStatus::PAYMENT_FAILED:
            return "PAYMENT_FAILED";
        case FvmOrderStatus::PREPARATION_STARTED:
            return "PREPARATION_STARTED";
        case FvmOrderStatus::READY:
            return "READY";
        case FvmOrderStatus::PICKED_UP:
            return "PICKED_UP";
        case FvmOrderStatus::CANCELLED:
            return "CANCELLED";
        }
        return "UNKNOWN";
    }
};

} // namespace food_vm

#endif // FVM_ORDER_HPP
