using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using IoTalkie.Common;
using Microsoft.Azure.Devices;
using Microsoft.Extensions.Options;

namespace IoTalkie.MessageSenderCmd
{
    class UpdateContactsFunctionality
    {
        private readonly AzureSettings settings;

        public UpdateContactsFunctionality(IOptions<AzureSettings> provider)
        {
            this.settings = provider.Value;
        }

        internal async Task Run(string localFilePath, string deviceId)
        {
            if (!File.Exists(localFilePath))
            {
                Console.WriteLine("File not found");
                return;
            }

            var fileName = Path.GetFileName(localFilePath);

            // Create a BlobServiceClient object which will be used to create a container client
            BlobServiceClient blobServiceClient = new BlobServiceClient(settings.BlobStorageConnectionString);

            // Create the container and return a container client object
            if (!blobServiceClient.GetBlobContainers().AsPages().First().Values.Any(c => c.Name.Equals(settings.ContactsContainerName)))
            {
                await blobServiceClient.CreateBlobContainerAsync(settings.ContactsContainerName);
            }
            BlobContainerClient containerClient = blobServiceClient.GetBlobContainerClient(settings.ContactsContainerName);

            // Get a reference to a blob
            BlobClient blobClient = containerClient.GetBlobClient(fileName);

            Console.WriteLine("Uploading to Blob storage as blob:\n\t {0}\n", blobClient.Uri);

            // Open the file and upload its data
            using FileStream uploadFileStream = File.OpenRead(localFilePath);
            await blobClient.UploadAsync(uploadFileStream, true);
            uploadFileStream.Close();

            var sasBuilder = new BlobSasBuilder(BlobSasPermissions.Read, expiresOn: DateTimeOffset.UtcNow.AddDays(60))
            {
                StartsOn = DateTimeOffset.UtcNow.AddMinutes(-5),
                BlobContainerName = settings.ContactsContainerName,
                BlobName = fileName
            };

            var downloadUrl = blobClient.GenerateSasUri(sasBuilder);

            Console.WriteLine($"Download Url (with sas token): {downloadUrl}");

            ServiceClient service = ServiceClient.CreateFromConnectionString(settings.IoTHubConnectionString);
            
            var ms = new MemoryStream();
            var jsonMessage = new UpdateContactsCmd() { RemoteUrl = downloadUrl.ToString(), Size = new FileInfo(localFilePath).Length };
            var options = new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase, Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping };
            var jsonString = JsonSerializer.Serialize(jsonMessage, options);

            Console.WriteLine("Sending message: " + jsonString);

            Message message = new Message(Encoding.ASCII.GetBytes(jsonString));
            message.ExpiryTimeUtc = DateTime.UtcNow.AddHours(8);

            await service.SendAsync(deviceId, message);
            
            Console.WriteLine("Message sent");
        }
    }
}