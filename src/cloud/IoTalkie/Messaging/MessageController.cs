using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace IoTalkie.Messaging
{
    [Route("api/message")]
    [ApiController]
    public class MessageController : ControllerBase
    {
        private readonly ILogger<MessageController> _logger;

        public MessageController(ILogger<MessageController> logger)
        {
            _logger = logger;
        }

        [HttpPost("{messageId}")]
        public async Task<IActionResult> SubmitMessage(string messageId, string recipientId)
        {
            try
            {
                var userAgent = GetHeaderValue("User-Agent");
                var clientId = GetHeaderValue("ClientId");


                string path2 = $"to_{recipientId}-{Path.GetRandomFileName().Replace(".", "")}.wav";

                using (var file = new FileStream(Path.Combine("messages", path2), FileMode.CreateNew))
                {
                    await Request.Body.CopyToAsync(file);
                }


                Debug.WriteLine($"Saved to '{Path.GetFullPath(Path.Combine("messages", path2)) }'");
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
}
