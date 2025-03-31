//
// Created by HuyN on 3/26/2025.
//
#pragma once

#include "Vector2.h"
#include "string"

#ifndef BOX_H
#define BOX_H

using HuyNVector::Vector2;

namespace QuadTree {

    template<typename T>
    class Box {
    public:

        // ****************************** BOX INITIALIZATION ****************************** //

        T top;
        T left;
        T width;    // must be positive
        T height;   // must be positive

        constexpr Box(T left, T top, T width, T height) noexcept : top(top), left(left), width(width), height(height) {}

        explicit constexpr Box(Vector2<T>& position = {0, 0}, Vector2<T>& size = {0, 0}) noexcept : top(position.y), left(position.x), width(size.x), height(size.y) {}

        // ******************************** BOX FUNCTIONS ******************************** //

        [[nodiscard]] constexpr T getRight() const noexcept { return left + width; }

        [[nodiscard]] constexpr T getBottom() const noexcept { return top + height; }

        [[nodiscard]] constexpr Vector2<T>& getTopLeft() const noexcept { return Vector2<T>(top, left); }

        [[nodiscard]] constexpr Vector2<T>& getCenter() const noexcept { return Vector2<T>(left + width / 2, top + height / 2); }

        [[nodiscard]] constexpr bool contains(Vector2<T> position) const noexcept {
            return (left <= position.x &&
                    this->getRight() >= position.x &&
                    top <= position.y &&
                    this->getBottom() >= position.y);
        }

        [[nodiscard]] constexpr bool intersects(const Box& other) const noexcept {
            return !(left >= this->getRight() ||
                     this->getRight() <= left ||
                     top >= this->getBottom() ||
                     this->getBottom() <= top);
        }

        [[nodiscard]] constexpr Box subdivide(const std::string &quadrant) const noexcept {
            T halfWidth = this->width / 2;
            T halfHeight = this->height / 2;

            T extraWidth = static_cast<int>(this->width)%2;
            T extraHeight = static_cast<int>(this->height)%2;

            if (quadrant == "nw") return Box(left, top, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "ne") return Box(left + halfWidth, top, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "sw") return Box(left, top + halfHeight, halfWidth + extraWidth, halfHeight + extraHeight);
            if (quadrant == "se") return Box(left + halfWidth, top + halfHeight, halfWidth + extraWidth, halfHeight + extraHeight);

            return Box(0, 0, 0, 0);
        }

    };

}

#endif //BOX_H
