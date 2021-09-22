using IoTalkie.Messaging.Channel.Telegram;

namespace IoTalkie.Messaging.Channel
{
    public class EndpointRegistry
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
}