// Copyright (c) 2023 Finn Thomas
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ORTHTREE_H
#define ORTHTREE_H

#include <string>
#include <vector>
#include <queue>
#include <array>
#include <functional>

template<size_t dimensions = 2, typename T = float>
class Orthtree
{
public:
    struct VecN
    {
        VecN() = default;
        VecN(std::array<T, dimensions> data) : mData(data) {}
        T& operator[](size_t index) { return mData.at(index); }
        VecN& operator=(std::array<T, dimensions> data) { mData = data; return *this; }
        VecN operator+(T b)  { for (auto& d : mData) d += b; return *this; }
        VecN operator-(T b)  { for (auto& d : mData) d -= b; return *this; }
        VecN operator*(T b)  { for (auto& d : mData) d *= b; return *this; }
        VecN operator/(T b)  { for (auto& d : mData) d /= b; return *this; }
        VecN operator+=(T b) { for (auto& d : mData) d += b; return *this; }
        VecN operator-=(T b) { for (auto& d : mData) d -= b; return *this; }
        VecN operator*=(T b) { for (auto& d : mData) d *= b; return *this; }
        VecN operator/=(T b) { for (auto& d : mData) d /= b; return *this; }
        
        T Distance(VecN& point) const noexcept
        {
            T dSqr = static_cast<T>(0);
            for (size_t i = 0; i < dimensions; ++i)
            {
                T diff = mData[i] - point[i];
                dSqr += diff * diff;
            }
            return std::sqrt(dSqr);
        }
    private:
        std::array<T, dimensions> mData;
    };

    struct Node
    {
        VecN pos, size, centre;
        size_t level = 0;
        std::vector<Node*> children;
        bool isLeaf = true;

        Node() = default;
        Node(const VecN& pos, const VecN& size) : pos(pos), size(size) {};
        Node(const VecN& pos, const VecN& size, const VecN& centre, uint32_t level) :
                pos(pos), size(size), centre(centre), level(level) {};

        [[nodiscard]] bool ContainsPoint(VecN& point) noexcept
        {
            for (size_t d = 0; d < dimensions; ++d)
                if (point[d] < pos[d] || point[d] >= pos[d] + size[d])
                    return false;
            return true;
        }
    };
private:
    std::vector<Node> mNodes;
    std::queue<Node>  mNodeQueue;
public:
    Orthtree()
    {
        static_assert(dimensions, "Orthtree error: Cannot have a 0-dimensional tree.");
        static_assert(std::is_arithmetic_v<T>, "Orthtree error: Type T must be numerical.");
    }

    [[nodiscard]] size_t Size() const noexcept
    {
        return mNodes.size();
    }

    [[nodiscard]] Node& operator[](size_t index)
    {
        if (index > mNodes.size() - 1)
            throw std::out_of_range("Orthree error: index " + std::to_string(index) +
                                    " is out of range. Tree size is " + std::to_string(mNodes.size()));
        else
            return mNodes[index];
    }

    void Generate(VecN lowerBounds,
                  VecN upperBounds,
                  size_t maxDepth,
                  std::function<bool(Node&)> subdivisionCondition)
    {
        mNodes.clear();
        const size_t numChildren = pow(2, dimensions);

        // Create root node
        VecN rootSize, rootCentre;
        for (size_t d = 0; d < dimensions; ++d)
        {
            rootSize[d]   = upperBounds[d] - lowerBounds[d];
            rootCentre[d] = lowerBounds[d] + rootSize[d] / static_cast<T>(2);
        }
        mNodeQueue.push({ lowerBounds, rootSize, rootCentre, 0 });

        while (!mNodeQueue.empty())
        {
            auto currNode = mNodeQueue.front();
            mNodeQueue.pop();

            // Subdivide
            if (currNode.level < maxDepth && subdivisionCondition(currNode))
            {
                auto halfSize = currNode.size / 2;
                currNode.children.resize(numChildren);
                for (size_t i = 0; i < numChildren; ++i)
                {
                    mNodeQueue.push(Node(currNode.pos, halfSize));
                    auto& currChild = mNodeQueue.back();
                    currChild.level = currNode.level + 1;
                    size_t index = i;
                    for (size_t d = 0; d < dimensions; ++d)
                    {
                        currChild.pos[d]   += (index % 2) * currNode.size[d];
                        currChild.centre[d] = currChild.pos[d] + currChild.size[d] / static_cast<T>(2);
                        index /= 2;
                    }
                    currNode.children[i] = &currChild;
                }
            }
            else
                currNode.isLeaf = false;
            mNodes.push_back(std::move(currNode));
        }
    }

    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = Node;
        using pointer           = Node*;
        using reference         = Node&;

        explicit Iterator(pointer ptr) : mPtr(ptr) {}

        reference operator*() const { return *mPtr; }
        pointer operator->() { return mPtr; }
        Iterator& operator++() { mPtr++; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.mPtr == b.mPtr; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.mPtr != b.mPtr; };
    private:
        pointer mPtr;
    };

    Iterator begin() { return Iterator(mNodes.begin().base()); }
    Iterator end()   { return Iterator(mNodes.end().base()); }
};

#endif // ORTHTREE_H
