#pragma once

#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <iostream>
#include <iomanip>

/*
    Ast.hpp
    Darko Sever 28.5.2025
*/

struct AstNode {
    int line;
    int col;

    AstNode(int l, int c) : line(l), col(c) {}
    virtual ~AstNode() = default;

    virtual void print(std::ostream& out, int indentLevel = 0) const {
        out << std::string(indentLevel * 2, ' ') << "AstNode (Generic)" << " @ (" << line << "," << col << ")" << std::endl;
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
};

struct HexLiteralNode : public ExprNode {
    std::string value;
    HexLiteralNode(std::string val, int l, int c) : ExprNode(l, c), value(std::move(val)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "HexLiteralNode: " << value << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct VariableAccessNode : public ExprNode {
    std::string name;
    VariableAccessNode(std::string varName, int l, int c) : ExprNode(l, c), name(std::move(varName)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "VariableAccessNode: " << name << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct ParenExpressionNode : public ExprNode {
    std::unique_ptr<ExprNode> expression;
    ParenExpressionNode(std::unique_ptr<ExprNode> expr, int l, int c) : ExprNode(l, c), expression(std::move(expr)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ParenExpressionNode @ (" << line << "," << col << ")" << std::endl;
        if (expression) expression->print(out, indentLevel + 1);
    }
};

struct UnaryOpNode : public ExprNode {
    UnaryOperatorType op;
    std::unique_ptr<ExprNode> operand;
    UnaryOpNode(UnaryOperatorType oper, std::unique_ptr<ExprNode> prim, int l, int c)
        : ExprNode(l, c), op(oper), operand(std::move(prim)) {}
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
};

struct BinaryOpNode : public ExprNode {
    std::unique_ptr<ExprNode> left;
    BinaryOperatorType op_type;
    std::unique_ptr<ExprNode> right;
    BinaryOpNode(std::unique_ptr<ExprNode> l_node, BinaryOperatorType oper, std::unique_ptr<ExprNode> r_node, int line_val, int col_val)
        : ExprNode(line_val, col_val), left(std::move(l_node)), op_type(oper), right(std::move(r_node)) {}
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
};

struct PointNode : public AstNode {
    std::unique_ptr<ExprNode> x;
    std::unique_ptr<ExprNode> y;
    PointNode(std::unique_ptr<ExprNode> _x, std::unique_ptr<ExprNode> _y, int l, int c)
        : AstNode(l, c), x(std::move(_x)), y(std::move(_y)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PointNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "X:" << std::endl;
        if (x) x->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Y:" << std::endl;
        if (y) y->print(out, indentLevel + 2);
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
        : AstNode(l, c), start_point(std::move(s)), end_point(std::move(e)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LineNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Start:" << std::endl;
        if (start_point) start_point->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "End:" << std::endl;
        if (end_point) end_point->print(out, indentLevel + 2);
    }
};

struct PolylineNode : public AstNode {
    std::unique_ptr<PointsNode> points;
    PolylineNode(std::unique_ptr<PointsNode> pts, int l, int c) : AstNode(l, c), points(std::move(pts)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PolylineNode @ (" << line << "," << col << ")" << std::endl;
        if (points) points->print(out, indentLevel + 1);
    }
};

struct BendNode : public AstNode {
    std::unique_ptr<PointNode> start_point;
    std::unique_ptr<PointNode> control_point;
    std::unique_ptr<ExprNode> third_param;
    BendNode(std::unique_ptr<PointNode> p1, std::unique_ptr<PointNode> p2, std::unique_ptr<ExprNode> param, int l, int c)
        : AstNode(l, c), start_point(std::move(p1)), control_point(std::move(p2)), third_param(std::move(param)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BendNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Point1:" << std::endl;
        if (start_point) start_point->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Point2:" << std::endl;
        if (control_point) control_point->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Param:" << std::endl;
        if (third_param) third_param->print(out, indentLevel + 2);
    }
};

struct BoxNode : public AstNode {
    std::unique_ptr<PointNode> point1;
    std::unique_ptr<PointNode> point2;
    BoxNode(std::unique_ptr<PointNode> p1, std::unique_ptr<PointNode> p2, int l, int c)
        : AstNode(l, c), point1(std::move(p1)), point2(std::move(p2)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BoxNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Point1:" << std::endl;
        if (point1) point1->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Point2:" << std::endl;
        if (point2) point2->print(out, indentLevel + 2);
    }
};

struct ArbitraryPolygonNode : public AstNode {
    std::unique_ptr<PointsNode> points;
    ArbitraryPolygonNode(std::unique_ptr<PointsNode> pts, int l, int c) : AstNode(l, c), points(std::move(pts)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ArbitraryPolygonNode @ (" << line << "," << col << ")" << std::endl;
        if (points) points->print(out, indentLevel + 1);
    }
};

struct CircNode : public AstNode {
    std::unique_ptr<PointNode> center;
    std::unique_ptr<ExprNode> radius;
    CircNode(std::unique_ptr<PointNode> p_center, std::unique_ptr<ExprNode> p_radius, int p_line, int p_col)
        : AstNode(p_line, p_col), center(std::move(p_center)), radius(std::move(p_radius)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "CircNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Center:" << std::endl;
        if (center) center->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Radius:" << std::endl;
        if (radius) radius->print(out, indentLevel + 2);
    }
};

struct LinesBendOptNode : public AstNode {
    std::unique_ptr<ExprNode> value;
    LinesBendOptNode(std::unique_ptr<ExprNode> val, int l, int c) : AstNode(l, c), value(std::move(val)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinesBendOptNode @ (" << line << "," << col << ")" << std::endl;
        if (value) {
            value->print(out, indentLevel + 1);
        } else {
            out << std::string((indentLevel + 1) * 2, ' ') << "(Epsilon - no value)" << std::endl;
        }
    }
};

struct LinesItemContentAfterParenNode : public AstNode {
    std::variant<
        std::unique_ptr<PointsNode>,
        std::tuple<std::unique_ptr<PointNode>, std::unique_ptr<PointNode>, std::unique_ptr<LinesBendOptNode>>
    > content;
    LinesItemContentAfterParenNode(int l, int c) : AstNode(l,c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinesItemContentAfterParenNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<PointsNode>>) {
                if (arg) arg->print(out, indentLevel + 1);
            } else if constexpr (std::is_same_v<T, std::tuple<std::unique_ptr<PointNode>, std::unique_ptr<PointNode>, std::unique_ptr<LinesBendOptNode>>>) {
                out << std::string((indentLevel + 1) * 2, ' ') << "Tuple:" << std::endl;
                if (std::get<0>(arg)) std::get<0>(arg)->print(out, indentLevel + 2);
                if (std::get<1>(arg)) std::get<1>(arg)->print(out, indentLevel + 2);
                if (std::get<2>(arg)) std::get<2>(arg)->print(out, indentLevel + 2);
            }
        }, content);
    }
};

struct LinesContentNode : public AstNode {
    std::unique_ptr<LinesItemContentAfterParenNode> item_content;
    LinesContentNode(std::unique_ptr<LinesItemContentAfterParenNode> ic, int l, int c)
        : AstNode(l, c), item_content(std::move(ic)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinesContentNode @ (" << line << "," << col << ")" << std::endl;
        if (item_content) item_content->print(out, indentLevel + 1);
    }
};

struct LinePropertyNode : public AstNode {
    std::string name;
    std::unique_ptr<LinesContentNode> content;
    LinePropertyNode(std::string p_name, std::unique_ptr<LinesContentNode> p_content, int p_line, int p_col)
        : AstNode(p_line, p_col), name(std::move(p_name)), content(std::move(p_content)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinePropertyNode: " << name << " @ (" << line << "," << col << ")" << std::endl;
        if (content) content->print(out, indentLevel + 1);
    }
};

struct LinesNode : public AstNode {
    std::vector<std::unique_ptr<LinePropertyNode>> properties;
    LinesNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LinesNode @ (" << line << "," << col << ")" << std::endl;
        if (properties.empty()) {
             out << std::string((indentLevel + 1) * 2, ' ') << "(No properties - epsilon)" << std::endl;
        }
        for (const auto& prop : properties) {
            if (prop) prop->print(out, indentLevel + 1);
        }
    }
};

struct ElementVsebinaNode : public AstNode {
    ElementVsebinaNode(int l, int c) : AstNode(l,c) {}
     void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ElementVsebinaNode (Base)" << " @ (" << line << "," << col << ")" << std::endl;
    }
};

struct PathVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<LineNode>, std::unique_ptr<PolylineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    PathVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PathVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Shape:" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 2); }, shape);
        out << std::string((indentLevel + 1) * 2, ' ') << "Properties:" << std::endl;
        if (lines_properties) lines_properties->print(out, indentLevel + 2);
    }
};

