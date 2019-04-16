#ifndef VECEX_H_
#define VECEX_H_
#include <vector>
#include <stack>

template<typename ty>
struct vecex {
    std::vector<ty> vec;
    std::vector<bool> is_erased;
    std::stack<size_t> erased_id;
    vecex() {};
    vecex(size_t sz) : vec(sz), is_erased(sz, false) {}
    size_t insert(const ty& inserted) {
        if (erased_id.empty()) {
            vec.push_back(inserted);
            is_erased.push_back(false);
            return vec.size() - 1;
        }
        size_t id = erased_id.top();
        erased_id.pop();
        vec[id] = inserted;
        is_erased[id] = false;
        return id;
    }
    void erase(size_t id) {
        is_erased[id] = true;
        erased_id.push(id);
    }
};

#endif