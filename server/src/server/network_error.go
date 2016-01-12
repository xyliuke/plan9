package server

const (
	NETWORK_ERROR_ILLEGAL_PROTOCOL = 0
	NETWORK_ERROR_NOT_STRING_DATA = 1
)

type NetworkError struct {
	ErrorType int
	ErrorString string
}

func (err *NetworkError) setErrorStringByType(t int)  {
	err.ErrorType = t
	switch t {
	case NETWORK_ERROR_ILLEGAL_PROTOCOL:
		err.ErrorString = "the data illeagl protocol format"
	case NETWORK_ERROR_NOT_STRING_DATA:
		err.ErrorString = "the data is not string"
	}
}

func (err NetworkError) Error() string {
	return err.ErrorString
}

func CreateNetworkError(errorType int) *NetworkError {
	e := new(NetworkError)
	e.setErrorStringByType(errorType)
	return e
}
