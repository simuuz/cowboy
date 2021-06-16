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
    std::array<Entry, ENTRIES_MAX> entries;
    bool is_new_event = false;

    EventQueue()
    {
        entries.fill(Entry());
    }

    void push(Entry entry)
    {
        actual_entries[pos--] = entry;
    }
    
    void pop()
    {
        new_entries[pos++] = Entry();
    }
private:
    int pos = ENTRIES_MAX - 1;
};
} // natsukashii::core