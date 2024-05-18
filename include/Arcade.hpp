#pragma once
#include <flecs.h>
#include <time.h>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

struct Position {
    int x, y;
};

struct Velocity {
    int x, y;
};

struct Body {
    std::vector<Position> list;
    int length;
    bool alive;
};

struct Text {
    std::string text;
    SDL_Color fontColor;
    int fontSize;
    Position pos;
};

struct RenderData {
    SDL_Texture* tex;
    std::vector<SDL_Vertex> obj;
};

struct OptionSelect {
    int selected;
    int optionsLen;
    bool finished;
};

struct UserPaths {
    char *base, *font;
};

struct Active {  };

class Scene {
protected:
    Scene(SDL_Renderer*, flecs::world*, int, int, std::string);
    int _width, _height, _wCenter, _hCenter;
    std::string _base, _font;
    SDL_Renderer* _ren;
    flecs::entity _display, _data;
    RenderData createRenderDataFromText(Text) const;
public:
    void activate() const; // give children of _display active tag
    void deactivate() const; // remove active tag from children of _display
    virtual void reset() const = 0; // sets components back to default vals
    virtual void update() const = 0; // moves scene forward in time
    virtual void renderUpdate() const = 0; // updates RenderData components
    virtual bool isGameOver() const = 0; // returns whether the game is over or not
    virtual void exec(int) const = 0; // execute an input
    // colors
    const SDL_Color GREEN {0, 155, 0, 255};
    const SDL_Color RED {155, 0, 0, 255};
    const SDL_Color OFF_WHITE {155, 155, 155, 255};
    const SDL_Color WHITE {255, 255, 255, 255};
};

class GameOver : public Scene {
private:
    SDL_Texture *_backdrop;
    SDL_Color _defaultColor, _selectedColor;
    flecs::entity _input;
    flecs::entity _optionData[2];
    flecs::entity _optionDisplay[2];
public:
    GameOver(SDL_Renderer*, flecs::world*, int, int, std::string);
    void reset() const;
    void update() const;
    void renderUpdate() const;
    bool isGameOver() const;
    void exec(int) const;
};

struct SceneManager {
    int current;
    std::vector<Scene*> scenes;
};

class Game {
protected:
    SDL_Renderer *_ren;
    flecs::entity _sceneManager;
    flecs::system _activateCurrent, _deactivateCurrent, _renderDraw;
    void setScene(int) const;
public:
    Game(SDL_Renderer*, flecs::world*, std::vector<Scene*>);
    virtual void exec(int) const = 0;
    virtual void update() const = 0;
};