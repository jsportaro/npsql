using System;
using System.Collections.Generic;
using System.Text;
using Xunit;

namespace NpSql.Tests
{
    public class ConnectionStringTests
    {
        [Fact]
        public void Should_Return_Host_From_String()
        {
            var expectedHost = "testHostName";
            var connectionString = $"Host={expectedHost}";

            var sut = new ConnectionString(connectionString);

            Assert.Equal(expectedHost, sut.Host);
        }

        [Fact]
        public void Should_Return_Port_From_String()
        {
            var expectedHost = "testHostName";
            var expectedPort = 1234;
            var connectionString = $"Host={expectedHost};Port={expectedPort}";

            var sut = new ConnectionString(connectionString);

            Assert.Equal(expectedPort, sut.Port);
        }

        [Fact]
        public void Should_Return_Default_Port_If_Not_Specified()
        {
            var expectedHost = "testHostName";
            var expectedPort = 8883;
            var connectionString = $"Host={expectedHost}";

            var sut = new ConnectionString(connectionString);

            Assert.Equal(expectedPort, sut.Port);
        }

        [Fact]
        public void Should_Return_ConnectionTimeout_From_String()
        {
            var expectedHost = "testHostName";
            var expectedTimeout = 60;
            var connectionString = $"Host={expectedHost};ConnectionTimeout={expectedTimeout}";

            var sut = new ConnectionString(connectionString);

            Assert.Equal(expectedTimeout, sut.ConnectionTimeout);
        }
    }
}
