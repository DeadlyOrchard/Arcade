#include <stdio.h>
#include <SDL2/SDL.h>
#include <flecs.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>

// components
struct Highscore {
    std::string game;
    time_t date;
    int score;
};

struct UserData {
    Highscore snake;
};

struct GameData {
    time_t start, end;
    int score;
};

struct Position {
    int x, y;
};

struct PositionList {
    std::vector<Position> list;
};

struct Velocity {
    int x, y;
};

struct Snake {
    int length;
    int alive;
};

// tags
struct Food {  };
struct Grow {  };

// phases
struct SnakeUpdate {  };
struct SnakeDead {  };

// rendering constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int WIN_FLAGS = SDL_WINDOW_BORDERLESS;
const int NODE_SIZE = 20;

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);
    ecs.set_target_fps(12);
    srand(time(0));

    /* Default Heirarchy
     *  - SnakeGame [GameData]
     *      - Head [Position, Velocity]
     *      - Body [PositionList]
     *      - Food [Position, Food]
     */
    flecs::entity snake_game = ecs.entity("SnakeGame")
        .set<GameData>({time(0), -1, 0});
    int xFood = (rand() % (WIDTH / NODE_SIZE)) * NODE_SIZE;
    int yFood = (rand() % (WIDTH / NODE_SIZE)) * NODE_SIZE;
    flecs::entity food = ecs.entity("Food")
        .child_of(snake_game)
        .set<Position>({xFood, yFood})
        .add<Food>();
    Position startingPos = {WIDTH / 2, HEIGHT / 2};
    flecs::entity head = ecs.entity("Snake")
        .child_of(snake_game)
        .set<Position>(startingPos)
        .set<Velocity>({1, 0})
        .set<Snake>({1, true});
    flecs::entity body = ecs.entity()
        .child_of(snake_game).set<PositionList>({});
    body.get_mut<PositionList>()->list.push_back(startingPos);

    flecs::system snakeUpdate = ecs.system("SnakeUpdate")
        .kind(flecs::OnUpdate)
        .iter([head, body, food] (flecs::iter &it) {
            // get next position
            Position *snakePos = head.get_mut<Position>();
            const Velocity *snakeVel = head.get<Velocity>();
            Position nextPos = {snakePos->x + NODE_SIZE * snakeVel->x, snakePos->y + NODE_SIZE * snakeVel->y};
            // check for walls
            if (nextPos.x > WIDTH || nextPos.x < 0 || nextPos.y > HEIGHT || nextPos.y < 0) {
                Snake* s = head.get_mut<Snake>();
                s->alive = false;
            }
            // check for food
            Position *foodPos = food.get_mut<Position>();
            if (nextPos.x == foodPos->x && nextPos.y == foodPos->y) {
                foodPos->x = (rand() % (WIDTH / NODE_SIZE)) * NODE_SIZE;
                foodPos->y = (rand() % (HEIGHT / NODE_SIZE)) * NODE_SIZE;
                Snake *s = head.get_mut<Snake>();
                s->length++;
            }
            // check for collision with self
            PositionList* nodes = body.get_mut<PositionList>();
            for (Position node : nodes->list) {
                if (node.x == nextPos.x && node.y == nextPos.y) {
                    Snake* s = head.get_mut<Snake>();
                    s->alive = false;
                }
            }
            // update head and body
            snakePos->x = nextPos.x;
            snakePos->y = nextPos.y;
            nodes->list.insert(nodes->list.begin(), nextPos);
            // remove last if snake too long
            Snake* s = head.get_mut<Snake>();
            if (nodes->list.size() > s->length) {
                nodes->list.pop_back();
            }
        });
    
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, WIN_FLAGS, &win, &ren);
    if (win == nullptr) {
        std::cout << "Could not create window" << std::endl;
        return -1;
    }
    if (ren == nullptr) {
        std::cout << "Could not create renderer" << std::endl;
        return -1;
    }

    flecs::system renDraw = ecs.system("RenderDraw")
        .kind(flecs::PreStore)
        .iter([ren, body, food] (flecs::iter &it) {
            SDL_SetRenderDrawColor(ren, 33, 33, 33, 255);
            SDL_RenderClear(ren);
            SDL_SetRenderDrawColor(ren, 0, 155, 0, 255);
            const PositionList *nodes = body.get<PositionList>();
            SDL_Rect rect = {0, 0, NODE_SIZE, NODE_SIZE};
            for (Position node : nodes->list) {
                rect.x = node.x;
                rect.y = node.y;
                SDL_RenderFillRect(ren, &rect);
            }
            SDL_SetRenderDrawColor(ren, 155, 0, 0, 255);
            const Position *foodPos = food.get<Position>();
            rect.x = foodPos->x;
            int offset = HEIGHT + NODE_SIZE;
            rect.y = foodPos->y;
            SDL_RenderFillRect(ren, &rect);
        });
    
    flecs::system renPresent = ecs.system<const Position>("RenderPresent")
        .kind(flecs::OnStore)
        .iter([ren] (flecs::iter &it) {
            SDL_RenderPresent(ren);
        });
    
    bool running = true;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                Velocity* v = head.get_mut<Velocity>();
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_w:
                case SDLK_UP:
                    v->x = 0;
                    v->y = -1;
                    break;
                case SDLK_r:
                case SDLK_RIGHT:
                    v->x = 1;
                    v->y = 0;
                    break;
                case SDLK_d:
                case SDLK_DOWN:
                    v->x = 0;
                    v->y = 1;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    v->x = -1;
                    v->y = 0;
                    break;
                }
                break;
            }
        }
        const Snake *s = head.get<Snake>();
        if (!s->alive) {
            Snake *snake = head.get_mut<Snake>();
            Position *headPos = head.get_mut<Position>();
            PositionList *bodyList = body.get_mut<PositionList>();
            Position startingPos = {WIDTH / 2, HEIGHT / 2};
            headPos->x = startingPos.x;
            headPos->y = startingPos.y;
            bodyList->list.clear();
            bodyList->list.push_back(startingPos);
            snake->alive = true;
            snake->length = 1;
            SDL_Delay(3000);
        }
        ecs.progress();
    }

    return 0;
}