#include <NodeAllocator.h>
#include <Node.h>

namespace binpacking
{

void NodeDeleter::operator()(Node* n)
{
    auto alloc = n->allocator.lock();
    if (alloc) {
        alloc->ReturnNode(n);
    }
    else {
        NodeAllocator::DeleteNode(n);
    }
}

NodeAllocator::NodeAllocator(uint32_t initialCapacity)
{
    nodes.reserve(initialCapacity);

    for (uint32_t i = 0; i < initialCapacity; ++i)
        nodes.push_back((Node*)::operator new(sizeof(Node)));
}

NodeAllocator::~NodeAllocator()
{
    for (auto& node : nodes)
        DeleteNode((Node*)node);
}

NodePtr NodeAllocator::GetNode()
{
    Node* node = nullptr;

    if (!nodes.empty()) {
        node = nodes.back();
        nodes.pop_back();
    }
    else {
        node = (Node*)::operator new(sizeof(Node));
    }

    return NodePtr(new (node) Node(shared_from_this()));
}

void NodeAllocator::ReturnNode(Node* node) {
    nodes.push_back(node);
}

void NodeAllocator::DeleteNode(Node* node) {
    ::operator delete(node);
}

};