struct RiverVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<LineNode>, std::unique_ptr<PolylineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    RiverVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
     void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "RiverVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Shape:" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 2); }, shape);
        out << std::string((indentLevel + 1) * 2, ' ') << "Properties:" << std::endl;
        if (lines_properties) lines_properties->print(out, indentLevel + 2);
    }
};

struct StripVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>> shape;
    StripVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "StripVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, shape);
    }
};

struct BenchVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<ArbitraryPolygonNode> polygon;
    BenchVsebinaNode(std::unique_ptr<ArbitraryPolygonNode> poly, int l, int c)
        : ElementVsebinaNode(l, c), polygon(std::move(poly)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BenchVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        if (polygon) polygon->print(out, indentLevel + 1);
    }
};

struct SpoilpitVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    SpoilpitVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "SpoilpitVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, shape);
    }
};

struct PlantVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    PlantVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PlantVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, shape);
    }
};

enum class WellType { MONITORING, INJECTION, RECOVERY };
struct WellVsebinaNode : public ElementVsebinaNode {
    WellType type;
    std::unique_ptr<PointNode> point;
    WellVsebinaNode(WellType t, std::unique_ptr<PointNode> p, int l, int c)
        : ElementVsebinaNode(l, c), type(t), point(std::move(p)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "WellVsebinaNode: Type=";
        switch(type) {
            case WellType::MONITORING: out << "MONITORING"; break;
            case WellType::INJECTION: out << "INJECTION"; break;
            case WellType::RECOVERY: out << "RECOVERY"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (point) point->print(out, indentLevel + 1);
    }
};

enum class ElevatorType { HUMAN, ORE };
struct ElevatorVsebinaNode : public ElementVsebinaNode {
    std::variant<ElevatorType, std::unique_ptr<PointNode>> content;
    ElevatorVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ElevatorVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, ElevatorType>) {
                out << std::string((indentLevel + 1) * 2, ' ') << "Type: ";
                switch(arg) {
                    case ElevatorType::HUMAN: out << "HUMAN"; break;
                    case ElevatorType::ORE: out << "ORE"; break;
                }
                out << std::endl;
            } else if constexpr (std::is_same_v<T, std::unique_ptr<PointNode>>) {
                if (arg) arg->print(out, indentLevel + 1);
            }
        }, content);
    }
};

