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
            services.AddSingleton<UpdateContactsFunctionality>();

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

            var sendMessage = new Command("message", "Send a voice message to a device from a specific user")
            {
                new Argument<string>("file", "The message to be sent"),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
                new Argument<string>("senderId", "From which user the message should come from"),
            };
            sendMessage.Handler = CommandHandler.Create<string, string, string, IConsole>(SendMessageAsync);

            var updateFirmware = new Command("updateFirmware", "Upload firmware and advice a single device download to update.")
            {
                new Argument<string>("file", "The file to be sent."),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
            };
            updateFirmware.Handler = CommandHandler.Create<string, string, IConsole>(UpdateFirmwareAsync);

            var updateContacts = new Command("updateContacts", "Upload contacts.json and advice single device to download & update.")
            {
                new Argument<string>("file", "The file to be sent."),
                new Argument<string>("deviceId", "Target device the message should be sent to"),
            };
            updateContacts.Handler = CommandHandler.Create<string, string, IConsole>(UpdateContactsAsync);

            var cmd = new RootCommand { sendMessage, updateFirmware, updateContacts };

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
        private static async Task UpdateContactsAsync(string file, string deviceId, IConsole arg4)
        {
            await services.GetService<UpdateContactsFunctionality>().Run(file, deviceId);
        }
    }
}