using NpSql;
using System;
using System.IO;
using System.Linq;
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

                    using (var reader = (NpSqlDataReader)command.ExecuteReader())
                    {
                        int rowLength = WriteColumnHeader(reader);
                        rowLength = WriteVerticalSeperator(reader, rowLength);

                        while (reader.Read())
                        {
                            WriteRow(reader);
                        }

                        rowLength = WriteVerticalSeperator(reader, rowLength);

                    }
                }
            }
            catch (NpSqlException e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }

        private static void WriteRow(NpSqlDataReader reader)
        {
            Console.Write('|');

            var i = 0;
            var leadingSpace = 0;
            var s = string.Empty;
            foreach (NpSqlColumnDefinition column in reader.GetColumnSchema())
            {
                var columnLength = column.Name.Length + 8;

                switch (column.Type)
                {

                    case NpSql.Nqp.NqpTypes.Char:
                        s = reader.GetString(0).Trim();
                        break;
                    case NpSql.Nqp.NqpTypes.Int:
                        s = reader.GetInt32(1).ToString();
                        break;
                }

                if (s.Length > columnLength)
                {
                    s = s.Substring(0, s.Length - 5);
                    s += "...";
                }

                leadingSpace = columnLength - (s.Length + 1);

                Console.Write(new string(' ', leadingSpace));
                Console.Write(s);
                Console.Write(' ');
                Console.Write("|");
                i++;
            }

            Console.WriteLine();
        }

        private static int WriteColumnHeader(NpSqlDataReader reader)
        {
            var rowLength = 2;

            rowLength = WriteVerticalSeperator(reader, rowLength);
            Console.Write("|");
            foreach (NpSqlColumnDefinition column in reader.GetColumnSchema())
            {

                var minLength = column.Name.Length + 8; // 4 whitespace before and aftername
                                                        //Write beginning whitespace
                Console.Write(new string(' ', 4));
                //Write column Name
                Console.Write(column.Name);
                //write trailing whitespace
                Console.Write(new string(' ', 4));
                Console.Write("|");

                rowLength += minLength + 1;
            }


            Console.WriteLine();
            return rowLength;
        }

        private static int WriteVerticalSeperator(NpSqlDataReader reader, int rowLength)
        {
            Console.Write("+");
            foreach (NpSqlColumnDefinition column in reader.GetColumnSchema())
            {

                var minLength = column.Name.Length + 8; // 4 whitespace before and aftername
                Console.Write(new string('-', minLength));
                Console.Write("+");

                rowLength += minLength + 1;
            }

            Console.WriteLine();
            return rowLength;
        }
    }
}
