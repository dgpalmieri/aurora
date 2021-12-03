#ifndef STACK_HPP
#define STACK_HPP

#include "rays.hpp"
#include "octree.hpp"
#include <iostream>
#include <memory>
#include <vector>

using stackElement = Point;

class stackData {
    public:
        stackData() : min({.x = 0, .y = 0, .z = 0}), max({.x = 0, .y = 0, .z = 0}), octptr(nullptr) {}
        stackData(const std::vector<double> &, const std::vector<double> &, std::shared_ptr<OCTREE>);

        stackElement min;
        stackElement max;
        std::shared_ptr<OCTREE> octptr;
};

class stackNode {
    public:
        stackElement min;
        stackElement max;
        std::shared_ptr<OCTREE> octptr;
        std::shared_ptr<stackNode> next;
};

class stack{
    public:
        stack() : size(0), _top(nullptr) {}

        std::shared_ptr<stackNode> top();

        void push(std::shared_ptr<OCTREE>, const stackElement &, const stackElement &);

        int pop(stackData &);

        int getSize();

    private:
        int size;
        std::shared_ptr<stackNode> _top;
};

#endif // STACK_HPP
