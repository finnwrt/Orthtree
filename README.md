# Orthtree

This is a simple library to generate an N-dimensional orthtrees non-recursively. An orthtree is a tree structure in which each node is subdivided into 2<sup>N</sup> equal nodes. An example of orthtrees are quadtrees and octrees. These structures can be very useful for graphics, image processing, collision detection and much more.

To get started just include `Orthree.h` and make sure you’re using at least `C++17`.

## Usage

Everything takes place inside the `Orthtree` class.
```cpp
template<size_t dimensions = 2, typename T = float> class Orthtree
```
`Dimensions` as you’ve might guessed is the number of dimensions, i.e. 2 for a quad tree. `T` represents the data type which all points use. Note that if `T` is integral you might have some precision loss.

To generate the tree use the generate method:
```cpp
void Generate(VecN lowerBounds,
              VecN upperBounds,
              size_t maxDepth,
              std::function<bool(Node&)> subdivisionCondition)

```
`lowerBounds` and `upperBounds` represent the `N-D` space which the tree represents. `maxDepth` is how many times the root node can be subdivided. Lastly `subdivisionCondition` is a lambda which takes the current `Node` being queried as input. If the lambda returns `true` then the node is subdivided.

Then we have a couple of other utility functions:
```cpp
// Gets Euclidean distance between 2 points
T VecN::Distance(VecN& point) const noexcept;
// Checks if a point resides within a Node
bool Node::ContainsPoint(VecN& point) noexcept;
// Gets the number of Nodes in the tree
size_t Orthtree::Size() const noexcept;
// Gets node with index in tree (may throw std::out_of_range)
Node& Orthtree::operator[](size_t index);
```

## Examples

### Point-region quadtree

```cpp
// Generate some random points
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<size_t> dist(0, 512);
for (size_t i = 0; i < 100; ++i)
     points.push_back({{ dist(rng), dist(rng) }});
// Generate point-region quadtree
size_t bucketCapacity = 1;
tree.Generate({{ 0, 0 }}, {{ 512, 512 }}, 16, [&](auto& node) {
     int nPoints = 0;
     for (auto& p : points)
          if (node.ContainsPoint(p))
               nPoints++;
     return nPoints > bucketCapacity;
});
```
![Point-region quadtree. bucketCapacity = 1](https://github.com/finnwrt/Orthtree/blob/main/test/test2.png)

### Subdivide by camera distance

```cpp
Orthtree<2>::VecN camera = {{ (float)GetMouseX(), (float)GetMouseY() }};

tree.Generate({{ 0.0f, 0.0f }}, {{ 512.0f, 512.0f }}, 8, [&](auto& node) {
     auto dist = camera.Distance(node.centre);
     return dist < node.size[0] * 2.0f;
});
```

![Subdivide by camera distance](https://github.com/finnwrt/Orthtree/blob/main/test/test1.gif)