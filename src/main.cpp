#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_mouse.h>

#define HuyN_ int main(int argc, char *argv[])

using std::cout, std::endl, std::cerr, std::string, std::trunc, std::ceil, std::round, std::abs, std::vector, std::sqrt;

class SDLException final : public std::runtime_error {
public:
    explicit SDLException(const std::string& message) : std::runtime_error(message + "\n" + SDL_GetError()) {
        cerr << "SDL Error: " << message << endl;
    }
};


constexpr uint64_t UpdateInterval = 10;
uint64_t LatestUpdatedTick = 0;

struct size {
    int w;
    int h;
};

struct pos {
    int x;
    int y;
};

struct Point {
    int x;
    int y;

    void show(SDL_Renderer* renderer) const {
        SDL_RenderDrawPoint(renderer, x, y);
    }

};

bool is_holding_left_click = false;
bool is_holding_right_click = false;
Point MousePos{0, 0};

vector<Point> OnScreenPoints;

class Rectangle {
public:
    int w;
    int h;
    int x;
    int y;

    double left = x - w / 2;
    double top = y - h / 2;
    double right = x + w / 2;
    double bottom = y + h / 2;

    Rectangle(const int x,const int y,const int w,const int h): w(w),
                                                                h(h),
                                                                x(x),
                                                                y(y),
                                                                left(x - w / 2),
                                                                top(y - h / 2),
                                                                right(x + w / 2),
                                                                bottom(y + h / 2) {};

    [[nodiscard]] bool contain(const Point point) const {
        return (left <= point.x &&
            right >= point.x &&
            top <= point.y &&
            bottom >= point.y);
    }

    [[nodiscard]] Rectangle subDivide(const string &quadrant) const {
        int halfW = w / 2;
        int halfH = h / 2;

        int extraW = w % 2; // If w is odd, we add 1 to one of the children
        int extraH = h % 2; // If h is odd, we add 1 to one of the children

        if (quadrant == "nw") {
            return Rectangle{x - halfW / 2, y - halfH / 2, halfW + extraW, halfH + extraH};
        }
        if (quadrant == "ne") {
            return Rectangle{x + halfW / 2, y - halfH / 2, halfW, halfH + extraH};
        }
        if (quadrant == "sw") {
            return Rectangle{x - halfW / 2, y + halfH / 2, halfW + extraW, halfH};
        }
        if (quadrant == "se") {
            return Rectangle{x + halfW / 2, y + halfH / 2, halfW, halfH};
        }

        return Rectangle{0, 0, 0, 0}; // Fallback case
    }



    void show(SDL_Renderer *renderer) const {
        const SDL_Rect rect{static_cast<int>(floor(left)), static_cast<int>(floor(top)), w, h};
        SDL_RenderDrawRect(renderer, &rect);
    }
};

class QuadTree {
public:
    int capacity;
    Rectangle boundary;
    bool divided;

    vector<Point> Points{};

    QuadTree *child[4]{};

    explicit QuadTree(const Rectangle &_boundary, const int _capacity)
        : capacity(_capacity), boundary(_boundary), divided(false) {
        std::ranges::fill(child, nullptr);
    }

    [[nodiscard]] QuadTree** getChild() {
        if (divided) {
            return child;
        }
        return nullptr;
    }

    [[nodiscard]] vector<Point> getPoints() const {
        if (divided) {
            for (auto& b : child) {
                if (b->boundary.contain(MousePos)) return b->getPoints();
            }
        }
        return Points;
    }

    [[nodiscard]] Rectangle getBounding() const {
        if (divided) {
            for (auto& b : child) {
                if (b->boundary.contain(MousePos)) return b->getBounding();
            }
        }
        return boundary;
    }

    void subdivide() {
        child[0] = new QuadTree(boundary.subDivide("ne"), capacity);
        child[1] = new QuadTree(boundary.subDivide("nw"), capacity);
        child[2] = new QuadTree(boundary.subDivide("se"), capacity);
        child[3] = new QuadTree(boundary.subDivide("sw"), capacity);

        divided = true;

        // Move points to children
        for (const auto& p : Points) {
            const bool inserted = child[0]->insert(p) ||
                                  child[1]->insert(p) ||
                                  child[2]->insert(p) ||
                                  child[3]->insert(p);
            if (!inserted) {
                // throw SDLException("Capacity must be greater than 0");
                for (const auto& p : Points) {
                    cerr << p.x << ", " << p.y << endl;
                }
            }
        }
        Points.clear();
    }

    [[nodiscard]] bool insert(const Point point) {
        if (!boundary.contain(point)) return false;
        if (!divided) {
            if (Points.size() < capacity) {
                Points.push_back(point);
                return true;
            }
            this->subdivide();
        }
        return (
            child[0]->insert(point) ||
            child[1]->insert(point) ||
            child[2]->insert(point) ||
            child[3]->insert(point)
        );
    }

    void show(SDL_Renderer *renderer) const {
        boundary.show(renderer);
        for (auto &c : child) {
            if (c != nullptr) {
                c->show(renderer);
            }
        }
        for (const auto &p : Points) {
            p.show(renderer);
        }
    }
};

size WindowSize = {1280, 720};


void HoldingRightClick(QuadTree root){
    SDL_GetMouseState(&MousePos.x, &MousePos.y);
    OnScreenPoints.push_back({MousePos.x, MousePos.y});
    if (!root.insert(MousePos)) {
        throw SDLException("Failed to insert point");
    }
}

void GetPointsInBoundary(const QuadTree &root){
    SDL_GetMouseState(&MousePos.x, &MousePos.y);
    cout << "RESULT:\n";
    const Rectangle bound = root.getBounding();
    cout << bound.x << ", " << bound.y << ", " << bound.w << ", " << bound.h << ", " << bound.left << ", " << bound.right << endl;
}

void loop(SDL_Renderer *renderer) {
    const Rectangle rootBoundary{ WindowSize.w - WindowSize.w/2, WindowSize.h - WindowSize.h/2, WindowSize.h - 20, WindowSize.h - 20};
    QuadTree root{rootBoundary, 4};
    for (auto &p : OnScreenPoints) if (!root.insert(p)) throw SDLException("Failed to insert point");
    if (is_holding_left_click) HoldingRightClick(root);
    if (is_holding_right_click) GetPointsInBoundary(root);
    root.show(renderer);
}


HuyN_ {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) throw SDLException("SDL could not initialize");

    SDL_Window *window{SDL_CreateWindow("QuadTree", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WindowSize.w, WindowSize.h, SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE)};
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
                    // break;
                case SDL_MOUSEBUTTONUP:
                    if (is_holding_left_click && event.button.button == SDL_BUTTON_LEFT) {
                        is_holding_left_click = false;
                    } else if (is_holding_right_click && event.button.button == SDL_BUTTON_RIGHT) {
                        is_holding_right_click = false;
                    }
                    // break;
                default:
                    break;
            }
        }

        if (const uint64_t CurrentTick = SDL_GetTicks(); CurrentTick - LatestUpdatedTick >= UpdateInterval) {
            loop(renderer);
            LatestUpdatedTick = CurrentTick;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}