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
    std::vector<Text> options;
    bool finished;
};

struct Active {  };

// frequently used colors
const SDL_Color GREEN {0, 155, 0, 255};
const SDL_Color RED {155, 0, 0, 255};
const SDL_Color OFF_WHITE {155, 155, 155, 255};
const SDL_Color WHITE {255, 255, 255, 255};

class Scene {
protected:
    Scene(SDL_Renderer*, flecs::world*, int, int, std::string);
    int _width, _height, _wCenter, _hCenter;
    std::string _base, _font;
    SDL_Renderer* _ren;
    flecs::entity _display, _data;
    RenderData createRenderDataFromText(Text, char) const;
public:
    void activate() const; // give children of _display active tag
    void deactivate() const; // remove active tag from children of _display
    virtual void reset() const = 0; // sets components back to default vals
    virtual void update() const = 0; // moves scene forward in time
    virtual void renderUpdate() const = 0; // updates RenderData components
    virtual bool isGameOver() const = 0; // returns whether the game is over or not
    virtual void exec(int) const = 0; // execute an input
};

// for easily creating GameOver/Pause screens
class Menu : public Scene {
private:
    SDL_Color _defaultColor, _selectedColor;
    flecs::entity _input;
    std::vector<flecs::entity> _optionDisplay;
public:
    Menu(SDL_Renderer*, flecs::world*, int, int, std::string, int, std::string, SDL_Color, std::vector<std::string>, SDL_Color, SDL_Color);
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
private:
    SDL_Renderer *_ren;
    flecs::system _activateCurrent, _deactivateCurrent, _resetCurrent, _sceneUpdate, _renderDraw;
protected:
    flecs::entity _sceneManager;
    void updateScene() const;
    void deactivateScene() const;
    void activateScene() const;
    void resetScene() const;
    void setScene(int) const;
public:
    Game(SDL_Renderer*, flecs::world*, std::vector<Scene*>);
    void exec(int) const;
    virtual void update() const = 0;
};