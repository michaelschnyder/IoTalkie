using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace IoTalkie.Controllers
{
    [ApiController]
    [Route("api/status")]
    public class StatusController : ControllerBase
    {
        private readonly ILogger<StatusController> _logger;

        public StatusController(ILogger<StatusController> logger)
        {
            _logger = logger;
        }

        [HttpGet]
        public IActionResult Get()
        {
            _logger.LogDebug("Status endpoint hit");
            return this.Ok("All fine");
        }
    }
}
