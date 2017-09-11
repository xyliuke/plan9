package cn.gocoding.common.tuple;

import java.util.Optional;

/**
 * 四个元素的元组
 * Created by liuke on 16/3/6.
 */
public class Tuple4<A, B, C, D> {
    public Tuple4(A a, B b, C c, D d) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
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

    @Override
    public String toString() {
        return "[" + a + ", " + b + ", " + c +  ", " + d + "]";
    }

    private final A a;
    private final B b;
    private final C c;
    private final D d;
}