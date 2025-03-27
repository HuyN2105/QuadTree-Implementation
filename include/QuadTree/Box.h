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
        T top;
        T left;
        T width;    // must be positive
        T height;   // must be positive

        constexpr Box(T left, T top, T width, T height) noexcept : top(top), left(left), width(width), height(height) {}

        explicit constexpr Box(Vector2<T>& position = {0, 0}, Vector2<T>& size = {0, 0}) noexcept : top(position.y), left(position.x), width(size.x), height(size.y) {}

        [[nodiscard]] constexpr T getRight() const noexcept { return left + width; }

        [[nodiscard]] constexpr T getBottom() const noexcept { return top + height; }

        [[nodiscard]] constexpr Vector2<T>& getTopLeft() const noexcept { return Vector2<T>(top, left); }

        [[nodiscard]] constexpr Vector2<T>& getCenter() const noexcept { return Vector2<T>(left + width / 2, top + height / 2); }

        [[nodiscard]] constexpr bool contains(Vector2<T>& position) const noexcept {
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

        [[nodiscard]] constexpr Box<T> subdivide(const std::string &quadrant) const noexcept {
            T halfWidth = this->width / 2;
            T halfHeight = this->height / 2;

            T extraWidth = this->width%2;
            T extraHeight = this->height%2;

            switch (quadrant) {
                case "nw":
                    return Box<T>(left, top, halfWidth + extraWidth, halfHeight + extraHeight);
                    break;
                case "ne":
                    return Box<T>(left + halfWidth, top, halfWidth + extraWidth, halfHeight + extraHeight);
                    break;
                case "sw":
                    return Box<T>(left, top + halfHeight, halfWidth + extraWidth, halfHeight + extraHeight);
                    break;
                case "se":
                    return Box<T>(left + halfWidth, top, halfWidth + extraWidth, halfHeight + extraHeight);
                    break;
                default:
                    return Box<T>(0, 0, 0, 0);
                    break;
            }
        }

    };

}

#endif //BOX_H
