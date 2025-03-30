#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_mouse.h>

#include <QuadTree.h>


#define HuyN_ int main(int argc, char *argv[])

using std::cout, std::endl, std::cerr, std::string, std::trunc, std::ceil, std::round, std::abs, std::vector, std::sqrt;

class SDLException final : public std::runtime_error {
public:
    explicit SDLException(const std::string& message) : std::runtime_error(message + "\n" + SDL_GetError()) {
        cerr << "SDL Error: " << message << endl;
    }
};


constexpr uint64_t UpdateInterval = 10, mouseActionInterval = 50;
uint64_t LatestUpdatedTick = 0, currentTick = 0, LatestMouseActionUpdatedTick = 0;

bool is_holding_left_click = false;
bool is_holding_right_click = false;

Vector2<double> MousePos{0, 0};

vector<Vector2<double>> onScreenPoints{};

Vector2<double> WindowSize{1280, 720};


void drawTree(SDL_Renderer *renderer, const QuadTree::QuadTree<double>& T) {
    if (T.divided) {
        for (auto & c : T.child) drawTree(renderer, *c);
    }
    const QuadTree::Box<double> bound = T.boundary;
    const SDL_Rect rect{static_cast<int>(bound.left), static_cast<int>(bound.top), static_cast<int>(bound.width), static_cast<int>(bound.height)};
    SDL_RenderDrawRect(renderer, &rect);
}

void drawer(SDL_Renderer *renderer, const QuadTree::QuadTree<double>& root) {
    drawTree(renderer, root);
    for (const auto &p : onScreenPoints) {
        SDL_RenderDrawPoint(renderer, p.x, p.y);
    }
}


void HoldingLeftClick(QuadTree::QuadTree<double>& root){
    Vector2<int> tempPos;
    SDL_GetMouseState(&tempPos.x, &tempPos.y);
    MousePos = Vector2<double>(tempPos.x, tempPos.y);
    onScreenPoints.push_back(MousePos);
    if (!root.insert(MousePos)) {
        cerr << "Failed to insert point " << MousePos.x << ' ' << MousePos.y << endl;
        onScreenPoints.erase(onScreenPoints.begin() + onScreenPoints.size() - 1);
    }
    LatestMouseActionUpdatedTick = SDL_GetTicks();
}

void GetBoundary(QuadTree::QuadTree<double> &root){
    Vector2<int> tempPos;
    SDL_GetMouseState(&tempPos.x, &tempPos.y);
    MousePos = Vector2<double>(tempPos.x, tempPos.y);
    const QuadTree::QuadTree<double> *c = root.getChild(MousePos);
    const QuadTree::Box<double> bound = c->boundary;
    cout << bound.top << ", " << bound.left << ", " << bound.width << ", " << bound.height << ", " << endl;
    cout << c->points.size() << endl;
    LatestMouseActionUpdatedTick = SDL_GetTicks();
}

void MouseActionProcess(QuadTree::QuadTree<double>& root) {
    currentTick = SDL_GetTicks();

    Vector2<int> tempPos;
    SDL_GetMouseState(&tempPos.x, &tempPos.y);
    auto mousePos = Vector2<double>(tempPos.x, tempPos.y);

    if (is_holding_left_click && currentTick - LatestMouseActionUpdatedTick >= mouseActionInterval && mousePos != MousePos) HoldingLeftClick(root);
    if (is_holding_right_click && currentTick - LatestMouseActionUpdatedTick >= mouseActionInterval && mousePos != MousePos) GetBoundary(root);
}

void loop(SDL_Renderer *renderer) {
    const QuadTree::Box<double> rootBoundary{10, 10, WindowSize.y - 20, WindowSize.y - 20};
    QuadTree::QuadTree<double> root{rootBoundary, 4};

    for (int i = 0; i < onScreenPoints.size(); i++) if (!root.insert(onScreenPoints[i])) {
        // throw SDLException("Failed to insert point");
        cerr << "Failed to insert point " << onScreenPoints[i].x << ", " << onScreenPoints[i].y << endl;
        onScreenPoints.erase(onScreenPoints.begin() + i);
    }

    MouseActionProcess(root);

    drawer(renderer, root);

}


HuyN_ {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) throw SDLException("SDL could not initialize");

    SDL_Window *window{SDL_CreateWindow("QuadTree", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowSize.x, WindowSize.y, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE)};
    if (!window) throw SDLException("SDL could not create window");

    SDL_Renderer *renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if (!renderer) throw SDLException("SDL could not create renderer");

    SDL_Event event;
    bool isRunning = true;

    SDL_ShowWindow(window);

    while (isRunning) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        is_holding_left_click = true;
                        loop(renderer);
                    } else {
                        is_holding_right_click = true;
                        loop(renderer);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (is_holding_left_click && event.button.button == SDL_BUTTON_LEFT) {
                        is_holding_left_click = false;
                    } else if (is_holding_right_click && event.button.button == SDL_BUTTON_RIGHT) {
                        is_holding_right_click = false;
                    }
                    break;
                default:
                    break;
            }
        }
        currentTick = SDL_GetTicks();
        if (currentTick - LatestUpdatedTick >= UpdateInterval) {
            loop(renderer);
            LatestUpdatedTick = currentTick;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}