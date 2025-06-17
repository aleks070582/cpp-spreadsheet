#include "sheet.h"

#include "cell.h"
#include "common.h"
#include<deque>
#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include<cassert>
using namespace std::literals;



Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException(" ");
    }
    std::optional<std::unique_ptr<Cell>> temp=std::nullopt;
    if (table_.find(pos) != table_.end()) {
        temp.emplace(std::move(table_[pos]));
    }
    table_[pos] = std::make_unique<Cell>(*this);
    table_[pos].get()->Set(text);
    for (const Position& referenced_cells : table_[pos].get()->GetReferencedCells()) {
        if (table_.find(referenced_cells) != table_.end()) {
            table_[referenced_cells].get()->SetReferenced();
        }
        else {
            table_[referenced_cells] = std::make_unique<Cell>(*this);
            table_[referenced_cells].get()->Set("");
        }
        graph_[referenced_cells].push_back(pos);
    }
    if (IsCircleReference(pos)) {
        if (temp.has_value()) {
            table_[pos] = std::move(temp.value());
        }
        throw CircularDependencyException("fff");
    }

}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("position");
    }
    if (!pos.IsValid()) {
        throw InvalidPositionException("position");
    }
    if (table_.find(pos) == table_.end()) {
        return nullptr;
    }
    return table_.at(pos).get();
}

CellInterface* Sheet::GetCell(Position pos) {
   
    return const_cast<CellInterface*>(static_cast<const Sheet*>(this)->GetCell(pos));
}

void Sheet::ClearCell(Position pos) {

    if (!pos.IsValid()) {
        throw InvalidPositionException("position");
    }
    if (table_.find(pos) == table_.end()) {
        return;
    }
    table_.erase(pos);
}


Size Sheet::GetPrintableSize() const {
    Size result;
    if (table_.empty()) {
        return{ 0,0 };
    }
    for (auto& [pos, pu_cell] : table_) {
        if (pu_cell.get()->GetText() == "") {
            continue;
        }
        result.cols = pos.col > result.cols ? pos.col : result.cols;
        result.rows = pos.row > result.rows ? pos.row : result.rows;
    }
    result.cols = result.cols + 1;
    result.rows = result.rows + 1;
    return result;
}

void Sheet::PrintValues(std::ostream& output) const {
    Size limit = GetPrintableSize();
    Position pos;
    for (int i = 0; i < limit.rows; ++i) {
        bool first = true;
        for (int j = 0; j < limit.cols; ++j) {
            if (first != true) {
                output << '\t';
            }
            pos = { i,j };
            if (table_.find(pos) != table_.end()) {
                CellInterface::Value value = table_.at(pos).get()->GetValue();
                if (std::holds_alternative<std::string>(value)) {
                    output << std::get<std::string>(value);
                }
                else if (std::holds_alternative<double>(value)) {
                    output << std::get<double>(value);
                }
                else {
                    output << std::get<FormulaError>(value);
                }
            }
            else {
                output << "";
            }
            first = false;
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    Size limit = GetPrintableSize();
    Position pos;
    for (int i = 0; i < limit.rows; ++i) {

        bool first = true;
        for (int j = 0; j < limit.cols; ++j) {
            if (first != true) {
                output << '\t';
            }
            pos = { i,j };
            if (table_.find(pos) != table_.end()) {
                output << table_.at(pos).get()->GetText();
            }
            else {
                output << "";
            }
            first = false;
        }
        output << '\n';
    }
}


bool Sheet::IsCircleReference(Position pos){
    std::deque<Position> stack_;
    stack_.push_back(pos);
    Position current_pos;
    while (!stack_.empty()) {
        current_pos = stack_.back();
        stack_.pop_back();
        for (const Position& referenced_cell : GetCell(current_pos)->GetReferencedCells()) {
            if (referenced_cell == pos) {
                return true;
            }
            stack_.push_back(referenced_cell);
        }
    }
    return false;
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}