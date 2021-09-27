using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Common;
using Microsoft.Extensions.Options;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class BotCredentialsRegistry
    {
        private readonly AzureSettings _settings;

        public BotCredentialsRegistry(IOptions<AzureSettings> settings)
        {
            _settings = settings.Value;
        }

        public async Task<string> GetToken(ContactPrincipal contactPrincipal, string service)
        {
            var client = await CreateTableClient();

            var result = client.Query<BotCredentialEntity>(ent => ent.UserId == contactPrincipal.UserId && ent.Service == service);

            if (result.Any())
            {
                var entity = result.AsPages().First().Values.First();

                if (!string.IsNullOrWhiteSpace(entity.Credential))
                {
                    return entity.Credential;
                }
            }

            return null;
        }

        private async Task<TableClient> CreateTableClient()
        {
            var client = new TableClient(_settings.BlobStorageConnectionString, "BotCredentials");
            await client.CreateIfNotExistsAsync();
            return client;
        }

        public async Task<IEnumerable<BotCredentialEntity>> GetAll(string service)
        {
            var client = await CreateTableClient();

            var result = client.Query<BotCredentialEntity>(ent => ent.Service == service);

            var pages = result.AsPages();
            return pages.First().Values;

        }
    }


}
