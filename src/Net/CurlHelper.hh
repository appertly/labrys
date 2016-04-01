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
namespace Labrys\Net;

/**
 * A trait for using cURL
 */
trait CurlHelper
{
    /**
     * Does a decent job of sending off a PSR-7 request using cURL
     *
     * @param $request - The request
     * @return The async response
     */
    protected function send(\Psr\Http\Message\RequestInterface $request) : Awaitable<string>
    {
        $ch = curl_init((string)$request->getUri());
        if ($request->getMethod() != 'GET') {
            curl_setopt($ch, CURLOPT_CUSTOMREQUEST, $request->getMethod());
            curl_setopt($ch, CURLOPT_POSTFIELDS, (string)$request->getBody());
        }
        $uri = $request->getUri();
        if ($uri->getScheme() == 'http' && $uri->getPort() !== null && $uri->getPort() !== 80) {
            curl_setopt($ch, CURLOPT_PORT, $uri->getPort());
        } elseif ($uri->getScheme() == 'https' && $uri->getPort() !== null && $uri->getPort() !== 443) {
                curl_setopt($ch, CURLOPT_PORT, $uri->getPort());
        }
        $version = $request->getProtocolVersion();
        if ($version == 1.1) {
            curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        } elseif ($version == 2.0) {
            curl_setopt($ch, CURLOPT_HTTP_VERSION,
                defined('CURL_HTTP_VERSION_2_0') ? constant('CURL_HTTP_VERSION_2_0') : 3);
        } else {
            curl_setopt($ch, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
        }
        $headers = [];
        foreach ($request->getHeaders() as $k => $v) {
            $headers[] = "$k: " . implode(", ", $v);
        }
        if (!$request->hasHeader('User-Agent')) {
            $headers[] = "User-Agent: " . $this->getUserAgent();
        }
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        return $this->exec($ch);
    }

    /**
     * Gets a User-Agent string.
     *
     * @return a reasonable user-agent string
     */
    protected function getUserAgent() : string
    {
        if (extension_loaded('curl') && function_exists('curl_version')) {
            return 'curl/' . curl_version()['version'];
        } else {
            $ua = ini_get('user_agent');
            return strlen($ua) > 0 ? $ua : 'PHP/' . PHP_VERSION;
        }
    }

    /**
     * A convenience wrapper around
     * [`curl_multi_await`](/hack/reference/function/curl_multi_await/).
     *
     * Pass a cURL handle, or, more simply, a string containing a URL (and the
     * cURL handle will be created for you), and the cURL request will be executed
     * via async and the `string` result will be retuned.
     *
     * @param $urlOrHandle - An existing cURL handle or a URL as a `string`. String
     *                       URLs will create a default cURL GET handle.
     * @return Awaitable<string> - An `Awaitable` representing the `string` result
     *                             of the cURL request.
     */
    protected async function exec(mixed $urlOrHandle): Awaitable<string>
    {
        if (is_string($urlOrHandle)) {
            $ch = curl_init($urlOrHandle);
        } else if (is_resource($urlOrHandle) && get_resource_type($urlOrHandle) == "curl") {
            $ch = $urlOrHandle;
        } else {
            throw new \InvalidArgumentException(__FUNCTION__ . " expects string or cURL handle");
        }
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_FAILONERROR, false);
        curl_setopt($ch, CURLINFO_HEADER_OUT, true);
        $mh = curl_multi_init();
        curl_multi_add_handle($mh, $ch);
        $sleep_ms = 10;
        do {
            $active = 1;
            do {
                $status = curl_multi_exec($mh, $active);
            } while ($status == CURLM_CALL_MULTI_PERFORM);
            if (!$active) {
                break;
            }
            $select = await curl_multi_await($mh);
            /* If cURL is built without ares support, DNS queries don't have a socket
            * to wait on, so curl_multi_await() (and curl_select() in PHP5) will return
            * -1, and polling is required.
            */
            if ($select == -1) {
                await SleepWaitHandle::create($sleep_ms * 1000);
                if ($sleep_ms < 1000) {
                    $sleep_ms *= 2;
                }
            } else {
                $sleep_ms = 10;
            }
        } while ($status === CURLM_OK);
        $info = curl_multi_info_read($mh);
        $code = $info['result'];
        $content = curl_multi_getcontent($ch);
        $cinfo = curl_getinfo($ch);
        if ($code !== CURLE_OK) {
            if (Exception\Unreachable::isUsable($code)) {
                throw new Exception\Unreachable($cinfo, curl_error($ch), $code);
            } elseif (Exception\Misconfigured::isUsable($code)) {
                throw new Exception\Misconfigured($cinfo, curl_error($ch), $code);
            } else {
                throw new Exception\Unexpected($content, $cinfo, curl_error($ch), $code);
            }
        } elseif ($cinfo['http_code'] >= 400) {
            throw new Exception\Unexpected($content, $cinfo, "The requested URL returned error: " . $cinfo['http_code'], CURLE_HTTP_NOT_FOUND);
        }
        curl_multi_remove_handle($mh, $ch);
        curl_multi_close($mh);
        return (string)$content;
    }
}
