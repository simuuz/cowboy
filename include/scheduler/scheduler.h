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
};

struct Entry
{
    Event event;
    uint64_t time = 0;
    uint64_t cycles = 0;
    
    Entry() : event(None)
    { }

    Entry(Event event, uint64_t cycles) : event(event), cycles(cycles), time(0)
    { }
};

struct Scheduler
{
    std::array<Entry, ENTRIES_MAX> entries{};

    Scheduler();
    void push(Entry entry);
    void pop();
    int pos = 0;
};
} // natsukashii::core