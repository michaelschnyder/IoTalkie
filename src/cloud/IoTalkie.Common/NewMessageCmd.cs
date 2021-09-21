using System;

namespace IoTalkie.Common
{
    public class NewMessageCmd
    {
        public string Cmd { get; set; } = "newMessage";

        public string MessageId { get; set; } = Guid.NewGuid().ToString();

        public long Timestamp { get; set; } = DateTimeOffset.Now.ToUnixTimeSeconds();
        
        public string SenderId { get; set; }

        public long Size { get; set; }

        public string RemoteUrl { get; set; }
    }
}