using System;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Azure.Data.Tables;
using IoTalkie.Messaging.Processing;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Telegram.Bot;
using Telegram.Bot.Extensions.Polling;
using Telegram.Bot.Types;
using Telegram.Bot.Types.Enums;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class BotMessagesHandler : IHostedService
    {
        private readonly BotCredentialsRegistry _botCredentialsRegistry;
        private readonly UserEndpointRegistry _endpointRegistry;
        private readonly MessageHandler _messageHandler;
        private readonly AudioPayloadStore _audioPayloadStore;

        private readonly ILogger<BotMessagesHandler> _logger;
        private readonly ILoggerFactory _loggerFactory;

        public BotMessagesHandler(BotCredentialsRegistry botCredentialsRegistry, UserEndpointRegistry endpointRegistry, MessageHandler messageHandler, AudioPayloadStore audioPayloadStore, ILogger<BotMessagesHandler> logger, ILoggerFactory loggerFactory)
        {
            _botCredentialsRegistry = botCredentialsRegistry;
            _endpointRegistry = endpointRegistry;
            _messageHandler = messageHandler;
            _audioPayloadStore = audioPayloadStore;
            _logger = logger;
            _loggerFactory = loggerFactory;
        }

        public async Task StartAsync(CancellationToken cancellationToken)
        {  
            var allBots = await _botCredentialsRegistry.GetAll("Telegram");

            _logger.LogInformation($"Found {allBots} bots to be started.");

            foreach (var bot in allBots)
            {
                var client = new TelegramBotClient(bot.Credential);
                var me = await client.GetMeAsync(cancellationToken);
                
                _logger.LogInformation($"Bot '{me.Username}' is ready");

                var handler = new ReceivingHandler(bot, client, _endpointRegistry, _messageHandler, _audioPayloadStore, _loggerFactory.CreateLogger<ReceivingHandler>());

            }
        }

        public Task StopAsync(CancellationToken cancellationToken)
        {
            return Task.CompletedTask;
        }
    }

    public class ReceivingHandler
    {
        private readonly UserEndpointRegistry _userEndpointRegistry;
        private readonly MessageHandler _messageHandler;
        private readonly AudioPayloadStore _audioPayloadStore;
        private readonly ILogger<ReceivingHandler> _logger;
        private readonly ContactPrincipal _recipient;

        public ReceivingHandler(BotCredentialEntity botCredentials, ITelegramBotClient client, UserEndpointRegistry userEndpointRegistry, MessageHandler messageHandler, 
            AudioPayloadStore audioPayloadStore, ILogger<ReceivingHandler> logger)
        {
            _userEndpointRegistry = userEndpointRegistry;
            _messageHandler = messageHandler;
            _audioPayloadStore = audioPayloadStore;
            _logger = logger;

            _recipient = new ContactPrincipal(botCredentials.UserId);

            client.StartReceiving(UpdateHandler, ErrorHandler);
        }

        private Task ErrorHandler(ITelegramBotClient arg1, Exception arg2, CancellationToken arg3)
        {
            return Task.CompletedTask;
        }

        private Task UpdateHandler(ITelegramBotClient client, Update update, CancellationToken cancellationToken)
        {
            if (update.Message is { Type: MessageType.Voice } voiceMessage)
            {
                var messageId = Guid.NewGuid().ToString();

                _logger.LogInformation($"New Audio message from '{voiceMessage.From?.Username}' (id: {voiceMessage.From.Id}. Duration : {voiceMessage.Voice?.Duration}");

                // Find all the contact information
                var sender = _userEndpointRegistry.Translate(voiceMessage.From.Id.ToString(), "Telegram").GetAwaiter().GetResult();
                var recipient = _recipient;

                var ms = new MemoryStream();
                try
                {
                    var fileInfo = client.GetFileAsync(voiceMessage.Voice.FileId).GetAwaiter().GetResult();

                    client.DownloadFileAsync(fileInfo.FilePath, ms).GetAwaiter().GetResult();
                }
                catch (Exception e)
                {
                    _logger.LogError(e, "Unable to download voice for message");
                    return Task.CompletedTask;
                }

                ms.Position = 0;
                var payload = _audioPayloadStore.Store(messageId, ms, voiceMessage.Voice.MimeType).GetAwaiter().GetResult();
                var message = new RoutingMessage(messageId, sender, recipient, payload);

                _messageHandler.ProcessAsync(message);
            }

            return Task.CompletedTask;
        }
    }
}
