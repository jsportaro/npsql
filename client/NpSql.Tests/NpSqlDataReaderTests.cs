using NpSql.Nqp;
using NpSql.Tests.Nqp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xunit;

namespace NpSql.Tests
{
    public class NpSqlDataReaderTests
    {
        //[Fact]
        //public void Should_Send_Query_To_Server_When_ExecuteReader()
        //{
        //    var runs = new List<Task>();

        //    for (int i = 0; i < 10000; i++)
        //    {

        //        runs.Add(Task.Run(() =>
        //        {

        //            using (var connection = new NpSqlConnection("Host=localhost;Port=15151"))
        //            {
        //                connection.Open();

        //                using (var command = new NpSqlCommand(connection))
        //                {
        //                    var recordNumber = 0;

        //                    try
        //                    {
        //                        command.CommandText = "select * from test_table_name where name = 'heather'";
        //                        var reader = command.ExecuteReader();
        //                        while (reader.Read())
        //                        {
        //                            var name = reader.GetString(0).Trim();
        //                            var age = reader.GetInt32(1);

        //                            Assert.Equal("joseph", name);
        //                            Assert.Equal(32, age);

        //                            recordNumber++;
        //                        }

        //                        Thread.Sleep(1000);
        //                        Assert.Equal(105, recordNumber);
        //                    }
        //                    catch (NpSqlException ex)
        //                    {

        //                    }
        //                }
        //            }
        //        }));
        //    }

        //    Task.WaitAll(runs.ToArray());
        //}

        [Fact]
        public void Should_Send_Query_To_Server_When_ExecuteReader()
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

                        Assert.Equal(command.CommandText, server.GetLastQuery(connection.ConnectionId));
                    }
                }
            }
        }

        [Fact]
        public void Should_Set_Field_Count()
        {
            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(0)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        Assert.Equal(reader.FieldCount, server.Setup.QueryResults.First().FieldCount);
                    }
                }
            }
        }

        [Fact]
        public void Read_Should_Return_True_When_Rows_Returned()
        {
            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(1)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        var rows = reader.Read();

                        Assert.True(rows);
                    }
                }
            }
        }

        [Fact]
        public void Read_Should_Return_False_When_No_Rows_Returned()
        {
            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(0)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        var rows = reader.Read();

                        Assert.False(rows);
                    }
                }
            }
        }

        [Fact]
        public void Should_Read_All_Rows()
        {
            var expectedRows = 80;
            
            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(expectedRows)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        reader.FastForwardAndAssert(expectedRows);
                    }
                }
            }
        }

        [Fact]
        public void Should_Return_Expected_Int_Value_From_Row()
        {
            var firstSet = 10;
            var secondSet = 5;

            var expectedInt = 33;

            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(firstSet)
                            .Yields("Heather", expectedInt)
                            .Yields(secondSet)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();

                        for (var i = 0; i < firstSet; i++)
                        {
                            reader.Read();
                        }

                        reader.Read();

                        Assert.Equal(expectedInt, reader.GetInt32(1));

                        for (var i = 0; i < secondSet; i++)
                        {
                            reader.Read();
                        }
                    }
                }
            }
        }

        [Fact]
        public void Should_Return_Expected_String_Value_From_Row()
        {
            var firstSet = 10;
            var secondSet = 5;

            var expectedString = "Heather";

            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(firstSet)
                            .Yields(expectedString, 33)
                            .Yields(secondSet)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";

                        var reader = command.ExecuteReader();
                        reader.FastForward(firstSet);

                        reader.Read();

                        Assert.Equal(expectedString, reader.GetString(0));
                        reader.FastForward(secondSet);
                    }
                }
            }
        }

        [Fact]
        public void Should_Continue_To_Next_Query_Result_On_NextResult()
        {
            var expectedFirstSetCount = 10;
            var expectedSecondSetCount = 12;

            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(expectedFirstSetCount)
                            .Completed()
                        .ForQuery()
                            .WithColumn(NqpTypes.Int, "int1")
                            .WithColumn(NqpTypes.Int, "int2")
                            .WithColumn(NqpTypes.Int, "int3")
                            .Yields(expectedSecondSetCount)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_one; select * from test_table_two";

                        var reader = command.ExecuteReader();
                        reader.FastForwardAndAssert(expectedFirstSetCount);

                        reader.NextResult();
                        reader.FastForwardAndAssert(expectedSecondSetCount);
                    }
                }
            }
        }

        [Fact(Skip = "Not supporting multiple rowsets yet - Exercise for the reader :)")]
        public void Should_Not_Require_Reading_To_End_Of_Result_For_NextResult()
        {
            var expectedSecondSetCount = 12;

            using (var server = new TestNpServer())
            {
                server
                    .Setup
                        .ForQuery()
                            .WithColumn(NqpTypes.Char, "name", 10)
                            .WithColumn(NqpTypes.Int, "age")
                            .Yields(10)
                            .Completed()
                        .ForQuery()
                            .WithColumn(NqpTypes.Int, "int1")
                            .WithColumn(NqpTypes.Int, "int2")
                            .WithColumn(NqpTypes.Int, "int3")
                            .Yields(expectedSecondSetCount)
                            .Yields(123, 456, 789)
                            .Completed()
                        .Ready();

                using (var connection = new NpSqlConnection(server.ConnectionString))
                {
                    connection.Open();

                    using (var command = new NpSqlCommand(connection))
                    {
                        command.CommandText = "select * from test_table_one; select * from test_table_two";

                        var reader = command.ExecuteReader();

                        reader.NextResult();
                        reader.FastForwardAndAssert(expectedSecondSetCount);

                        Assert.Equal(456, reader.GetInt32(1));
                    }
                }
            }
        }
    }
}
