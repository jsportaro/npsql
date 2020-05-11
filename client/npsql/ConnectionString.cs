using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace NpSql
{ 
    internal class ConnectionString
    {
        private readonly string baseString = string.Empty;

        public string BaseString { get => baseString; }

        public string Host { get; private set; }

        public int Port { get; private set; }

        public int ConnectionTimeout { get; private set; }

        public ConnectionString(string baseString)
        {
            this.baseString = baseString;

            Host = string.Empty;
            Port = 8883;
            ConnectionTimeout = 30;

            Parse();
        }

        private void Parse()
        {
            foreach (var kv in this.baseString.Split(new char[] { ';' }).Select(kvp => kvp.Split(new char[] { '=' })))
            {
                switch (kv[0].ToUpper())
                {
                    case "HOST":
                        Host = kv[1];
                        break;
                    case "PORT":
                        Port = int.Parse(kv[1]);
                        break;
                    case "CONNECTIONTIMEOUT":
                        ConnectionTimeout = int.Parse(kv[1]);
                        break;
                }
            }
        }
    }
}
