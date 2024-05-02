#pragma once

struct DrawData {
    int dim[2];
    int pos[2];
    DrawData* next;
};

class DataList {
private:
    DrawData* head;
    int length;
public:
    DataList(DrawData* head, int length);
    DataList();
    int getLength();
    DrawData* getData();
    void append(DrawData* d);
    void insertAtHead(DrawData* d);
    void cut(int i);
};

class Snake {
private:
    DrawData* food;
    DataList data;
    bool isAlive;
    int length;
    int nodeSize;
    char direction;
    int bounds[4];
    DrawData* makeFood();

public:
    Snake(int length, int nodeSize, char direction, int x, int y, int xMin, int yMin, int xMax, int yMax);
    void turn(char direction);
    void update();
    bool getStatus();          // returns snake->isDead
    DataList getData();
};