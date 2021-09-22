using System;
using System.Linq;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Common;
using IoTalkie.Messaging.Processing;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

using Telegram.Bot;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class TelegramMessageForwarder : IMessageForwarder
    {
        private readonly ILogger<TelegramMessageForwarder> _logger;
        private readonly AzureSettings _settings;

        public TelegramMessageForwarder(IOptions<AzureSettings> options, ILogger<TelegramMessageForwarder> logger)
        {
            _logger = logger;
            _settings = options.Value;
        } 

        public bool Supports(Type @from, Type to)
        {
            return from == typeof(ContactPrincipal) && to == typeof(TelegramPrincipal);
        }

        public async Task Process(RoutingMessage routingMessage)
        {
            var botIdentity = await GetToken((routingMessage.Sender as ContactPrincipal));

            if (botIdentity == null)
            {
                _logger.LogError($"Unable to find BotIdentity for sending user {routingMessage.Sender}. Unable to process");
                return;
            }

            var telegramRecipient = (TelegramPrincipal)routingMessage.Recipient;

            var client = new TelegramBotClient(botIdentity);
            var result = await client.SendTextMessageAsync(telegramRecipient.TelegramId, $"New Message '{routingMessage.MessageId}'");

            _logger.LogInformation($"Message '{routingMessage.MessageId}' sent");
        }

        private async Task<string> GetToken(ContactPrincipal contactPrincipal)
        {
            var client = new TableClient(_settings.BlobStorageConnectionString, "BotCredentials");
            await client.CreateIfNotExistsAsync();

            var result = client.Query<BotCredentialEntity>(ent => ent.UserId == contactPrincipal.UserId && ent.Service == "Telegram");

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
    }
}