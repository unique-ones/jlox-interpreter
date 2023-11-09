package grupa.Statements;

import grupa.Interpreter.RuntimeError;

import java.util.ArrayList;
import java.util.List;

public class Block extends Stmt {
    List<Stmt> stmts = new ArrayList<>();

    public Block(List<Stmt> stmts) {
        this.stmts = stmts;
    }
    public List<Stmt> getStmts() {
        return stmts;
    }

    @Override
    public <R> R accept(StmtVisitor<R> stmtVisitor) throws RuntimeError {
        return stmtVisitor.visitBlockStatement(this);
    }
}