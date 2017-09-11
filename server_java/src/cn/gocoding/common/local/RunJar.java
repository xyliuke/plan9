package cn.gocoding.common.local;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * 运行本地的可执行jar文件
 * Created by liuke on 16/3/18.
 */
public class RunJar {

    public static void run(String jarFile, String[] args) throws IOException{
        if (jarFile == null) {
            return;
        }

        List<String> command = new ArrayList<>();
        command.add("java");
        command.add("-jar");
        command.add(jarFile);
        if (args != null) {
            for (int i = 0; i < args.length; i ++) {
                command.add(args[i]);
            }
        }
        ProcessBuilder processBuilder = new ProcessBuilder(command);
        Process p = processBuilder.start();
    }

}
