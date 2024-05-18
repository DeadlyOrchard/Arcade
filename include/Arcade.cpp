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

RenderData Scene::createRenderDataFromText(Text t) const {
    SDL_Rect iRect = {t.pos.x, t.pos.y, 0, 0};
    TTF_Font* font = TTF_OpenFont(_font.data(), t.fontSize);
    TTF_SizeText(font, t.text.data(), &iRect.w, &iRect.h);
    SDL_FRect fRect = {(float)iRect.x, (float)iRect.y, (float)iRect.w, (float)iRect.h};
    SDL_Surface *surf = TTF_RenderText_Solid(font, t.text.data(), t.fontColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(_ren, surf);
    std::vector<SDL_Vertex> obj = {
        {{fRect.x, fRect.y}, WHITE, {0, 0}}, // top left
        {{fRect.x + fRect.w, fRect.y}, WHITE, {1, 0}}, // top right
        {{fRect.x, fRect.y + fRect.h}, WHITE, {0, 1}}, // bottom left
        {{fRect.x + fRect.w, fRect.y + fRect.h}, WHITE, {1, 1}}, // bottom right
        {{fRect.x + fRect.w, fRect.y}, WHITE, {1, 0}}, // top right
        {{fRect.x, fRect.y + fRect.h}, WHITE, {0, 1}} // bottom left
    };
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

GameOver::GameOver(SDL_Renderer* ren, flecs::world* ecs, int screenWidth, int screenHeight, std::string basePath) :
Scene(ren, ecs, screenWidth, screenHeight, basePath) {
    _defaultColor = OFF_WHITE;
    _selectedColor = WHITE;
    
    // input component
    _input = ecs->entity()
        .set<OptionSelect>({0, 2, false})
        .child_of(_data);
    
    // get vals to calc spacing
    int lineHeight;
    TTF_Font* font = TTF_OpenFont(_font.data(), 24);
    TTF_SizeText(font, "A", nullptr, &lineHeight);
    TTF_CloseFont(font);
    int linePadding = lineHeight / 2;
    // calc spacing
    int padding = 20;
    Position optionPos[2];
    Position msgPos = {padding, padding};
    for (int i = 1; i < 3; i++) { // start at one because we already did one (msgPos)
        optionPos[i - 1] = {padding, padding + lineHeight * i + linePadding * i};
    }
    // save positions for potential .restart()
    for (int i = 0; i < 2; i++) {
        _optionData[i] = ecs->entity()
            .set<Position>(optionPos[i])
            .child_of(_data);
    }
    // create Text components
    Text msgT = {"Game Over", RED, 24, msgPos};
    Text optionT[2];
    std::string optionMsg[2] = {"New Game", "Game Select"};
    for (int i = 0; i < 2; i++) {
        optionT[i] = {optionMsg[i], _defaultColor, 18, optionPos[i]};
    }
    // create RenderData components
    RenderData optionD[2];
    RenderData msgD = createRenderDataFromText(msgT);
    for (int i = 0; i < 2; i++) {
        optionD[i] = createRenderDataFromText(optionT[i]);
    }
    // save them to entities
    ecs->entity()
        .set<RenderData>(msgD)
        .child_of(_display);
    for (int i = 0; i < 2; i++) {
        _optionDisplay[i] = ecs->entity()
            .set<RenderData>(optionD[i])
            .set<Text>(optionT[i])
            .child_of(_display);
    }
}

void GameOver::reset() const {
    std::string optionMsg[2] = {"New Game", "Game Select"};
    for (int i = 0; i < 2; i++) {
        const Position *optionPos = _optionData[i].get<Position>();
        Text optionT = {optionMsg[i], _defaultColor, 18, *optionPos};
        RenderData optionD = createRenderDataFromText(optionT);
        RenderData *option = _optionDisplay[i].get_mut<RenderData>();
        *option = optionD;
    }
    OptionSelect *inData = _input.get_mut<OptionSelect>();
    inData->finished = false;
}

void GameOver::update() const {
    // no game logic
}

void GameOver::renderUpdate() const {
    const OptionSelect *selection = _input.get<OptionSelect>();
    for (int i = 0; i < 2; ++i) {
        Text *t = _optionDisplay[i].get_mut<Text>();
        RenderData *d = _optionDisplay[i].get_mut<RenderData>();
        if (i == selection->selected) {
            t->fontColor = WHITE;
        } else {
            t->fontColor = OFF_WHITE;
        }
        SDL_Texture *old = d->tex;
        *d = createRenderDataFromText(*t);
        SDL_DestroyTexture(old);
    }
}

void GameOver::exec(int in) const {
    OptionSelect *inData = _input.get_mut<OptionSelect>();
    if (!inData->finished) {
        switch(in) {
        case SDLK_UP:
        case SDLK_w:
            inData->selected++;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            inData->selected--;
            break;
        case SDLK_SPACE:
            inData->finished = true;
            break;
        }
        if (inData->selected == inData->optionsLen) {
            inData->selected = 0;
        } else if (inData->selected < 0) {
            inData->selected = inData->optionsLen - 1;
        }
    }
}

bool GameOver::isGameOver() const {
    const OptionSelect *inData = _input.get<OptionSelect>();
    return inData->finished;
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

    // update game logic - ran automatically each OnUpdate phase
    ecs->system<SceneManager>()
        .kind(flecs::OnUpdate)
        .each([](const SceneManager &sm) {
            sm.scenes[sm.current]->update();
        });

    // update render components - ran automatically each PreStore phase
    ecs->system<SceneManager>()
        .kind(flecs::PreStore)
        .each([this](const SceneManager &sm) {
            SDL_SetRenderDrawColor(_ren, 33, 33, 33, 255);
            SDL_RenderClear(_ren);
            sm.scenes[sm.current]->renderUpdate();
        });

    // draw all RenderData components - ran manually
    _renderDraw = ecs->system<RenderData, Active>()
        .each([this](const RenderData &data, const Active &status) {
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
            SDL_RenderPresent(_ren);
        });

    _ren = ren;
    _sceneManager = ecs->entity()
        .set<SceneManager>({0, scenes});
    _activateCurrent.run();
}

void Game::setScene(int index) const {
    _deactivateCurrent.run();
    SceneManager *sm = _sceneManager.get_mut<SceneManager>();
    if (index >= 0 && index < sm->scenes.size()) {
        sm->scenes[sm->current]->reset();
        sm->current = index;
    }
    _activateCurrent.run();
}