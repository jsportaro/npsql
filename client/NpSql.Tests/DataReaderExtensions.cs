using System;
using System.Data;
using Xunit;

namespace NpSql.Tests
{
    public static class DataReaderExtensions
    {
        public static void FastForwardAndAssert(this IDataReader reader, int expectedCount)
        {
            var actualCount = 0;

            while (reader.Read())
            {
                actualCount++;
            }

            Assert.Equal(expectedCount, actualCount);
        }

        public static void FastForward(this IDataReader reader, int count)
        {
            for (var i = 0; i < count; i++)
            {
                if (!reader.Read())
                {
                    throw new InvalidOperationException();
                }
            }
        }
    }
}