struct OrebedVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    OrebedVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "OrebedVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, shape);
    }
};

struct TunnelVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<PolylineNode>, std::unique_ptr<LineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    TunnelVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "TunnelVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Shape:" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 2); }, shape);
        out << std::string((indentLevel + 1) * 2, ' ') << "Properties:" << std::endl;
        if (lines_properties) lines_properties->print(out, indentLevel + 2);
    }
};

struct ShaftVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<PolylineNode>, std::unique_ptr<LineNode>, std::unique_ptr<BendNode>> shape;
    std::unique_ptr<LinesNode> lines_properties;
    ShaftVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ShaftVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Shape:" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 2); }, shape);
        out << std::string((indentLevel + 1) * 2, ' ') << "Properties:" << std::endl;
        if (lines_properties) lines_properties->print(out, indentLevel + 2);
    }
};

struct PlatformVsebinaNode : public ElementVsebinaNode {
    std::variant<std::unique_ptr<ArbitraryPolygonNode>, std::unique_ptr<BoxNode>, std::unique_ptr<CircNode>> shape;
    PlatformVsebinaNode(int l, int c) : ElementVsebinaNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PlatformVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, shape);
    }
};

struct PumpVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<PointNode> point;
    PumpVsebinaNode(std::unique_ptr<PointNode> p, int l, int c) : ElementVsebinaNode(l,c), point(std::move(p)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PumpVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        if (point) point->print(out, indentLevel + 1);
    }
};

