/*
 * Based on the C stack implementation by J.R. Manes. (jr@jrmanes.com)
 * Heavily refactored + updated to C++17 by Dylan Palmieri (@dgpalmieri)
 *
 * This keeps track of an origin point and direction vector (see stack.hpp)
 */

#include "stack.hpp"

stackData::stackData(const std::vector<double> & min_, const std::vector<double> & max_, std::shared_ptr<OCTREE> octptr_): octptr(octptr_) {
    min = (stackElement){.x = min_[0], .y = min_[1], .z = min_[2]};
    max = (stackElement){.x = max_[0], .y = max_[1], .z = max_[2]};
}

void stack::push(std::shared_ptr<OCTREE> o, const stackElement & min, const stackElement & max) {
    std::shared_ptr<stackNode> newnode;

    try{
        newnode = std::make_shared<stackNode>();
    }
    catch( const std::bad_alloc & e ){
        std::cerr << "ERROR: Stack full! (number of elements: " << this->size << ")" << std::endl;
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    newnode->min.x = min.x;
    newnode->min.y = min.y;
    newnode->min.z = min.z;
    newnode->max.x = max.x;
    newnode->max.y = max.y;
    newnode->max.z = max.z;
    newnode->octptr = o;
    newnode->next = this->_top;
    this->_top = newnode;
    this->size++;
}

int stack::pop(stackData & s) {
     if (this->size < 1) return 0;

     this->size--;

     std::vector<double> min{this->_top->min.x, this->_top->min.y , this->_top->min.z};
     std::vector<double> max{this->_top->max.x, this->_top->max.y , this->_top->max.z};
     s = stackData(min, max, this->_top->octptr);

     this->_top = this->_top->next;

     return 1;
}

std::shared_ptr<stackNode> stack::top(){ return this->_top; }

int stack::getSize() { return this->size; }
