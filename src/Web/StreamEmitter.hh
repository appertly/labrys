<?hh
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
namespace Labrys\Web;

use Psr\Http\Message\ResponseInterface;
use RuntimeException;
use Zend\Diactoros\Response\EmitterInterface;
use Zend\Diactoros\Response\SapiEmitterTrait;

/**
 * A memory-efficient stream emitter
 */
class StreamEmitter implements EmitterInterface
{
    use SapiEmitterTrait;

    /**
     * Emits a response for a PHP SAPI environment.
     *
     * Emits the status line and headers via the header() function, and the
     * body content via the output buffer.
     *
     * @param $response - The response
     * @param $maxBufferLevel - Maximum output buffering level to unwrap.
     */
    public function emit(ResponseInterface $response, ?int $maxBufferLevel = null)
    {
        if (headers_sent()) {
            throw new RuntimeException('Unable to emit response; headers already sent');
        }
        $this->emitStatusLine($response);
        $this->emitHeaders($response);
        $this->flush();
        $this->emitBody($response, $maxBufferLevel);
    }

    /**
     * Emit the message body.
     *
     * Loops through the output buffer, flushing each, before emitting
     * the response body using `echo()`.
     *
     * @param $response - The response
     * @param $maxBufferLevel - Flush up to this buffer level.
     */
    private function emitBody(ResponseInterface $response, ?int $maxBufferLevel)
    {
        $body = $response->getBody();
        $fp = $body->detach();
        rewind($fp);
        stream_copy_to_stream($fp, fopen('php://output', 'w'));
        $body->close();
    }
}
