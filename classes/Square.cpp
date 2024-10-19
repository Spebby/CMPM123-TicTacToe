#include "Square.h"
#include "../tools/Logger.h"

void Square::initHolder(const ImVec2 &position, const char *spriteName, const int column, const int row) {
    _column = column;
    _row = row;
    int odd = (column + row) % 2;
    ImVec4 color = ImVec4(1,1,1,1);
    if (odd == 0) {
        color = ImVec4(0.5,0.5,0.75,1);
    }

    Logger::getInstance().log("Square created at: (" + std::to_string(column) + ", " + std::to_string(row) + ") -- ("
	 + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");

    BitHolder::initHolder(position, color, spriteName);
}