using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using IoTalkie.Common;
using IoTalkie.Messaging.Channel;
using IoTalkie.Messaging.Channel.Devices;
using IoTalkie.Messaging.Channel.Telegram;
using IoTalkie.Messaging.Processing;

namespace IoTalkie
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllers();

            services.AddHostedService<BotMessagesHandler>();

            services.AddTransient<MessageHandler>();
            services.AddTransient<AudioPayloadStore>();

            services.AddSingleton<DeviceRegistry>();
            services.AddSingleton<UserEndpointRegistry>();
            services.AddSingleton<BotCredentialsRegistry>();


            services.AddTransient<IMessageForwarder, TelegramMessageForwarder>();
            services.AddTransient<IMessageForwarder, DeviceMessageForwarder>();
            
            var configuration = new ConfigurationBuilder()
                .AddJsonFile("appsettings.json", true, true)
                .AddEnvironmentVariables()
                .AddUserSecrets(typeof(Program).Assembly)
                .Build();

            services.Configure<AzureSettings>(configuration.GetSection(typeof(AzureSettings).Name));
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseHttpsRedirection();

            app.UseRouting();

            app.UseAuthorization();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllers();
            });
        }
    }
}
