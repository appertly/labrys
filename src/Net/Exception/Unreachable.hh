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
namespace Labrys\Net\Exception;

/**
 * An exception for connection errors.
 */
class Unreachable extends \RuntimeException implements \Labrys\Net\Exception
{
    private static ImmSet<int> $errors = ImmSet{
        CURLE_OPERATION_TIMEOUTED, CURLE_COULDNT_RESOLVE_HOST,
        CURLE_COULDNT_RESOLVE_PROXY, CURLE_COULDNT_CONNECT, CURLE_SSL_CONNECT_ERROR,
        CURLE_SSL_PEER_CERTIFICATE, CURLE_SSL_CACERT, CURLE_SEND_ERROR,
        CURLE_RECV_ERROR, CURLE_GOT_NOTHING
    };

    /**
     * @var The cURL handle info
     */
    private ImmMap<string,mixed> $info;

    /**
     * @param $info - The cURL handle info
     * @param $msg - The message
     * @param $code - The code
     * @param $previous - Any nested exception
     */
    public function __construct(KeyedContainer<string,mixed> $info, string $msg, int $code = 0, ?\Exception $previous = null)
    {
        $this->info = new ImmMap($info);
        parent::__construct($msg, $code, $previous);
    }

    /**
     * Gets the cURL handle info.
     *
     * @return the handle info
     */
    public function getInfo() : ImmMap<string,mixed>
    {
        return $this->info;
    }

    /**
     * Determines whether the code returned from cURL is one this class supports.
     *
     * @param $code - The code
     * @return whether this class should be used
     */
    public static function isUsable(int $code) : bool
    {
        return self::$errors->contains($code);
    }
}
