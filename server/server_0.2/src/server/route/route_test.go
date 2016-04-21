package route

import "testing"

func Test_route(t *testing.T) {
	rs := new(RouteServerConfig)
	rs.Port = 9096
	NewRouteServer(rs).Run()
}
