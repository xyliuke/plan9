package test.cn.gocoding.server.protocol; 

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.protocol.Protocol;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* Protocol Tester. 
* 
* @author <Authors name> 
* @since <pre>三月 10, 2016</pre> 
* @version 1.0 
*/ 
public class ProtocolTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: isLegalProtocol(byte[] data, int len) 
* 
*/ 
@Test
public void testIsLegalProtocol() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: getProtocolItem(byte[] data, int len) 
* 
*/ 
@Test
public void testGetProtocolItem() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: isPingType(byte type) 
* 
*/ 
@Test
public void testIsPingType() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: isPongType(byte type) 
* 
*/ 
@Test
public void testIsPongType() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: getServerType(byte type) 
* 
*/ 
@Test
public void testGetServerType() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: getDataType(byte type) 
* 
*/ 
@Test
public void testGetDataType() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: removeFirstProtocol(byte[] data, int len) 
* 
*/ 
@Test
public void testRemoveFirstProtocol() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: createPingProtocol(int id, byte version, byte serverType) 
* 
*/ 
@Test
public void testCreatePingProtocol() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: createPongProtocol(int id, byte version) 
* 
*/ 
@Test
public void testCreatePongProtocol() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: createProtocol(int id, byte version, byte serverType, byte dataType, int len, byte[] data) 
* 
*/ 
@Test
public void testCreateProtocol() throws Exception { 
//TODO: Test goes here...
    String str = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789";
    Tuple2<ErrorCode, byte[]> data = Protocol.createProtocol(0xFF00FF00, (byte) 12, Protocol.LOGIN_SERVER_TYPE, Protocol.PING_DATA_TYPE, str.length(), str.getBytes());
    Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item = Protocol.getProtocolItem(data._2().get(), data._2().get().length);
    Protocol.modifyID(item._7().get(), 0x1);
//    byte[] d = item._6().get();
} 


/** 
* 
* Method: getDataLen(byte[] data) 
* 
*/ 
@Test
public void testGetDataLen() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = Protocol.getClass().getMethod("getDataLen", byte[].class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

/** 
* 
* Method: getClientID(byte[] data) 
* 
*/ 
@Test
public void testGetClientID() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = Protocol.getClass().getMethod("getClientID", byte[].class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

/** 
* 
* Method: getVersion(byte[] data) 
* 
*/ 
@Test
public void testGetVersion() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = Protocol.getClass().getMethod("getVersion", byte[].class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

/** 
* 
* Method: getType(byte[] data) 
* 
*/ 
@Test
public void testGetType() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = Protocol.getClass().getMethod("getType", byte[].class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

/** 
* 
* Method: getData(byte[] data) 
* 
*/ 
@Test
public void testGetData() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = Protocol.getClass().getMethod("getData", byte[].class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

} 
