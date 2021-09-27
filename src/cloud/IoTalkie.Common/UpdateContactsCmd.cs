using System;

namespace IoTalkie.Common
{
    public class UpdateContactsCmd
    {
        public string Cmd { get; set; } = "updateContacts";

        public string MessageId { get; set; } = Guid.NewGuid().ToString();

        public long Timestamp { get; set; } = DateTimeOffset.Now.ToUnixTimeSeconds();


        public long Size { get; set; }

        public string RemoteUrl { get; set; }
    }
}