#ifndef FVM_NOTIFICATION_HPP
#define FVM_NOTIFICATION_HPP

#include <functional>

namespace food_vm {

template <typename Event> using NotificationCallback = std::function<void(Event)>;

template <typename Event> class FvmNotification {
  public:
    FvmNotification(NotificationCallback<Event> callback);

    void notify(Event e);

  private:
    NotificationCallback<Event> m_callback;
};

template <typename Event>
FvmNotification<Event>::FvmNotification(NotificationCallback<Event> callback)
    : m_callback(std::move(callback))
{
}

template <typename Event> void FvmNotification<Event>::notify(Event event)
{
    m_callback(std::move(event));
}

} // namespace food_vm

#endif // FVM_NOTIFICATION_HPP
