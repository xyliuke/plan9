package cn.gocoding.common;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class FileUtils {

	public static void copyAssetDirToFiles(Context context, String luaPath, String dirname)
			throws IOException {
		File dir = new File(dirname);
		if (!dir.exists()) {
			dir.mkdirs();
		}

		AssetManager assetManager = context.getAssets();
		String[] children = assetManager.list(luaPath);
		for (String child : children) {
			String ch = luaPath + "/" + child;
			String[] grandChildren = assetManager.list(ch);
			String dest = dirname + "/" + child;
			if (0 == grandChildren.length) {
				copyAssetFileToFiles(context, ch, dest);
			}
			else
				copyAssetDirToFiles(context, ch, dest);
		}
	}

	/**
	 * 复制文件或者文件夹
	 * @param srcFile 源文件路径
	 * @param destFile 目标路径
	 */
	public static void copy(File srcFile, File destFile) {
		if (srcFile.isDirectory()) {
			File[] files = srcFile.listFiles();
			for (File f : files){
				if (f.isFile()){
					copyFile(f, new File(destFile, f.getName()));
				}else if (f.isDirectory()){
					File d = new File(destFile, f.getName());
					if (!d.exists()){
						d.mkdirs();
					}
					copy(f, d);
				}
			}
		}else if (srcFile.isFile()) {
			copyFile(srcFile, destFile);
		}
	}

	private static void copyFile(File src, File dest){
		if (src.isFile()){
			if (dest.exists() && dest.isFile() && dest.length() == src.length()){
				return;
			}
			InputStream input = null;
			OutputStream output = null;
			try {
				input = new FileInputStream(src);
				output = new FileOutputStream(dest);
				byte[] buf = new byte[1024];
				int bytesRead;
				while ((bytesRead = input.read(buf)) > 0) {
					output.write(buf, 0, bytesRead);
				}
			}catch (IOException e){
				e.printStackTrace();
			} finally {
				try {
					if (input != null){
						input.close();
					}
					if (output != null) {
						output.close();
					}
				}catch (Exception e){

				}
			}
		}
	}



	public static void copyAssetFileToFiles(Context context, String filename, String dest)
			throws IOException {
		InputStream is = context.getAssets().open(filename);
		byte[] buffer = new byte[is.available()];
		is.read(buffer);
		is.close();
		
		File of = new File(dest);
		of.createNewFile();
		FileOutputStream os = new FileOutputStream(of);
		os.write(buffer);
		os.close();
	}
}