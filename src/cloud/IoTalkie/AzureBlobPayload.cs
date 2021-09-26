namespace IoTalkie
{
    public class AzureBlobPayload
    {
        private readonly string _messageId;

        public string Filename { get; }
        public long Length { get; }

        public string MimeType { get; }

        public AzureBlobPayload(string messageId, string filename, string mimeType, long contentLength)
        {
            _messageId = messageId;
            MimeType = mimeType;
            Filename = filename;
            Length = contentLength;
        }
    }
}