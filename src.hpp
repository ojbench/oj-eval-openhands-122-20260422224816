#ifndef SRC_HPP
#define SRC_HPP

#include <bits/stdc++.h>
#include "event.h"

// Implement CustomNotifyLateEvent::GetNotification exactly as specified
inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  // 1) Call base class implementation
  std::string base = NotifyLateEvent::GetNotification(n);
  // 2) Call generator to get custom message
  std::string extra = generator_(n);
  // 3) Concatenate and return
  return base + extra;
}

class Memo {
 public:
  Memo() = delete;

  // simulate hours [1..duration]
  explicit Memo(int duration) : duration_(duration), current_(0) {}

  ~Memo() = default; // We do not own the Event pointers; caller manages their lifetime

  void AddEvent(const Event *event) {
    events_.push_back(event); // non-owning
  }

  void Tick() {
    if (current_ >= duration_) return; // nothing to do beyond planned duration
    ++current_;

    for (const Event *ev : events_) {
      if (!ev || ev->IsComplete()) continue;

      int dl = ev->GetDeadline();
      // Custom late event first (is-a NotifyLateEvent)
      if (auto cle = dynamic_cast<const CustomNotifyLateEvent*>(ev)) {
        int freq = cle->GetFrequency();
        if (current_ == dl) {
          std::cout << cle->GetNotification(0) << std::endl;
        } else if (current_ > dl && freq > 0) {
          int delta = current_ - dl;
          if (delta % freq == 0) {
            int n = delta / freq; // n >= 1
            std::cout << cle->GetNotification(n) << std::endl;
          }
        }
        continue;
      }

      // Late event
      if (auto le = dynamic_cast<const NotifyLateEvent*>(ev)) {
        int freq = le->GetFrequency();
        if (current_ == dl) {
          std::cout << le->GetNotification(0) << std::endl;
        } else if (current_ > dl && freq > 0) {
          int delta = current_ - dl;
          if (delta % freq == 0) {
            int n = delta / freq; // n >= 1
            std::cout << le->GetNotification(n) << std::endl;
          }
        }
        continue;
      }

      // Notify before event
      if (auto nbe = dynamic_cast<const NotifyBeforeEvent*>(ev)) {
        int pre_t = dl - nbe->GetNotifyTime() + 1; // schedule rule inferred from sample
        if (current_ == pre_t) {
          std::cout << nbe->GetNotification(0) << std::endl;
        }
        if (current_ == dl) {
          std::cout << nbe->GetNotification(1) << std::endl;
        }
        continue;
      }

      // Normal event
      if (dynamic_cast<const NormalEvent*>(ev)) {
        if (current_ == dl) {
          std::cout << ev->GetNotification(0) << std::endl;
        }
        continue;
      }

      // Fallback: if some unknown derived type appears, do nothing
    }
  }

 private:
  int duration_;
  int current_;
  std::vector<const Event*> events_;
};

#endif // SRC_HPP
