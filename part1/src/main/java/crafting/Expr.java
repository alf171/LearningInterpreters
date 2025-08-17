package crafting;

/**
 * This component is kept functional since no one really owns Expr.
 * This more so serves as a bag of data benefitting from a functional impl.
 */
abstract class Expr {
    interface Visitor<R> {
        R visitBinaryExpr(Binary expr);

        // R visitCallExpr(Call expr);
        // R visitGetExpr(Get expr);
        R visitGroupingExpr(Grouping expr);

        R visitLiteralExpr(Literal expr);

        // R visitLogicalExpr(Logical expr);
        // R visitSetExpr(Set expr);
        // R visitSuperExpr(Super expr);
        // R visitThisExpr(This expr);
        R visitUnaryExpr(Unary expr);

        R visitVariableExpr(Variable variable);

        R visitAssignExpr(Assign expr);
        // R VariableExpr(Variable expr);
    }

    abstract <R> R accept(Visitor<R> visitor);

    static class Binary extends Expr {
        public final Expr left;
        public final Token operator;
        public final Expr right;

        public Binary(Expr left, Token operator, Expr right) {
            this.left = left;
            this.operator = operator;
            this.right = right;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitBinaryExpr(this);
        }
    }

    // static class Call extends Expr {
    // public final Expr callee;
    // public final Token paren;
    // public final List<Expr> arguments;
    //
    // public Call(Expr callee, Token paren, List<Expr> arguments) {
    // this.callee = callee;
    // this.paren = paren;
    // this.arguments = arguments;
    // }
    //
    // @Override
    // <R> R accept(Visitor<R> visitor) {
    // return visitor.visitCallExpr(this);
    // }
    // }

    static class Grouping extends Expr {
        public final Expr expression;

        public Grouping(Expr expression) {
            this.expression = expression;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitGroupingExpr(this);
        }
    }

    static class Literal extends Expr {
        public final Object value;

        public Literal(Object value) {
            this.value = value;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitLiteralExpr(this);
        }
    }

    static class Unary extends Expr {
        public final Token operator;
        public final Expr right;

        public Unary(Token operator, Expr right) {
            this.operator = operator;
            this.right = right;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitUnaryExpr(this);
        }
    }

    static class Variable extends Expr {
        public final Token name;

        public Variable(Token name) {
            this.name = name;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitVariableExpr(this);
        }
    }

    static class Assign extends Expr {
        public final Token name;
        public final Expr value;

        public Assign(Token name, Expr value) {
            this.name = name;
            this.value = value;
        }

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitAssignExpr(this);
        }
    }
}
