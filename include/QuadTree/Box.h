//
// Created by HuyN on 3/26/2025.
//
#pragma once

#include "Vector2.h"

#ifndef BOX_H
#define BOX_H

using HuyNVector::Vector2;

namespace quadTree {

    template<typename T>
    class Box {
    public:
        T top;
        T left;
        T width;    // must be positive
        T height;   // must be positive

        constexpr Box(T top, T left, T width, T height) noexcept : top(top), left(left), width(width), height(height) {}

        explicit constexpr Box(Vector2<T>& position = {0, 0}, Vector2<T>& size = {0, 0}) noexcept : top(position.y), left(position.x), width(size.x), height(size.y) {}

        [[nodiscard]] constexpr T getRight() const noexcept { return left + width; }

        [[nodiscard]] constexpr T getBottom() const noexcept { return top + height; }

        [[nodiscard]] constexpr Vector2<T>& getTopLeft() const noexcept { return Vector2<T>(top, left); }

        [[nodiscard]] constexpr Vector2<T>& getCenter() const noexcept { return Vector2<T>(left + width / 2, top + height / 2); }

        [[nodiscard]] constexpr bool containPoint(Vector2<T>& position) const noexcept {
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

    };

}

#endif //BOX_H
