package test.cn.gocoding.main; 

import cn.gocoding.main.ConfigFile;
import com.alibaba.fastjson.JSONObject;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* ConfigFile Tester. 
* 
* @author <Authors name> 
* @since <pre>三月 15, 2016</pre> 
* @version 1.0 
*/ 
public class ConfigFileTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: readConfigFile(String file) 
* 
*/ 
@Test
public void testReadConfigFile() throws Exception { 
//TODO: Test goes here...
    JSONObject ret = ConfigFile.readConfigFile("/Users/liuke/Desktop/config.json");
    if (ret != null) {
        System.out.println(ret.getString("a"));
    }
} 


/** 
* 
* Method: getStringFromConfigFile(String file) 
* 
*/ 
@Test
public void testGetStringFromConfigFile() throws Exception { 
//TODO: Test goes here... 
/* 
try { 
   Method method = ConfigFile.getClass().getMethod("getStringFromConfigFile", String.class); 
   method.setAccessible(true); 
   method.invoke(<Object>, <Parameters>); 
} catch(NoSuchMethodException e) { 
} catch(IllegalAccessException e) { 
} catch(InvocationTargetException e) { 
} 
*/ 
} 

} 
