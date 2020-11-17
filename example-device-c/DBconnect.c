#include <stdio.h>
#include <stdlib.h>

#include <mysql/mysql.h>

void main(){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *usr = "root";
	char *password = "";
	char *database = "new_tracking";

	if(!(conn=mysql_init((MYSQL*)NULL))){
		printf("init fail\n");
		exit(1);
	}

	printf("mysql_init sucsess.\n");
	
	if(mysql_real_connect(conn,server,usr,password,NULL,3306,NULL,0)){
		printf("connect error.\n");
		exit(1);
	}

	printf("mysql_real_connect sucsess.\n");
	
	if(mysql_query(conn,"INSERT INTO new_tracking.test VALUES(1,2)")){
		mysql_close(conn);
		exit(1);	

	}
/*
	if(mysql_select_db(conn,database)!=0){
		mysql_close(conn);
		printf("select_db fail.\n");
		exit(1);
	}
	printf("select db sucsess.\n");
*/
/*
	if(mysql_query(conn,"select* from new_tracking.new_table")){
		printf("query fail\n");
		exit(1);
	}

	printf("query fail\n");
*/
/*
	res = mysql_store_result(conn);
	printf("res sucsess\n");
*/
	mysql_close(conn);
}
