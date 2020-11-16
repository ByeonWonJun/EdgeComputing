#include <stdio.h>
#include <stdlib.h>

#include <mysql/mysql.h>

void main(){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *usr = "root";
	char *password = "78590q";
	char *database = "new_tracking"

	if(conn=mysql_init((MYSQL*)!=NULL)){
		fprintf("init fail\n");
		return 1;
	}

	fprintf("mysql_init sucsess.\n");
	
	if(mysql_real_connect(conn,server,usr,password,NULL,3306,NULL,0)){
		fprintf("connect error.\n");
		return 1;
	}

	fprintf("mysql_real_connect sucsess.\n");

	if(mysql_select_db(conn,database)!=0){
		mysql_close(conn);
		fprintf("select_db fail.\n");
		return 1;
	}
	fprintf("select db sucsess.\n");

	if(mysql_query(conn,"select* from new_tracking.new_table")){
		fprintf("query fail\n");
		return 1;
	}

	fprintf("query fail\n");

	res = mysql_store_result(conn);
	fprintf("res sucsess\n");

	mysql_close(conn);
}
