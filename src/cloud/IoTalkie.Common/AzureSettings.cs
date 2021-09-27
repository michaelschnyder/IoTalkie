namespace IoTalkie.Common
{
    public class AzureSettings
    {
        public string IoTHubConnectionString { get; set; }

        public string BlobStorageConnectionString { get; set; }
        
        public string AudioMessagesContainerName { get; set; }
        
        public string FirmwareUpdatesContainerName { get; set; }

        public string ContactsContainerName { get; set; }
    }
}