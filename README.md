# cpp-spreadsheet

Минималистичная реализация Excel-подобной электронной таблицы на C++, с использованием ANTLR для парсинга и построения дерева разбора.  
Проект поддерживает формулы, ссылки между ячейками и граф зависимостей.

## Особенности

- Поддержка формул вида `=A1 + 2`, `=1+2)` и текстовых ячеек.  
- Хранение и вычисление значений ячеек с учётом зависимостей.  
- Обнаружение циклических ссылок и обработка ошибок ( `#REF!`).  
- Архитектура с разделением на модули: ячейки, формулы, парсер, таблица.

## Пример использования

```cpp
Sheet sheet;
sheet.SetCell("A1", "10");
sheet.SetCell("A2", "=A1 + 5");
std::cout << sheet.GetCell("A2")->GetValue() << std::endl; // Выведет 15
