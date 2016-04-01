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
    protected static ImmMap<int,string> $messages = ImmMap{
        403 => "You are not allowed to perform this action.",
        404 => "We don't have anything at this URL. Double-check the URL you requested.",
        405 => "You can't use that HTTP method for this URL. Check the Allow response header for the ones you can.",
        406 => "We don't have any content available in the MIME type you specified in your Accept header. Try specifying additional MIME types.",
        422 => "There was a problem with the data you submitted. Review the messages below and try again.",
        423 => "This data is locked. You have permission, but it is no longer allowed to be changed.",
        500 => "It looks like we have a problem on our end! Our staff has been notified. Please try again later."
    };

    /**
     * Creates a new SimpleContingency.
     *
     * @param $debug - Whether to include exception stack trace information (should be false in production!). Defaults to false.
     * @param $site - The name of this application. Defaults to "Labrys".
     * @param $authUrl - The URL for authentication form. Defaults to "/auth/login".
     */
    public function __construct(private bool $debug = false, private string $site = "Labrys", private string $authUrl = "/auth/login")
    {
    }

    /**
     * Handles an exception.
     *
     * This is your chance for logging, changing the HTTP status header, and
     * rendering some kind of message for the client.
     *
     * @param $e The exception to process
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
                if ($needLogin) {
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
            $values['title'] = $e->getMessage();
            $values['status'] = 403;
            $values['detail'] = self::$messages[403];
        } elseif ($e instanceof \Labrys\Db\Exception\Retrieval) {
            $values['title'] = $e->getMessage();
            $values['status'] = 404;
            $values['detail'] = self::$messages[404];
        } elseif ($e instanceof \Labrys\Db\Exception\Integrity) {
            $values['title'] = $e->getMessage();
            $values['status'] = 409;
            $values['detail'] = 'The data you submitted violates unique constraints. Most likely, this is a result of an existing record with similar data. Double-check existing records and try again.';
        } elseif ($e instanceof \Labrys\Db\Exception\Concurrency) {
            $values['title'] = $e->getMessage();
            $values['status'] = 409;
            $values['detail'] = 'Someone else saved changes to this same data while you were editing. Try your request again using the latest copy of the record.';
        } elseif ($e instanceof \Caridea\Validate\Exception\Invalid) {
            $values['title'] = 'Data Validation Failure';
            $values['status'] = 422;
            $values['detail'] = self::$messages[422];
            $errors = [];
            foreach ($e->getErrors() as $field => $code) {
                $errors[] = ['field' => $field, 'code' => $code];
            }
            $extra['errors'] = $errors;
        } elseif ($e instanceof \Labrys\Db\Exception\Locked) {
            $values['title'] = $e->getMessage();
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
            new \Zend\Diactoros\Uri('http://httpstatus.es/' . ((string) $values['status'])),
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
        return <page:error site={$this->site}>
            <h1>{$values['title']}</h1>
            <p>{$values['detail']}</p>
            {$frag}
        </page:error>;
    }
}
