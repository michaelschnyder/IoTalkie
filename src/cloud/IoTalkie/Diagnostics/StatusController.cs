using IoTalkie.Common;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace IoTalkie.Diagnostics
{
    [ApiController]
    [Route("api/status")]
    public class StatusController : ControllerBase
    {
        private readonly ILogger<StatusController> _logger;
        private readonly AzureSettings _options;

        public StatusController(ILogger<StatusController> logger, IOptions<AzureSettings> options)
        {
            _logger = logger;
            _options = options.Value;
        }

        [HttpGet]
        public IActionResult Get()
        {
            _logger.LogDebug("Status endpoint hit");
            return this.Ok("All fine");
        }
    }
}
