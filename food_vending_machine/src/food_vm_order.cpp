#include <food_vm_order.hpp>
#include <iomanip>
#include <sstream>

using namespace food_vm;

FoodVmOrder::FoodVmOrder(FoodVmDatabase& food_vm_db)
    : m_food_vm_db{food_vm_db}, m_processing_order{false}
{
}

const OrderInfo& FoodVmOrder::get_order_info() const
{
    return m_order;
}

void FoodVmOrder::clear_order()
{
    m_processing_order = false;
}

void FoodVmOrder::add_product(std::vector<size_t> product_ids)
{
    if (!m_processing_order) {
        create_order();
    }

    for (const size_t& id : product_ids) {
        m_order.product_ids.insert(id);
    }

    std::unordered_map<size_t, ProductInfo> prod_id_mapped =
        m_food_vm_db.get_product_info_id_mapped();

    m_order.total_price = 0.0;
    for (const size_t& id : m_order.product_ids) {
        auto prod = prod_id_mapped.find(id);
        if (prod != prod_id_mapped.end()) {
            m_order.total_price += prod->second.price;
        }
    }
    update_selected_products_table_message(prod_id_mapped);
}

void FoodVmOrder::create_order()
{
    m_processing_order = true;
    m_order = m_food_vm_db.create_order(order_event_to_string(FoodVmOrderStatus::CREATED));
    register_event(FoodVmOrderStatus::CREATED);
}

void FoodVmOrder::update_selected_products_table_message(
    std::unordered_map<size_t, ProductInfo>& prod_id_mapped)
{
    std::ostringstream out;

    out << std::string(19, '-') << "\n"
        << "Selected products\n"
        << std::string(19, '-') << "\n";

    out << std::fixed << std::setprecision(2);

    for (const size_t& p : m_order.product_ids) {
        auto prod = prod_id_mapped.find(p);
        if (prod != prod_id_mapped.end()) {
            out << std::left << std::setw(10) << prod->second.name << std::right << std::setw(8)
                << prod->second.price << "\n";
        }
    }

    out << std::string(19, '-') << "\n";
    out << "Total:" << std::right << std::setw(12) << m_order.total_price << "\n";
    out << std::string(19, '-') << "\n";

    m_selected_products_table_message = out.str();
}

const std::string& FoodVmOrder::get_selected_products_table_message() const
{
    return m_selected_products_table_message;
}

void FoodVmOrder::register_event(FoodVmOrderStatus event)
{
    std::string event_str = order_event_to_string(event);
    m_food_vm_db.register_order_event(event_str, m_order);
}

bool FoodVmOrder::separate_order(std::string& error_msg)
{
    if (!m_processing_order) {
        error_msg = "No order in progress.";
        return false;
    }

    if (m_order.product_ids.empty()) {
        error_msg = "No products selected.";
        return false;
    }

    if (!m_food_vm_db.separate_order(m_order, error_msg)) {
        return false;
    }

    register_event(FoodVmOrderStatus::CONFIRMED);

    return true;
}

void FoodVmOrder::register_order_products()
{
    if (!m_processing_order) {
        return;
    }

    if (m_order.product_ids.empty()) {
        return;
    }

    m_food_vm_db.register_order_products(m_order);
}
