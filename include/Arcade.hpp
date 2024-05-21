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

struct SceneStatus {
    bool paused;
    bool over;
    int menuSelection;
};

struct Active {  };

struct MenuSelect {
    int selected;
    int numberOfOptions;
};

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
    flecs::entity _display, _data, _status;
    RenderData createRenderDataFromText(Text, char) const;
public:
    // scene data getters
    bool isOver() const; // returns SceneStatus.over
    bool isPaused() const; // returns SceneStatus.paused
    int getSelection() const; // returns SceneStatus.menuSelection
    // scene data mutators
    void kill() const; // set SceneStatus.over to true
    void birth() const; // set SceneStatus.over to false
    void pause() const; // set SceneStatus.paused to true
    void resume() const; // set SceneStatus.paused to false
    void setSelection(int) const; // set SceneStatus.menuSelection
    void resetSceneData() const;
    // other mutators
    void activate() const; // give children of _display active tag
    void deactivate() const; // remove active tag from children of _display
    // pure virtuals
    virtual void reset() const = 0; // sets components back to default vals
    virtual void update() const = 0; // moves scene forward in time
    virtual void renderUpdate() const = 0; // updates RenderData components
    virtual void exec(int) const = 0; // execute an input
};

// for easily creating GameOver/Pause screens
class Menu : public Scene {
private:
    SDL_Color _defaultColor, _selectedColor;
    flecs::entity _select;
    std::vector<flecs::entity> _optionDisplay;
public:
    Menu(SDL_Renderer*, flecs::world*, int, int, std::string, int, std::string, SDL_Color, std::vector<std::string>, SDL_Color, SDL_Color);
    void reset() const;
    void update() const;
    void renderUpdate() const;
    void exec(int) const;
};

struct SceneManager {
    int current;
    std::vector<Scene*> scenes;
};

struct GameStatus {
    bool running;
};

class Game {
private:
    SDL_Renderer *_ren;
    flecs::entity _sceneManager, _status;
protected:
    // scene data getters
    Scene* getCurrentScene() const;
    int getCurrentIndex() const;
    // scene data setter
    Scene* setScene(int) const;
    // game status setter
    void kill() const;
public:
    Game(SDL_Renderer*, flecs::world*, std::vector<Scene*>);
    bool getStatus() const;
    void exec(int) const;
    virtual void update() const = 0;
};
