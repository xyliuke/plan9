package cn.gocoding.common.error;

/**
 * 错误类型处理类
 * Created by liuke on 16/3/5.
 */
public enum ErrorCode {

    NOT_ERROR(0, "this is no error"),
    PROTOLOC_FORMAT_ILLEGAL_ERROR(1, "the protocol is not legal format"),
    UNKNOW_ERROR(2, "unknow error");









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
}
