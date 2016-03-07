package cn.gocoding.main;

import cn.gocoding.common.error.ErrorCode;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;



/**
 * 入口函数
 * Created by liuke on 16/3/3.
 */
public class Main {
    private static Logger logger = LogManager.getLogger(Main.class);
    public static void main(String[] args) {

        byte b = (byte)0xFF;
        int a = (int)b;
        logger.info("a : {}", Byte.toUnsignedInt(b));

//        try {
//            BaseServer baseServer = new BaseServer(8081, new ServerOperationManager());
//            baseServer.listen();
//        } catch (IOException e) {
//            logger.info(e.getMessage());
//        }
//        byte[] buf = new byte[10];
//        logger.info("buf size {}", buf.length);
//        ErrorCode e1 = ErrorCode.NOT_ERROR;

//        logger.info("error num {}, msg {}, {}", e1.getErrorCode(), e1.getErrorMsg(), ErrorCode.isError(e1));
//        tuple<Integer, String, Double> t = tuple.of(1, "abc", 1.2);
//        logger.info("tuple 1 {}, 2 {} , 3 {}, string : {}", t._1().get().getClass(), t._2(), t._3(), t);
//        T<Integer, String, Float> t = new T<Integer, String, Float>(1, "", 1.2);
//        T<Integer, String, Float> t1 = new T<Integer, String, Float>(1, "");
//        T<Integer> t1;

//        Tuple2<Integer, String> t = tuple.of(1, "");

//        List<String> list = new ArrayList<Integer>();
//        logger.info("tuple 1 {}, 2 {} , string : {}", t._1(), t._2(), t);
    }
}
