package cn.gocoding.common.tuple;

import java.util.Optional;

/**
 * 七个元素的元组
 * Created by liuke on 16/3/6.
 */
public class Tuple7<A, B, C, D, E, F, G> {
    public Tuple7(A a, B b, C c, D d, E e, F f, G g) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
        this.e = e;
        this.f = f;
        this.g = g;
    }

    public Optional<A> _1() {
        return Optional.of(a);
    }

    public Optional<B> _2() {
        return Optional.of(b);
    }

    public Optional<C> _3() {
        return Optional.of(c);
    }

    public Optional<D> _4() {
        return Optional.of(d);
    }

    public Optional<E> _5() {
        return Optional.of(e);
    }

    public Optional<F> _6() {
        return Optional.of(f);
    }

    public Optional<G> _7() {
        return Optional.of(g);
    }

    @Override
    public String toString() {
        return "[" + a + ", " + b + ", " + c + ", " + d + ", " + e + ", " + f + ", " + g + "]";
    }

    private final A a;
    private final B b;
    private final C c;
    private final D d;
    private final E e;
    private final F f;
    private final G g;
}
