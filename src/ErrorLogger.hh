<?hh // strict
/**
 * Labrys
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @copyright 2015-2016 Appertly
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */
namespace Labrys;

use Psr\Log\LoggerInterface as Logger;
use Psr\Log\LogLevel;

/**
 * Responsible for logging exceptions
 */
class ErrorLogger implements \Psr\Log\LoggerAwareInterface
{
    use \Psr\Log\LoggerAwareTrait;

    /**
     * @var Stores the Exception class name to log level
     */
    private ImmMap<string,string> $levels;

    /**
     * Creates a new ErrorLogger.
     *
     * If an exception isn't found in the `$levels` map, this class assumes a
     * level of `LogLevel::ERROR`.
     *
     * ```hack
     * $elog = new ErrorLogger(
     *     $logger,
     *     Map {"MyException" => LogLevel::DEBUG, "RuntimeException" => LogLevel::WARN}
     * );
     * ```
     *
     * @param $logger - The logger; will use `Psr\Log\NullLogger` by default
     * @param $levels - Map of Exception names to log levels. Order matters!
     */
    public function __construct(?Logger $logger = null, ?\ConstMap<string,string> $levels = null)
    {
        $this->logger = $logger ? $logger : new \Psr\Log\NullLogger();
        $this->levels = $levels ? $levels->toImmMap() : ImmMap{};
    }

    /**
     * Logs an exception.
     *
     * If an exception isn't found in the `$levels` map, this class assumes a
     * level of `LogLevel::ERROR`.
     *
     * @param $e - The exception to log
     */
    public function log(\Exception $e) : void
    {
        $out = LogLevel::ERROR;
        foreach ($this->levels as $class => $level) {
            /* HH_IGNORE_ERROR[4026]: We know this works */
            if ($e instanceof $class) {
                $out = $level;
                break;
            }
        }
        /*  HH_IGNORE_ERROR[4064]: This isn't null, we set it in the constructor */
        $this->logger->log($out, $e->getMessage(), ['exception' => $e]);
    }
}
