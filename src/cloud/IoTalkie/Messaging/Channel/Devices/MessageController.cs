using System;
using System.Linq;
using System.Threading.Tasks;
using IoTalkie.Messaging.Processing;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace IoTalkie.Messaging.Channel.Devices
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

        /// <summary>
        ///     Processes a message from a client device.
        /// </summary>
        /// <example>
        ///     curl -X POST --header "ClientId: Gander-fcf5c42f71c0" -v --data Bla https://localhost:5001/api/message/123456?recipientId=8274387298
        /// </example>

        [HttpPost("{messageId}")]
        public async Task<IActionResult> SubmitMessage(string messageId, string recipientId)
        {
            _logger.LogDebug($"Starting to process message '{messageId}'");
            try
            {
                var userAgent = GetHeaderValue("User-Agent");
                var clientId = GetHeaderValue("ClientId");

                var sender = new DevicePrincipal(clientId, userAgent);
                var recipient = new ContactPrincipal(recipientId);
                var payload = await _store.Store(messageId, Request.Body);

                var routingMessage = new RoutingMessage(messageId, sender, recipient, payload);

                await _handler.ProcessAsync(routingMessage);
            }
            catch (Exception e)
            {
                _logger.LogError(e , $"Handling of message '{messageId}' has failed.");
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
}
