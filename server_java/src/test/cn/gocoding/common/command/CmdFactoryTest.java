package test.cn.gocoding.common.command; 

import cn.gocoding.common.command.CmdCallback;
import cn.gocoding.common.command.CmdFactory;
import cn.gocoding.common.command.CmdFunction;
import com.alibaba.fastjson.JSONObject;
import org.junit.Test;
import org.junit.Before; 
import org.junit.After; 

/** 
* CmdFactory Tester. 
* 
* @author <Authors name> 
* @since <pre>三月 12, 2016</pre> 
* @version 1.0 
*/ 
public class CmdFactoryTest { 

@Before
public void before() throws Exception { 
} 

@After
public void after() throws Exception { 
} 

/** 
* 
* Method: getInstance() 
* 
*/ 
@Test
public void testGetInstance() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: registerCmd(String cmd, CmdFunction function) 
* 
*/ 
@Test
public void testRegisterCmd() throws Exception { 
//TODO: Test goes here...
    CmdFactory.registerCmd("test", new CmdFunction() {
        @Override
        public void run(JSONObject param) {
            System.out.println("test function run");
            CmdFactory.execute("test1", param, new CmdCallback() {
                @Override
                public void callback(JSONObject result) {
//                    CmdFactory.callback(result, 10, "this is a test");
                    JSONObject data = new JSONObject();
                    data.put("a", "b");
                    CmdFactory.callback(result, data);
//                    CmdFactory.callback(result);
                }
            });
        }
    });

    CmdFactory.registerCmd("test1", new CmdFunction() {
        @Override
        public void run(JSONObject param) {
            System.out.println("test1 function run");
            CmdFactory.callback(param);
        }
    });

    JSONObject param = new JSONObject();
    JSONObject aux = new JSONObject();
    aux.put("to", "test");

    param.put("aux", aux);

    CmdFactory.execute(param, new CmdCallback() {
        @Override
        public void callback(JSONObject result) {
            System.out.println("callback from test function " + result);
        }
    });
} 

/** 
* 
* Method: execute(String cmd, JSONObject param) 
* 
*/ 
@Test
public void testExecuteForCmdParam() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: execute(JSONObject param, CmdCallback callback) 
* 
*/ 
@Test
public void testExecuteForParamCallback() throws Exception { 
//TODO: Test goes here... 
} 

/** 
* 
* Method: callback(JSONObject result) 
* 
*/ 
@Test
public void testCallback() throws Exception { 
//TODO: Test goes here... 
} 


} 
