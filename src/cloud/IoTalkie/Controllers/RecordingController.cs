using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;

namespace IoTalkie.Controllers
{
    [Route("api/recording")]
    [ApiController]
    public class RecordingController : ControllerBase
    {
        private readonly ILogger<RecordingController> _logger;

        public RecordingController(ILogger<RecordingController> logger)
        {
            _logger = logger;
        }

        [HttpPost]
        public async Task<IActionResult> AddNewRecordAsync()
        {
            try
            {
                var filename = Path.Combine("messages", Path.GetRandomFileName().Replace(".", "") + ".wav");

                using (var file = new FileStream(filename, FileMode.CreateNew))
                {
                    await Request.Body.CopyToAsync(file);
                }
                
                
                Debug.WriteLine($"Saved to '{Path.GetFullPath(filename) }'");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
            return this.Ok();
        }
    }
}
