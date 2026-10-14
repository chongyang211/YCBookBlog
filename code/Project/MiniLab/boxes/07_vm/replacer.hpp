// replacer.hpp — 五种页替换算法
#pragma once
#include "trace.hpp"
#include <climits>
#include <deque>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mlab::vm {

class Replacer {
public:
    explicit Replacer(int frames) : cap_(frames) {}
    virtual ~Replacer() = default;

    bool access(int vpn) {
        total_++;
        if (resident_.count(vpn)) {
            hits_++;
            on_hit(vpn);
            return true;
        }
        faults_++;
        if ((int)resident_.size() >= cap_) {
            int v = pick_victim();
            resident_.erase(v);
            on_evict(v);
        }
        resident_.insert(vpn);
        on_insert(vpn);
        return false;
    }

    int faults() const { return faults_; }
    int hits()   const { return hits_;   }
    int total()  const { return total_;  }
    double hit_rate() const { return total_ ? (double)hits_ / total_ : 0; }
    virtual std::string name() const = 0;

protected:
    virtual void on_insert(int vpn) = 0;
    virtual void on_hit(int) {}
    virtual void on_evict(int) {}
    virtual int  pick_victim() = 0;

    int cap_;
    std::unordered_set<int> resident_;
    int total_ = 0, hits_ = 0, faults_ = 0;
};

class FIFO : public Replacer {
public:
    explicit FIFO(int f) : Replacer(f) {}
    std::string name() const override { return "FIFO"; }
protected:
    void on_insert(int v) override { q_.push_back(v); }
    int pick_victim() override { int v = q_.front(); q_.pop_front(); return v; }
private:
    std::deque<int> q_;
};

class LRU : public Replacer {
public:
    explicit LRU(int f) : Replacer(f) {}
    std::string name() const override { return "LRU"; }
protected:
    void on_insert(int v) override { order_.push_front(v); pos_[v] = order_.begin(); }
    void on_hit(int v) override {
        order_.erase(pos_[v]);
        order_.push_front(v);
        pos_[v] = order_.begin();
    }
    void on_evict(int v) override { pos_.erase(v); }
    int pick_victim() override { int v = order_.back(); order_.pop_back(); return v; }
private:
    std::list<int> order_;
    std::unordered_map<int, std::list<int>::iterator> pos_;
};

class Clock : public Replacer {
public:
    explicit Clock(int f) : Replacer(f), ring_(f, -1), ref_(f, 0) {}
    std::string name() const override { return "Clock"; }
protected:
    void on_insert(int v) override {
        for (int i = 0; i < cap_; ++i) {
            int idx = (hand_ + i) % cap_;
            if (ring_[idx] == -1) { ring_[idx] = v; ref_[idx] = 1; return; }
        }
    }
    void on_hit(int v) override {
        for (int i = 0; i < cap_; ++i) if (ring_[i] == v) ref_[i] = 1;
    }
    void on_evict(int v) override {
        for (int i = 0; i < cap_; ++i) if (ring_[i] == v) ring_[i] = -1;
    }
    int pick_victim() override {
        while (true) {
            if (ring_[hand_] == -1) { hand_ = (hand_ + 1) % cap_; continue; }
            if (ref_[hand_] == 0) {
                int v = ring_[hand_];
                hand_ = (hand_ + 1) % cap_;
                return v;
            }
            ref_[hand_] = 0;
            hand_ = (hand_ + 1) % cap_;
        }
    }
private:
    std::vector<int> ring_, ref_;
    int hand_ = 0;
};

class EnhClock : public Replacer {
public:
    explicit EnhClock(int f) : Replacer(f), ring_(f, -1), r_(f, 0), m_(f, 0) {}
    std::string name() const override { return "EnhClock"; }
protected:
    void on_insert(int v) override {
        for (int i = 0; i < cap_; ++i) {
            int idx = (hand_ + i) % cap_;
            if (ring_[idx] == -1) { ring_[idx] = v; r_[idx] = 1; m_[idx] = 0; return; }
        }
    }
    void on_hit(int v) override {
        for (int i = 0; i < cap_; ++i) if (ring_[i] == v) r_[i] = 1;
    }
    void on_evict(int v) override {
        for (int i = 0; i < cap_; ++i) if (ring_[i] == v) ring_[i] = -1;
    }
    int pick_victim() override {
        int depth = 0;
        while (depth++ < 3 * cap_) {
            for (int i = 0; i < cap_; ++i) {
                int idx = (hand_ + i) % cap_;
                if (ring_[idx] != -1 && r_[idx] == 0 && m_[idx] == 0) {
                    int v = ring_[idx]; hand_ = (idx + 1) % cap_; return v;
                }
            }
            for (int i = 0; i < cap_; ++i) {
                int idx = (hand_ + i) % cap_;
                if (ring_[idx] != -1 && r_[idx] == 0 && m_[idx] == 1) {
                    int v = ring_[idx]; hand_ = (idx + 1) % cap_; return v;
                }
                r_[idx] = 0;
            }
        }
        return -1;
    }
private:
    std::vector<int> ring_, r_, m_;
    int hand_ = 0;
};

class OPT : public Replacer {
public:
    OPT(int f, const Trace& t) : Replacer(f), future_(t) {}
    std::string name() const override { return "OPT"; }
protected:
    void on_insert(int) override { pos_++; }
    void on_hit(int)    override { pos_++; }
    int pick_victim() override {
        int farthest = -1, victim = -1;
        for (int p : resident_) {
            int nx = next_use(p);
            if (nx > farthest) { farthest = nx; victim = p; }
        }
        return victim;
    }
private:
    int next_use(int vpn) const {
        for (int i = pos_ + 1; i < (int)future_.size(); ++i)
            if (future_[i] == vpn) return i;
        return INT_MAX;
    }
    const Trace& future_;
    int pos_ = 0;
};

}  // namespace mlab::vm
