#include "scheduler.h"
#include <numeric>
#include <algorithm>
#include <cstring>

namespace natsukashii::core
{
Scheduler::Scheduler()
{
  entries.fill(Entry());
  entries[0].time = UINT64_MAX;
}

void Scheduler::push(Entry entry)
{
  if(pos < ENTRIES_MAX) {
    for(int i = 0; i < pos; i++) {
      if(entries[i].time > entry.time) {
        memmove(&entries.data()[i + 1], &entries.data()[i], sizeof(Entry));
        entries[i] = entry;
        break;
      }
    }

    pos++;
  }
}

void Scheduler::pop()
{
  if(pos > 0) {
    entries[pos--] = Entry();
  }
}

}