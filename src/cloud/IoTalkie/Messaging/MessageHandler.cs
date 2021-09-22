using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Telegram.Bot;

namespace IoTalkie.Messaging
{
    public class MessageHandler
    {
        private readonly MessageHandler _handler;
        private readonly List<MessageForwarder> messageForwarders = new(new []{ new TelegramMessageForwarder() });
        private readonly DeviceRegistry _deviceRegistry = new();
        private readonly EndpointRegistry _endpointRegistry = new();

        public MessageHandler()
        {
            _endpointRegistry = new EndpointRegistry();
            _handler = this;
        }

        public async Task ProcessAsync(RoutingMessage routingMessage)
        {
            if (routingMessage.Sender is DevicePrincipal devicePrincipal)
            {
                // Update
                var sender = _deviceRegistry.GetOwner(devicePrincipal);
                routingMessage.Sender = sender;

                await _handler.ProcessAsync(routingMessage);
                return;
            }

            if (routingMessage.Recipient is ContactPrincipal contactPrincipal)
            {
                // Update
                var recipient = _endpointRegistry.GetTarget(contactPrincipal);
                routingMessage.Recipient = recipient;

                await _handler.ProcessAsync(routingMessage);
                return;
            }

            if (routingMessage.Sender == null)
            {
                return;
            }

            if (routingMessage.Recipient == null)
            {
                return;
            }

            var matchingMessageForwarder = messageForwarders.FirstOrDefault(fwd =>fwd.Supports(routingMessage.Sender.GetType(), routingMessage.Recipient.GetType()));
            await matchingMessageForwarder?.Process(routingMessage);

        }
    }

    internal class EndpointRegistry
    {
        public Principal GetTarget(ContactPrincipal contactPrincipal)
        {
            if (contactPrincipal.UserId == "8274387298")
            {
                return new TelegramPrincipal("947344985", contactPrincipal);
            }

            return null;
        }
    }

    internal class TelegramPrincipal : Principal
    {
        public Principal OriginalPrincipal { get; }

        public string TelegramId { get; }

        public TelegramPrincipal(string telegramId, Principal originalPrincipal)
        {
            OriginalPrincipal = originalPrincipal;
            TelegramId = telegramId;
        }
    }

    internal class DeviceRegistry
    {
        
        public ContactPrincipal GetOwner(DevicePrincipal principal)
        {
            if (principal.ClientId == "Gander-fcf5c42f71c0")
            {
                var emmaUserId = "324238947";
                return new ContactPrincipal("324238947", principal);
            }

            return null;
        }
    }

    internal abstract class MessageForwarder
    {
        public abstract bool Supports(Type from, Type to);
        public abstract Task Process(RoutingMessage routingMessage);
    }

    class TelegramMessageForwarder : MessageForwarder
    {
        public override bool Supports(Type @from, Type to)
        {
            return from == typeof(ContactPrincipal) && to == typeof(TelegramPrincipal);
        }

        public override async Task Process(RoutingMessage routingMessage)
        {
            var botIdentity = GetToken((routingMessage.Sender as ContactPrincipal));

            if (botIdentity == null)
            {
                return;
            }

            var telegramRecipient = (TelegramPrincipal)routingMessage.Recipient;

            var client = new TelegramBotClient(botIdentity);
            var result = await client.SendTextMessageAsync(telegramRecipient.TelegramId, "New Message");
        }

        private string GetToken(ContactPrincipal contactPrincipal)
        {
            if (contactPrincipal.UserId == "324238947")
            {
                return "1998383061:AAGqEYr_bUU3p2zYkLzGwxuzqTQBab4Z50M";
            }

            return null;
        }
    }
}
