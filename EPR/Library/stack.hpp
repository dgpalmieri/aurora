#include "rays.h"
#include "octree.h"
#include <iostream>
#include <memory>

using stackElement = Point;

class stackData {
    public:
        stackElement min;
        stackElement max;
        std::shared_ptr<OCTREE> octptr;
};

class stackNode {
    public:
        stackElement min;
        stackElement max;
        std::shared_ptr<OCTREE> octptr;
        std::unique_ptr<stackNode> next;
};

class stack{
    public:
        stack() : size(0), _top(nullptr) {}

        std::shared_ptr<stackNode> top();

        void push(std::shared_ptr<OCTREE>, const stackElement &, const stackElement &);

        stackData pop();

    private:
        int size;
        std::shared_ptr<stackNode> _top;
};
