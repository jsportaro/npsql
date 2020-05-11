using System;
using System.Collections.Generic;
using System.Text;

namespace NpSql.Tests.Nqp
{
    internal class Setup
    {
        internal short MaxMessageSize { get; private set; } = 1024;
        internal bool SaySorry { get; private set; } = false;
        internal List<QuerySetup> QueryResults { get; set; } = new List<QuerySetup>();

        public Setup SetMaxMessageSize(short size)
        {
            MaxMessageSize = size;

            return this;
        }

        public Setup SetSaySorry()
        {
            SaySorry = true;

            return this;
        }

        public QuerySetup ForQuery()
        {
            var querySetup = new QuerySetup(this);

            QueryResults.Add(querySetup);

            return querySetup;
        }

        public void Ready()
        {

        }
    }
}
