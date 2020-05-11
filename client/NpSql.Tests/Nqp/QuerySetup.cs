using NpSql.Nqp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal class QuerySetup
    {
        private Setup setup;
        private List<(NqpTypes, string, int)> schema = new List<(NqpTypes, string, int)>();
        private bool simulateFailure;
        private List<IYieldSet> yieldSets = new List<IYieldSet>();

        public int FieldCount { get { return schema.Count; } }
        public int RowCount { get => YieldSets.Sum(ys => ys.RowCount); }
        public int RowLength { get; private set; }

        internal IList<(NqpTypes type, string name, int length)> Schema => schema.ToList();

        internal IEnumerable<IYieldSet> YieldSets { get => yieldSets.AsEnumerable(); }

        internal QuerySetup(Setup setup)
        {
            this.setup = setup;
        }

        internal QuerySetup WithColumn(NqpTypes type, string name, int length = 0)
        {
            if (type == NqpTypes.Char && length <= 0)
            {
                throw new ArgumentOutOfRangeException($"{nameof(length)} should be > 0");
            }

            if (type == NqpTypes.Int)
            {
                length = 4;
            }

            RowLength += length;

            schema.Add((type, name, length));

            return this;
        }

        internal QuerySetup Yields(int rowCount)
        {
            yieldSets.Add(new RandomYieldSet(rowCount));

            return this;
        }

        internal QuerySetup SimulateFailure()
        {
            this.simulateFailure = true;

            return this;
        }

        internal Setup Completed()
        {
            return setup;
        }

        internal QuerySetup Yields(params object[] values)
        {
            yieldSets.Add(new GivenYieldSet(values));

            return this;
        }
    }
}
