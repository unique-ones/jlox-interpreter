package grupa.tools;

import grupa.Expressions.*;
import grupa.Parser.RuntimeError;
import grupa.Scanner.Token;
import grupa.Scanner.TokenType;

import java.util.List;

public class AstPrinter implements Visitor<String> {


    public static void main(String[] args) throws RuntimeError {
        //Expression expression = new Binary(new Grouping(new Binary(new Literal(1),new Token("-", TokenType.MINUS,null,1),new Literal(2))), new Token("*", TokenType.STAR, null, 1), new Literal(2));
        //Expression expression = new Binary(new Grouping(new Unary(new Token("!", TokenType.BANG, null, 1), new Literal(1))), new Token("*", TokenType.STAR, null, 1), new Literal(2));
        //Expression expression = new Conditional(new Binary(new Literal(1),new Token(">",TokenType.GREATER,null,1),new Literal(2)), new Literal(1), new Literal(2));
        //System.out.println(new AstPrinter().print(expression));
    }

    public String print(Expression expression) throws RuntimeError {
        return expression.accept(this);
    }

    @Override
    public String visitBinaryExpression(Binary expression) throws RuntimeError {
        return parenthesize(expression.getOperator().getLexeme(), expression.getLeft(), expression.getRight());
    }

    @Override
    public String visitGroupingExpression(Grouping expression) throws RuntimeError {
        return parenthesize("grouping", expression.getExpression());
    }

    @Override
    public String visitLiteralExpression(Literal expression) {
        if (expression.getValue() == null) return "nil";
        return expression.getValue().toString();
    }

    @Override
    public String visitUnaryExpression(Unary expression) throws RuntimeError {
        return parenthesize(expression.getOperator().getLexeme(), expression.getRight());
    }

    @Override
    public String visitConditionalExpression(Conditional expression) throws RuntimeError {
        return parenthesize("?:", expression.getCondition(), expression.getTrueBranch(), expression.getFalseBranch());
    }

    private String parenthesize(String name, Expression... exprs) throws RuntimeError {
        StringBuilder builder = new StringBuilder();
        builder.append("(").append(name);
        for (var expr : exprs) {
            builder.append(" ");
            builder.append(expr.accept(this));
        }
        builder.append(")");
        return builder.toString();
    }
}
