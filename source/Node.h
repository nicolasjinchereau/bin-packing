#pragma once
#include <Rect.h>
#include <RectMapping.h>
#include <NodeAllocator.h>
#include <memory>

namespace binpacking
{

enum class NodeType
{
    Empty,   // no contents, no children
    Branch,  // has contents, has children
    Leaf     // has contents, no children
};

class Node
{
public:
    std::weak_ptr<NodeAllocator> allocator;
    Rect rect = Rect();
    NodeType type = NodeType::Empty;
    RectMapping* pMapping = nullptr;
    NodePtr left;
    NodePtr right;

    Node(const std::weak_ptr<NodeAllocator>& allocator);

    void Reset(const Rect& rc);
    Node* Insert(RectMapping& mapping, int padding, bool allowRotation);
    void SplitBranch(int padding);
};

}
