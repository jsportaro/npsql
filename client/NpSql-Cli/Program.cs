using NpSql;
using System;
using System.Text;

namespace NpSql_Cli
{
    class Program
    {
        static NpSqlConnection connection = default(NpSqlConnection);
        static void Main(string[] args)
        {
            //Host=localhost;Port=15151
            var commandSplitChar = new char []{ ' ' };
            var exit = false;
            var hasConnection = false;
            var connectionString = string.Empty;
            var defaultPort = 15151;
            NpSqlConnection connection = null;

            var prompt =  "npsql > ";
            var qprompt = "      > ";

            while (!exit)
            {
                Console.Write(prompt);

                var sql = Console.ReadLine();
                var commandParts = sql.Split(commandSplitChar);

                switch(commandParts[0].ToLower())
                {
                    case "quit":
                        exit = true;
                        break;
                    case "connect":
                        if (commandParts.Length == 3)
                        {
                            connectionString = $"Host={commandParts[1]};Port={commandParts[2]}";
                        }
                        else if (commandParts.Length == 2)
                        {
                            connectionString = $"Host={commandParts[1]};Port={defaultPort}";
                        }
                        else
                        {
                            Console.WriteLine("Wasn't expecting that.");
                        }

                        connection = new NpSqlConnection(connectionString);
                        try
                        {
                            connection.Open();
                            hasConnection = true;
                        }
                        catch(NpSqlException e)
                        {
                            Console.WriteLine(e.Message);
                        }
                        break;
                    case "disconnect":
                        connection.Dispose();
                        connection = null;
                        break;
                    case "query":
                        if (!hasConnection)
                        {
                            Console.WriteLine("You need to connect first");
                        }
                        else
                        {
                            StringBuilder sb = new StringBuilder();
                            Console.Write(qprompt);
                            var entry = Console.ReadLine();

                            while (entry != "!s")
                            {
                                sb.Append(entry);

                                Console.Write(qprompt);
                                entry = Console.ReadLine();
                            }

                            IssueQuery(sb.ToString(), connection);
                        }
                        break;
                    default:
                        if (!hasConnection)
                        {
                            Console.WriteLine("You need to connect first");
                        }
                        else
                        {
                            IssueQuery(sql, connection);
                        }
                        // Assume its sql and see what a happens
                        break;
                }
            }


            if (connection != null)
            {
                connection.Dispose();
                connection = null;
            }
        }

        private static void IssueQuery(string sql, NpSqlConnection conn)
        {
            try
            {
                using (var command = new NpSqlCommand(conn))
                {
                    command.CommandText = sql;

                    using (var reader = command.ExecuteReader())
                    {

                    }
                }
            }
            catch (NpSqlException e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }
    }
}
