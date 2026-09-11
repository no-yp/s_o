#pragma once
#include <memory>
#include <string>
#include <iostream>

struct Expression {
  virtual ~Expression() = default;
  virtual void print(std::ostream& os, int indent = 0) const = 0;
};

using ExprPtr = std::unique_ptr<Expression>;

struct NumberLiteral : Expression {
  std::string value; bool isFloat;

  NumberLiteral(std::string v, bool f): value(std::move(v)), isFloat(f) {}

  void print(std::ostream& os, int indent) const override {
    os << std::string(indent, ' ') << "Number(" << value << ")\n";
  }
};

struct Identifier : Expression {
  std::string name;
  Identifier(std::string n): name(std::move(n)) {}
  void print(std::ostream& os, int indent) const override {
    os << std::string(indent, ' ') <<"Identifier("<< name <<")\n";
  }
};

struct BinaryOperation : Expression {
  std::string op; ExprPtr left, right;

  BinaryOperation(std::string o, ExprPtr l, ExprPtr r):
    op(std::move(o)), left(std::move(l)), right(std::move(r)) {}

  void print(std::ostream& os, int indent) const override {
    os << std::string(indent, ' ') << "Binary(" << op << ")\n";
    left->print(os, indent + 2);
    right->print(os, indent + 2);
  }
};

struct Statement {
  virtual ~Statement() = default;
  virtual void print(std::ostream& os, int indent = 0) const = 0;
};

using StmtPtr = std::unique_ptr<Statement>;

struct VariableDeclaration : Statement {
  std::string name; ExprPtr initializer;

  VariableDeclaration(std::string n, ExprPtr init):
    name(std::move(n)), initializer(std::move(init)) {}

  void print(std::ostream& os, int indent) const override {
    os << std::string(indent, ' ') << "VariableDeclaration("<< name <<")\n";
    initializer->print(os, indent + 2);
  }
};

struct ExpressionStatement : Statement {
  ExprPtr expr;
  ExpressionStatement(ExprPtr e): expr(std::move(e)) {}

  void print (std::ostream& os, int indent) const override {
    os << std::string(indent, ' ') << "ExpressionStatement\n";
    expr->print(os, indent + 2);
  }
};
