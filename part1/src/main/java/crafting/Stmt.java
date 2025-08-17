package crafting;

abstract class Stmt {

    interface Visitor<R> {
        R visitExpressionStmt(Expression stmt);

        R visitPrintStmt(Print stmt);
    }

    public abstract <R> R accept(Visitor<R> visitor);

    static class Expression extends Stmt {
        public final Expr expression;

        public Expression(Expr expr) {
            this.expression = expr;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitExpressionStmt(this);
        }
    }

    static class Print extends Stmt {
        // public final Token keyword;
        public final Expr value;

        public Print(
                // Token keyword,
                Expr value) {
            // this.keyword = keyword;
            this.value = value;
        }

        @Override
        public <R> R accept(Visitor<R> visitor) {
            return visitor.visitPrintStmt(this);
        }
    }
}
