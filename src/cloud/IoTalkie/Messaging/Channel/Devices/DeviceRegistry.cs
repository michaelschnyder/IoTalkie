using System.Linq;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Common;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Telegram.Bot.Types;

namespace IoTalkie.Messaging.Channel.Devices
{
    public class DeviceRegistry
    {
        private readonly ILogger<DeviceRegistry> _logger;
        private readonly AzureSettings _settings;

        public DeviceRegistry(IOptions<AzureSettings> settings, ILogger<DeviceRegistry> logger)
        {
            _logger = logger;
            _settings = settings.Value;
        }

        public async Task<ContactPrincipal> GetOwner(DevicePrincipal principal)
        {
            var client = await GetTableClient();

            var result = client.Query<DeviceAssignmentEntity>(ent => ent.DeviceId == principal.ClientId);

            if (!result.Any())
            {
                _logger.LogError($"Unable to find owner user for device with DeviceId/ClientId '{principal.ClientId}'");
                return null;
            }

            var userId = result.AsPages().First().Values.First().UserId;

            return new ContactPrincipal(userId, principal);
        }

        private async Task<TableClient> GetTableClient()
        {
            var client = new TableClient(_settings.BlobStorageConnectionString, "DeviceAssignments");
            await client.CreateIfNotExistsAsync();
            return client;
        }

        public async Task<DevicePrincipal> GetDevice(ContactPrincipal principal)
        {
            var client = await GetTableClient();
            var result = client.Query<DeviceAssignmentEntity>(ent => ent.UserId == principal.UserId);

            if (!result.Any())
            {
                _logger.LogError($"Unable to find device for owner '{principal.UserId}'");
                return null;
            }

            var deviceId = result.AsPages().First().Values.First().DeviceId;

            return new DevicePrincipal(deviceId, principal);

        }
    }
}