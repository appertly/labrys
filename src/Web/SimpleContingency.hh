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

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;
use Caridea\Http\ProblemDetails;

/**
 * A pretty basic Contingency.
 */
class SimpleContingency implements Contingency
{
    /**
     * @var Whether to include exception information in responses
     */
    protected bool $debug;
    /**
     * @var The name of this site; used only in HTML responses
     */
    protected string $site;
    /**
     * @var The URL for the standalone login form
     */
    protected string $authUrl;

    /**
     * @var Convenient map of HTTP status codes to human-readable explanations
     */
    protected static ImmMap<int,string> $messages = ImmMap{
        403 => "You are not allowed to perform this action.",
        404 => "We don't have anything at this URL. Double-check the URL you requested.",
        405 => "You can't use that HTTP method for this URL. Check the Allow response header for the ones you can.",
        406 => "We don't have any content available in the MIME type you specified in your Accept header. Try specifying additional MIME types.",
        422 => "There was a problem with the data you submitted. Review the messages for each field and try again.",
        423 => "This data is locked. You have permission, but it is no longer allowed to be changed.",
        500 => "It looks like we have a problem on our end! Our staff has been notified. Please try again later."
    };

    /**
     * Creates a new SimpleContingency.
     *
     * The following options are available:
     * * `options` – Whether to include exception stack trace information (should be false in production!). Defaults to false.
     * * `site` – The name of this application (used in HTML responses)
     * * `authUrl` – The URL for a standalone login form
     *
     * @param $options - The options
     */
    public function __construct(\ConstMap<string,mixed> $options = ImmMap{})
    {
        $this->debug = (bool)($options['debug'] ?? false);
        $this->site = (string)($options['site'] ?? '');
        $this->authUrl = (string)($options['authUrl'] ?? '');
    }

    /**
     * Handles an exception.
     *
     * This is your chance for logging, changing the HTTP status header, and
     * rendering some kind of message for the client.
     *
     * @param $request - The request
     * @param $response - The response
     * @param $e - The exception to process
     * @return The new response
     */
    public function process(Request $request, Response $response, \Exception $e) : Response
    {
        $needLogin = false;
        if ($e instanceof Exception\Unroutable) {
            $needLogin = $e->getCode() === 403;
            $response = $response->withStatus($e->getCode(), $e->getMessage());
            foreach ($e->getHeaders() as $k => $v) {
                $response = $response->withHeader($k, $v);
            }
        } elseif ($e instanceof \Caridea\Acl\Exception\Forbidden) {
            $response = $response->withStatus(403, 'Forbidden');
        } elseif ($e instanceof \Labrys\Db\Exception\Retrieval) {
            $response = $response->withStatus(404, 'Not Found');
        } elseif ($e instanceof \Labrys\Db\Exception\Concurrency ||
            $e instanceof \Labrys\Db\Exception\Integrity) {
            $response = $response->withStatus(409, 'Conflict');
        } elseif ($e instanceof \Caridea\Validate\Exception\Invalid) {
            $response = $response->withStatus(422, 'Unprocessable Entity');
        } elseif ($e instanceof \Labrys\Db\Exception\Locked) {
            $response = $response->withStatus(423, 'Locked');
        } else {
            $response = $response->withStatus(500, 'Internal Server Error');
        }
        $values = $this->getValues($request, $e);
        $types = new \Caridea\Http\AcceptTypes($request->getServerParams());
        switch ($types->preferred(['application/json', 'text/html'])) {
            /* HH_IGNORE_ERROR[4110]: Not sure why hh_client has a problem with this */
            case 'application/json':
                $response = $response->withHeader('Content-Type', ProblemDetails::MIME_TYPE_JSON);
                $response->getBody()->write((string)$this->renderJson($request, $e, $values));
                break;
            default:
                if ($needLogin && strlen($this->authUrl) > 0) {
                    $response = $response->withStatus(303)->withHeader(
                        'Location', $this->authUrl . '?then=' . (string)$request->getRequestTarget()
                    );
                } else {
                    $response = $response->withHeader('Content-Type', 'text/html');
                    $response->getBody()->write((string)$this->renderHtml($request, $e, $values));
                }
        }
        return $response;
    }

