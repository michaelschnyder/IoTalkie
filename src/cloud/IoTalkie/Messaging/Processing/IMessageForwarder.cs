using System;
using System.Threading.Tasks;

namespace IoTalkie.Messaging.Processing
{
    public interface IMessageForwarder
    {
        public abstract bool Supports(Type from, Type to);
        public abstract Task Process(RoutingMessage routingMessage);
    }
}