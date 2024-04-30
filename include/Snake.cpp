#include "Snake.hpp"
#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>

Snake::Snake(int length, int speed, int nodeSize, char direction, int x, int y) {
    this->speed = speed;
    this->direction = direction;
    if (nodeSize % 2 != 0) {
        throw std::invalid_argument("value nodeSize must be even.");
    }
    this->nodeSize = nodeSize;
    head.x = x;
    head.y = y;
    head.next = nullptr;
    this->length = length;
    SnakeNode curr = this->head;
    for (int i = 0; i < length; i++) {
        curr.next = new SnakeNode { x, y, nullptr };
        curr = *curr.next;
    }
}

void Snake::draw(SDL_Renderer* r) {
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SnakeNode curr = this->head;
    SDL_Rect nodeRect { 0, 0, this->nodeSize, this->nodeSize };
    int padding = nodeSize / 2;
    for (int i = 0; i < this->length; i++) {
        // origin is in the top left
        nodeRect.x = curr.x + padding;
        nodeRect.y = curr.y - padding;
        SDL_RenderDrawRect(r, &nodeRect);
        curr = *curr.next;
    }
}

void Snake::grow() {
    this->isGrowing = true;
}

void Snake::turn(char direction) {
    this->direction = direction;
}

void Snake::move() {
    // create new head, then remove the last node unless the snake isGrowing
    SnakeNode newNode;
    switch (this->direction) {
    case 'r': // right
        newNode.x = this->head.x + this->speed;
        newNode.y = this->head.y;
        break;
    case 'l': // left
        newNode.x = this->head.x - this->speed;
        newNode.y = this->head.y;
        break;
    case 'u': // up
        newNode.y = this->head.y + this->speed;
        newNode.x = this->head.x;
        break;
    case 'd': // down
        newNode.y = this->head.y - this->speed;
        newNode.x = this->head.x;
        break;
    }
    newNode.next = &this->head;
    this->head = newNode;
    this->length++;
    if (!this->isGrowing) {
        SnakeNode curr = this->head;
        for (int i = 0; i < this->length - 1; i++) {
            curr = *curr.next;
        }
        curr.next = nullptr;
        this->length--;
    } else {
        this->isGrowing = false;
    }
}
