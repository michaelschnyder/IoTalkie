using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using IoTalkie.Messaging.Channel;
using IoTalkie.Messaging.Channel.Devices;
using Microsoft.Extensions.Logging;

namespace IoTalkie.Messaging.Processing
{
    public class MessageHandler
    {
        private readonly ILogger<MessageHandler> _logger;
        private readonly MessageHandler _handler;
        private readonly IEnumerable<IMessageForwarder> _messageForwarders;
        private readonly DeviceRegistry _deviceRegistry;
        private readonly UserEndpointRegistry _userEndpointRegistry;

        public MessageHandler(ILogger<MessageHandler> logger, IEnumerable<IMessageForwarder> messageForwarders, UserEndpointRegistry userEndpointRegistry, DeviceRegistry deviceRegistry)
        {
            _handler = this;
            _logger = logger;

            _messageForwarders = messageForwarders;
            _userEndpointRegistry = userEndpointRegistry;
            _deviceRegistry = deviceRegistry;
        }

        public async Task ProcessAsync(RoutingMessage routingMessage)
        {
            _logger.LogDebug($"Processing message '{routingMessage.MessageId}' from  {routingMessage.Sender.GetType()} to {routingMessage.Recipient.GetType()}");
            
            if (routingMessage.Sender is DevicePrincipal devicePrincipal)
            {
                // Update
                var sender = await _deviceRegistry.GetOwner(devicePrincipal);

                if (sender is null)
                {
                    _logger.LogError($"Unable to find owner for '{devicePrincipal.ClientId}'. Skipping message.");
                    return;
                }

                routingMessage.Sender = sender;

                _logger.LogDebug($"Upgraded message '{routingMessage.MessageId}' with sender as '{sender.GetType().Name}'");
                await _handler.ProcessAsync(routingMessage);
                return;
            }

            if (routingMessage.Recipient is ContactPrincipal contactPrincipal)
            {
                // Update
                var recipient = await _userEndpointRegistry.GetTarget(contactPrincipal);

                if (recipient is null)
                {
                    _logger.LogError($"Unable to find contact '{contactPrincipal.UserId}' to send message to. Skipping message.");
                    return;
                }

                routingMessage.Recipient = recipient;

                _logger.LogDebug($"Upgraded message '{routingMessage.MessageId}' with recipient as '{recipient.GetType().Name}'");
                await _handler.ProcessAsync(routingMessage);
                return;
            }

            var fromType = routingMessage.Sender.GetType().Name;
            var toType = routingMessage.Recipient.GetType().Name;

            _logger.LogInformation($"Selecting Forwarder for message '{routingMessage.MessageId}'. From: '{fromType}' to: '{toType}'");
            
            var matchingMessageForwarders = _messageForwarders.Where(fwd =>fwd.Supports(routingMessage.Sender.GetType(), routingMessage.Recipient.GetType())).ToList();

            if (!matchingMessageForwarders.Any())
            {
                _logger.LogError($"Missing forwarder from '{fromType}' -> '{toType}' for {routingMessage.MessageId}. Exiting.");
                return;
            }
            else if (matchingMessageForwarders.Count() > 1) 
            {
                _logger.LogError($"Multiple valid forwarders from '{fromType}' -> '{toType}' {routingMessage.MessageId}. Exiting.");
                return;
            }

            try
            {
                var forwarder = matchingMessageForwarders.First();

                _logger.LogInformation($"Forwarding message {routingMessage.MessageId} to {forwarder.GetType().Name}");
                await forwarder.Process(routingMessage);

                _logger.LogInformation("Message processed");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Unable to process message with id {routingMessage.MessageId}");
            }
        }
    }
}
