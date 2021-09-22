using System;
using Azure;
using Azure.Data.Tables;

namespace IoTalkie.Messaging.Channel
{
    public class UserEndpointEntity : ITableEntity
    {
        public UserEndpointEntity() { }

        public string UserId { get; set; }

        public string Endpoint { get; set; }

        public string TargetUserId { get; set; }
        
        public string PartitionKey { get; set; }
        
        public string RowKey { get; set; }

        public DateTimeOffset? Timestamp { get; set; }

        public ETag ETag { get; set; }
    }
}