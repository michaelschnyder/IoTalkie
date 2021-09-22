namespace IoTalkie.Messaging.Channel.Telegram
{
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
}