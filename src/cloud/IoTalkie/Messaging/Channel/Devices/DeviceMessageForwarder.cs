using System;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Azure.Storage.Blobs;
using Azure.Storage.Sas;
using IoTalkie.Common;
using IoTalkie.Messaging.Processing;
using Microsoft.Azure.Devices;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace IoTalkie.Messaging.Channel.Devices
{
    public class DeviceMessageForwarder : IMessageForwarder
    {
        private readonly DeviceRegistry _deviceRegistry;
        private readonly AudioPayloadStore _audioPayloadStore;
        private readonly AzureSettings _settings;
        private readonly ILogger<DeviceMessageForwarder> _logger;

        public DeviceMessageForwarder(DeviceRegistry deviceRegistry, AudioPayloadStore audioPayloadStore, IOptions<AzureSettings> settings, ILogger<DeviceMessageForwarder> logger)
        {
            _deviceRegistry = deviceRegistry;
            _audioPayloadStore = audioPayloadStore;
            _settings = settings.Value;
            _logger = logger;
        }

        public bool Supports(Type @from, Type to)
        {
            return from == typeof(ContactPrincipal) && to == typeof(ContactPrincipal);
        }

        public async Task Process(RoutingMessage routingMessage)
        {
            var targetDevice = await _deviceRegistry.GetDevice((routingMessage.Recipient as ContactPrincipal));
            var deviceId = targetDevice.ClientId;

            var downloadUrl = await _audioPayloadStore.GetDownloadLink(routingMessage.MessageId, "audio/mpeg");

            _logger.LogInformation($"Download Url (with sas token): {downloadUrl}");

            ServiceClient service = ServiceClient.CreateFromConnectionString(_settings.IoTHubConnectionString);

            var jsonMessage = new NewMessageCmd
            {
                MessageId = routingMessage.MessageId,
                SenderId = (routingMessage.Sender as ContactPrincipal).UserId, 
                RemoteUrl = downloadUrl.ToString(), 
                MimeType = routingMessage.Payload.MimeType,
                Size = routingMessage.Payload.Length
            };
            var options = new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase, Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping };
            var jsonString = JsonSerializer.Serialize(jsonMessage, options);

            _logger.LogInformation($"Sending message to '{deviceId}': " + jsonString);

            Message message = new Message(Encoding.ASCII.GetBytes(jsonString));
            message.ExpiryTimeUtc = DateTime.UtcNow.AddHours(8);

            await service.SendAsync(deviceId, message);


        }
    }
}   
