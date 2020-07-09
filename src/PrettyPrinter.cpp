#include "PrettyPrinter.h"
#include <iostream>

void PrettyPrinter::print(AST::Program *p, std::ostream &s, char c, int n) {
   PrettyPrinter visitor(s, c, n);
   p->accept(&visitor);
}

void PrettyPrinter::endVisit(AST::Program * element) {
  std::string programString = "";
  for (auto &fn : element->getFunctions()) {
    programString = visitResults.back() + "\n" + programString;
    visitResults.pop_back();
  }
  os << programString;
  os.flush();
}

/*
 * General approach taken by visit methods.
 * - visit() is used to increase indentation (decrease should happen in endVisit).  
 * - endVisit() should expect a string for all of its AST nodes in reverse order in visitResults.
 *   Communicate the single string for the visited node by pushing to the back of visitedResults.
 */

/* 
 * Before visiting function, record string for signature and setup indentation for body.
 * This visit method pushes a string result, that the endVisit method should extend.
 */
bool PrettyPrinter::visit(AST::Function * element) {
  indentLevel++;
  return true;
}

/*
 * After visiting function, collect the string representations for the:
 *   statements, declarations, formals, and then function name
 * they are on the visit stack in that order.
 */
void PrettyPrinter::endVisit(AST::Function * element) {
  std::string bodyString = "";
  for (auto &stmt : element->getStmts()) {
    bodyString = visitResults.back() + "\n" + bodyString;
    visitResults.pop_back();
  }

  for (auto &decl : element->getDeclarations()) {
    bodyString = visitResults.back() + "\n" + bodyString;
    visitResults.pop_back();
  }

  std::string formalsString = "";
  bool skip = true;
  for(auto &formal : element->getFormals()) {
    if (skip) {
      formalsString = visitResults.back() + formalsString;
      visitResults.pop_back();
      skip = false;
      continue;
    }
    formalsString = visitResults.back() + ", " + formalsString;
    visitResults.pop_back();
  }

  // function name is last element on stack
  std::string functionString = visitResults.back();
  visitResults.pop_back();

  functionString += "(" + formalsString + ") {\n" + bodyString + "}\n";

  indentLevel--;

  visitResults.push_back(functionString);
}

void PrettyPrinter::endVisit(AST::NumberExpr * element) {
  visitResults.push_back(std::to_string(element->getValue()));
}

void PrettyPrinter::endVisit(AST::VariableExpr * element) {
  visitResults.push_back(element->getName());
}

void PrettyPrinter::endVisit(AST::BinaryExpr * element) {
  std::string rightString = visitResults.back();
  visitResults.pop_back();
  std::string leftString = visitResults.back();
  visitResults.pop_back();

  visitResults.push_back("(" + leftString + " " + element->getOp() + " " + rightString + ")");
}

void PrettyPrinter::endVisit(AST::InputExpr * element) {
  visitResults.push_back("input");
}

void PrettyPrinter::endVisit(AST::FunAppExpr * element) {
  std::string funAppString;

  /* 
   * Skip printing of comma separator for last arg.
   */ 
  std::string actualsString = "";
  bool skip = true;
  for (auto &arg : element->getActuals()) {
    if (skip) {
      actualsString = visitResults.back() + actualsString;
      visitResults.pop_back();
      skip = false;
      continue;
    }
    actualsString = visitResults.back() + ", " + actualsString;
    visitResults.pop_back();
  } 

  funAppString = visitResults.back() + "(" + actualsString + ")";
  visitResults.pop_back();

  visitResults.push_back(funAppString);
}

void PrettyPrinter::endVisit(AST::AllocExpr * element) {
  std::string init = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("alloc " + init);
}

void PrettyPrinter::endVisit(AST::RefExpr * element) {
  std::string var = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("&" + var);
}

void PrettyPrinter::endVisit(AST::DeRefExpr * element) {
  std::string base = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back("*" + base);
}

void PrettyPrinter::endVisit(AST::NullExpr * element) {
  visitResults.push_back("null");
}

