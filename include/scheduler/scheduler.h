#pragma once
#include <array>

namespace natsukashii::core
{
constexpr int ENTRIES_MAX = 32;

enum Event
{
    None,
    PPU,
    Timer,
};

struct Entry
{
    Event event;
    int cycles = 0;
    Entry() : event(None)
    { }

    Entry(Event event) : event(event) 
    { }
};

struct EventQueue
{
    std::array<Entry, ENTRIES_MAX> actual_entries;
    std::array<Entry, ENTRIES_MAX> new_entries;
    bool is_new_event = false;

    EventQueue()
    {
        actual_entries.fill(Entry());
        new_entries.fill(Entry());
    }

    void push(Entry entry)
    {
        new_entries[pos--] = entry;
    }
    
    void pop()
    {
        actual_entries[pos++] = Entry();
    }
private:
    int pos = ENTRIES_MAX - 1;
};
} // natsukashii::core