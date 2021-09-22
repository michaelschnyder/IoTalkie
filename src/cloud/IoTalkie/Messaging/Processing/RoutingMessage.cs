using IoTalkie.Messaging.Channel;
using IoTalkie.Messaging.Channel.Devices;

namespace IoTalkie.Messaging.Processing
{
    public class RoutingMessage
    {
        private readonly string _messageId;
        private AzureBlobPayload _payload;

        public RoutingMessage(string messageId, DevicePrincipal sender, ContactPrincipal recipient, AzureBlobPayload payload)
        {
            Sender = sender;
            _messageId = messageId;
            Recipient = recipient;
            _payload = payload;
        }

        public Principal Sender { get; set; }

        public Principal Recipient { get; set; }

        public AzureBlobPayload Payload => _payload;

        public string MessageId => _messageId;
    }
}