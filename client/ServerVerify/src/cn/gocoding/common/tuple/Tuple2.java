package cn.gocoding.common.tuple;

import java.util.Optional;

/**
 * 两个元组值
 * Created by liuke on 16/3/6.
 */
public class Tuple2<A, B> {

    public Tuple2(A a, B b) {
        this.a = a;
        this.b = b;
    }

    public Optional<A> _1() {
        return Optional.of(a);
    }

    public Optional<B> _2() {
        return Optional.of(b);
    }

    @Override
    public String toString() {
        return "[" + a + ", " + b + "]";
    }

    private final A a;
    private final B b;
}

