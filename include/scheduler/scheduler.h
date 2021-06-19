#pragma once
#include <array>
#include <cstdint>

namespace natsukashii::core
{
constexpr int ENTRIES_MAX = 32;

enum Event
{
    None,
    PPU,
    Timer,
    Panic,
};

struct Entry
{
    Event event;
    uint64_t time = 0;
    
    Entry() : event(None)
    { }

    Entry(Event event, uint64_t time) : event(event), time(time)
    { }
};

struct Scheduler
{
    std::array<Entry, ENTRIES_MAX> entries{};

    Scheduler();
    void push(Entry entry);
    void pop(int count);
    int pos = 1;
};
} // natsukashii::core