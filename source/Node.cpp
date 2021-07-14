#include <Node.h>
#include <NodeAllocator.h>
#include <cassert>

namespace binpacking
{
Node::Node(const std::weak_ptr<NodeAllocator>& allocator)
    : allocator(allocator) {}

void Node::Reset(const Rect& rc)
{
    rect = rc;
    pMapping = nullptr;
    type = NodeType::Empty;
}

Node* Node::Insert(RectMapping& mapping, int padding, bool allowRotation)
{
    if(type == NodeType::Empty)
    {
        if(mapping.inputSize.x == rect.w &&
           mapping.inputSize.y == rect.h)
        {
            pMapping = &mapping;
            pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
            pMapping->rotated = false;
            type = NodeType::Leaf;
            return this;
        }
        else if(allowRotation &&
            mapping.inputSize.x == rect.h &&
            mapping.inputSize.y == rect.w)
        {
            pMapping = &mapping;
            pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
            pMapping->rotated = true;
            type = NodeType::Leaf;
            return this;
        }
        else if(mapping.inputSize.x <= rect.w &&
                mapping.inputSize.y <= rect.h)
        {
            pMapping = &mapping;
            pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.x, mapping.inputSize.y);
            pMapping->rotated = false;
            SplitBranch(padding);
            type = NodeType::Branch;
            return this;
        }
        else if(allowRotation &&
                mapping.inputSize.x <= rect.h &&
                mapping.inputSize.y <= rect.w)
        {
            pMapping = &mapping;
            pMapping->mappedRect = Rect(rect.x, rect.y, mapping.inputSize.y, mapping.inputSize.x);
            pMapping->rotated = true;
            SplitBranch(padding);
            type = NodeType::Branch;
            return this;
        }
    }
    else if(type == NodeType::Branch)
    {
        auto ret = left->Insert(mapping, padding, allowRotation);
        if(!ret) ret = right->Insert(mapping, padding, allowRotation);
        return ret;
    }

    return nullptr;
}

void Node::SplitBranch(int padding)
{
    auto alloc = allocator.lock();
    assert(alloc);
    
    if(!left) left = alloc->GetNode();
    if(!right) right = alloc->GetNode();

    int remWidth = rect.w - pMapping->mappedRect.w;
    int remHeight = rect.h - pMapping->mappedRect.h;

    if(remWidth > remHeight)
    {
        // split vertically
        left->rect = Rect(rect.x, rect.y + pMapping->mappedRect.h, pMapping->mappedRect.w, remHeight);
        left->type = NodeType::Empty;
        left->pMapping = nullptr;
        right->rect = Rect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, rect.h);
        right->type = NodeType::Empty;
        right->pMapping = nullptr;

        left->rect.y += padding;
        left->rect.h -= padding;
        right->rect.x += padding;
        right->rect.w -= padding;
    }
    else
    {
        // split horizontally
        left->rect = Rect(rect.x + pMapping->mappedRect.w, rect.y, remWidth, pMapping->mappedRect.h);
        left->type = NodeType::Empty;
        left->pMapping = nullptr;
        right->rect = Rect(rect.x, rect.y + pMapping->mappedRect.h, rect.w, remHeight);
        right->type = NodeType::Empty;
        right->pMapping = nullptr;

        left->rect.x += padding;
        left->rect.w -= padding;
        right->rect.y += padding;
        right->rect.h -= padding;
    }
}

}
