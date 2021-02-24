using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using Microsoft.Azure.Devices;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Options;
using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace IoTalkie.MessageSenderCmd
{
    public static class ServiceProviderBuilder
    {
        public static IServiceProvider GetServiceProvider(string[] args)
        {
            var configuration = new ConfigurationBuilder()
                .AddJsonFile("appsettings.json", true, true)
                .AddEnvironmentVariables()
                .AddUserSecrets(typeof(Program).Assembly)
                .Build();

            var services = new ServiceCollection();

            services.Configure<AzureSettings>(configuration.GetSection(typeof(AzureSettings).Name));
            services.AddSingleton<SendMessageFunctionality>();
            var provider = services.BuildServiceProvider();
            return provider;
        }
    }

    class Program
    {
        private static IServiceProvider services;

        static int Main(string[] args)
        {
            services = ServiceProviderBuilder.GetServiceProvider(args);
            var options = services.GetRequiredService<IOptions<AzureSettings>>();

            Command sendMessage = new Command("message", "Send a voice message to a device from a specific user")
            {
                new Argument<string>("file", "The message to be sent"),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
                new Argument<string>("senderId", "From which user the message should come from"),
            };

            sendMessage.Handler = CommandHandler.Create<string, string, string, IConsole>(SendMessageAsync);
            var cmd = new RootCommand { sendMessage };

            return cmd.Invoke(args);
        }

        private static async Task SendMessageAsync(string file, string deviceId, string senderId, IConsole arg4)
        {
            await services.GetService<SendMessageFunctionality>().Run(file, deviceId, senderId);
        }
    }

    public class NewMessageCmd
    {
        public string Cmd { get; set; } = "newMessage";

        public string MessageId { get; set; } = Guid.NewGuid().ToString();

        public long Timestamp { get; set; } = DateTimeOffset.Now.ToUnixTimeSeconds();
        
        public string SenderId { get; set; }

        public long Size { get; set; }

        public string RemoteUrl { get; set; }
    }

    class SendMessageFunctionality
    {
        private readonly AzureSettings settings;

        public SendMessageFunctionality(IOptions<AzureSettings> provider)
        {
            this.settings = provider.Value;
        }

        internal async Task Run(string localFilePath, string deviceId, string senderId)
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
            BlobContainerClient containerClient = blobServiceClient.GetBlobContainerClient(settings.AudioMessagesContainerName);

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
                BlobContainerName = settings.AudioMessagesContainerName,
                BlobName = fileName
            };

            var downloadUrl = blobClient.GenerateSasUri(sasBuilder);

            Console.WriteLine($"Download Url (with sas token): {downloadUrl}");

            ServiceClient service = ServiceClient.CreateFromConnectionString(settings.IoTHubConnectionString);

            var ms = new MemoryStream();
            var jsonMessage = new NewMessageCmd { SenderId = senderId, RemoteUrl = downloadUrl.ToString(), Size = new FileInfo(localFilePath).Length };
            var options = new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase, Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping };
            var jsonString = JsonSerializer.Serialize(jsonMessage, options);

            Console.WriteLine("Sending message: " + jsonString);

            // new Message(Encoding.ASCII.GetBytes(ms.ToArray());


            await service.SendAsync(deviceId, new Message(Encoding.ASCII.GetBytes(jsonString)));
            
            Console.WriteLine("Message sent");

        }
    }
}