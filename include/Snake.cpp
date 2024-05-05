#include "Snake.hpp"
#include <time.h>
#include <iostream>

DrawData::DrawData(DrawDataItem* head, int length, Color c, DrawData* next) {
    this->len = length;
    this->head = new DrawDataItem{{head->dim[0], head->dim[1]}, {head->pos[0], head->pos[1]}, nullptr};
    DrawDataItem* curr = this->head;
    DrawDataItem* temp = head;
    for (int i = 1; i < length; i++) {
        temp = temp->next;
        curr->next = new DrawDataItem{ {temp->dim[0], temp->dim[1]}, {temp->pos[0], temp->pos[1]}, nullptr };
        curr = curr->next;
    }
    this->next = next;
    this->color = c;
}

DrawData::DrawData() {
    this->head = nullptr;
    this->len = 0;
}

// getters
int DrawData::getLength() { return this->len; }
DrawDataItem* DrawData::getHead() { return this->head; }
Color DrawData::getColor() { return this->color; }

// setter
void DrawData::setColor(Color c) {
    this->color = c;
}

// mutators
void DrawData::insertAtHead(DrawDataItem* d) {
    DrawDataItem* newHead = new DrawDataItem{ {d->dim[0], d->dim[1]}, {d->pos[0], d->pos[1]}, this->head };
    this->head = newHead;
    this->len++;
}
void DrawData::append(DrawDataItem* d) {
    DrawDataItem* curr = this->head;
    for (int i = 0; i < this->len - 1; i++) {
        curr = curr->next;
    }
    curr->next = new DrawDataItem{ {d->dim[0], d->dim[1]}, {d->pos[0], d->pos[1]}, nullptr };
    this->len++;
}
void DrawData::cut(int i) {
    DrawDataItem* curr = this->head;
    for (int j = 0; j < i - 1; j++) {
        curr = curr->next;
    }
    curr->next = nullptr;
    int diff = this->len - i;
    this->len -= diff;
}

RenderData::RenderData(DrawData* head, int length) {
    this->len = length;
    this->head = new DrawData(head->getHead(), head->getLength(), head->getColor(), nullptr);
    DrawData* curr = this->head;
    DrawData* temp = head;
    for (int i = 1; i < length; i++) {
        temp = temp->next;
        curr->next = new DrawData(temp->getHead(), temp->getLength(), temp->getColor(), nullptr);
        curr = curr->next;
    }
    this->next = next;
}

RenderData::RenderData() {
    this->head = nullptr;
    this->len = 0;
}

// getters
int RenderData::getLength() {
    return this->len;
}
DrawData* RenderData::getHead() {
    return this->head;
}

// mutators
void RenderData::append(DrawData* d) {
    DrawData* curr = this->head;
    for (int i = 0; i < this->len - 1; i++) {
        curr = curr->next;
    }
    DrawDataItem* data = d->getHead();
    int len = d->getLength();
    Color c = d->getColor();
    curr->next = new DrawData(data, len, c, nullptr);
    this->len++;
}

Snake::Snake(int length, int nodeSize, char direction, int x, int y, int minX, int minY, int maxX, int maxY) {
    this->nodeSize = nodeSize;
    this->direction = direction;
    this->length = length;
    DrawDataItem* head = new DrawDataItem{{this->nodeSize, this->nodeSize}, {x, y}, nullptr};
    Color c = Color{255, 0, 0, 255};
    DrawData* body = new DrawData(head, 1, c, nullptr);
    this->renData = RenderData(body, 1);
    this->bounds[0] = minX;
    this->bounds[1] = minY;
    this->bounds[2] = maxX;
    this->bounds[3] = maxY;
    this->isAlive = true;
    srand(time(0));
    this->food = this->makeFood();
}

bool Snake::getStatus() { return this->isAlive; }

RenderData Snake::getData() {
    DrawData f = this->food;
    RenderData r = this->renData;
    r.append(&f);
    return r;
}

DrawData Snake::makeFood() {
    int x = rand() % (this->bounds[2] / this->nodeSize) * this->nodeSize;
    int y = rand() % (this->bounds[3] / this->nodeSize) * this->nodeSize;
    DrawDataItem* food = new DrawDataItem{{this->nodeSize, this->nodeSize}, {x, y}, nullptr};
    Color c = Color{0, 0, 255, 255};
    DrawData d = DrawData(food, 1, c, nullptr);
    return d;
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
    DrawData* body = this->renData.getHead();
    DrawDataItem* head = body->getHead();
    int pos[2] = {head->pos[0], head->pos[1]};
    int x;
    int y;
    switch (this->direction) {
    case 'r': // right
        x = pos[0] + this->nodeSize;
        y = pos[1];
        break;
    case 'l': // left
        x = pos[0] - this->nodeSize;
        y = pos[1];
        break;
    case 'd': // down
        y = pos[1] + this->nodeSize;
        x = pos[0];
        break;
    case 'u': // up
        y = pos[1] - this->nodeSize;
        x = pos[0];
        break;
    }

    // check bounds
    bool outOfBounds = x < this->bounds[0] || x > this->bounds[2] || y < this->bounds[1] || y > this->bounds[3];
    if (outOfBounds) {
        this->isAlive = false;
        return;
    }
    
    // check for food
    DrawDataItem* foodPos = this->food.getHead();
    bool foundFood = x == foodPos->pos[0] && y == foodPos->pos[1];
    if (foundFood) {
        this->length++;
        this->food = this->makeFood();
    }
    
    // check collisions
    // iterate every other node in order to store previous node for likely removal later
    DrawDataItem* prev = head;
    DrawDataItem* curr = prev->next;
    int len = body->getLength();
    for (int i = 2; i < body->getLength(); i += 2) {
        bool collision = (x == curr->pos[0] && y == curr->pos[1]) || (x == prev->pos[0] && y == prev->pos[1]);
        if (collision) { 
            this->isAlive = false;
            return;
        }
        prev = curr->next;
        curr = prev->next;
    }
    // if length is uneven, we've missed one
    if (len % 2 == 1) {
        bool collision = x == prev->pos[0] && y == prev->pos[1];
        if (collision) { 
            this->isAlive = false;
            return;
        }
    }

    // create newNode and make it the head of the list
    DrawDataItem* newHead = new DrawDataItem {{this->nodeSize, this->nodeSize}, {x, y}, head};
    body->insertAtHead(newHead);

    // if the snake shouldn't grow -> remove the last node
    if (body->getLength() > this->length) {
        body->cut(this->length);
    }
}
