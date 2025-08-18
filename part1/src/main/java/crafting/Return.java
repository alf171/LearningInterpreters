package crafting;

public class Return extends RuntimeException {
    public final Object value;

    // disable some stuff from stack traces
    public Return(Object value) {
        super(null, null, false, false);
        this.value = value;
    }
}
