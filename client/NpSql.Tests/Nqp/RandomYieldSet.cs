using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal class RandomYieldSet : IYieldSet
    {
        private static Random random = new Random();

        public int RowCount { get; private set; }

        public RandomYieldSet(int count)
        {
            RowCount = count;
        }

        public byte[] GetInt(int ordinal)
        {
            return BitConverter.GetBytes(random.Next(0, 999));
        }

        public byte[] GetChar(int length, int ordinal)
        {
            return RandomString(length);
        }

        private static byte[] RandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            var fullLength = new string(Enumerable.Repeat(chars, length)
              .Select(s => s[random.Next(s.Length)]).ToArray());
            var subLength = random.Next(length);
            var stringBytes = new List<byte>(Encoding.UTF8.GetBytes(fullLength.Substring(0, subLength)));
            var padding = Enumerable.Repeat((byte)0, length - subLength);
            var finalProduct = stringBytes.Concat(padding).ToArray();
            var peakAString = Encoding.UTF8.GetString(finalProduct);

            return finalProduct;
        }


    }
}
