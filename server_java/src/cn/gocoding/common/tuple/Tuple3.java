package cn.gocoding.common.tuple;

import java.util.Optional;

/**
 * 三个值的元组
 * Created by liuke on 16/3/6.
 */
public class Tuple3<A, B, C> {
    public Tuple3(A a, B b, C c) {
        this.a = a;
        this.b = b;
        this.c = c;
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

    @Override
    public String toString() {
        return "[" + a + ", " + b + ", " + c + "]";
    }

    private final A a;
    private final B b;
    private final C c;
}
