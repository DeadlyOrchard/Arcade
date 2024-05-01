#include "Snake.hpp"
#include <time.h>
#include <iostream>

Snake::Snake(int length, int nodeSize, char direction, int x, int y, int minX, int minY, int maxX, int maxY) {
    this->nodeSize = nodeSize;
    this->direction = direction;
    this->length = length;
    this->currLength = 1;
    this->head = new SnakeNode{x, y, nullptr};
    this->bounds[0] = minX;
    this->bounds[1] = minY;
    this->bounds[2] = maxX;
    this->bounds[3] = maxY;
    this->isAlive = true;
    srand(time(0));
    this->food = this->makeFood();
}

SnakeNode* Snake::makeFood() {
    int x = rand() % (this->bounds[2] / this->nodeSize) * this->nodeSize;
    int y = rand() % (this->bounds[3] / this->nodeSize) * this->nodeSize;
    return new SnakeNode{x, y, nullptr};
}

SnakeNode* Snake::getHead() {
    return this->head;
}

SnakeNode* Snake::getFood() {
    return this->food;
}

int Snake::getCurrentLength() {
    return this->currLength;
}

bool Snake::getStatus() {
    return this->isAlive;
}

void Snake::grow() {
    this->length++;
}

void Snake::turn(char direction) {
    // if moving horizontally, we can only turn vertically and vice versa
    switch (this->direction) {
    case 'r':
    case 'l':
        switch(direction) {
        case 'u':
        case 'd':
            this->direction = direction;
            break;
        }
        break;
    case 'u':
    case 'd':
        switch(direction) {
        case 'r':
        case 'l':
            this->direction = direction;
            break;
        }
        break;
    }
}

void Snake::update() {
    // create new head, then remove the last node unless the snake needs to grow
    // first, find the new x and y
    int x;
    int y;
    switch (this->direction) {
    case 'r': // right
        x = this->head->x + this->nodeSize;
        y = this->head->y;
        break;
    case 'l': // left
        x = this->head->x - this->nodeSize;
        y = this->head->y;
        break;
    case 'd': // down
        y = this->head->y + this->nodeSize;
        x = this->head->x;
        break;
    case 'u': // up
        y = this->head->y - this->nodeSize;
        x = this->head->x;
        break;
    }

    // check bounds
    bool outOfBounds = x < this->bounds[0] || x > this->bounds[2] || y < this->bounds[1] || y > this->bounds[3];
    if (outOfBounds) {
        this->isAlive = false;
        return;
    }
    
    // check for food
    bool foundFood = x == this->food->x && y == this->food->y;
    if (foundFood) {
        this->grow();
        this->food = this->makeFood();
    }
    
    // check collisions
    // iterate every other node in order to store previous node for likely removal later
    SnakeNode* prev = this->head;
    SnakeNode* curr = prev->next;
    for (int i = 2; i < this->currLength; i += 2) {
        bool collision = (x == curr->x && y == curr->y) || (x == prev->x && y == prev->y);
        if (collision) { 
            this->isAlive = false;
            return;
        }
        prev = curr->next;
        curr = prev->next;
    }
    // if length is uneven, we've missed one
    if (this->currLength % 2 == 1) {
        bool collision = x == prev->x && y == prev->y;
        if (collision) { 
            this->isAlive = false;
            return;
        }
    }

    // create newNode and make it the head of the list
    SnakeNode* newHead = new SnakeNode {x, y, this->head};
    this->head = newHead;
    this->currLength++;

    // if the snake shouldn't grow -> remove the last node
    if (this->currLength > this->length) {
        prev->next = nullptr;
        this->currLength--;
    }
}
