using System;
using NpSql;

namespace NpSql_Cli
{
    class Program
    {
        static NpSqlConnection connection = default(NpSqlConnection);
        static void Main(string[] args)
        {
            var commandSplitChar = new char []{ ' ' };
            var exit = false;
            var hasConnection = false;
            
            while (!exit)
            {
                Console.Write("npsql > ");

                var sql = Console.ReadLine();
                var commandParts = sql.Split(commandSplitChar);

                switch(commandParts[0].ToLower())
                {
                    case "quit":
                        exit = true;
                        break;
                    case "connect":
                        var serverName = "localhost";
                        connection = new NpSqlConnection($"Host={serverName};Port=15151");
                        connection.Open();
                        break;
                    default:
                        if (!hasConnection)
                        {
      
                        }
                        else
                        {
                            IssueQuery(sql);
                        }
                        // Assume its sql and see what a happens
                        break;
                }

            }
        }

        private static void IssueQuery(string sql)
        {
            using (var command = new NpSqlCommand(connection))
            {
                command.CommandText = "select * from test_table_name where name = 'heather'";
                var reader = command.ExecuteReader();
            }
        }
    }
}
