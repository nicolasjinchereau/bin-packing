#pragma once
#include <vector>
#include <memory>

namespace binpacking
{

class Node;

struct NodeDeleter {
    void operator()(Node* n);
};

typedef std::unique_ptr<Node, NodeDeleter> NodePtr;

class NodeAllocator : public std::enable_shared_from_this<NodeAllocator>
{
    std::vector<Node*> nodes;
    
    void ReturnNode(Node* node);
    static void DeleteNode(Node* node);

    friend struct NodeDeleter;
public:
    NodeAllocator(uint32_t initialCapacity = 0);
    ~NodeAllocator();

    NodePtr GetNode();
};

};
