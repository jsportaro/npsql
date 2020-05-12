using System;
using System.Collections.Concurrent;
using Xunit;

namespace NpSql.Tests.Integration
{
    public class ConnectDisconnectTests
    {

        [Fact]
        public void Attempt_To_Connect_Past_Session_Limit()
        {
            for (int loop = 0; loop < 10000; loop++)
            {
                var connection1 = new NpSqlConnection("Host=localhost;Port=15151");

                connection1.Open();

                connection1.Dispose();

                var connections = new ConcurrentStack<NpSqlConnection>();
                var failures = 0;
                for (int i = 0; i < 1; i++)
                {
                    Task.Run(() =>
                    {
                        try
                        {

                            var connection = new NpSqlConnection("Host=localhost;Port=15151");

                            connection.Open();
                            connections.Push(connection);

                        }
                        catch
                        {
                            failures++;
                        }
                    });
                }

                for (int i = 0; i < 3; i++)
                {
                    NpSqlConnection connection;

                    if (connections.TryPop(out connection))
                    {
                        connection.Dispose();
                    }
                }

                failures = 0;

                for (int i = 0; i < 2; i++)
                {
                    try
                    {
                        var connection = new NpSqlConnection("Host=localhost;Port=15151");

                        connection.Open();
                        connections.Push(connection);
                    }
                    catch
                    {
                        failures++;
                    }
                }


                foreach (var connection in connections)
                {
                    connection.Dispose();
                }
            }
        }
    }
}
