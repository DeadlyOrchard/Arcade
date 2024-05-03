#pragma once

struct Color {
    int r;
    int g;
    int b;
    int a;
};

struct DrawDataItem {
    int dim[2];
    int pos[2];
    DrawDataItem* next;
};

class DrawData {
private:
    DrawDataItem* drawHead;
    int drawLen;
    Color color;
public:
    DrawData(DrawDataItem* head, int length, Color c, DrawData* next);
    DrawData();

    DrawData* next;

    int getLength();
    DrawDataItem* getHead();
    Color getColor();

    void setColor(Color c);
    void append(DrawDataItem* d);
    void insertAtHead(DrawDataItem* d);
    void cut(int i);
};

class RenderData {
private:
    DrawData* dataHead;
    int dataLen;
    DrawData* next;
public:
    RenderData(DrawData* head, int length);
    RenderData();

    int getLength();
    DrawData* getHead();

    void append(DrawData* d);
};

class Snake {
private:
    DrawData food;
    DrawData body;
    RenderData renData;
    bool isAlive;
    int length;
    int nodeSize;
    char direction;
    int bounds[4];
    DrawData makeFood();

public:
    Snake(int length, int nodeSize, char direction, int x, int y, int xMin, int yMin, int xMax, int yMax);
    void turn(char direction);
    void update();
    bool getStatus();          // returns snake->isDead
    RenderData getData();
};

