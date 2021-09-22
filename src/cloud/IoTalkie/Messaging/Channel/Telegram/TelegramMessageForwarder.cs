using System;
using System.Threading.Tasks;
using IoTalkie.Messaging.Processing;
using Telegram.Bot;

namespace IoTalkie.Messaging.Channel.Telegram
{
    public class TelegramMessageForwarder : IMessageForwarder
    {
        public bool Supports(Type @from, Type to)
        {
            return from == typeof(ContactPrincipal) && to == typeof(TelegramPrincipal);
        }

        public async Task Process(RoutingMessage routingMessage)
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