namespace IoTalkie.Messaging.Channel.Devices
{
    public class DevicePrincipal : Principal
    {
        private readonly Principal _original;
        private readonly string _clientId;
        private readonly string _userAgent;

        public DevicePrincipal(string clientId, string userAgent)
        {
            _clientId = clientId;
            _userAgent = userAgent;
        }

        public DevicePrincipal(string clientId, Principal original)
        {
            _clientId = clientId;
            _original = original;
        }

        public string ClientId => _clientId;
    }
}