using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;

namespace IoTalkie.Controllers
{
    [Route("api/message")]
    [ApiController]
    public class RecordingController : ControllerBase
    {
        private readonly ILogger<RecordingController> _logger;

        public RecordingController(ILogger<RecordingController> logger)
        {
            _logger = logger;
        }

        [HttpPost("{recipientId}")]
        public async Task<IActionResult> AddNewRecordAsync(string recipientId)
        {
            try
            {
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
    }
}
