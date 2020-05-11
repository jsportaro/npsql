using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NpSql.Tests.Nqp
{
    public class GivenYieldSet : IYieldSet
    {
        private object[] values;

        public GivenYieldSet(params object[] values)
        {
            this.values = values;
        }

        public int RowCount => 1;

        public byte[] GetInt(int ordinal)
        {
            return BitConverter.GetBytes((int)values[ordinal]);
        }

        public byte[] GetChar(int length, int ordinal)
        {
            var value = (string)values[ordinal];
            var stringBytes = new List<byte>(Encoding.UTF8.GetBytes(value));
            var padding = Enumerable.Repeat((byte)0, length - value.Length);
            var finalProduct = stringBytes.Concat(padding).ToArray();

            return finalProduct;
        }
    }
}
