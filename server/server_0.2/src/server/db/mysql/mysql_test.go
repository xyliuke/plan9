package mysql

import (
	"testing"
	"database/sql"
	"common/log"
)

func Test_Mysql(t *testing.T) {
	db, err := sql.Open("mysql", "root:123456@/plan9")
	defer db.Close()
	if err == nil {
		log.I_COM("connect to db success")

		result, result_err := db.Exec("insert into user(id, mobile, name) values(11,'11', '11')")
		if result_err == nil {
			log.I_COM(result.RowsAffected())
		}

		rows, row_err := db.Query("select * from user")
		defer rows.Close()

		if row_err == nil {
			for rows.Next() {
				var id int
				var mobile1 string
				var name string
				rows.Scan(&id, &mobile1, &name)
				log.I_COM(id, mobile1, name)
			}
		}
	}
}

