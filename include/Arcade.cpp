#include "Arcade.hpp"

Scene::Scene(SDL_Renderer *ren, flecs::world *ecs, int screenWidth, int screenHeight, std::string basePath) {
    _width = screenWidth;
    _height = screenHeight;
    _wCenter = _width / 2;
    _hCenter = _height / 2;
    _ren = ren;
    _base = basePath;
    _font = (basePath + "PressStart.ttf");
    _display = ecs->entity();
    _data = ecs->entity();
    _status = ecs->entity()
        .child_of(_data)
        .set<SceneStatus>({false, false, 0});
}

RenderData Scene::createRenderDataFromText(Text t, char alignment) const {
    SDL_Rect iRect = {t.pos.x, t.pos.y, 0, 0};
    TTF_Font* font = TTF_OpenFont(_font.data(), t.fontSize);
    TTF_SizeText(font, t.text.data(), &iRect.w, &iRect.h);
    SDL_FRect fRect = {(float)iRect.x, (float)iRect.y, (float)iRect.w, (float)iRect.h};
    SDL_Surface *surf = TTF_RenderText_Solid(font, t.text.data(), t.fontColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(_ren, surf);
    std::vector<SDL_Vertex> obj;
    switch(alignment) {
    case 'l': // left
        obj = {
            {{fRect.x, fRect.y}, WHITE, {0, 0}}, // top left
            {{fRect.x + fRect.w, fRect.y}, WHITE, {1, 0}}, // top right
            {{fRect.x, fRect.y + fRect.h}, WHITE, {0, 1}}, // bottom left
            {{fRect.x + fRect.w, fRect.y + fRect.h}, WHITE, {1, 1}}, // bottom right
            {{fRect.x + fRect.w, fRect.y}, WHITE, {1, 0}}, // top right
            {{fRect.x, fRect.y + fRect.h}, WHITE, {0, 1}} // bottom left
        };
        break;
    case 'r': // right
        // same as left, but subtract fRect.w from each x coordinate
        obj = {
            {{fRect.x - fRect.w, fRect.y}, WHITE, {0, 0}}, // top left
            {{fRect.x, fRect.y}, WHITE, {1, 0}}, // top right
            {{fRect.x - fRect.w, fRect.y + fRect.h}, WHITE, {0, 1}}, // bottom left
            {{fRect.x, fRect.y + fRect.h}, WHITE, {1, 1}}, // bottom right
            {{fRect.x, fRect.y}, WHITE, {1, 0}}, // top right
            {{fRect.x - fRect.w, fRect.y + fRect.h}, WHITE, {0, 1}} // bottom left
        };
        break;
    }
    TTF_CloseFont(font);
    SDL_FreeSurface(surf);
    return RenderData{tex, obj};
}

bool Scene::isOver() const {
    return _status.get<SceneStatus>()->over;
}

bool Scene::isPaused() const {
    return _status.get<SceneStatus>()->paused;
}

int Scene::getSelection() const {
    return _status.get<SceneStatus>()->menuSelection;
}

void Scene::kill() const {
    _status.get_mut<SceneStatus>()->over = true;
}

void Scene::birth() const {
    _status.get_mut<SceneStatus>()->over = false;
}

void Scene::pause() const {
    _status.get_mut<SceneStatus>()->paused = true;
}

void Scene::resume() const {
    _status.get_mut<SceneStatus>()->paused = false;
}

void Scene::resetSceneData() const {
    SceneStatus *status = _status.get_mut<SceneStatus>();
    status->paused = false;
    status->over = false;
    status->menuSelection = 0;
}

void Scene::setSelection(int selection) const {
    _status.get_mut<SceneStatus>()->menuSelection = selection;
}

void Scene::activate() const {
    _display.children([](flecs::entity child) {
        child.add<Active>();
    });
}

void Scene::deactivate() const {
    _display.children([](flecs::entity child) {
        child.remove<Active>();
    });
}

Menu::Menu(SDL_Renderer* ren, flecs::world* ecs, int screenWidth, int screenHeight, std::string basePath, 
    int fontSize, std::string msg, SDL_Color msgColor, std::vector<std::string> options, SDL_Color defaultColor, SDL_Color selectedColor) :
    Scene(ren, ecs, screenWidth, screenHeight, basePath) {
    std::cout << "creating menu" << std::endl;
    _defaultColor = defaultColor;
    _selectedColor = selectedColor;
    
    // get vals to calc spacing
    int lineHeight;
    TTF_Font* font = TTF_OpenFont(_font.data(), 18);
    TTF_SizeText(font, "A", nullptr, &lineHeight);
    TTF_CloseFont(font);
    int linePadding = lineHeight / 2;
    // calc spacing
    int padding = 20;
    std::vector<Position> posOpts;

    Position msgPos = {padding, padding};
    for (int i = 1; i <= options.size(); i++) { // start at one because we already did one (msgPos)
        posOpts.push_back({padding, padding + lineHeight * i + linePadding * i});
    }

    // create Text components
    std::vector<Text> textOpts;
    for (int i = 0; i < options.size(); ++i) {
        switch(i) {
        case 0:
            textOpts.push_back({options[i], _selectedColor, fontSize, posOpts[i]});
            break;
        default:
            textOpts.push_back({options[i], _defaultColor, fontSize, posOpts[i]});
            break;
        }
    }

    // create RenderData components
    std::cout << "create renderdata comps" << std::endl;
    std::vector<RenderData> dataOpts;
    RenderData dataPrompt = createRenderDataFromText({msg, msgColor, fontSize, msgPos}, 'l');
    for (int i = 0; i < options.size(); i++) {
        dataOpts.push_back(createRenderDataFromText(textOpts[i], 'l'));
    }

    _select = ecs->entity()
        .set<MenuSelect>({0, (int)textOpts.size()})
        .child_of(_data);

    // give components to entities
    ecs->entity()
        .set<RenderData>(dataPrompt)
        .child_of(_display);
    for (int i = 0; i < dataOpts.size(); i++) {
        _optionDisplay.push_back(ecs->entity()
            .set<RenderData>(dataOpts[i])
            .set<Text>(textOpts[i])
            .child_of(_display));
    }
    std::cout << "finished making menu" << std::endl;
}

void Menu::reset() const {
    // reset options
    MenuSelect *select = _select.get_mut<MenuSelect>();
    for (int i = 0; i < select->numberOfOptions; ++i) {
        RenderData dataOpt = createRenderDataFromText(*_optionDisplay[i].get<Text>(), 'l');
        RenderData *option = _optionDisplay[i].get_mut<RenderData>();
        *option = dataOpt;
    }
    select->selected = 0;
    resetSceneData();
}

void Menu::update() const {} // no game logic to execute for menus

void Menu::renderUpdate() const {
    const MenuSelect *select = _select.get<MenuSelect>();
    for (int i = 0; i < select->numberOfOptions; ++i) {
        Text *t = _optionDisplay[i].get_mut<Text>();
        RenderData *d = _optionDisplay[i].get_mut<RenderData>();
        if (i == select->selected) {
            t->fontColor = _selectedColor;
        } else {
            t->fontColor = _defaultColor;
        }
        SDL_Texture *old = d->tex;
        *d = createRenderDataFromText(*t, 'l');
        SDL_DestroyTexture(old);
    }
}

void Menu::exec(int in) const {
    MenuSelect *select = _select.get_mut<MenuSelect>();
    if (isOver()) { return; }
    switch(in) {
    case SDLK_UP:
    case SDLK_w:
        select->selected--;
        break;
    case SDLK_DOWN:
    case SDLK_s:
        select->selected++;
        break;
    case SDLK_RETURN:
        setSelection(select->selected);
        kill();
        break;
    }

    // round robin
    if (select->selected == select->numberOfOptions) {
        select->selected = 0;
    } else if (select->selected < 0) {
        select->selected = select->numberOfOptions - 1;
    }
}

Game::Game(SDL_Renderer *ren, flecs::world *ecs, std::vector<Scene*> scenes) {
    std::cout << "creating game" << std::endl;
    // systems
    // update render components - ran automatically each PostUpdate phase
    ecs->system<const SceneManager>()
        .kind(flecs::PostUpdate)
        .each([this](const SceneManager &sm) {
            // std::cout << "render update" << '\t';
            SDL_SetRenderDrawColor(_ren, 33, 33, 33, 255);
            SDL_RenderClear(_ren);
            sm.scenes[sm.current]->renderUpdate();
        });

    // draw all RenderData components - ran automatically each PreStore phase
    ecs->system<const RenderData, const Active>()
        .kind(flecs::PreStore)
        .each([this](const RenderData &data, const Active &status) {
            // std::cout << "render draw" << '\t';
            int err = SDL_RenderGeometry(_ren, data.tex, data.obj.data(), data.obj.size(), nullptr, 0);
            if (err != 0) {
                printf("%s\n", SDL_GetError());
            }
        });
    
    // present renderer - ran automatically each OnStore phase
    ecs->system()
        .kind(flecs::OnStore)
        .iter([this] (flecs::iter &it) {
            // std::cout << "render present" << std::endl;
            SDL_RenderPresent(_ren);
        });

    // member variables
    _ren = ren;
    _sceneManager = ecs->entity()
        .set<SceneManager>({0, scenes});
    _status = ecs->entity()
        .set<GameStatus>({true});
    
    // activate first scene in list
    flecs::system setup = ecs->system<const SceneManager>()
        .kind(0)
        .each([](const SceneManager &sm) {
            sm.scenes[0]->activate();
        });
    setup.run();
}

Scene* Game::getCurrentScene() const {
    const SceneManager *sm = _sceneManager.get<SceneManager>();
    return sm->scenes[sm->current];
}

int Game::getCurrentIndex() const {
    return _sceneManager.get<SceneManager>()->current;
}

Scene* Game::setScene(int i) const {
    SceneManager *sm = _sceneManager.get_mut<SceneManager>();
    sm->current = i;
    return sm->scenes[sm->current];
}

void Game::kill() const {
    _status.get_mut<GameStatus>()->running = false;
}

bool Game::getStatus() const {
    return _status.get<GameStatus>()->running;
}

void Game::exec(int in) const {
    const SceneManager *sm = _sceneManager.get<SceneManager>();
    sm->scenes[sm->current]->exec(in);
}