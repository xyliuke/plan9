package network

const (
	NETWORK_ERROR_ILLEGAL_PROTOCOL = 0
	NETWORK_ERROR_NOT_STRING_DATA = 1
)

type NetworkError struct {
	ErrorType int
	ErrorString string
}

//var N0 *NetworkError = &NetworkError{ErrorType:NETWORK_ERROR_ILLEGAL_PROTOCOL, ErrorString:"the data illeagl protocol format"}
//var N1 *NetworkError = &NetworkError{ErrorType:NETWORK_ERROR_NOT_STRING_DATA, ErrorString:"the data is not string"}

var network_error_map map[int]*NetworkError = map[int]*NetworkError{
	NETWORK_ERROR_ILLEGAL_PROTOCOL : &NetworkError{ErrorType:NETWORK_ERROR_ILLEGAL_PROTOCOL, ErrorString:"the data illeagl protocol format"},
	NETWORK_ERROR_NOT_STRING_DATA : &NetworkError{ErrorType:NETWORK_ERROR_NOT_STRING_DATA, ErrorString:"the data is not string"},
}

//func (err *NetworkError) setErrorStringByType(t int)  {
//	err.ErrorType = t
//	switch t {
//	case NETWORK_ERROR_ILLEGAL_PROTOCOL:
//		err.ErrorString = "the data illeagl protocol format"
//	case NETWORK_ERROR_NOT_STRING_DATA:
//		err.ErrorString = "the data is not string"
//	}
//}

func (err NetworkError) Error() string {
	return err.ErrorString
}

func CreateNetworkError(errorType int) *NetworkError {
	e := network_error_map[errorType]
	return e
}
