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
        private readonly BotCredentialsRegistry _botCredentialsRegistry;
        private readonly ILogger<TelegramMessageForwarder> _logger;

        public TelegramMessageForwarder(BotCredentialsRegistry botCredentialsRegistry, ILogger<TelegramMessageForwarder> logger)
        {
            _botCredentialsRegistry = botCredentialsRegistry;
            _logger = logger;
        } 

        public bool Supports(Type @from, Type to)
        {
            return from == typeof(ContactPrincipal) && to == typeof(TelegramPrincipal);
        }

        public async Task Process(RoutingMessage routingMessage)
        {
            var botIdentity = await _botCredentialsRegistry.GetToken((routingMessage.Sender as ContactPrincipal), "Telegram");

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
    }
}