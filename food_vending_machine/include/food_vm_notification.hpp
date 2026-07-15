#ifndef _FOOD_VM_NOTIFICATION_HPP_
#define _FOOD_VM_NOTIFICATION_HPP_

#include <functional>

namespace food_vm {

template<typename Event>
using NotificationCallback = std::function<void(Event)>;

template<typename Event>
class FoodVmNotification {
    public:
        FoodVmNotification(NotificationCallback<Event> callback);

        void notify(Event e);

    private:
        NotificationCallback<Event> m_callback;
};


template<typename Event>
FoodVmNotification<Event>::FoodVmNotification(
    NotificationCallback<Event> callback)
    : m_callback(std::move(callback))
{
}

template<typename Event>
void FoodVmNotification<Event>::notify(Event event)
{
    m_callback(std::move(event));
}

} // food_vm

#endif // _FOOD_VM_NOTIFICATION_HPP_
