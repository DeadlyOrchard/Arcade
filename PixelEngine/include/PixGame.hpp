#pragma once

struct Pixel {
    int x, y;
    Pixel* next; // for Color's singly linked list
};

struct Color {
    int r, g, b, a;
};

class ColorManager {
private:
    Color _color;
    int _len;
    Pixel* _head;
    Pixel* _pointer;
    int _pointerPos;
public:
    ColorManager(Color);
    ColorManager* next; // for GameObject's singly linked list

    // getters
    int getLen();
    Color getColor();
    
    // mutators
    void addPixel(Pixel);
    Pixel process();
    void reset();
};

class GameObject {
private:
    int _len;
    ColorManager* _head;
    ColorManager* _pointer;
    int _pointerPos;
    Pixel _origin;
public:
    GameObject(int, int);
    GameObject* next; // for Game's singly linked list

    // getters
    int getLen();
    Pixel getOrigin();

    // mutators
    void addColorMan(ColorManager);
    ColorManager process();
    void reset();
    void moveTo(Pixel);
};

class Game {
private:
    int _pixelSize;
    int _len;
    GameObject* _head;
    GameObject* _pointer;
    int _pointerPos;
public:
    Game(int);
    
    // getters
    int getLen();
    int getPixelSize();

    // mutators
    GameObject process();
    void reset();
    void update();
};

class Snake : Game {
private:
    Pixel _head;
    Pixel _food;
    ColorManager _body;
    int _len;
    char _direction;
public:
    Snake(int, int);
    void update();
};