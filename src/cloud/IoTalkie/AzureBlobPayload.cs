namespace IoTalkie
{
    public class AzureBlobPayload
    {
        private string _messageId;

        public AzureBlobPayload(string messageId)
        {
            this._messageId = messageId;
        }
    }
}