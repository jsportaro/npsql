using System;

namespace NpSql_Cli
{
    class Program
    {
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
            throw new NotImplementedException();
        }
    }
}
