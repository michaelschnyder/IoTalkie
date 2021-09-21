﻿using System;
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

                var sender = new DeviceMessageSender(clientId, userAgent);
                var recipient = new ContactMessageRecipient(recipientId);
                var payload = await _store.Store(messageId, Request.Body);

                var routingMessage = new RoutingMessage(sender, recipient, payload);

                _handler.Process(routingMessage);

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
        private readonly DeviceMessageSender _sender;
        private readonly ContactMessageRecipient _recipientId;
        private readonly AzureBlobPayload _payload;

        public RoutingMessage(DeviceMessageSender sender, ContactMessageRecipient recipientId, AzureBlobPayload payload)
        {
            _sender = sender;
            _recipientId = recipientId;
            _payload = payload;
        }
    }

    public class ContactMessageRecipient
    {
        private readonly string _recipientId;

        public ContactMessageRecipient(string recipientId)
        {
            _recipientId = recipientId;
        }
    }

    public class DeviceMessageSender
    {
        private readonly string _clientId;
        private readonly string _userAgent;

        public DeviceMessageSender(string clientId, string userAgent)
        {
            _clientId = clientId;
            _userAgent = userAgent;
        }
    }
}
