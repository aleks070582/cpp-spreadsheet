#include "cell.h"
#include"sheet.h"
#include <cassert>
#include <iostream>
#include <string>
#include <optional>



class Cell::Impl {
public:
    virtual CellInterface::Value GetValue() const = 0;
    virtual  std::vector<Position> GetReferencedCells() const = 0;
    virtual std::string GetText() const = 0;
    virtual ~Impl() = default;
};

std::optional<double> IsDouble(const std::string& value) {
    try {
        size_t pos;
        double num = std::stod(value, &pos);
        if (pos == value.size()) {
            return num;
        }
        else {
            return std::nullopt; 
        }
    }
    catch (...) {
        return std::nullopt; 
    }
}


class Cell::TextImpl :public Cell::Impl {
    std::string value_;
public:
    TextImpl(std::string str) :value_(str) {}

    CellInterface::Value GetValue() const override {
        std::string result = value_;
        if (value_[0] == '\'') {
            result.erase(0, 1);
        }
        if (value_.empty()) {
            return 0.0;
        }
        else if (std::optional<double> number = IsDouble(value_); number.has_value()) {
            return number.value();
        }
        return result;
    }
    std::string GetText() const override {
        return value_;
    }
    std::vector<Position> GetReferencedCells() const override {
        return std::vector<Position>();
    }

};

class Cell::FormulaImpl :public Cell::Impl {
    std::unique_ptr<FormulaInterface> formula_;
    Sheet& sheet_;
public:
    //не уверен что хороший выход кидать исключение в конструкторе
    FormulaImpl(std::string text,Sheet& sheet) :formula_(ParseFormula(text)),sheet_(sheet) {
        // копирование вектора???
        auto ref_cells = formula_->GetReferencedCells();
        for (auto& ch : ref_cells) {
            if (!ch.IsValid()) {
                throw FormulaException("invalid formula position");
            }
        }
    }
    CellInterface::Value GetValue() const override {
        auto result = formula_->Evaluate(sheet_);
        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        }
        else {
            return std::get<FormulaError>(result);
        }
    }
    std::string GetText() const override {
        std::string result = "=" + formula_->GetExpression();
        return result;
    }
    std::vector<Position> GetReferencedCells() const override {
        return formula_->GetReferencedCells();
    }
};

class Cell::EmptyImpl : public Cell::Impl {
public:
    CellInterface::Value GetValue() const override {
        return 0.0; 
    }

    std::string GetText() const override {
        return std::string{};
    }

    std::vector<Position> GetReferencedCells() const override {
        return std::vector<Position>();
    }
};

Cell::Cell(Sheet& sheet):impl_(std::make_unique<Cell::EmptyImpl>()),sheet_(sheet){}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<Cell::EmptyImpl>();
    }
    else if (text[0] == '\'') {
        // экранированный текст
        impl_ = std::make_unique<Cell::TextImpl>(text);
    }
    else if (text[0] == '=' && text.size() > 1) {
        // формула
        impl_ = std::make_unique<Cell::FormulaImpl>(text.substr(1),sheet_);
    }
    else {
        //если число
    //    if (std::optional<double> number = IsDouble(text); number.has_value()) {
    //        impl_ = std::make_unique<Cell::NumberImpl>(number.value());
    //    }
    //    else {
            // обычный текст
            impl_ = std::make_unique<Cell::TextImpl>(text);
      //  }
    }
}
void Cell::Clear() {
    impl_ = std::make_unique<Cell::EmptyImpl>(Cell::EmptyImpl());
}

std::vector<Position> Cell::GetReferencedCells() const
{
    return impl_->GetReferencedCells();
}

void Cell::SetReferenced()
{
    is_referenced = true;
}

void Cell::DeleteReferenced()
{
    is_referenced = false;
}

bool Cell::IsReferenced() const
{
    return is_referenced;
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}
std::string Cell::GetText() const {
    return impl_->GetText();
}

