#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include <cstddef>
#include <optional>
#include <vector>

namespace bm {

template <typename T> struct SparseSet {
    using SparseIndex = std::size_t;
    using DenseIndex = std::size_t;

    struct DenseElement {
        SparseIndex sparseIndex;
        T value;
    };

    size_t count;
    std::vector<DenseIndex> sparse;
    std::vector<DenseElement> dense;

    SparseIndex add(T item) {
        auto denseIndex = count;

        count += 1;

        // try to reuse last freed index
        if (denseIndex < dense.size()) {
            auto denseElement = &dense[denseIndex];
            denseElement->value = item;
            return denseElement->sparseIndex;
        }

        // allocate new index
        auto sparseIndex = sparse.size();

        dense.push_back({ sparseIndex, item });
        sparse.push_back(denseIndex);

        return sparseIndex;
    }

    bool remove(SparseIndex sparseIndex) {
        if (!contains(sparseIndex)) {
            return false;
        }

        count -= 1;

        auto denseIndex = sparse[sparseIndex];
        auto endDenseIndex = count;

        // swap remove
        auto endElement = dense[endDenseIndex];

        dense[denseIndex] = endElement;
        sparse[endElement.sparseIndex] = denseIndex;

        // update end dense idx for reuse
        dense[endDenseIndex].sparseIndex = sparseIndex;
        sparse[sparseIndex] = endDenseIndex;

        return true;
    }

    std::optional<T *const> getPtr(SparseIndex sparseIndex) {
        if (!contains(sparseIndex)) {
            return std::nullopt;
        }

        return { &dense[sparse[sparseIndex]].value };
    }

    bool contains(SparseIndex sparseIndex) const {
        if (sparseIndex >= sparse.size()) {
            return false;
        }

        auto denseIndex = sparse[sparseIndex];
        auto currentIndex = dense[denseIndex].sparseIndex;

        return denseIndex < count && sparseIndex == currentIndex;
    }

    std::pair<typename std::vector<DenseElement>::iterator,
              typename std::vector<DenseElement>::iterator>
    iterator() {
        auto start = dense.begin();
        auto end = dense.begin() + count;

        return { start, end };
    }
};

} // namespace bm

#endif // SPARSE_SET_H