    protected function getValues(Request $request, \Exception $e) : Map<string,mixed>
    {
        $values = Map{};
        $extra = Map{'success' => false};
        if ($this->debug) {
            $extra['exception'] = get_class($e);
            $extra['stack'] = $e->getTraceAsString();
            $extra['message'] = $e->getMessage();
        }
        if ($e instanceof Exception\Unroutable) {
            $code = $e->getCode();
            $values['title'] = $e->getMessage();
            $values['status'] = $code;
            $values['detail'] = self::$messages[$code];
        } elseif ($e instanceof \Caridea\Acl\Exception\Forbidden) {
            $values['title'] = 'Access Denied';
            $values['status'] = 403;
            $values['detail'] = self::$messages[403];
        } elseif ($e instanceof \Labrys\Db\Exception\Retrieval) {
            $values['title'] = 'Resource Not Found';
            $values['status'] = 404;
            $values['detail'] = self::$messages[404];
        } elseif ($e instanceof \Labrys\Db\Exception\Integrity) {
            $values['title'] = 'Constraint Violation';
            $values['status'] = 409;
            $values['detail'] = 'The data you submitted violates unique constraints. Most likely, this is a result of an existing record with similar data. Double-check existing records and try again.';
        } elseif ($e instanceof \Labrys\Db\Exception\Concurrency) {
            $values['title'] = 'Concurrent Modification';
            $values['status'] = 409;
            $values['detail'] = 'Someone else saved changes to this same data while you were editing. Try your request again using the latest copy of the record.';
        } elseif ($e instanceof \Caridea\Validate\Exception\Invalid) {
            $values['title'] = 'Data Validation Failure';
            $values['status'] = 422;
            $values['detail'] = self::$messages[422];
            $errors = Vector{};
            foreach ($e->getErrors() as $field => $code) {
                $errors[] = Map{'field' => $field, 'code' => $code};
            }
            $extra['errors'] = $errors;
        } elseif ($e instanceof \Labrys\Db\Exception\Locked) {
            $values['title'] = 'Resource Locked';
            $values['status'] = 423;
            $values['detail'] = self::$messages[423];
        } else {
            $values['title'] = 'Internal Server Error';
            $values['status'] = 500;
            $values['detail'] = self::$messages[500];
        }
        $values['extra'] = $extra;
        return $values;
    }

    protected function renderJson(Request $request, \Exception $e, Map<string,mixed> $values) : ProblemDetails
    {
        return new ProblemDetails(
            null,
            (string) $values['title'],
            (int) $values['status'],
            (string) $values['detail'],
            null,
            (array) $values['extra']
        );
    }

    protected function renderHtml(Request $request, \Exception $e, Map<string,mixed> $values) : \XHPRoot
    {
        $frag = <x:frag/>;
        $extra = $values['extra'];
        if ($extra instanceof Map){
            $errors = $extra->get('errors');
            if ($errors instanceof Traversable) {
                $ul = <ul/>;
                foreach ($errors as $err) {
                    $err = $err instanceof Map ? $err : ImmMap{};
                    $ul->appendChild(<li>{$err['field'] ?? ''}: {$err['code'] ?? ''}</li>);
                }
                $frag->appendChild($ul);
            }
            if ($extra->containsKey('exception')) {
                $frag->appendChild(<h2>{$extra['exception']}</h2>);
            }
            if ($extra->containsKey('message')) {
                $frag->appendChild(<p>{$extra['message']}</p>);
            }
            if ($extra->containsKey('stack')) {
                $frag->appendChild(<pre>{$extra['stack']}</pre>);
            }
        }
        return <x:doctype>
            <html lang="en">
                <head>
                    <meta charset="utf-8"/>
                    <title>{$values['title']} | {$this->site}</title>
                </head>
                <body>
                    <header>
                        <h1>{$values['title']}</h1>
                    </header>
                    <main role="main">
                        <p>{$values['detail']}</p>
                        {$frag}
                    </main>
                </body>
            </html>
        </x:doctype>;
    }
}
