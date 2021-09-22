using System;
using Azure;
using Azure.Data.Tables;

namespace IoTalkie.Messaging.Channel.Devices
{
    public class DeviceAssignmentEntity : ITableEntity
    {
        public string PartitionKey { get; set; }
        public string RowKey { get; set; }
        public DateTimeOffset? Timestamp { get; set; }
        public ETag ETag { get; set; }
        
        public string DeviceId { get; set; }

        public string UserId { get; set; }
    }
}