void PrettyPrinter::endVisit(AST::FieldExpr * element) {
  std::string init = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(element->getField() + ":" + init);
}

void PrettyPrinter::endVisit(AST::RecordExpr * element) {
  /* 
   * Skip printing of comma separator for last record element.
   */ 
  std::string fieldsString = "";
  bool skip = true;
  for (auto &f : element->getFields()) {
    if (skip) {
      fieldsString = visitResults.back() + fieldsString;
      visitResults.pop_back();
      skip = false;
      continue;
    }
    fieldsString = visitResults.back() + ", " + fieldsString;
    visitResults.pop_back();
  } 

  std::string recordString = "{" + fieldsString + "}";
  visitResults.push_back(recordString);
}

void PrettyPrinter::endVisit(AST::AccessExpr * element) {
  std::string accessString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(accessString + '.' + element->getField());
}

void PrettyPrinter::endVisit(AST::DeclNode * element) {
  visitResults.push_back(element->getName());
}

void PrettyPrinter::endVisit(AST::DeclStmt * element) {
  std::string declString = "";
  bool skip = true;
  for (auto &id : element->getVars()) {
    if (skip) {
      declString = visitResults.back() + declString;
      visitResults.pop_back();
      skip = false;
      continue;
    }
    declString = visitResults.back() + ", " + declString;
    visitResults.pop_back();
  }

  declString = indent() + "var " + declString + ";";

  visitResults.push_back(declString);
}

void PrettyPrinter::endVisit(AST::AssignStmt * element) {
  std::string rhsString = visitResults.back();
  visitResults.pop_back();
  std::string lhsString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + lhsString + " = " + rhsString + ";");
}

bool PrettyPrinter::visit(AST::BlockStmt * element) {
  indentLevel++;
  return true;
}

void PrettyPrinter::endVisit(AST::BlockStmt * element) {
  std::string stmtsString = "";
  for (auto &s : element->getStmts()) {
    stmtsString = visitResults.back() + "\n" + stmtsString;
    visitResults.pop_back();
  } 

  indentLevel--;

  std::string blockString = indent() + "{\n" + stmtsString + indent() + "}";

  visitResults.push_back(blockString);
}

/*
 * For a while the body should be indented, but not the condition.
 * Since conditions are expressions and their visit methods never indent
 * incrementing here works.
 */
bool PrettyPrinter::visit(AST::WhileStmt * element) {
  indentLevel++;
  return true;
}

void PrettyPrinter::endVisit(AST::WhileStmt * element) {
  std::string bodyString = visitResults.back();
  visitResults.pop_back();
  std::string condString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string whileString = indent() + "while (" + condString + ") \n" + bodyString;
  visitResults.push_back(whileString);
}

bool PrettyPrinter::visit(AST::IfStmt * element) {
  indentLevel++;
  return true;
}

void PrettyPrinter::endVisit(AST::IfStmt * element) {
  std::string elseString;
  if (element->getElse() != nullptr) {
    elseString = visitResults.back();
    visitResults.pop_back();
  }

  std::string thenString = visitResults.back();
  visitResults.pop_back();

  std::string condString = visitResults.back();
  visitResults.pop_back();

  indentLevel--;

  std::string ifString = indent() + "if (" + condString + ") \n" + thenString;

  if (element->getElse() != nullptr) {
    ifString += "\n" + indent() + "else\n" + elseString; 
  }

  visitResults.push_back(ifString);
}

void PrettyPrinter::endVisit(AST::OutputStmt * element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "output " + argString + ";");
}

void PrettyPrinter::endVisit(AST::ErrorStmt * element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "error " + argString + ";");
}

void PrettyPrinter::endVisit(AST::ReturnStmt * element) {
  std::string argString = visitResults.back();
  visitResults.pop_back();
  visitResults.push_back(indent() + "return " + argString + ";");
}

std::string PrettyPrinter::indent() const {
  return std::string(indentLevel*indentSize, indentChar);
}
