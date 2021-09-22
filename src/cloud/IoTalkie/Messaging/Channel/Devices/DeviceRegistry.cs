namespace IoTalkie.Messaging.Channel.Devices
{
    public class DeviceRegistry
    {
        
        public ContactPrincipal GetOwner(DevicePrincipal principal)
        {
            if (principal.ClientId == "Gander-fcf5c42f71c0")
            {
                var emmaUserId = "324238947";
                return new ContactPrincipal(emmaUserId, principal);
            }

            return null;
        }
    }
}