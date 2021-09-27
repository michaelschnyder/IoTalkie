using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Common;
using IoTalkie.Messaging.Processing;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

using Telegram.Bot;
using Telegram.Bot.Types;
using Telegram.Bot.Types.InputFiles;
using File = System.IO.File;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class TelegramMessageForwarder : IMessageForwarder
    {
        private readonly BotCredentialsRegistry _botCredentialsRegistry;
        private readonly AudioPayloadStore _audioPayloadStore;
        private readonly ILogger<TelegramMessageForwarder> _logger;

        public TelegramMessageForwarder(BotCredentialsRegistry botCredentialsRegistry, AudioPayloadStore audioPayloadStore, ILogger<TelegramMessageForwarder> logger)
        {
            _botCredentialsRegistry = botCredentialsRegistry;
            _audioPayloadStore = audioPayloadStore;
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

            var ms = new MemoryStream();
            await _audioPayloadStore.DownloadTo(routingMessage.MessageId, ms, "audio/ogg");
            ms.Position = 0;
            ms.WriteTo(new FileStream(Path.GetTempFileName() + ".ogg", FileMode.CreateNew));

            var url = await _audioPayloadStore.GetDownloadLink(routingMessage.MessageId, "audio/ogg");

            await client.SendVoiceAsync(telegramRecipient.TelegramId, new InputOnlineFile(new Uri(url)));

            _logger.LogInformation($"Message '{routingMessage.MessageId}' sent");
        }
    }
}