#ifndef SRC_HPP
#define SRC_HPP

#include <bits/stdc++.h>
#include "event.h"

// Implement CustomNotifyLateEvent::GetNotification exactly as specified
inline std::string CustomNotifyLateEvent::GetNotification(int n) const {
  std::string base = NotifyLateEvent::GetNotification(n);
  std::string extra = generator_(n);
  return base + extra;
}

class Memo {
 public:
  Memo() = delete;

  // simulate hours [1..duration]
  explicit Memo(int duration) : duration_(duration), current_(0) {}

  ~Memo() = default; // Non-owning pointers

  void AddEvent(const Event *event) {
    if (!event) return;
    const int dl = event->GetDeadline();

    // Custom late event
    if (auto cle = dynamic_cast<const CustomNotifyLateEvent*>(event)) {
      const int freq = cle->GetFrequency();
      if (dl > current_ && dl <= duration_) schedule_[dl].push_back({event, 0});
      if (freq > 0) {
        for (long long t = 1LL * dl + freq, k = 1; t <= duration_; t += freq, ++k) {
          if (t > current_) schedule_[(int)t].push_back({event, (int)k});
        }
      }
      return;
    }

    // Late event
    if (auto le = dynamic_cast<const NotifyLateEvent*>(event)) {
      const int freq = le->GetFrequency();
      if (dl > current_ && dl <= duration_) schedule_[dl].push_back({event, 0});
      if (freq > 0) {
        for (long long t = 1LL * dl + freq, k = 1; t <= duration_; t += freq, ++k) {
          if (t > current_) schedule_[(int)t].push_back({event, (int)k});
        }
      }
      return;
    }

    // Notify before event
    if (auto nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      int pre_t = dl - nbe->GetNotifyTime();
      if (pre_t > current_ && pre_t >= 1 && pre_t <= duration_) {
        schedule_[pre_t].push_back({event, 0});
      }
      if (dl > current_ && dl <= duration_) {
        schedule_[dl].push_back({event, 1});
      }
      return;
    }

    // Normal event
    if (dynamic_cast<const NormalEvent*>(event)) {
      if (dl > current_ && dl <= duration_) schedule_[dl].push_back({event, 0});
      return;
    }
  }

  void Tick() {
    if (current_ >= duration_) return;
    ++current_;
    auto it = schedule_.find(current_);
    if (it == schedule_.end()) return;
    for (auto &p : it->second) {
      const Event *ev = p.first;
      int n = p.second;
      if (!ev || ev->IsComplete()) continue;
      std::cout << ev->GetNotification(n) << std::endl;
    }
  }

 private:
  int duration_;
  int current_;
  std::unordered_map<int, std::vector<std::pair<const Event*, int>>> schedule_;
};

#endif // SRC_HPP
