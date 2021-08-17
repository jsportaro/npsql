using NpSql;
using System;
using System.Diagnostics;
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
            var commandSplitChar = new char []{ ' ' };
            var exit             = false;
            var hasConnection    = false;
            var connectionString = string.Empty;
            var defaultPort      = 15151;
            var prompt           =  "npsql > ";
            var qprompt          = "      > ";

            NpSqlConnection connection = null;

            while (!exit)
            {
                Console.Write(prompt);

                var sql          = Console.ReadLine();
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
                        if (connection != null)
                        {
                            connection.Dispose();
                            connection = null;
                        }
                        break;
                    case "query":
                        if (!hasConnection)
                        {
                            Console.WriteLine("You need to connect first");
                        }
                        else
                        {
                            var sb = new StringBuilder();
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
                    var sw              = new Stopwatch();

                    sw.Start();
                    using (var reader = (NpSqlDataReader)command.ExecuteReader())
                    {
                        var rowLength = WriteColumnHeader(reader);
                        rowLength     = WriteVerticalSeperator(reader, rowLength);

                        while (reader.Read())
                        {
                            WriteRow(reader);
                        }

                        sw.Stop();
                        rowLength = WriteVerticalSeperator(reader, rowLength);
                        Console.WriteLine($"Query ran in ({sw.Elapsed})");
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
            var i            = 0;
            var leadingSpace = 0;
            var s            = string.Empty;

            Console.Write("  |");

            foreach (NpSqlColumnDefinition column in reader.GetColumnSchema())
            {
                var columnLength = column.Name.Length + 8;

                switch (column.Type)
                {
                    case NpSql.Nqp.NqpTypes.Char:
                        s = reader.GetString(i).Trim();
                        break;
                    case NpSql.Nqp.NqpTypes.Int:
                        s = reader.GetInt32(i).ToString();
                        break;
                }

                if (s.Length > columnLength)
                {
                    s = s.Substring(0, columnLength - 5);
                    s += "...";
                }

                leadingSpace = columnLength - (s.Length + 1);

                if (leadingSpace < 0)
                    leadingSpace = 0;

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

            if (reader.GetColumnSchema().Count() == 0)
            {
                return 0;
            }

            rowLength = WriteVerticalSeperator(reader, rowLength);
            Console.Write("  |");
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
            if (rowLength == 0)
            {
                return rowLength;
            }
            Console.Write("  +");
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