struct RiserVsebinaNode : public ElementVsebinaNode {
    std::unique_ptr<PointNode> point;
    RiserVsebinaNode(std::unique_ptr<PointNode> p, int l, int c) : ElementVsebinaNode(l,c), point(std::move(p)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "RiserVsebinaNode @ (" << line << "," << col << ")" << std::endl;
        if (point) point->print(out, indentLevel + 1);
    }
};

enum class LandElementCurlyType { PATH, RIVER };
struct LandElementCurlyNode : public AstNode {
    LandElementCurlyType type;
    std::unique_ptr<PathVsebinaNode> path_vsebina;
    std::unique_ptr<RiverVsebinaNode> river_vsebina;
    LandElementCurlyNode(LandElementCurlyType t, int l, int c) : AstNode(l,c), type(t) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LandElementCurlyNode: Type=";
        if (type == LandElementCurlyType::PATH) {
            out << "PATH @ (" << line << "," << col << ")" << std::endl;
            if (path_vsebina) path_vsebina->print(out, indentLevel + 1);
        } else {
            out << "RIVER @ (" << line << "," << col << ")" << std::endl;
            if (river_vsebina) river_vsebina->print(out, indentLevel + 1);
        }
    }
};

enum class LandElementBracketType { STRIP, BENCH, SPOILPIT, PLANT, WELL };
struct LandElementBracketNode : public AstNode {
    LandElementBracketType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    LandElementBracketNode(LandElementBracketType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l,c), type(t), vsebina(std::move(v)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LandElementBracketNode: Type=";
        switch(type) {
            case LandElementBracketType::STRIP: out << "STRIP"; break;
            case LandElementBracketType::BENCH: out << "BENCH"; break;
            case LandElementBracketType::SPOILPIT: out << "SPOILPIT"; break;
            case LandElementBracketType::PLANT: out << "PLANT"; break;
            case LandElementBracketType::WELL: out << "WELL"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (vsebina) vsebina->print(out, indentLevel + 1);
    }
};

struct LandElementNode : public AstNode {
    std::variant<std::unique_ptr<LandElementCurlyNode>, std::unique_ptr<LandElementBracketNode>> element_choice;
    LandElementNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LandElementNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, element_choice);
    }
};

struct LandElementsNode : public AstNode {
    std::vector<std::unique_ptr<LandElementNode>> elements;
    LandElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LandElementsNode @ (" << line << "," << col << ")" << std::endl;
        for (const auto& el : elements) {
            if (el) el->print(out, indentLevel + 1);
        }
    }
};

enum class UndergroundElementCurlyType { ELEVATOR, OREBED };
struct UndergroundElementCurlyNode : public AstNode {
    UndergroundElementCurlyType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    UndergroundElementCurlyNode(UndergroundElementCurlyType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l,c), type(t), vsebina(std::move(v)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "UndergroundElementCurlyNode: Type=";
        if (type == UndergroundElementCurlyType::ELEVATOR) out << "ELEVATOR";
        else out << "OREBED";
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (vsebina) vsebina->print(out, indentLevel + 1);
    }
};

enum class UndergroundElementBracketType { TUNNEL, SHAFT };
struct UndergroundElementBracketNode : public AstNode {
    UndergroundElementBracketType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    UndergroundElementBracketNode(UndergroundElementBracketType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l,c), type(t), vsebina(std::move(v)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "UndergroundElementBracketNode: Type=";
        if (type == UndergroundElementBracketType::TUNNEL) out << "TUNNEL";
        else out << "SHAFT";
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (vsebina) vsebina->print(out, indentLevel + 1);
    }
};

struct UndergroundElementNode : public AstNode {
    std::variant<std::unique_ptr<UndergroundElementCurlyNode>, std::unique_ptr<UndergroundElementBracketNode>> element_choice;
    UndergroundElementNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "UndergroundElementNode @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, element_choice);
    }
};

