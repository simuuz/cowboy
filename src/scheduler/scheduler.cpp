#include "scheduler.h"
#include <numeric>
#include <algorithm>
#include <cstring>

namespace natsukashii::core
{
Scheduler::Scheduler()
{
  entries.fill(Entry());
  entries[0] = Entry(Panic, UINT64_MAX);
}

void Scheduler::push(Entry entry)
{
  if(pos < ENTRIES_MAX) {
    for(int i = 0; i < pos; i++) {
      if(entries[i].time > entry.time) {
        memmove(&entries.data()[i + 1], &entries.data()[i], (pos - i) * sizeof(Entry));
        entries[i] = entry;
        break;
      }
    }

    pos++;
  }
}

void Scheduler::pop(int count)
{
  memmove(&entries.data()[0], &entries.data()[count], pos - count);
  pos -= count;
}
}