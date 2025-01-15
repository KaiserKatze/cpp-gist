#pragma once
#include <iostream>
#include "BinaryTree.hpp"

void HonoiMove(char fromPole, char toPole) {
    std::cout << "Move from `" << fromPole << "` to `" << toPole << "`.\n";
}
void HonoiState(char poleStart, char poleTransit, char poleDestination) {
    std::cout << "{ start: " << poleStart << ", transit: " << poleTransit << ", destination: " << poleDestination << " }.\n";
}

void Honoi(int countDisk, char poleStart, char poleTransit, char poleDestination) {
    HonoiState(poleStart, poleDestination, poleTransit);
    if (countDisk == 1) {
        HonoiMove(poleStart, poleDestination);
    }
    else {
        Honoi(countDisk - 1, poleStart, poleDestination, poleTransit);
        HonoiMove(poleStart, poleDestination);
        Honoi(countDisk - 1, poleTransit, poleStart, poleDestination);
    }
}

