#pragma once

#include "cell.h"
#include "common.h"
#include<map>
#include <functional>
#include<unordered_map>

struct PositionHasher {
    size_t operator()(const Position& pos) const {
        return  static_cast<size_t>(pos.col) * 16385 + pos.row;
    }
};


class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;
    bool IsReferencedCell();
	// Можете дополнить ваш класс нужными полями и методами

private:
	// Можете дополнить ваш класс нужными полями и методами
    bool IsCircleReference(Position pos);
    mutable   std::unordered_map<Position, std::unique_ptr<Cell>, PositionHasher> table_;
    std::unordered_map<Position, std::vector<Position>,PositionHasher> graph_;
    
};