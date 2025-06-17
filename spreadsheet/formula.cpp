#include "formula.h"

#include "FormulaAST.h"
#include "common.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        : ast_(ParseFormulaAST(expression))
    {
    }
    Value Evaluate(const SheetInterface& sheet) const override {
        //using value=std::variant<double, FormulaError>;
        double result= 0;
        try {
           result = ast_.Execute(sheet);
        }
        catch (const FormulaError& temp) {
            return temp;
        }
        return result;

    }
    std::string GetExpression() const override {
      // std::istringstream out;
       
        std::ostringstream out;
        ast_.PrintFormula(out);
        std::string temp=out.str();
        return temp;
    }
    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> referenced_cells(ast_.GetCells().begin(), ast_.GetCells().end());
        auto it=std::unique(referenced_cells.begin(), referenced_cells.end());
        referenced_cells.erase(it, referenced_cells.end());
        return referenced_cells;
    }
private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    std::unique_ptr<FormulaInterface> result;
    try {
        result = std::make_unique<Formula>(std::move(expression));
    }
    catch(...){
        throw FormulaException("invalid formula position");
    }
    return result;
}