struct UndergroundElementsNode : public AstNode {
    std::vector<std::unique_ptr<UndergroundElementNode>> elements;
    UndergroundElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "UndergroundElementsNode @ (" << line << "," << col << ")" << std::endl;
        for (const auto& el : elements) {
            if (el) el->print(out, indentLevel + 1);
        }
    }
};

enum class SeaElementCurlyType { PLATFORM, PUMP, RISER };
struct SeaElementCurlyNode : public AstNode {
    SeaElementCurlyType type;
    std::unique_ptr<ElementVsebinaNode> vsebina;
    SeaElementCurlyNode(SeaElementCurlyType t, std::unique_ptr<ElementVsebinaNode> v, int l, int c)
        : AstNode(l,c), type(t), vsebina(std::move(v)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "SeaElementCurlyNode: Type=";
        switch(type) {
            case SeaElementCurlyType::PLATFORM: out << "PLATFORM"; break;
            case SeaElementCurlyType::PUMP: out << "PUMP"; break;
            case SeaElementCurlyType::RISER: out << "RISER"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        if (vsebina) vsebina->print(out, indentLevel + 1);
    }
};

struct SeaElementsNode : public AstNode {
    std::vector<std::unique_ptr<SeaElementCurlyNode>> elements;
    SeaElementsNode(int l, int c) : AstNode(l, c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "SeaElementsNode @ (" << line << "," << col << ")" << std::endl;
        for (const auto& el : elements) {
            if (el) el->print(out, indentLevel + 1);
        }
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
    MinetypeNode(MineType t, int l, int c) : AstNode(l,c), type_enum(t) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "MinetypeNode: Type=";
         switch(type_enum) {
            case MineType::SURFACE: out << "SURFACE"; break;
            case MineType::UNDERGROUND: out << "UNDERGROUND"; break;
            case MineType::IN_PLACE: out << "IN_PLACE"; break;
            case MineType::IN_SITU: out << "IN_SITU"; break;
            case MineType::DEEP_SEA: out << "DEEP_SEA"; break;
        }
        out << " @ (" << line << "," << col << ")" << std::endl;
        std::visit([&](const auto& arg){ if(arg) arg->print(out, indentLevel + 1); }, elements_content);
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
};

struct StartNode : public StatementNode {
    std::string mine_name;
    std::unique_ptr<MinetypeNode> minetype_def;
    StartNode(std::string name, std::unique_ptr<MinetypeNode> mt, int l, int c)
        : AstNode(l,c), 
          StatementNode(l, c), 
          mine_name(std::move(name)), 
          minetype_def(std::move(mt)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "StartNode: " << mine_name << " @ (" << line << "," << col << ")" << std::endl;
        if (minetype_def) minetype_def->print(out, indentLevel + 1);
    }
};

struct ExprStatementNode : public StatementNode {
    std::unique_ptr<ExprNode> expression;
    ExprStatementNode(std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l,c),
          StatementNode(l, c), 
          expression(std::move(expr)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "ExprStatementNode @ (" << line << "," << col << ")" << std::endl;
        if (expression) expression->print(out, indentLevel + 1);
    }
};

struct AttributeNode : public AstNode {
    std::vector<std::unique_ptr<ExprNode>> expr_list;
    AttributeNode(int l, int c) : AstNode(l,c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "AttributeNode @ (" << line << "," << col << ")" << std::endl;
        if (expr_list.empty()){
            out << std::string((indentLevel+1)*2, ' ') << "(No attributes)" << std::endl;
        }
        for (const auto& expr : expr_list) {
            if (expr) expr->print(out, indentLevel + 1);
        }
    }
};

struct FunctionCallNode : public ExprNode {
    std::string function_name;
    std::unique_ptr<AttributeNode> arguments;
    FunctionCallNode(std::string name, std::unique_ptr<AttributeNode> args, int l, int c)
        : ExprNode(l, c), function_name(std::move(name)), arguments(std::move(args)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "FunctionCallNode: " << function_name << " @ (" << line << "," << col << ")" << std::endl;
        if (arguments) arguments->print(out, indentLevel + 1);
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
    CommandSetNode(int l, int c) : AstNode(l,c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "CommandSetNode @ (" << line << "," << col << ")" << std::endl;
        if(commands.empty()){
            out << std::string((indentLevel+1)*2, ' ') << "(Empty CommandSet)" << std::endl;
        }
        for (const auto& cmd : commands) {
            if (cmd) cmd->print(out, indentLevel + 1);
        }
    }
};

struct BeginPrimeBodyNode : public AstNode {
    std::variant<
        std::unique_ptr<CommandSetNode>,
        std::unique_ptr<ExprNode>
    > content;
    bool is_empty = false;
    BeginPrimeBodyNode(int l, int c) : AstNode(l,c) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "BeginPrimeBodyNode @ (" << line << "," << col << ")" << std::endl;
        if (is_empty) {
            out << std::string((indentLevel + 1) * 2, ' ') << "(Epsilon - empty body)" << std::endl;
        } else {
            std::visit([&](const auto& arg){
                if(arg) arg->print(out, indentLevel + 1);
                else out << std::string((indentLevel + 1) * 2, ' ') << "(Null content in BeginPrimeBody)" << std::endl;
            }, content);
        }
    }
};

struct AssignNode : public StatementNode, public CommandNode {
    std::string variable_name;
    std::unique_ptr<ExprNode> expression;
    
