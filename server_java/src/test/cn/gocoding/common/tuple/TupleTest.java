package test.cn.gocoding.common.tuple;

import cn.gocoding.common.tuple.*;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* tuple Tester.
* 
* @author <Authors name> 
* @since <pre>三月 6, 2016</pre> 
* @version 1.0 
*/ 
public class TupleTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: of(A a, B b) 
* 
*/ 
@Test
public void testOfForAB() throws Exception {
    Tuple2<Integer, String> t = Tuple.of(1, "hello");
    System.out.println(t._1().get());
    System.out.println(t._2().get());
    System.out.println(t);
} 

/** 
* 
* Method: of(A a, B b, C c) 
* 
*/ 
@Test
public void testOfForABC() throws Exception {
    Tuple3<Integer, String, Double> t = Tuple.of(1, "hello", 1.2);
    System.out.println(t._1().get());
    System.out.println(t._2().get());
    System.out.println(t._3().get());
    System.out.println(t);
} 

/** 
* 
* Method: of(A a, B b, C c, D d) 
* 
*/ 
@Test
public void testOfForABCD() throws Exception {
    Tuple4<Integer, String, Double, Boolean> t = Tuple.of(1, "hello", 1.2, true);
    System.out.println(t._1().get());
    System.out.println(t._2().get());
    System.out.println(t._3().get());
    System.out.println(t._4().get());
    System.out.println(t);
} 

/** 
* 
* Method: of(A a, B b, C c, D d, E e) 
* 
*/ 
@Test
public void testOfForABCDE() throws Exception {
    Tuple5<Integer, String, Double, Boolean, Float> t = Tuple.of(1, "hello", 1.2, true, 4.5f);
    System.out.println(t._1().get());
    System.out.println(t._2().get());
    System.out.println(t._3().get());
    System.out.println(t._4().get());
    System.out.println(t._5().get());
    System.out.println(t);
} 

/** 
* 
* Method: of(A a, B b, C c, D d, E e, F f) 
* 
*/ 
@Test
public void testOfForABCDEF() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: of(A a, B b, C c, D d, E e, F f, G g) 
* 
*/ 
@Test
public void testOfForABCDEFG() throws Exception { 
//TODO: Test goes here... 
} 


} 
