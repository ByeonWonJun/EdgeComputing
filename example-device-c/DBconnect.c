//Just add text in database
#include <stdio.h>
#include <stdlib.h>

#include <mysql/mysql.h>

void main(){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *server = "localhost";
	char *usr = "root";
	char *password = "p/w";
	char *database = "new_tracking";


	conn = mysql_init(NULL);
	
	if(conn == NULL){
		printf("no");
		exit(1);
	}
	
	if(mysql_real_connect(conn, server, usr, password, database,0,NULL,0) == NULL){
		printf("error");
		exit(1);
	}
	printf("connect \n");
	
	if(mysql_query(conn, "INSERT INTO new_tracking.test (name,number) VALUES ('33','33')")){
		printf("error 2 : %s\n", mysql_error(conn));
		exit(1);
	}
	


	mysql_close(conn);
}
