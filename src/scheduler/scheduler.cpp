#include "scheduler.h"

namespace natsukashii::core
{
EventQueue::EventQueue()
{
    entries.fill(Entry());
}

void EventQueue::push(Entry entry)
{
    entries[pos--] = entry;
}

void EventQueue::pop()
{
    entries[pos++] = Entry();
}
}