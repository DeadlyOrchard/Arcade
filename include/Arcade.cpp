#include "Arcade.hpp"

Scene::Scene(SDL_Renderer *ren, flecs::world *ecs, int screenWidth, int screenHeight, std::string basePath) {
    _width = screenWidth;
    _height = screenHeight;
    _wCenter = _width / 2;
    _hCenter = _height / 2;
    _ren = ren;
    _display = ecs->entity();
    _data = ecs->entity();
    _base = basePath;
    _font = (basePath + "PressStart.ttf");
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
        textOpts.push_back({options[i], _defaultColor, fontSize, posOpts[i]});
    }
    
    // input component
    _input = ecs->entity()
        .set<OptionSelect>({0, textOpts, false})
        .child_of(_data);

    // create RenderData components
    std::vector<RenderData> dataOpts;
    RenderData dataPrompt = createRenderDataFromText({msg, msgColor, fontSize, msgPos}, 'l');
    for (int i = 0; i < options.size(); i++) {
        dataOpts.push_back(createRenderDataFromText(textOpts[i], 'l'));
    }

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
}

void Menu::reset() const {
    OptionSelect *optSelect = _input.get_mut<OptionSelect>();
    for (int i = 0; i < optSelect->options.size(); ++i) {
        RenderData dataOpt = createRenderDataFromText(optSelect->options[i], 'l');
        RenderData *option = _optionDisplay[i].get_mut<RenderData>();
        *option = dataOpt;
    }
    optSelect->finished = false;
}

void Menu::update() const {} // no game logic to execute for menus

void Menu::renderUpdate() const {
    const OptionSelect *optSelect = _input.get<OptionSelect>();
    for (int i = 0; i < optSelect->options.size(); ++i) {
        Text *t = _optionDisplay[i].get_mut<Text>();
        RenderData *d = _optionDisplay[i].get_mut<RenderData>();
        if (i == optSelect->selected) {
            t->fontColor = _selectedColor;
        } else {
            t->fontColor = _defaultColor;
        }
        SDL_Texture *old = d->tex;
        *d = createRenderDataFromText(*t, 'l');
        SDL_DestroyTexture(old);
    }
}

bool Menu::isGameOver() const {
    const OptionSelect *inData = _input.get<OptionSelect>();
    return inData->finished;
}

void Menu::exec(int in) const {
    OptionSelect *inData = _input.get_mut<OptionSelect>();
    if (!inData->finished) {
        switch(in) {
        case SDLK_UP:
        case SDLK_w:
            inData->selected--;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            inData->selected++;
            break;
        case SDLK_SPACE:
            inData->finished = true;
            break;
        }
        if (inData->selected == inData->options.size()) {
            inData->selected = 0;
        } else if (inData->selected < 0) {
            inData->selected = inData->options.size() - 1;
        }
    }
}

Game::Game(SDL_Renderer *ren, flecs::world *ecs, std::vector<Scene*> scenes) {
    // systems
    // activate current scene - ran manually
    _activateCurrent = ecs->system()
        .iter([this](flecs::iter &it) {
            const SceneManager *sm = _sceneManager.get<SceneManager>();
            sm->scenes[sm->current]->activate();
        });
    _activateCurrent.disable();

    // deactivate current scene - ran manually
    _deactivateCurrent = ecs->system()
        .iter([this](flecs::iter &it) {
            const SceneManager *sm = _sceneManager.get<SceneManager>();
            sm->scenes[sm->current]->deactivate();
        });
    _deactivateCurrent.disable();

    // reset current scene - ran manually
    _resetCurrent = ecs->system()
        .iter([this](flecs::iter &it) {
            const SceneManager *sm = _sceneManager.get<SceneManager>();
            sm->scenes[sm->current]->reset();
        });
    _resetCurrent.disable();

    // update game logic - ran manually
    _sceneUpdate = ecs->system<const SceneManager>()
        .kind(0)
        .each([](const SceneManager &sm) {
            std::cout << "Scene update" << '\t';
            sm.scenes[sm.current]->update();
        });

    // update render components - ran automatically each PreStore phase
    ecs->system<const SceneManager>()
        .kind(flecs::PreStore)
        .each([this](const SceneManager &sm) {
            std::cout << "Render update" << '\t';
            SDL_SetRenderDrawColor(_ren, 33, 33, 33, 255);
            SDL_RenderClear(_ren);
            sm.scenes[sm.current]->renderUpdate();
        });

    // draw all RenderData components - ran manually
    _renderDraw = ecs->system<const RenderData, const Active>()
        .kind(0)
        .each([this](const RenderData &data, const Active &status) {
            std::cout << "Render draw" << '\t';
            int err = SDL_RenderGeometry(_ren, data.tex, data.obj.data(), data.obj.size(), nullptr, 0);
            if (err != 0) {
                printf("%s\n", SDL_GetError());
            }
        });
    
    // present renderer - ran automatically each OnStore phase
    ecs->system()
        .kind(flecs::OnStore)
        .iter([this] (flecs::iter &it) {
            _renderDraw.run();
            std::cout << "Render present" << std::endl;
            SDL_RenderPresent(_ren);
        });

    _ren = ren;
    _sceneManager = ecs->entity()
        .set<SceneManager>({0, scenes});
    _activateCurrent.run();
}

void Game::updateScene() const {
    _sceneUpdate.run();
}

void Game::deactivateScene() const {
    _deactivateCurrent.run();
}

void Game::activateScene() const {
    _activateCurrent.run();
}

void Game::resetScene() const {
    _resetCurrent.run();
}

void Game::setScene(int i) const {
    SceneManager *sm = _sceneManager.get_mut<SceneManager>();
    sm->current = i;
}

void Game::exec(int in) const {
    const SceneManager *sm = _sceneManager.get<SceneManager>();
    sm->scenes[sm->current]->exec(in);
}