#include "Snake.hpp"

Snake::Snake(flecs::world* ecs, int screenWidth, int screenHeight, int nodeSize, std::string basePath) {
    // drawing constants
    _width = screenWidth;
    _height = screenHeight;
    _wCenter = _width / 2;
    _hCenter = _height / 2;
    _nodeSize = nodeSize;
    // snake entities
    _data = ecs->entity()
        .set<GameData>({"Snake", time(0), -1, 1});
    Position startingPos = {_wCenter, _hCenter};
    _head = ecs->entity()
        .set<Position>(startingPos)
        .set<Velocity>({1, 0});
    _body = ecs->entity()
        .set<Body>({{startingPos}, true})
        .set<Text>({"1", 18, {20, 20}});
    srand(time(0));
    int xFood = (rand() % (_width / _nodeSize)) * _nodeSize;
    int yFood = (rand() % (_height / _nodeSize)) * _nodeSize;
    _food = ecs->entity()
        .set<Position>({xFood, yFood});

    // game over entities
    _gameOver = ecs->entity();
    int lineHeight;
    _fontPath = (basePath + "PressStart.ttf").data();
    TTF_Font* font = TTF_OpenFont(_fontPath, 24);
    TTF_SizeText(font, "A", nullptr, &lineHeight);
    int linePadding = lineHeight / 2;
    Position gameOverPos = {_wCenter, _hCenter};
    Position playAgainPos = {_wCenter, _hCenter + lineHeight + linePadding};
    Position spacebarPos = {_wCenter, _hCenter + (lineHeight * 2) + (linePadding * 2)};
    ecs->entity()
        .child_of(_gameOver)
        .set<Text>({"Game Over", 24, gameOverPos})
        .disable();
    ecs->entity()
        .child_of(_gameOver)
        .set<Text>({"Play Again?", 24, playAgainPos})
        .disable();
    ecs->entity()
        .child_of(_gameOver)
        .set<Text>({"[SPACEBAR]", 14, spacebarPos})
        .disable();
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
    if ((v->x != 0 && vel.y != 0) || (v->y != 0 && vel.x != 0)) {
        *v = vel;
    }
}

void Snake::update() const {
    Position *pos = _head.get_mut<Position>();
    const Velocity *vel = _head.get<Velocity>();
    Body *body = _body.get_mut<Body>();
    GameData *data = _data.get_mut<GameData>();
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
        data->score++; // update game data
        _body.get_mut<Text>()->text = std::to_string(data->score); // update UI
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
    if (body->list.size() > data->score) {
        body->list.pop_back();
    }
}

void Snake::restart() const {
    Position *pos = _head.get_mut<Position>();
    Position startingPos = {_wCenter, _hCenter};
    *pos = startingPos;
    Velocity *vel = _head.get_mut<Velocity>();
    Velocity startingVel = {1, 0};
    *vel = startingVel;
    Body *body = _body.get_mut<Body>();
    body->alive = true;
    body->list.clear();
    body->list.push_back(startingPos);
    GameData *data = _data.get_mut<GameData>();
    data->start = time(0);
    data->end = -1;
    data->score = 1;
    _gameOver.children([](flecs::entity child) {
        child.disable();
    });
}

void Snake::end() const {
    GameData *data = _data.get_mut<GameData>();
    data->end = time(0);
    _gameOver.children([](flecs::entity child) {
        child.enable();
    });
}

std::vector<SDL_Vertex> Snake::getRenderData() const {
    std::vector<SDL_Vertex> obj;
    const Body *body = _body.get<Body>();
    SDL_Color color = {0, 155, 0, 255};
    SDL_Vertex v;
    v.color = color;
    v.tex_coord = SDL_FPoint{ 0 };
    // local coordinates
    Position local[6] = {
        {0, 0}, {_nodeSize, 0}, {0, _nodeSize},
        {_nodeSize, _nodeSize}, {_nodeSize, 0}, {0, _nodeSize} };
    for (Position node : body->list) {
        Position origin = {node.x, node.y};
        for (Position pos : local) {
            v.position.x = origin.x + pos.x;
            v.position.y = origin.y + pos.y;
            obj.push_back(v);
        }
    }
    color = {155, 0, 0, 255};
    v.color = color;
    const Position *foodPos = _food.get<Position>();
    Position origin = {foodPos->x, foodPos->y};
    for (Position pos : local) {
        v.position.x = origin.x + pos.x;
        v.position.y = origin.y + pos.y;
        obj.push_back(v);
    }
    return obj;
}

bool Snake::isAlive() const {
    const Body *body = _body.get<Body>();
    return body->alive;
}