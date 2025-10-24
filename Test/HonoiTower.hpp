#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>
#include "BinaryTree.hpp"

struct HonoiTower {
    size_t countDisks;
    size_t countPoles;
    using PoleStack = std::vector<size_t>;
    std::vector<PoleStack> poles;

    HonoiTower(size_t countDisks, size_t countPoles) : countDisks{ countDisks }, countPoles{ countPoles }, poles(countPoles, PoleStack{}) {
        for (size_t i{ countDisks }; i > 0; --i) {
            poles[0].push_back(i);
        }
    }

    static size_t GetPoleIndex(char pole) {
        size_t index;
        switch (pole) {
            case 'A':
            case 'a':
                index = 0;
                break;
            case 'B':
            case 'b':
                index = 1;
                break;
            case 'C':
            case 'c':
                index = 2;
                break;
            default:
                throw std::invalid_argument{ "Invalid `pole`!" };
        }
        return index;
    }
    PoleStack& GetPole(char pole) {
        return GetPole(GetPoleIndex(pole));
    }
    const PoleStack& GetPole(char pole) const {
        return GetPole(GetPoleIndex(pole));
    }
    PoleStack& GetPole(size_t pole) {
        return poles.at(pole);
    }
    const PoleStack& GetPole(size_t pole) const {
        return poles.at(pole);
    }
    std::string PrintPole(size_t poleIndex) const {
        std::stringstream ss;
        const PoleStack& stack{ poles.at(poleIndex) };
        bool first{ true };
        for (auto bitr{ std::cbegin(stack) }; bitr != std::cend(stack); ++bitr) {
            const auto plate{ *bitr };
            if (!first) {
                ss << ", ";
            }
            ss << plate;
            first = false;
        }
        return ss.str();
    }
    std::string PrintAll() const {
        std::stringstream ss;
        for (size_t i{ 0 }; i < countPoles; ++i) {
            ss << '#' << i << '\t' << PrintPole(i) << '\n';
        }
        return ss.str();
    }
    void HonoiMove(size_t fromPole, size_t toPole) {
        std::cout << "Move from #" << fromPole << " to #" << toPole << ".\n";
        PoleStack& fromPoleStack{ GetPole(fromPole) };
        PoleStack& toPoleStack{ GetPole(toPole) };
        if (fromPoleStack.empty()) {
            throw std::runtime_error{ "There is no plate on 'from' pole" };
        }
        const auto plate{ fromPoleStack.back() };
        fromPoleStack.pop_back();
        toPoleStack.push_back(plate);
        std::cout << "Current State:\n" << PrintAll() << '\n';
    }
    void Honoi(size_t countDisks, size_t poleStart, size_t poleTransit, size_t poleDestination) {
        if (countDisks == 1) {
            HonoiMove(poleStart, poleDestination);
        }
        else {
            Honoi(countDisks - 1, poleStart, poleDestination, poleTransit);
            HonoiMove(poleStart, poleDestination);
            Honoi(countDisks - 1, poleTransit, poleStart, poleDestination);
        }
    }
    void Honoi() {
        std::cout << "Current State:\n" << PrintAll() << std::endl;
        Honoi(countDisks, 0, 1, 2);
    }
};
