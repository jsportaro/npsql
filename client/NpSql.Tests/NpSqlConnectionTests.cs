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
