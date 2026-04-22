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
  explicit Memo(int duration) : duration_(duration), current_(0), seq_(0) {}

  ~Memo() = default; // Non-owning pointers

  void AddEvent(const Event *event) {
    if (!event) return;
    const int dl = event->GetDeadline();

    auto rank_of = [](const Event* ev)->int{
      if (dynamic_cast<const NotifyBeforeEvent*>(ev)) return 0; // pre-reminders first
      if (dynamic_cast<const NormalEvent*>(ev)) return 1; // then normal
      if (dynamic_cast<const NotifyLateEvent*>(ev)) return 2; // then late
      return 3;
    };
    int r = rank_of(event);

    auto push_item = [&](int t, int n){
      if (t > current_ && t >= 1 && t <= duration_) schedule_[t].push_back({event, n, r, seq_});
    };

    // Custom late event
    if (auto cle = dynamic_cast<const CustomNotifyLateEvent*>(event)) {
      const int freq = cle->GetFrequency();
      push_item(dl, 0);
      if (freq > 0) {
        for (long long t = 1LL * dl + freq, k = 1; t <= duration_; t += freq, ++k) push_item((int)t, (int)k);
      }
      ++seq_;
      return;
    }

    // Late event
    if (auto le = dynamic_cast<const NotifyLateEvent*>(event)) {
      const int freq = le->GetFrequency();
      push_item(dl, 0);
      if (freq > 0) {
        for (long long t = 1LL * dl + freq, k = 1; t <= duration_; t += freq, ++k) push_item((int)t, (int)k);
      }
      ++seq_;
      return;
    }

    // Notify before event
    if (auto nbe = dynamic_cast<const NotifyBeforeEvent*>(event)) {
      int pre_t = dl - nbe->GetNotifyTime();
      push_item(pre_t, 0);
      push_item(dl, 1);
      ++seq_;
      return;
    }

    // Normal event
    if (dynamic_cast<const NormalEvent*>(event)) {
      push_item(dl, 0);
      ++seq_;
      return;
    }
  }

  void Tick() {
    if (current_ >= duration_) return;
    ++current_;
    auto it = schedule_.find(current_);
    if (it == schedule_.end()) return;
    auto &vec = it->second;
    std::sort(vec.begin(), vec.end(), [](const Item &a, const Item &b){
      if (a.rank != b.rank) return a.rank < b.rank;
      if (a.seq != b.seq) return a.seq < b.seq;
      if (a.ev == b.ev) return a.n < b.n; // ensure pre (n=0) before final (n=1) for same event
      return false;
    });
    for (auto &p : vec) {
      const Event *ev = p.ev;
      int n = p.n;
      if (!ev || ev->IsComplete()) continue;
      std::cout << ev->GetNotification(n) << std::endl;
    }
  }

 private:
  struct Item {
    const Event* ev;
    int n;
    int rank;
    long long seq;
  };
  int duration_;
  int current_;
  long long seq_;
  std::unordered_map<int, std::vector<Item>> schedule_;
};

#endif // SRC_HPP
