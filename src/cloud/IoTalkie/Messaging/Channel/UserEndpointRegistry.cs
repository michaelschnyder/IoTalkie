using System.Linq;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Common;
using IoTalkie.Messaging.Channel.Devices;
using IoTalkie.Messaging.Channel.Telegram;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Telegram.Bot.Types;

namespace IoTalkie.Messaging.Channel
{
    public class UserEndpointRegistry
    {
        private readonly AzureSettings _settings;
        private readonly ILogger<UserEndpointRegistry> _logger;

        public UserEndpointRegistry(IOptions<AzureSettings> settings, ILogger<UserEndpointRegistry> logger)
        {
            _settings = settings.Value;
            _logger = logger;
        }

        public async Task<Principal> GetTarget(ContactPrincipal contactPrincipal)
        {

            var client = new TableClient(_settings.BlobStorageConnectionString, "UserEndpoints");
            await client.CreateIfNotExistsAsync();

            var result = client.Query<UserEndpointEntity>(ent => ent.UserId == contactPrincipal.UserId);

            if (!result.Any())
            {
                _logger.LogError($"Unable to find Endpoint for User {contactPrincipal.UserId}");
                return null;
            }

            var entity = result.AsPages().First().Values.First();

            if (entity.Endpoint == "Telegram")
            {
                return new TelegramPrincipal(entity.TargetUserId, contactPrincipal);
            }

            _logger.LogError($"Unsupported Endpoint '{entity.Endpoint}' for UserId '{contactPrincipal.UserId}'");
            return null;
        }

        public async Task<ContactPrincipal> Translate(string serviceUserId, string serviceName)
        {
            var client = new TableClient(_settings.BlobStorageConnectionString, "UserEndpoints");
            await client.CreateIfNotExistsAsync();

            var result = client.Query<UserEndpointEntity>(ent => ent.TargetUserId == serviceUserId && ent.Endpoint == serviceName);

            if (!result.Any())
            {
                _logger.LogError($"Unable to find contact for UserIf {serviceUserId} from service '{serviceName}'");
                return null;
            }

            var entity = result.AsPages().First().Values.First();

            return new ContactPrincipal(entity.UserId);
        }
    }
}