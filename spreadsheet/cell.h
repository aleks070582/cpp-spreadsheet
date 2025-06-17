#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;
    void SetReferenced();
    void DeleteReferenced();
    bool IsReferenced() const;
protected:
    const Sheet& GetSheet() const{
        return sheet_;
    }

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
  

    std::unique_ptr<Impl> impl_;
    Sheet& sheet_;//неуверен насчет ссылки, может лучше указатель?
    bool is_referenced;
};