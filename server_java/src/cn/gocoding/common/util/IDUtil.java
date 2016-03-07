package cn.gocoding.common.util;

import java.util.Random;

/**
 * 生成分布式系统所需要的唯一ID
 * Created by liuke on 16/3/3.
 */
public class IDUtil {

    /**
     * 返回一个唯一的32个字符的ID,这个ID可以在不同的机器上生成,也能保证不重复.
     * @return 返回唯一的ID
     */
    public static String createUniqueID() {
        long time = System.currentTimeMillis();
        StringBuffer stringBuffer = new StringBuffer();
        stringBuffer.append("ID-");
        stringBuffer.append(String.format("%013d", time));
        stringBuffer.append(String.format("%06d", System.nanoTime() % 1000000));
        stringBuffer.append("-");
        stringBuffer.append(String.format("%06d", getRandom(999999)));
        stringBuffer.append("-");
        stringBuffer.append(getAlph());
        return stringBuffer.toString();
    }

    /**
     * 获取一个整数随机数,最小值为0
     * @param max 随机数的最大值
     * @return 返回随机数
     */
    public static int getRandom(int max) {
        Random random = new Random();
        return random.nextInt(max);
    }


    public synchronized static int getID(byte header) {
        int h = (int)header;
        h = h << 24;
        if (id_index > 0xFFFFFF) {
            id_index = 0;
        } else {
            id_index ++;
        }
        h = h + id_index;
        return h;
    }


    private synchronized static char[] getAlph() {
        if (index2 >= ALPH.length) {
            index2 = 0;
            index1 ++;
        }
        if (index1 >= ALPH.length) index1 = 0;

        char[] ret = {ALPH[index1], ALPH[index2]};
        index2 ++;
        return ret;
    }

    private static final char[] ALPH = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    private static int index1 = 0, index2 = 0;
    private static int id_index = 0;
}


