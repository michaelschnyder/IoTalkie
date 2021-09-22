using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Primitives;

namespace IoTalkie.Messaging
{
    [Route("api/message")]
    [ApiController]
    public class MessageController : ControllerBase
    {
        private readonly ILogger<MessageController> _logger;
        private readonly MessageHandler _handler;
        private readonly AudioPayloadStore _store;

        public MessageController(ILogger<MessageController> logger, MessageHandler handler, AudioPayloadStore store)
        {
            _logger = logger;
            _handler = handler;
            _store = store;
        }

        [HttpPost("{messageId}")]
        public async Task<IActionResult> SubmitMessage(string messageId, string recipientId)
        {
            try
            {
                var userAgent = GetHeaderValue("User-Agent");
                var clientId = GetHeaderValue("ClientId");

                var sender = new DevicePrincipal(clientId, userAgent);
                var recipient = new ContactPrincipal(recipientId);
                var payload = await _store.Store(messageId, Request.Body);

                var routingMessage = new RoutingMessage(messageId, sender, recipient, payload);

                await _handler.ProcessAsync(routingMessage);

                //string path2 = $"to_{recipientId}-{Path.GetRandomFileName().Replace(".", "")}.wav";

                //using (var file = new FileStream(Path.Combine("messages", path2), FileMode.CreateNew))
                //{
                //    await Request.Body.CopyToAsync(file);
                //}


                //Debug.WriteLine($"Saved to '{Path.GetFullPath(Path.Combine("messages", path2)) }'");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            return this.Ok();
        }

        private string GetHeaderValue(string key)
        {
            if (Request.Headers.ContainsKey(key))
            {
                return this.Request.Headers[key].First();
            }

            return "";
        }

        [HttpGet]
        public async Task<IActionResult> GetMessage(string messageId)
        {
            return this.Ok();
        }

    }

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

    public class ContactPrincipal : Principal
    {
        private readonly string _userId;
        private readonly Principal _original;

        public ContactPrincipal(string userId, Principal original = null)
        {
            _userId = userId;
            _original = original;
        }

        public string UserId => _userId;

        public Principal Original => _original;
    }

    public class DevicePrincipal : Principal
    {
        private readonly string _clientId;
        private readonly string _userAgent;

        public DevicePrincipal(string clientId, string userAgent)
        {
            _clientId = clientId;
            _userAgent = userAgent;
        }

        public string ClientId => _clientId;
    }

    public abstract class Principal
    {
    }
}
