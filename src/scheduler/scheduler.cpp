#include "scheduler.h"

EventQueue::EventQueue()
{
    actual_entries.fill(Entry());
    new_entries.fill(Entry());
}

void EventQueue::push(Entry entry)
{
    new_entries[pos--] = entry;
}

void EventQueue::pop()
{
    actual_entries[pos++] = Entry();
}