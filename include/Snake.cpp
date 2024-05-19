#include "Snake.hpp"
#include "Arcade.hpp"

Snake::Snake(SDL_Renderer* ren, flecs::world* ecs, int screenWidth, int screenHeight, int nodeSize, std::string basePath) :
Scene(ren, ecs, screenWidth, screenHeight, basePath) {
    // set up RenderData struct with local coordinates for one node
    _nodeSize = nodeSize;
    float fNodeSize = (float)_nodeSize;
    SDL_FPoint localCoords[6] = {
        {0, 0}, {fNodeSize, 0}, {0, fNodeSize},
        {fNodeSize, fNodeSize}, {fNodeSize, 0}, {0, fNodeSize}
    };
    SDL_Vertex v;
    v.color = { 0 };
    v.tex_coord = { 0 };
    std::vector<SDL_Vertex> nodeObj;
    for (SDL_FPoint localPoint : localCoords) {
        v.position = localPoint;
        nodeObj.push_back(v);
    }
    _nodeLocal = {nullptr, nodeObj};

    // snake entities
    Position startingPos = {_wCenter, _hCenter};
    int padding = 20;
    Text scoreText = {"1", OFF_WHITE, 18, {_width - padding, padding}};
    RenderData scoreData = createRenderDataFromText(scoreText, 'r');
    _head = ecs->entity()
        .set<Text>(scoreText)
        .set<Position>(startingPos)
        .set<Velocity>({1, 0})
        .child_of(_data);
    _score = ecs->entity()
        .set<RenderData>(scoreData)
        .child_of(_display);
    _body = ecs->entity()
        .set<Body>({{startingPos}, 1, true})
        .set<RenderData>(_nodeLocal)
        .child_of(_display);
    srand(time(0));
    Position newFoodPos = {
        (rand() % (_width / _nodeSize)) * _nodeSize,
        (rand() % (_height / _nodeSize)) * _nodeSize,
    };
    _food = ecs->entity()
        .set<Position>(newFoodPos);
}

void Snake::reset() const {
    Position *pos = _head.get_mut<Position>();
    Position startingPos = {_wCenter, _hCenter};
    *pos = startingPos;
    Velocity *vel = _head.get_mut<Velocity>();
    Velocity startingVel = {1, 0};
    *vel = startingVel;
    Text *score = _head.get_mut<Text>();
    score->text = "1";
    Body *body = _body.get_mut<Body>();
    body->alive = true;
    body->list.clear();
    body->list.push_back(startingPos);
    body->length = 1;
    Position newFoodPos = {
        (rand() % (_width / _nodeSize)) * _nodeSize,
        (rand() % (_height / _nodeSize)) * _nodeSize,
    };
    Position *foodPos = _food.get_mut<Position>();
    *foodPos = newFoodPos;
}

void Snake::exec(int in) const {
    Velocity *v = _head.get_mut<Velocity>();
    Velocity vel;
    switch(in) {
    case SDLK_w:
    case SDLK_UP:
        vel = {0, -1};
        break;
    case SDLK_r:
    case SDLK_RIGHT:
        vel = {1, 0};
        break;
    case SDLK_d:
    case SDLK_DOWN:
        vel = {0, 1};
        break;
    case SDLK_l:
    case SDLK_LEFT:
        vel = {-1, 0};
        break;
    }
    // snake can only go vertical if currently moving horizontal and vice versa
    if ((v->x != 0 && vel.x == 0) || (v->y != 0 && vel.y == 0)) {
        *v = vel;
    }
}

void Snake::update() const {
    Position *pos = _head.get_mut<Position>();
    const Velocity *vel = _head.get<Velocity>();
    Body *body = _body.get_mut<Body>();
    Position nextPos = {pos->x + _nodeSize * vel->x, pos->y + _nodeSize * vel->y};
    // check for walls
    if (nextPos.x >= _width || nextPos.x < 0 || nextPos.y >= _height || nextPos.y < 0) {
        body->alive = false;
    }
    // check for food
    Position *foodPos = _food.get_mut<Position>();
    if (nextPos.x == foodPos->x && nextPos.y == foodPos->y) {
        // move the food
        foodPos->x = (rand() % (_width / _nodeSize)) * _nodeSize;
        foodPos->y = (rand() % (_height / _nodeSize)) * _nodeSize;
        body->length++; // update game data
        _head.get_mut<Text>()->text = std::to_string(body->length).data(); // update UI
    }
    // check for collision with self
    for (Position node : body->list) {
        if (node.x == nextPos.x && node.y == nextPos.y) {
            body->alive = false;
        }
    }
    // update head and body if still alive
    if (!body->alive) { 
        return; 
    }
    pos->x = nextPos.x;
    pos->y = nextPos.y;
    body->list.insert(body->list.begin(), nextPos);
    // remove last if snake too long
    if (body->list.size() > body->length) {
        body->list.pop_back();
    }
}

void Snake::renderUpdate() const {
    const Body *body = _body.get<Body>();
    // local coordinates
    SDL_Vertex globalVert = _nodeLocal.obj[0];
    globalVert.color = GREEN;
    globalVert.tex_coord = { 0 };
    std::vector<SDL_Vertex> obj;
    // calc global coords
    for (Position node : body->list) {
        for (SDL_Vertex localVert : _nodeLocal.obj) {
            globalVert.position.x = node.x + localVert.position.x;
            globalVert.position.y = node.y + localVert.position.y;
            obj.push_back(globalVert);
        }
    }
    // same for food
    globalVert.color = RED;
    const Position *foodPos = _food.get<Position>();
    Position origin = {foodPos->x, foodPos->y};
    for (SDL_Vertex localVert : _nodeLocal.obj) {
        globalVert.position.x = origin.x + localVert.position.x;
        globalVert.position.y = origin.y + localVert.position.y;
        obj.push_back(globalVert);
    }
    // dynamic text needs to be turned into render data
    const Text *score = _head.get<Text>();

    // swap out the data
    RenderData *renBody = _body.get_mut<RenderData>();
    RenderData *renScore = _score.get_mut<RenderData>();
    *renBody = { nullptr, obj };
    SDL_Texture *old = renScore->tex;
    *renScore = createRenderDataFromText(*score, 'r');
    SDL_DestroyTexture(old);
}

bool Snake::isGameOver() const {
    const Body *body = _body.get<Body>();
    return !body->alive;
}

SnakeGame::SnakeGame(SDL_Renderer *ren, flecs::world *ecs, int screenWidth, int screenHeight, int nodeSize, std::string basePath) :
Game(ren, ecs, {
    new Snake(ren, ecs, screenWidth, screenHeight, nodeSize, basePath),
    new Menu(ren, ecs, screenWidth, screenHeight, basePath, 18, "Game Over", RED, {"New Game", "Game Select", "Quit"}, OFF_WHITE, WHITE)
}) {
}

void SnakeGame::update() const {
    const SceneManager *sm = _sceneManager.get<SceneManager>();
    if (sm->scenes[sm->current]->isGameOver()) {
        switch (sm->current) {
        case 0: // the game
            setScene(1);
            activateScene();
            break;
        case 1: // game over menu
            deactivateScene();
            resetScene();
            setScene(0);
            resetScene();
            break;
        }
    }
    updateScene();
}
