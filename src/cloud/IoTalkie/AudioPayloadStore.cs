using System;
using System.IO;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using IoTalkie.Common;
using IoTalkie.Media;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace IoTalkie
{
    public class AudioPayloadStore
    {
        private readonly ILogger<AudioPayloadStore> _logger;
        private readonly AzureSettings _settings;
        private readonly AudioConverter _audioConverter;

        public AudioPayloadStore(IOptions<AzureSettings> options, ILogger<AudioPayloadStore> logger)
        {
            _logger = logger;
            _settings = options.Value;
            _audioConverter = new AudioConverter();
        }

        public async Task<AzureBlobPayload> Store(string messageId, Stream content, string mimeType)
        {
            // Create a BlobServiceClient object which will be used to create a container client
            BlobServiceClient blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            BlobContainerClient containerClient =
                blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);

            var extension = GetExtensionFromMimeType(mimeType);

            // Get a reference to a blob
            var filename = messageId + extension;

            BlobClient blobClient = containerClient.GetBlobClient(filename);

            _logger.LogInformation("Uploading to Blob storage as blob: {0}", blobClient.Uri);

            // Open the file and upload its data
            var response = await blobClient.UploadAsync(content, true);

            var contentLength = (await blobClient.GetPropertiesAsync()).Value.ContentLength;
            _logger.LogDebug($"File {filename} is uploaded. Size: {contentLength}");

            return new AzureBlobPayload(messageId, filename, mimeType, contentLength);
        }

        private static string GetExtensionFromMimeType(string mimeType)
        {
            var extension = ".audio";

            if (mimeType == "audio/ogg")
            {
                extension = ".ogg";
            }

            if (mimeType == "audio/x-wav")
            {
                extension = ".wav";
            }

            if (mimeType == "audio/mpeg")
            {
                extension = ".mp3";
            }

            return extension;
        }

        public async Task DownloadTo(string messageId, MemoryStream targetMemoryStream, string mimeType)
        {
            var file = await GetCloudFile(messageId, mimeType);
            await file.DownloadToAsync(targetMemoryStream);
        }

        public async Task<string> GetDownloadLink(string messageId, string mimeType)
        {
            var targetFile = await GetCloudFile(messageId, mimeType);

            if (!await targetFile.ExistsAsync())
            {
                _logger.LogError($"Unable to find or create target format {mimeType} for {messageId}.");
                return null;
            }

            var sasBuilder = new BlobSasBuilder(BlobSasPermissions.Read, expiresOn: DateTimeOffset.UtcNow.AddDays(60))
            {
                StartsOn = DateTimeOffset.UtcNow.AddMinutes(-5),
                BlobContainerName = _settings.AudioMessagesContainerName,
                BlobName = targetFile.Name
            };

            var downloadUrl = targetFile.GenerateSasUri(sasBuilder);

            return downloadUrl.ToString();
        }

        private async Task<BlobClient> GetCloudFile(string messageId, string mimeType)
        {
            var filename = messageId + GetExtensionFromMimeType(mimeType);

            var containerClient = CreateBlobContainerClient();

            // Get a reference to a blob
            var targetFile = containerClient.GetBlobClient(filename);

            if (!await targetFile.ExistsAsync())
            {
                var oggSourceFile = containerClient.GetBlobClient(messageId + ".ogg");
                var wavSourceFile = containerClient.GetBlobClient(messageId + ".wav");

                if (await oggSourceFile.ExistsAsync() && mimeType == "audio/mpeg")
                {
                    var tempFileName = Path.GetTempFileName() + ".ogg";

                    await oggSourceFile.DownloadToAsync(tempFileName);
                    var mp3TargetFile = _audioConverter.ConvertOggToMp3(tempFileName);
                    await containerClient.UploadBlobAsync(filename, File.OpenRead(mp3TargetFile));
                }

                if (await wavSourceFile.ExistsAsync() && mimeType == "audio/ogg")
                {
                    var ms = new MemoryStream();
                    await wavSourceFile.DownloadToAsync(ms);
                    ms.Position = 0;

                    var oggTargetFileMemoryStream = _audioConverter.ConvertWavToOgg(ms);
                    await containerClient.UploadBlobAsync(filename, oggTargetFileMemoryStream);
                }
            }

            return targetFile;
        }

        private BlobContainerClient CreateBlobContainerClient()
        {
            // Create a BlobServiceClient object which will be used to create a container client
            var blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            var containerClient = blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);
            return containerClient;
        }
    }
}
