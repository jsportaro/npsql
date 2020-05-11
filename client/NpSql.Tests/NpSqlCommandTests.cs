using NpSql.Tests.Nqp;
using System;
using System.Collections.Generic;
using System.Text;
using Xunit;

namespace NpSql.Tests
{
    public class NpSqlCommandTests
    {
        [Fact]
        public void Should_Return_Reader_When_ExecuteReader()
        {
            using (var server = new TestNpServer())
            {
                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        Assert.NotNull(reader);
                    }
                }
            }
        }
    }
}
