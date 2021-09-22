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
}
