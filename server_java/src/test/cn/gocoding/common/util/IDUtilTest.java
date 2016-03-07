package test.cn.gocoding.common.util; 

import cn.gocoding.common.util.IDUtil;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* IDUtil Tester. 
* 
* @author <Authors name> 
* @since <pre>三月 3, 2016</pre> 
* @version 1.0 
*/ 
public class IDUtilTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: createUniqueID() 
* 
*/ 
@Test
public void testCreateUniqueID() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: getRandom(int max) 
* 
*/ 
@Test
public void testGetRandom() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: getID(byte header) 
* 
*/ 
@Test
public void testGetID() throws Exception {
    byte h = 0x0F;
    for (int i = 0; i < 1000; i ++)
        System.out.println(IDUtil.getID(h));
}

/**
* 
* Method: getAlph() 
* 
*/ 
@Test
public void testGetAlph() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = IDUtil.getClass().getMethod("getAlph"); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

} 
