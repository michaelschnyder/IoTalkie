namespace IoTalkie.Messaging.Channel
{
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
}