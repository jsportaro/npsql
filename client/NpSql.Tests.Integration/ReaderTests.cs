using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Xunit;

namespace NpSql.Tests.Integration
{
    public class ReaderTests
    {
        [Fact]
        public void Reader_Happy_Path()
        {
            using (var connection = new NpSqlConnection("Host=localhost;Port=15151"))
            {
                connection.Open();

                using (var command = new NpSqlCommand(connection))
                {
                    var recordNumber = 0;

                    try
                    {
                        command.CommandText = "select * from test_table_name where name = 'heather'";
                        var reader = command.ExecuteReader();
                        while (reader.Read())
                        {
                            var name = reader.GetString(0).Trim();
                            var age = reader.GetInt32(1);

                            Assert.Equal("joseph", name);
                            Assert.Equal(32, age);

                            recordNumber++;
                        }

                        Assert.Equal(105, recordNumber);

                        Console.WriteLine($"Got all {recordNumber}");
                    }
                    catch (NpSqlException ex)
                    {

                    }
                }
            }
        }

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

    }
}