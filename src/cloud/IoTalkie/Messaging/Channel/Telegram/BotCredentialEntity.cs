using System;
using Azure;
using Azure.Data.Tables;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class BotCredentialEntity : ITableEntity
    {
        public BotCredentialEntity() { }

        public string UserId { get; set; }

        public string Credential { get; set; }

        public string Service { get; set; }
        
        public string PartitionKey { get; set; }
        
        public string RowKey { get; set; }

        public DateTimeOffset? Timestamp { get; set; }

        public ETag ETag { get; set; }
    }
}