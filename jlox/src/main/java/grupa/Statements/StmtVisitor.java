package grupa.Statements;

import grupa.Parser.RuntimeError;

public interface StmtVisitor<R> {
    R visitExpressionStatement(Expression statement) throws RuntimeError;

    R visitPrintStatement(Print statement) throws RuntimeError;
}