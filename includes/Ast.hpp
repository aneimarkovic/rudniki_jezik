#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>

/*
    Ast.hpp
    Darko Sever 28.5.2025
    Spremembe za GeoJSON izpis implementirane po načrtu.
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct AstNode {
    int line;
    int col;

    AstNode(int l, int c) : line(l), col(c) {}
    virtual ~AstNode() = default;

    virtual void print(std::ostream& out, int indentLevel = 0) const {
        out << std::string(indentLevel * 2, ' ') << "AstNode (Generic)" << " @ (" << line << "," << col << ")" << std::endl;
    }

    virtual void printGeoJSON(std::ostream& out, int indent = 0) const {
        (void)out; (void)indent;
    }

    virtual double evaluate() const {
        return 0.0;
    }

protected:
    void printIndent(std::ostream& out, int indent) const {
        out << std::string(indent * 2, ' ');
    }
};

struct ExprNode;
struct IntLiteralNode;
struct HexLiteralNode;
struct VariableAccessNode;
struct FunctionCallNode;
struct ParenExpressionNode;
struct BinaryOpNode;
struct UnaryOpNode;
struct StatementNode;
struct ProgramNode;
struct StartNode;
struct FunctionNode;
struct AssignNode;
struct LoopNode;
struct PrintNode;
struct ExprStatementNode;
struct CommandNode;
struct CommandSetNode;
struct BeginPrimeBodyNode;
struct MinetypeNode;
struct LandElementsNode;
struct UndergroundElementsNode;
struct SeaElementsNode;
struct LandElementNode;
struct LandElementCurlyNode;
struct LandElementBracketNode;
struct PathVsebinaNode;
struct RiverVsebinaNode;
struct StripVsebinaNode;
struct BenchVsebinaNode;
struct SpoilpitVsebinaNode;
struct PlantVsebinaNode;
struct WellVsebinaNode;
struct UndergroundElementNode;
struct UndergroundElementCurlyNode;
struct UndergroundElementBracketNode;
struct ElevatorVsebinaNode;
struct OrebedVsebinaNode;
struct TunnelVsebinaNode;
struct ShaftVsebinaNode;
struct SeaElementCurlyNode;
struct PlatformVsebinaNode;
struct PumpVsebinaNode;
struct RiserVsebinaNode;
struct LinesNode;
struct LinePropertyNode;
struct LinesContentNode;
struct LinesItemContentAfterParenNode;
struct LinesBendOptNode;
struct ArbitraryPolygonNode;
struct PolylineNode;
struct LineNode;
struct BoxNode;
struct PointsNode;
struct PointNode;
struct BendNode;
struct CircNode;
struct AttributeNode;
struct ElementVsebinaNode;


enum class UnaryOperatorType { PLUS, MINUS, NONE };
enum class BinaryOperatorType { PLUS, MINUS, TIMES, DIVIDE, BW_AND, BW_OR };

struct ExprNode : public AstNode {
    ExprNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ExprNode (Base)" << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct IntLiteralNode : public ExprNode {
    long long value;
    IntLiteralNode(long long val, int l, int c) : ExprNode(l, c), value(val) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "IntLiteralNode: " << value << " @ (" << line << "," << col << ")" << std::endl;
    }
    double evaluate() const override {
        return static_cast<double>(value);
    }
};

struct HexLiteralNode : public ExprNode {
    std::string value;
    HexLiteralNode(std::string val, int l, int c) : ExprNode(l, c), value(std::move(val)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "HexLiteralNode: " << value << " @ (" << line << "," << col << ")" << std::endl;
    }
    double evaluate() const override {
        return static_cast<double>(std::stoll(value.substr(1), nullptr, 16));
    }
};

struct VariableAccessNode : public ExprNode {
    std::string name;
    VariableAccessNode(std::string varName, int l, int c) : ExprNode(l, c), name(std::move(varName)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "VariableAccessNode: " << name << " @ (" << line << "," << col << ")" << std::endl;
    }
    double evaluate() const override {
        return 0.0;
    }
};

struct ParenExpressionNode : public ExprNode {
    std::unique_ptr<ExprNode> expression;
    ParenExpressionNode(std::unique_ptr<ExprNode> expr, int l, int c) : ExprNode(l, c), expression(std::move(expr)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ParenExpressionNode @ (" << line << "," << col << ")" << std::endl;
        if (expression) expression->print(out, indentLevel + 1);
    }
    double evaluate() const override {
        return expression ? expression->evaluate() : 0.0;
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (expression) {
            expression->printGeoJSON(out, indent);
        }
    }
};

struct UnaryOpNode : public ExprNode {
    UnaryOperatorType op;
    std::unique_ptr<ExprNode> operand;
    UnaryOpNode(UnaryOperatorType oper, std::unique_ptr<ExprNode> prim, int l, int c)
        : ExprNode(l, c), op(oper), operand(std::move(prim)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "UnaryOpNode: ";
        switch (op) {
        case UnaryOperatorType::PLUS: out << "+"; break;
        case UnaryOperatorType::MINUS: out << "-"; break;
        case UnaryOperatorType::NONE: out << "(none)"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (operand) operand->print(out, indentLevel + 1);
    }
    double evaluate() const override {
        if (!operand) return 0.0;
        double val = operand->evaluate();
        switch (op) {
        case UnaryOperatorType::PLUS: return val;
        case UnaryOperatorType::MINUS: return -val;
        case UnaryOperatorType::NONE: return val;
        default: return 0.0;
        }
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (operand) {
            operand->printGeoJSON(out, indent);
        }
    }
};

struct BinaryOpNode : public ExprNode {
    std::unique_ptr<ExprNode> left;
    BinaryOperatorType op_type;
    std::unique_ptr<ExprNode> right;
    BinaryOpNode(std::unique_ptr<ExprNode> l_node, BinaryOperatorType oper, std::unique_ptr<ExprNode> r_node, int line_val, int col_val)
        : ExprNode(line_val, col_val), left(std::move(l_node)), op_type(oper), right(std::move(r_node)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BinaryOpNode: ";
        switch (op_type) {
        case BinaryOperatorType::PLUS: out << "+"; break;
        case BinaryOperatorType::MINUS: out << "-"; break;
        case BinaryOperatorType::TIMES: out << "*"; break;
        case BinaryOperatorType::DIVIDE: out << "/"; break;
        case BinaryOperatorType::BW_AND: out << "&"; break;
        case BinaryOperatorType::BW_OR: out << "|"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (left) left->print(out, indentLevel + 1);
        if (right) right->print(out, indentLevel + 1);
    }
    double evaluate() const override {
        if (!left || !right) return 0.0;
        double lval = left->evaluate();
        double rval = right->evaluate();
        switch (op_type) {
        case BinaryOperatorType::PLUS: return lval + rval;
        case BinaryOperatorType::MINUS: return lval - rval;
        case BinaryOperatorType::TIMES: return lval * rval;
        case BinaryOperatorType::DIVIDE: return rval != 0.0 ? lval / rval : 0.0;
        case BinaryOperatorType::BW_AND: return static_cast<double>(static_cast<long long>(lval) & static_cast<long long>(rval));
        case BinaryOperatorType::BW_OR: return static_cast<double>(static_cast<long long>(lval) | static_cast<long long>(rval));
        default: return 0.0;
        }
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (left) {
            left->printGeoJSON(out, indent);
        }
        if (right) {
            right->printGeoJSON(out, indent);
        }
    }
};

struct PointNode : public AstNode {
    std::unique_ptr<ExprNode> x;
    std::unique_ptr<ExprNode> y;
    PointNode(std::unique_ptr<ExprNode> _x, std::unique_ptr<ExprNode> _y, int l, int c)
        : AstNode(l, c), x(std::move(_x)), y(std::move(_y)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PointNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "X:" << std::endl;
        if (x) x->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Y:" << std::endl;
        if (y) y->print(out, indentLevel + 2);
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"Point\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [";
        double x_val = 0.0, y_val = 0.0;
        if (x) x_val = x->evaluate();
        if (y) y_val = y->evaluate();
        out << x_val << ", " << y_val << "]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct PointsNode : public AstNode {
    std::vector<std::unique_ptr<PointNode>> points_list;
    PointsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PointsNode @ (" << line << "," << col << ")" << std::endl;
        for (const auto& p : points_list) {
            if (p) p->print(out, indentLevel + 1);
        }
    }
};

struct LineNode : public AstNode {
    std::unique_ptr<PointNode> start_point;
    std::unique_ptr<PointNode> end_point;
    LineNode(std::unique_ptr<PointNode> s, std::unique_ptr<PointNode> e, int l, int c)
        : AstNode(l, c), start_point(std::move(s)), end_point(std::move(e)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LineNode @ (" << line << "," << col << ")" << std::endl;
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"LineString\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [\n";
        printIndent(out, indent + 2);
        out << "[";
        if (start_point && start_point->x && start_point->y) {
            out << start_point->x->evaluate() << ", " << start_point->y->evaluate();
        }
        else {
            out << "0, 0";
        }
        out << "],\n";
        printIndent(out, indent + 2);
        out << "[";
        if (end_point && end_point->x && end_point->y) {
            out << end_point->x->evaluate() << ", " << end_point->y->evaluate();
        }
        else {
            out << "0, 0";
        }
        out << "]\n";
        printIndent(out, indent + 1);
        out << "]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct PolylineNode : public AstNode {
    std::unique_ptr<PointsNode> points;
    PolylineNode(std::unique_ptr<PointsNode> pts, int l, int c) : AstNode(l, c), points(std::move(pts)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PolylineNode @ (" << line << "," << col << ")" << std::endl;
        if (points) points->print(out, indentLevel + 1);
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"LineString\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [\n";
        if (points) {
            bool first_coord = true;
            for (const auto& point_ptr : points->points_list) {
                if (point_ptr && point_ptr->x && point_ptr->y) {
                    if (!first_coord) {
                        out << ",\n";
                    }
                    printIndent(out, indent + 2);
                    out << "[" << point_ptr->x->evaluate() << ", " << point_ptr->y->evaluate() << "]";
                    first_coord = false;
                }
            }
            if (!first_coord) out << "\n";
        }
        printIndent(out, indent + 1);
        out << "]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct BendNode : public AstNode {
    std::unique_ptr<PointNode> start_point;
    std::unique_ptr<PointNode> control_point;
    std::unique_ptr<ExprNode> third_param;

    BendNode(std::unique_ptr<PointNode> p1, std::unique_ptr<PointNode> p2, std::unique_ptr<ExprNode> param, int l, int c)
        : AstNode(l, c), start_point(std::move(p1)), control_point(std::move(p2)), third_param(std::move(param)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BendNode @ (" << line << "," << col << ")" << std::endl;
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (!start_point || !start_point->x || !start_point->y ||
            !control_point || !control_point->x || !control_point->y) {
            return;
        }

        double p0x = start_point->x->evaluate();
        double p0y = start_point->y->evaluate();
        double p1x = control_point->x->evaluate();
        double p1y = control_point->y->evaluate();

        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"LineString\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [\n";
        printIndent(out, indent + 2);
        out << "[" << p0x << ", " << p0y << "],\n";
        printIndent(out, indent + 2); out << "[" << p1x << ", " << p1y << "]\n";
        printIndent(out, indent + 1);
        out << "]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct BoxNode : public AstNode {
    std::unique_ptr<PointNode> point1;
    std::unique_ptr<PointNode> point2;
    BoxNode(std::unique_ptr<PointNode> p1, std::unique_ptr<PointNode> p2, int l, int c)
        : AstNode(l, c), point1(std::move(p1)), point2(std::move(p2)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BoxNode @ (" << line << "," << col << ")" << std::endl;
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"Polygon\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [[\n";

        if (point1 && point1->x && point1->y && point2 && point2->x && point2->y) {
            double x1_eval = point1->x->evaluate();
            double y1_eval = point1->y->evaluate();
            double x2_eval = point2->x->evaluate();
            double y2_eval = point2->y->evaluate();

            double min_x = std::min(x1_eval, x2_eval);
            double min_y = std::min(y1_eval, y2_eval);
            double max_x = std::max(x1_eval, x2_eval);
            double max_y = std::max(y1_eval, y2_eval);

            printIndent(out, indent + 2); out << "[" << min_x << ", " << min_y << "],\n";
            printIndent(out, indent + 2); out << "[" << max_x << ", " << min_y << "],\n";
            printIndent(out, indent + 2); out << "[" << max_x << ", " << max_y << "],\n";
            printIndent(out, indent + 2); out << "[" << min_x << ", " << max_y << "],\n";
            printIndent(out, indent + 2); out << "[" << min_x << ", " << min_y << "]\n";
        }
        else {
            printIndent(out, indent + 2); out << "[0, 0],\n[0, 0],\n[0, 0],\n[0, 0],\n[0, 0]\n";
        }
        printIndent(out, indent + 1);
        out << "]]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct ArbitraryPolygonNode : public AstNode {
    std::unique_ptr<PointsNode> points;
    ArbitraryPolygonNode(std::unique_ptr<PointsNode> pts, int l, int c) : AstNode(l, c), points(std::move(pts)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ArbitraryPolygonNode @ (" << line << "," << col << ")" << std::endl;
        if (points) points->print(out, indentLevel + 1);
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"Polygon\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [[\n";
        if (points && !points->points_list.empty()) {
            bool first_coord = true;
            for (const auto& point_ptr : points->points_list) {
                if (point_ptr && point_ptr->x && point_ptr->y) {
                    if (!first_coord) {
                        out << ",\n";
                    }
                    printIndent(out, indent + 2);
                    out << "[" << point_ptr->x->evaluate() << ", " << point_ptr->y->evaluate() << "]";
                    first_coord = false;
                }
            }
            const auto& first_point_ptr = points->points_list.front();
            if (!first_coord && first_point_ptr && first_point_ptr->x && first_point_ptr->y) {
                out << ",\n";
                printIndent(out, indent + 2);
                out << "[" << first_point_ptr->x->evaluate() << ", " << first_point_ptr->y->evaluate() << "]";
            }
            if (!first_coord) out << "\n";
        }
        printIndent(out, indent + 1);
        out << "]]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct CircNode : public AstNode {
    std::unique_ptr<PointNode> center;
    std::unique_ptr<ExprNode> radius;
    CircNode(std::unique_ptr<PointNode> p_center, std::unique_ptr<ExprNode> p_radius, int p_line, int p_col)
        : AstNode(p_line, p_col), center(std::move(p_center)), radius(std::move(p_radius)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "CircNode @ (" << line << "," << col << ")" << std::endl;
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"Polygon\",\n";
        printIndent(out, indent + 1);
        out << "\"coordinates\": [[\n";

        if (center && center->x && center->y && radius) {
            double cx = center->x->evaluate();
            double cy = center->y->evaluate();
            double r_val = radius->evaluate();
            const int SEGMENTS = 36;
            const double PI_CONST = 3.14159265358979323846;

            for (int i = 0; i <= SEGMENTS; ++i) {
                double angle = 2 * PI_CONST * static_cast<double>(i) / SEGMENTS;
                double x_coord = cx + r_val * std::cos(angle);
                double y_coord = cy + r_val * std::sin(angle);
                printIndent(out, indent + 2);
                out << "[" << x_coord << ", " << y_coord << "]";
                if (i < SEGMENTS) {
                    out << ",";
                }
                out << "\n";
            }
        }
        else {
            printIndent(out, indent + 2); out << "[0,0],[0,0],[0,0],[0,0]\n";
        }
        printIndent(out, indent + 1);
        out << "]]\n";
        printIndent(out, indent);
        out << "}";
    }
};

struct LinesBendOptNode : public AstNode {
    std::unique_ptr<ExprNode> value;
    LinesBendOptNode(std::unique_ptr<ExprNode> val, int l, int c) : AstNode(l, c), value(std::move(val)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinesBendOptNode @ (" << line << "," << col << ")" << std::endl;
        if (value) value->print(out, indentLevel + 1);
        else out << std::string((indentLevel + 1) * 2, ' ') << "(Epsilon - no value)" << std::endl;
    }
};

struct LinesItemContentAfterParenNode : public AstNode {
    std::variant<
        std::unique_ptr<PointsNode>,
        std::tuple<std::unique_ptr<PointNode>, std::unique_ptr<PointNode>, std::unique_ptr<LinesBendOptNode>>
    > content;
    LinesItemContentAfterParenNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<PointsNode>>) {
                if (arg) arg->printGeoJSON(out, indent);
            }
            else if constexpr (std::is_same_v<T, std::tuple<std::unique_ptr<PointNode>, std::unique_ptr<PointNode>, std::unique_ptr<LinesBendOptNode>>>) {
                if (std::get<0>(arg)) std::get<0>(arg)->printGeoJSON(out, indent);
                if (std::get<1>(arg)) std::get<1>(arg)->printGeoJSON(out, indent);
                if (std::get<2>(arg)) std::get<2>(arg)->printGeoJSON(out, indent);
            }
            }, content);
    }
};


struct LinesContentNode : public AstNode {
    std::unique_ptr<LinesItemContentAfterParenNode> item_content;
    LinesContentNode(std::unique_ptr<LinesItemContentAfterParenNode> ic, int l, int c)
        : AstNode(l, c), item_content(std::move(ic)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (item_content) {
            item_content->printGeoJSON(out, indent);
        }
    }
};

struct LinePropertyNode : public AstNode {
    std::string name;
    std::unique_ptr<LinesContentNode> content;
    LinePropertyNode(std::string p_name, std::unique_ptr<LinesContentNode> p_content, int p_line, int p_col)
        : AstNode(p_line, p_col), name(std::move(p_name)), content(std::move(p_content)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override {
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (content) {
            content->printGeoJSON(out, indent);
        }
    }
};

struct LinesNode : public AstNode {
    std::vector<std::unique_ptr<LinePropertyNode>> properties;
    LinesNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
    }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        for (const auto& prop : properties) {
            if (prop) {
                prop->printGeoJSON(out, indent);
            }
        }
    }
};

struct ElementVsebinaNode : public AstNode {
    ElementVsebinaNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ElementVsebinaNode (Base)" << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct PathVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<LineNode>, std::unique_ptr<PolylineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    PathVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct RiverVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<LineNode>, std::unique_ptr<PolylineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    RiverVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct StripVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>> shape;
    StripVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct BenchVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<ArbitraryPolygonNode> polygon;
    BenchVsebinaNode(std::unique_ptr<ArbitraryPolygonNode> poly, int l, int c)
        : ElementVsebinaNode(l, c), polygon(std::move(poly)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (polygon) {
            polygon->printGeoJSON(out, indent);
        }
    }
};

struct SpoilpitVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    SpoilpitVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct PlantVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    PlantVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

enum class WellType { MONITORING, INJECTION, RECOVERY };
struct WellVsebinaNode : public ElementVsebinaNode {
    WellType type;
    std::unique_ptr<PointNode> point;
    WellVsebinaNode(WellType t, std::unique_ptr<PointNode> p, int l, int c)
        : ElementVsebinaNode(l, c), type(t), point(std::move(p)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (point) {
            point->printGeoJSON(out, indent);
        }
    }
};

enum class ElevatorType { HUMAN, ORE };
struct ElevatorVsebinaNode : public ElementVsebinaNode {
    std::variant<ElevatorType, std::unique_ptr<PointNode>> content;
    ElevatorVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<PointNode>>) {
                if (c) c->printGeoJSON(out, indent);
            }
            }, content);
    }
};

struct OrebedVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    OrebedVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct TunnelVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<PolylineNode>, std::unique_ptr<LineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    TunnelVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct ShaftVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<PolylineNode>, std::unique_ptr<LineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    ShaftVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct PlatformVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    PlatformVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& s) {
            if (s) s->printGeoJSON(out, indent);
            }, shape);
    }
};

struct PumpVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<PointNode> point;
    PumpVsebinaNode(std::unique_ptr<PointNode> p, int l, int c) : ElementVsebinaNode(l, c), point(std::move(p)) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (point) {
            point->printGeoJSON(out, indent);
        }
    }
};

struct RiserVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<PointNode> point;
    RiserVsebinaNode(std::unique_ptr<PointNode> p, int l, int c) : ElementVsebinaNode(l, c), point(std::move(p)) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (point) {
            point->printGeoJSON(out, indent);
        }
    }
};

enum class LandElementCurlyType { PATH, RIVER };
struct LandElementCurlyNode : public AstNode {
    LandElementCurlyType type;
    std::unique_ptr<PathVsebinaNode> path_vsebina;
    std::unique_ptr<RiverVsebinaNode> river_vsebina;
    LandElementCurlyNode(LandElementCurlyType t, int l, int c) : AstNode(l, c), type(t) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (type == LandElementCurlyType::PATH && path_vsebina) {
            path_vsebina->printGeoJSON(out, indent);
        }
        else if (type == LandElementCurlyType::RIVER && river_vsebina) {
            river_vsebina->printGeoJSON(out, indent);
        }
    }
};

enum class LandElementBracketType { STRIP, BENCH, SPOILPIT, PLANT, WELL };
struct LandElementBracketNode : public AstNode {
    LandElementBracketType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    LandElementBracketNode(LandElementBracketType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l, c), type(t), vsebina(std::move(v)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (vsebina) {
            vsebina->printGeoJSON(out, indent);
        }
    }
};

struct LandElementNode : public AstNode {
    std::variant<std::unique_ptr<LandElementCurlyNode>, std::unique_ptr<LandElementBracketNode>> element_choice;
    LandElementNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& choice) {
            if (choice) choice->printGeoJSON(out, indent);
            }, element_choice);
    }
};

template<typename T>
void printGeoJSONForElementList(const std::vector<std::unique_ptr<T>>& elements, std::ostream& out, int indent) {
    bool first_element_printed = true;
    for (const auto& element : elements) {
        if (element) {
            std::ostringstream single_geom_stream;
            element->printGeoJSON(single_geom_stream, indent);
            std::string single_geom_str = single_geom_stream.str();

            if (!single_geom_str.empty()) {
                if (!first_element_printed) {
                    out << ",\n";
                }
                out << single_geom_str;
                first_element_printed = false;
            }
        }
    }
}

struct LandElementsNode : public AstNode {
    std::vector<std::unique_ptr<LandElementNode>> elements;
    LandElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printGeoJSONForElementList(elements, out, indent);
    }
};

enum class UndergroundElementCurlyType { ELEVATOR, OREBED };
struct UndergroundElementCurlyNode : public AstNode {
    UndergroundElementCurlyType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    UndergroundElementCurlyNode(UndergroundElementCurlyType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l, c), type(t), vsebina(std::move(v)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (vsebina) vsebina->printGeoJSON(out, indent);
    }
};

enum class UndergroundElementBracketType { TUNNEL, SHAFT };
struct UndergroundElementBracketNode : public AstNode {
    UndergroundElementBracketType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    UndergroundElementBracketNode(UndergroundElementBracketType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l, c), type(t), vsebina(std::move(v)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (vsebina) vsebina->printGeoJSON(out, indent);
    }
};

struct UndergroundElementNode : public AstNode {
    std::variant<std::unique_ptr<UndergroundElementCurlyNode>, std::unique_ptr<UndergroundElementBracketNode>> element_choice;
    UndergroundElementNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& choice) {
            if (choice) choice->printGeoJSON(out, indent);
            }, element_choice);
    }
};

struct UndergroundElementsNode : public AstNode {
    std::vector<std::unique_ptr<UndergroundElementNode>> elements;
    UndergroundElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printGeoJSONForElementList(elements, out, indent);
    }
};

enum class SeaElementCurlyType { PLATFORM, PUMP, RISER };
struct SeaElementCurlyNode : public AstNode {
    SeaElementCurlyType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    SeaElementCurlyNode(SeaElementCurlyType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l, c), type(t), vsebina(std::move(v)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (vsebina) vsebina->printGeoJSON(out, indent);
    }
};

struct SeaElementsNode : public AstNode {
    std::vector<std::unique_ptr<SeaElementCurlyNode>> elements;
    SeaElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printGeoJSONForElementList(elements, out, indent);
    }
};

enum class MineType { SURFACE, UNDERGROUND, IN_PLACE, IN_SITU, DEEP_SEA };
struct MinetypeNode : public AstNode {
    MineType type_enum;
    std::variant<
        std::unique_ptr<LandElementsNode>,
        std::unique_ptr<UndergroundElementsNode>,
        std::unique_ptr<SeaElementsNode>
    > elements_content;
    MinetypeNode(MineType t, int l, int c) : AstNode(l, c), type_enum(t) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        std::visit([&](const auto& elements_ptr) {
            if (elements_ptr) {
                elements_ptr->printGeoJSON(out, indent);
            }
            }, elements_content);
    }
};

struct StatementNode : virtual public AstNode {
    StatementNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "StatementNode (Base)" << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct ProgramNode : public AstNode {
    std::vector<std::unique_ptr<StatementNode>> statements;
    ProgramNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ProgramNode @ (" << line << "," << col << ")" << std::endl;
        for (const auto& stmt : statements) {
            if (stmt) stmt->print(out, indentLevel + 1);
        }
    }

    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printIndent(out, indent);
        out << "{\n";
        printIndent(out, indent + 1);
        out << "\"type\": \"FeatureCollection\",\n";
        printIndent(out, indent + 1);
        out << "\"features\": [\n";

        bool first_feature_printed = true;
        for (const auto& stmt : statements) {
            if (stmt) {
                std::ostringstream geometries_stream;
                stmt->printGeoJSON(geometries_stream, indent + 5);
                std::string geom_str = geometries_stream.str();

                if (!geom_str.empty()) {
                    if (!first_feature_printed) {
                        out << ",\n";
                    }

                    printIndent(out, indent + 2); out << "{\n";
                    printIndent(out, indent + 3); out << "\"type\": \"Feature\",\n";
                    printIndent(out, indent + 3); out << "\"geometry\": {\n";
                    printIndent(out, indent + 4); out << "\"type\": \"GeometryCollection\",\n";
                    printIndent(out, indent + 4); out << "\"geometries\": [\n";

                    out << geom_str;

                    if (geom_str.back() != '\n') {
                        out << "\n";
                    }

                    printIndent(out, indent + 4); out << "]\n";
                    printIndent(out, indent + 3); out << "},\n";
                    printIndent(out, indent + 3); out << "\"properties\": {}\n";
                    printIndent(out, indent + 2); out << "}";

                    first_feature_printed = false;
                }
            }
        }

        if (!first_feature_printed) {
            out << "\n";
        }
        printIndent(out, indent + 1); out << "]\n";
        printIndent(out, indent); out << "}";
    }
};


struct StartNode : public StatementNode {
    std::string mine_name;
    std::unique_ptr<MinetypeNode> minetype_def;
    StartNode(std::string name, std::unique_ptr<MinetypeNode> mt, int l, int c)
        : AstNode(l, c), StatementNode(l, c), mine_name(std::move(name)), minetype_def(std::move(mt)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (minetype_def) {
            minetype_def->printGeoJSON(out, indent);
        }
    }
};

struct ExprStatementNode : public StatementNode {
    std::unique_ptr<ExprNode> expression;
    ExprStatementNode(std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l, c), StatementNode(l, c), expression(std::move(expr)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (expression) {
            expression->printGeoJSON(out, indent);
        }
    }
};

struct AttributeNode : public AstNode {
    std::vector<std::unique_ptr<ExprNode>> expr_list;
    AttributeNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        for (const auto& expr : expr_list) {
            if (expr) {
                expr->printGeoJSON(out, indent);
            }
        }
    }
};

struct FunctionCallNode : public ExprNode {
    std::string function_name;
    std::unique_ptr<AttributeNode> arguments;
    FunctionCallNode(std::string name, std::unique_ptr<AttributeNode> args, int l, int c)
        : ExprNode(l, c), function_name(std::move(name)), arguments(std::move(args)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (arguments) {
            arguments->printGeoJSON(out, indent);
        }
    }
};

struct CommandNode : virtual public AstNode {
    CommandNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "CommandNode (Base)" << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct CommandSetNode : public AstNode {
    std::vector<std::unique_ptr<CommandNode>> commands;
    CommandSetNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        printGeoJSONForElementList(commands, out, indent);
    }
};

struct BeginPrimeBodyNode : public AstNode {
    std::variant<
        std::unique_ptr<CommandSetNode>,
        std::unique_ptr<ExprNode>
    > content;
    bool is_empty = false;
    BeginPrimeBodyNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (!is_empty) {
            std::visit([&](const auto& c) {
                if (c) c->printGeoJSON(out, indent);
                }, content);
        }
    }
};

struct AssignNode : public StatementNode, public CommandNode {
    std::string variable_name;
    std::unique_ptr<ExprNode> expression;
    AssignNode(std::string name, std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l, c), StatementNode(l, c), CommandNode(l, c), variable_name(std::move(name)), expression(std::move(expr)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (expression) {
            expression->printGeoJSON(out, indent);
        }
    }
};

struct PrintNode : public StatementNode, public CommandNode {
    std::unique_ptr<ExprNode> expression;
    PrintNode(std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l, c), StatementNode(l, c), CommandNode(l, c), expression(std::move(expr)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
};

struct LoopNode : public StatementNode, public CommandNode {
    std::unique_ptr<AssignNode> initialization;
    std::unique_ptr<ExprNode> to_condition;
    std::unique_ptr<BeginPrimeBodyNode> body;
    LoopNode(std::unique_ptr<AssignNode> init, std::unique_ptr<ExprNode> cond, std::unique_ptr<BeginPrimeBodyNode> b, int l, int c)
        : AstNode(l, c), StatementNode(l, c), CommandNode(l, c),
        initialization(std::move(init)), to_condition(std::move(cond)), body(std::move(b)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
    void printGeoJSON(std::ostream& out, int indent = 0) const override {
        if (body) body->printGeoJSON(out, indent);
    }
};

struct FunctionNode : public StatementNode {
    std::string function_name;
    std::unique_ptr<AttributeNode> parameters;
    std::unique_ptr<BeginPrimeBodyNode> body;
    std::unique_ptr<ExprNode> return_expression;
    FunctionNode(std::string name, std::unique_ptr<AttributeNode> params,
        std::unique_ptr<BeginPrimeBodyNode> b, std::unique_ptr<ExprNode> retExpr, int l, int c)
        : AstNode(l, c), StatementNode(l, c), function_name(std::move(name)), parameters(std::move(params)),
        body(std::move(b)), return_expression(std::move(retExpr)) {
    }
    void print(std::ostream& out, int indentLevel = 0) const override { /* ... */ }
};