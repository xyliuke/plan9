package cn.gocoding.common.error;

/**
 * 错误类型处理类
 * Created by liuke on 16/3/5.
 */
public enum ErrorCode {

    NOT_ERROR(0, "this is no error"),

    //网络协议相关错误  1-255
    PROTOLOC_FORMAT_ILLEGAL_ERROR(0x1, "the protocol is not legal format"),
    PROTOLOC_ERROR_MAX(0xFF, "protocol max error num"),

    //普通错误  256-511
    PARAMTER_ERROR(0x100, "parameter error"),
    PARAMTER_ERROR_MAX(0x1FF, "parameter error"),

    UNKNOW_ERROR(0xFFFFFFFF, "unknow error");










    private ErrorCode(int errorCode, String errorMsg) {
        this.errorCode = errorCode;
        this.errorMsg = errorMsg;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public String getErrorMsg() {
        return errorMsg;
    }

    public static boolean isError(ErrorCode e) {
        return e.getErrorCode() != NOT_ERROR.getErrorCode();
    }



    private int errorCode;
    private String errorMsg;

    @Override
    public String toString() {
        return "error code : " + getErrorCode() + ", error msg : " + getErrorMsg();
    }
}
