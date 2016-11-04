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
 * @license   Apache-2.0
 */
namespace Labrys\Http;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

/**
 * CSRF token verification plugin for the middleware dispatcher.
 *
 * @since 0.6.5
 */
class SeaSurfer implements \Labrys\Route\Plugin
{
    /**
     * The HTTP default methods
     */
    protected static array<string> $defaultMethods = ['POST', 'PUT', 'DELETE'];

    /**
     * The HTTP methods where this verification will happen
     */
    protected ImmSet<string> $methods;

    /**
     * Creates a new SeaSurfer.
     *
     * @param $plugin - The CSRF session plugin
     * @param $errorLogger - The error logger
     * @param $field - The body field in which to find the CSRF token
     * @param $hostname - The expected hostname to match
     * @param $blockMissingSource - If we should stop requests with no Origin/Referer
     * @param $methods - The HTTP methods under which this check is run
     */
    public function __construct(
        protected \Caridea\Session\CsrfPlugin $plugin,
        protected \Labrys\ErrorLogger $errorLogger,
        protected string $field = 'csrfToken',
        protected string $hostname = '',
        protected bool $blockMissingSource = true,
        Traversable<string> $methods = [],
    ) {
        $this->methods = new ImmSet($methods ?: self::$defaultMethods);
    }

    /**
     * Gets the plugin priority; larger means first.
     *
     * @return - The plugin priority
     */
    public function getPriority(): int
    {
        return 1999999;
    }

    /**
     * Allows a plugin to issue a response before the request is dispatched.
     *
     * @param $request - The server request
     * @param $response - The response
     * @return - The response
     */
    public function __invoke(Request $request, Response $response, (function (Request,Response): Response) $next): Response
    {
        if ($this->methods->contains($request->getMethod())) {
            try {
                $principal = $request->getAttribute('principal');
                invariant($principal instanceof \Caridea\Auth\Principal, "Request attribute 'principal' must be a Principal");
                if (!$principal->isAnonymous()) {
                    $this->verifyOrigin($request);
                    $this->verifyToken($request);
                }
            } catch (\OutOfBoundsException $e) {
                $this->errorLogger->log($e);
                $response->getBody()->write($e->getMessage());
                return $response->withStatus(449, "Retry With");
            } catch (\UnexpectedValueException $e) {
                $this->errorLogger->log($e);
                $response->getBody()->write($e->getMessage());
                return $response->withStatus(440, "Login Timeout");
            }
        }
        return $next($request, $response);
    }

    /**
     * Performs the verification.
     */
    protected function verifyOrigin(Request $request): void
    {
        $source = strstr($request->getHeaderLine('Origin'), ' ', true) ?:
            $request->getHeaderLine('Referer');
        if (!$source && $this->blockMissingSource) {
            throw new \OutOfBoundsException("CSRF: Origin and Referer headers missing");
        }
        $hostname = $this->hostname;
        if (!$hostname) {
            $hostname = $request->getHeaderLine('Host');
            $forwardedHost = $request->getHeaderLine('X-Forwarded-Host');
            if ($forwardedHost) {
                $hostname = $forwardedHost;
            }
        }
        $sourceUrl = parse_url($source);
        $sourceHost = $source ? $sourceUrl['host'] : '';
        if ($source && strpos($hostname, ':') !== false) {
            $sourceHost .= ':' . $sourceUrl['port'];
        }
        if ($source && strcasecmp($sourceHost, $hostname) !== 0) {
            throw new \UnexpectedValueException("CSRF: Unauthenticated session");
        }
    }

    /**
     * Performs the verification.
     */
    protected function verifyToken(Request $request): void
    {
        if ($request->getHeaderLine('X-Requested-With') == 'XMLHttpRequest') {
            return;
        }
        $body = $request->getParsedBody();
        $token = $body instanceof KeyedContainer ? ($body[$this->field] ?? null) : null;
        if (!$this->plugin->isValid($token ?? '')) {
            throw new \UnexpectedValueException("CSRF: Unauthenticated session");
        }
    }
}
