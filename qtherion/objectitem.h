#ifndef OBJECTITEM_H
#define OBJECTITEM_H

#include <QString>
#include <QList>
#include <variant>
#include <memory>
#include "linepointmodel.h"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct Point {
    QString className;
    QString typeName;
    double x{};
    double y{};
};

struct Line {
    QString className;
    QString typeName;
    std::unique_ptr<LinePointModel> points;
};

using ObjectItem = std::variant<Point, Line>;

#endif // OBJECTITEM_H
