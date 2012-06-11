#include <libpq-fe.h>
#include <iostream>
#include <time.h>

class Post {
    std::string title;
    std::string body;
    std::string uuid;
    time_t created_datetime;

public:

    Post(std::string title, std::string body, std::string uuid,time_t created_datetime = time(NULL))
    :title(title), body(body), uuid(uuid), created_datetime(created_datetime)
    {
    }
    ~Post(){
    }
    std::string getInsertSql(){
        char buff[20];
        strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&created_datetime));
        std::ostringstream stringStream;
        stringStream << "INSERT INTO blog_posts VALUES ('" << title << "', '" << body << "','" << uuid << "','"<< buff << "');";
        return stringStream.str();
    }
};

static void exit_nicely(PGconn *conn)
{
  PQfinish(conn);
}

bool PQexecStatusCheck(PGconn *conn, const char* cmd, const char* message){
  PGresult   *res;
  res = PQexec(conn, cmd);
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "%s %s",message, PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }
  PQclear(res);
  return 1;
}

int createPost(Post& p){
  const char *conninfo = "dbname = cppblog";
  PGconn     *conn;
  PGresult   *res;
  conn = PQconnectdb(conninfo);
  if (PQstatus(conn) != CONNECTION_OK)
    {
      fprintf(stderr, "Connection to database failed: %s",
              PQerrorMessage(conn));
      exit_nicely(conn);
    }
  if(PQexecStatusCheck(conn, "BEGIN","Begin failed")){
      if(PQexecStatusCheck(conn, p.getInsertSql().c_str(),"insert failed")){
          PQexecStatusCheck(conn,"END","error finishingup");
      }
  }
  PQfinish(conn);
}


int test_connection()
{
  std::cout << "test db1" <<std::endl;
  const char *conninfo = "dbname = cppblog";
  PGconn     *conn;
  PGresult   *res;
  int                     nFields;
  int                     i,
    j;
  /* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  std::cout << "test db2" <<std::endl;
  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK)
    {
      fprintf(stderr, "Connection to database failed: %s",
              PQerrorMessage(conn));
      exit_nicely(conn);
    }

  /*
   * Our test case here involves using a cursor, for which we must be
   * inside a transaction block.  We could do the whole thing with a
   * single PQexec() of "select * from pg_database", but that's too
   * trivial to make a good example.
   */

  /* Start a transaction block */
  std::cout << "test db3" <<std::endl;
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
      PQclear(res);
      exit_nicely(conn);
    }

  /*
   * Should PQclear PGresult whenever it is no longer needed to avoid
   * memory leaks
   */
  PQclear(res);

  /*
   * Fetch rows from pg_database, the system catalog of databases
   */
  std::cout << "test db4" <<std::endl;
  res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
      PQclear(res);
      exit_nicely(conn);
    }
  PQclear(res);/* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK)
    {
      fprintf(stderr, "Connection to database failed: %s",
              PQerrorMessage(conn));
      exit_nicely(conn);
    }

  /*
   * Our test case here involves using a cursor, for which we must be
   * inside a transaction block.  We could do the whole thing with a
   * single PQexec() of "select * from pg_database", but that's too
   * trivial to make a good example.
   */

  /* Start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
      PQclear(res);
      exit_nicely(conn);
    }

  /*
   * Should PQclear PGresult whenever it is no longer needed to avoid
   * memory leaks
   */
  PQclear(res);

  /*
   * Fetch rows from pg_database, the system catalog of databases
   */
  res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
      fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
      PQclear(res);
      exit_nicely(conn);
    }
  res = PQexec(conn, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
      fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
      PQclear(res);
      exit_nicely(conn);
    }

  /* first, print out the attribute names */
  nFields = PQnfields(res);
  for (i = 0; i < nFields; i++)
    printf("%-15s", PQfname(res, i));
  printf("\n\n");

  /* next, print out the rows */
  for (i = 0; i < PQntuples(res); i++)
    {
      for (j = 0; j < nFields; j++)
        printf("%-15s", PQgetvalue(res, i, j));
      printf("\n");
    }

  PQclear(res);

  /* close the portal ... we don't bother to check for errors ... */
  res = PQexec(conn, "CLOSE myportal");
  PQclear(res);

  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);
  /* close the connection to the database and cleanup */
  PQfinish(conn);
}