    AssignNode(std::string name, std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l,c),
          StatementNode(l,c), 
          CommandNode(l,c), 
          variable_name(std::move(name)), 
          expression(std::move(expr)) {}

    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "AssignNode: " << variable_name << " := @ (" << line << "," << col << ")" << std::endl;
        if (expression) expression->print(out, indentLevel + 1);
    }
};

struct PrintNode : public StatementNode, public CommandNode {
    std::unique_ptr<ExprNode> expression;
    
    PrintNode(std::unique_ptr<ExprNode> expr, int l, int c)
        : AstNode(l,c),
          StatementNode(l,c), 
          CommandNode(l,c), 
          expression(std::move(expr)) {}

    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "PrintNode (CONSOLE) @ (" << line << "," << col << ")" << std::endl;
        if (expression) expression->print(out, indentLevel + 1);
    }
};

struct LoopNode : public StatementNode, public CommandNode {
    std::unique_ptr<AssignNode> initialization;
    std::unique_ptr<ExprNode> to_condition;
    std::unique_ptr<BeginPrimeBodyNode> body;
    
    LoopNode(std::unique_ptr<AssignNode> init, std::unique_ptr<ExprNode> cond, std::unique_ptr<BeginPrimeBodyNode> b, int l, int c)
        : AstNode(l,c),
          StatementNode(l,c), 
          CommandNode(l,c), 
          initialization(std::move(init)), 
          to_condition(std::move(cond)), 
          body(std::move(b)) {}

    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "LoopNode (FOR) @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Initialization:" << std::endl;
        if (initialization) initialization->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "To Condition:" << std::endl;
        if (to_condition) to_condition->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Body:" << std::endl;
        if (body) body->print(out, indentLevel + 2);
    }
};

struct FunctionNode : public StatementNode {
    std::string function_name;
    std::unique_ptr<AttributeNode> parameters;
    std::unique_ptr<BeginPrimeBodyNode> body;
    std::unique_ptr<ExprNode> return_expression;
    FunctionNode(std::string name, std::unique_ptr<AttributeNode> params,
                 std::unique_ptr<BeginPrimeBodyNode> b, std::unique_ptr<ExprNode> retExpr, int l, int c)
        : AstNode(l,c),
          StatementNode(l, c), 
          function_name(std::move(name)), 
          parameters(std::move(params)),
          body(std::move(b)), 
          return_expression(std::move(retExpr)) {}
    void print(std::ostream& out, int indentLevel = 0) const override {
        out << std::string(indentLevel * 2, ' ') << "FunctionNode: " << function_name << " @ (" << line << "," << col << ")" << std::endl;
        out << std::string((indentLevel + 1) * 2, ' ') << "Parameters:" << std::endl;
        if (parameters) parameters->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Body:" << std::endl;
        if (body) body->print(out, indentLevel + 2);
        out << std::string((indentLevel + 1) * 2, ' ') << "Return Expression:" << std::endl;
        if (return_expression) return_expression->print(out, indentLevel + 2);
    }
};