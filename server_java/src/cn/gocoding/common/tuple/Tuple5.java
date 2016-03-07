package cn.gocoding.common.tuple;

import java.util.Optional;

/**
 * 一个元素的元组
 * Created by liuke on 16/3/6.
 */
public class Tuple5<A, B, C, D, E> {
    public Tuple5(A a, B b, C c, D d, E e) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
        this.e = e;
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

    @Override
    public String toString() {
        return "[" + a + ", " + b + ", " + c + ", " + d + ", " + e + "]";
    }

    private final A a;
    private final B b;
    private final C c;
    private final D d;
    private final E e;
}

