namespace IoTalkie.Messaging.Channel.Devices
{
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
}