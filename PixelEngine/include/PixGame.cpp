#include "PixGame.hpp"
#include <iostream>

ColorManager::ColorManager(Color color) {
    _color = color;
    _len = 0;
    _head = nullptr;
    _pointer = nullptr;
    _pointerPos = -1;
    next = nullptr;
}

// getters
int ColorManager::getLen() { return _len; }
Color ColorManager::getColor() { return _color; }

// mutators
void ColorManager::addPixel(Pixel p) {
    if (_len == 0) {
        _head = new Pixel{p.x, p.y, p.next};
        _len++;
        _pointer = _head;
        _pointerPos = 0;
        return;
    }

    Pixel* temp = _head;
    for (int i = 1; i < _len; i++) {
        temp = temp->next;
    }
    temp->next = new Pixel{p.x, p.y, p.next};
    _len++;
}
Pixel ColorManager::process() {
    if (_len == 0) {
        std::cout << "Error: cannot process ColorManager with no Pixels" << std::endl;
        return Pixel{-1, -1, nullptr};
    }
    Pixel temp = *_pointer;
    _pointer = _pointer->next;
    _pointerPos++;
    return temp;
}
void ColorManager::reset() {
    _pointerPos = 0;
    _pointer = _head;
}

GameObject::GameObject(int x, int y) {
    _origin = Pixel{x, y, nullptr};
    _len = 0;
    _head = nullptr;
    _pointer = nullptr;
    _pointerPos = -1;
    next = nullptr;
}

// getters
int GameObject::getLen() { return _len; }
Pixel GameObject::getOrigin() { return _origin; }

// mutators
void GameObject::addColorMan(ColorManager c) {
    if (_len == 0) {
        _head = new ColorManager(c);
        _len++;
        _pointer = _head;
        _pointerPos = 0;
        return;
    }

    ColorManager* temp = _head;
    for (int i = 1; i < _len; i++) {
        temp = temp->next;
    }
    temp->next = new ColorManager(c);
    _len++;
}
ColorManager GameObject::process() {
    if (_len == 0) {
        std::cout << "Error: cannot process GameObject with no ColorManagers" << std::endl;
        Color c = Color{-1, -1, -1, -1};
        return ColorManager(c);
    }
    ColorManager temp = *_pointer;
    _pointer = _pointer->next;
    _pointerPos++;
    return temp;
}
void GameObject::reset() {
    _pointerPos = 0;
    _pointer = _head;
}
void GameObject::moveTo(Pixel p) {
    _origin = p;
}

Game::Game(int pixelSize) {
    _pixelSize = pixelSize;
    _len = 1;
    GameObject obj = GameObject(0, 0);
    Color w = Color{255, 255, 255, 255};
    Color r = Color{255, 0, 0, 255};
    ColorManager white = ColorManager(w);
    ColorManager red = ColorManager(r);
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            Pixel p = Pixel{i, j, nullptr};
            if (i % 2 == 0) {
                white.addPixel(p);
            } else {
                red.addPixel(p);
            }
        }
    }
    obj.addColorMan(white);
    obj.addColorMan(red);
    _head = new GameObject(obj);
    _pointer = _head;
    _pointerPos = 0;
}

// getters
int Game::getLen() { return _len; }
int Game::getPixelSize() { return _pixelSize; }

GameObject Game::process() {
    if (_len == 0) {
        std::cout << "Error: cannot process Game with no game objects" << std::endl;
        return GameObject(-1, -1);
    }
    GameObject temp = *_pointer;
    _pointer = _pointer->next;
    _pointerPos++;
    return temp;
}
void Game::reset() {
    _pointerPos = 0;
    _pointer = _head;
}
void Game::update() {
    Pixel currLoc = _head->getOrigin();
    Pixel nextLoc = Pixel {currLoc.x + 6, currLoc.y + 6, nullptr};
    _head->moveTo(nextLoc);
}