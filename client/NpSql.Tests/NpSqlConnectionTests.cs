using NpSql.Tests.Nqp;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Data;
using System.Threading.Tasks;
using Xunit;

namespace NpSql.Tests
{
    public class NpSqlConnectionTests
    {
        [Fact]
        public void Should_Connect_To_Server_On_Open()
        {
            using (var server = new TestNpServer())
            {
                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();
                }
            }
        }

        //[Fact]
        //public void These_Go_To_Eleven()
        //{
        //    for (int loop = 0; loop < 10000; loop++)
        //    {
        //        var connection1 = new NpSqlConnection("Host=localhost;Port=15151");

        //        connection1.Open();

        //        connection1.Dispose();

        //        var connections = new ConcurrentStack<NpSqlConnection>();
        //        var failures = 0;
        //        for (int i = 0; i < 1; i++)
        //        {
        //            Task.Run(() =>
        //            {
        //                try
        //                {

        //                    var connection = new NpSqlConnection("Host=localhost;Port=15151");

        //                    connection.Open();
        //                    connections.Push(connection);

        //                }
        //                catch
        //                {
        //                    failures++;
        //                }
        //            });
        //        }

        //        for (int i = 0; i < 3; i++)
        //        {
        //            NpSqlConnection connection;
                    
        //            if (connections.TryPop(out connection))
        //            {
        //                connection.Dispose();
        //            }
        //        }

        //        failures = 0;

        //        for (int i = 0; i < 2; i++)
        //        {
        //            try
        //            {
        //                var connection = new NpSqlConnection("Host=localhost;Port=15151");

        //                connection.Open();
        //                connections.Push(connection);
        //            }
        //            catch
        //            {
        //                failures++;
        //            }
        //        }


        //        foreach (var connection in connections)
        //        {
        //            connection.Dispose();
        //        }
        //    }
        //}

        [Fact]
        public void Should_Close_Connection_On_Dispose()
        {
            using (var server = new TestNpServer())
            {
                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();
                }

                Assert.False(server.ClientConnected);
            }
        }

        [Fact]
        public void Should_Set_Connection_Opened_On_Success()
        {
            using (var server = new TestNpServer())
            {
                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    Assert.Equal(ConnectionState.Open, connection.State);
                }
            }
        }

        [Fact]
        public void Should_Set_Connection_Closed_On_Success()
        {
            using (var server = new TestNpServer())
            {
                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();
                    connection.Close();

                    Assert.Equal(ConnectionState.Closed, connection.State);
                }
            }
        }

        [Fact]
        public void Should_Set_MaxMessageSize_When_Opened()
        {
            using (var server = new TestNpServer())
            {
                server.Setup.SetMaxMessageSize(512);

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    Assert.Equal(server.Setup.MaxMessageSize, connection.Client.MaxMessageSize);
                }
            }
        }

        [Fact]
        public void If_Connection_Refused_Throw_Exception()
        {
            using (var server = new TestNpServer())
            {
                server.Setup.SetSaySorry();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    Assert.Throws<NpSqlException>(() => connection.Open());
                }
            }
        }
    }
}
