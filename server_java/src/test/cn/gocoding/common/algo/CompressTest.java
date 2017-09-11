package test.cn.gocoding.common.algo; 

import cn.gocoding.common.algo.Compress;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* Compress Tester. 
* 
* @author <Authors name> 
* @since <pre>三月 28, 2016</pre> 
* @version 1.0 
*/ 
public class CompressTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: compress() 
* 
*/ 
@Test
public void testCompress() throws Exception { 
//TODO: Test goes here...
    byte[] c = Compress.compress("hello world".getBytes());
    byte[] dc = Compress.decompress(c);
    System.out.println(dc);

    byte[] nc = new byte[] {(byte) 120, (byte)156, (byte)202, 72, (byte)205, (byte)201, (byte)201, 87, 40, (byte)207, 47, (byte)202, 73, 1, 4, 0, 0, (byte)255, (byte)255, 26, 11, 4, 93};
    byte[] ndc = Compress.decompress(nc);
    System.out.println(new String(ndc));
} 


} 
