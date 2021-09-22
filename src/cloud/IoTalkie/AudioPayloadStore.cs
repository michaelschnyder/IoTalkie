using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using IoTalkie.Common;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace IoTalkie
{
    public class AudioPayloadStore
    {
        private readonly ILogger<AudioPayloadStore> _logger;
        private readonly AzureSettings _settings;

        public AudioPayloadStore(IOptions<AzureSettings> options, ILogger<AudioPayloadStore> logger)
        {
            _logger = logger;
            _settings = options.Value;
        }

        public async Task<AzureBlobPayload> Store(string messageId, Stream content)
        {
            // Create a BlobServiceClient object which will be used to create a container client
            BlobServiceClient blobServiceClient = new BlobServiceClient(_settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            BlobContainerClient containerClient = blobServiceClient.GetBlobContainerClient(_settings.AudioMessagesContainerName);

            // Get a reference to a blob
            var filename = messageId + ".wav";

            BlobClient blobClient = containerClient.GetBlobClient(filename);

            _logger.LogInformation("Uploading to Blob storage as blob: {0}", blobClient.Uri);

            // Open the file and upload its data
            await blobClient.UploadAsync(content, true);

            return new AzureBlobPayload(messageId);
        }


    }
}
