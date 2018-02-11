#ifndef incl_STATICHASH_H__
#define incl_STATICHASH_H__

#include <array>

namespace baduk {

class HashTableFull : public std::exception {
};

// Statically allocated fixed-size hash table.
template<typename T, unsigned int TABLESIZE>
class StaticHash {
public:
    void insert(T value) {
        auto attempts = TABLESIZE;
        T index = value;
        while (attempts != 0) {
            if (!filled_[index % TABLESIZE]) {
                filled_.set(index % TABLESIZE);
                values_[index % TABLESIZE] = value;
                return;
            }
            ++index;
            --attempts;
        }
        throw HashTableFull();
    }

    bool contains(T value) const {
        auto attempts = TABLESIZE;
        T index = value;
        while (attempts != 0 ) {
            if (filled_[index % TABLESIZE]) {
                if (values_[index % TABLESIZE] == value) {
                    return true;
                }
            } else {
                return false;
            }
            ++index;
            --attempts;
        }
        return false;
    }

private:
    std::array<T, TABLESIZE> values_;
    std::bitset<TABLESIZE> filled_;
};

}

#endif
