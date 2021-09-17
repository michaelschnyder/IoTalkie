using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Options;
using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using System.Threading.Tasks;

namespace IoTalkie.MessageSenderCmd
{
    public static class ServiceProviderBuilder
    {
        public static IServiceProvider GetServiceProvider(string[] args)
        {
            var configuration = new ConfigurationBuilder()
                .AddJsonFile("appsettings.json", true, true)
                .AddEnvironmentVariables()
                .AddUserSecrets(typeof(Program).Assembly)
                .Build();

            var services = new ServiceCollection();

            services.Configure<AzureSettings>(configuration.GetSection(typeof(AzureSettings).Name));
            services.AddSingleton<SendMessageFunctionality>();
            services.AddSingleton<UpdateFirmwareFunctionality>();

            var provider = services.BuildServiceProvider();
            return provider;
        }
    }

    class Program
    {
        private static IServiceProvider services;

        static int Main(string[] args)
        {
            services = ServiceProviderBuilder.GetServiceProvider(args);
            var options = services.GetRequiredService<IOptions<AzureSettings>>();

            Command sendMessage = new Command("message", "Send a voice message to a device from a specific user")
            {
                new Argument<string>("file", "The message to be sent"),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
                new Argument<string>("senderId", "From which user the message should come from"),
            };

            sendMessage.Handler = CommandHandler.Create<string, string, string, IConsole>(SendMessageAsync);

            Command updateFirmware = new Command("updateFirmware", "Upload firmware and advice a single device download to update.")
            {
                new Argument<string>("file", "The file to be sent."),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
            };

            updateFirmware.Handler = CommandHandler.Create<string, string, IConsole>(UpdateFirmwareAsync);

            var cmd = new RootCommand { sendMessage, updateFirmware };

            return cmd.Invoke(args);
        }

        private static async Task SendMessageAsync(string file, string deviceId, string senderId, IConsole arg4)
        {
            await services.GetService<SendMessageFunctionality>().Run(file, deviceId, senderId);
        }
        private static async Task UpdateFirmwareAsync(string file, string deviceId, IConsole arg4)
        {
            await services.GetService<UpdateFirmwareFunctionality>().Run(file, deviceId);
        }
    }

    public class NewMessageCmd
    {
        public string Cmd { get; set; } = "newMessage";

        public string MessageId { get; set; } = Guid.NewGuid().ToString();

        public long Timestamp { get; set; } = DateTimeOffset.Now.ToUnixTimeSeconds();
        
        public string SenderId { get; set; }

        public long Size { get; set; }

        public string RemoteUrl { get; set; }
    }

    public class UpdateFirmwareCmd
    {
        public string Cmd { get; set; } = "updateFirmware";

        public string MessageId { get; set; } = Guid.NewGuid().ToString();

        public long Timestamp { get; set; } = DateTimeOffset.Now.ToUnixTimeSeconds();


        public long Size { get; set; }

        public string RemoteUrl { get; set; }
    }

}