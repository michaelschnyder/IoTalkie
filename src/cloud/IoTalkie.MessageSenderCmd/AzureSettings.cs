namespace IoTalkie.MessageSenderCmd
{
    public class AzureSettings
    {
        public string IoTHubConnectionString { get; set; }

        public string BlobStorageConnectionString { get; set; }
        public string AudioMessagesContainerName { get; set; }
    }
}