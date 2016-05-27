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
 * @license   Apache-2.0
 */
namespace Labrys\Http;

use HackPack\HackUnit\Contract\Assert;

class RescuerTest
{
    <<Test>>
    public async function testRun1(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 406;
        $next = function ($req, $res) use ($status) {
            try {
                $c = new \Caridea\Container\Objects([]);
                $c->named('foo', \RuntimeException::class);
                return $res;
            } catch (\Exception $e) {
                throw new \Labrys\Route\Exception\Unroutable("Could not route", $status, $e);
            }
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Could not route");
        $assert->string($details['detail'])->is("We don't have any content available in the MIME type you specified in your Accept header. Try specifying additional MIME types.");
        $assert->string($details['exception']['class'])->is(\Labrys\Route\Exception\Unroutable::class);
        $assert->string($details['exception']['message'])->is("Could not route");
        $assert->string($details['exception']['previous']['class'])->is(\UnexpectedValueException::class);
        $assert->string($details['exception']['previous']['message'])->is("A RuntimeException was requested, but null was found");
    }

    <<Test>>
    public async function testRun2(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 405;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Route\Exception\Unroutable("Method Not Allowed", $status, null, ImmMap{'Allows' => 'application/json'});
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getHeaderLine('Allows'))->is('application/json');
        $assert->int($out->getStatusCode())->eq($status);
        $details = (string)$out->getBody();
        $body = '<!DOCTYPE html><html lang="en"><head><meta charset="utf-8">'
            . '<title>Method Not Allowed</title></head><body><header>'
            . '<h1>Method Not Allowed</h1></header><main role="main">'
            . "<p>You can't use that HTTP method for this URL. "
            . "Check the Allow response header for the ones you can.</p>"
            . "<div><h2>Labrys\Route\Exception\Unroutable</h2><p>Method Not Allowed</p><pre>#0";
        $assert->bool(substr($details, 0, strlen($body)) === $body)->is(true);
    }

    <<Test>>
    public async function testRun3(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 404;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Route\Exception\Unroutable("Not Found", $status);
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Not Found');
        $assert->int($out->getStatusCode())->eq($status);
    }

    <<Test>>
    public async function testRun4(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 404;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Db\Exception\Retrieval();
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Not Found');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Resource Not Found");
        $assert->string($details['detail'])->is("We don't have anything at this URL. Double-check the URL you requested.");
    }

    <<Test>>
    public async function testRun5(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 403;
        $next = function ($req, $res) use ($status) {
            throw new \Caridea\Acl\Exception\Forbidden();
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Forbidden');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Access Denied");
        $assert->string($details['detail'])->is("You are not allowed to perform this action.");
    }

    <<Test>>
    public async function testRun6(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 409;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Db\Exception\Concurrency();
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Conflict');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Concurrent Modification");
        $assert->string($details['detail'])->is("Someone else saved changes to this same data while you were editing. Try your request again using the latest copy of the record.");
    }

    <<Test>>
    public async function testRun7(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 409;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Db\Exception\Integrity();
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Conflict');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Constraint Violation");
        $assert->string($details['detail'])->is("The data you submitted violates unique constraints. Most likely, this is a result of an existing record with similar data. Double-check existing records and try again.");
    }

    <<Test>>
    public async function testRun8(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 422;
        $next = function ($req, $res) use ($status) {
            throw new \Caridea\Validate\Exception\Invalid(['foobar' => 'REQUIRED']);
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Unprocessable Entity');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Data Validation Failure");
        $assert->string($details['detail'])->is("There was a problem with the data you submitted. Review the messages for each field and try again.");
        $assert->mixed($details['errors'])->looselyEquals([['field' => 'foobar', 'code' => 'REQUIRED']]);
    }

    <<Test>>
    public async function testRun9(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 423;
        $next = function ($req, $res) use ($status) {
            throw new \Labrys\Db\Exception\Locked();
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Locked');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Resource Locked");
        $assert->string($details['detail'])->is("This data is locked. You have permission, but it is no longer allowed to be changed.");
    }

    <<Test>>
    public async function testRun10(Assert $assert): Awaitable<void>
    {
        $object = new Rescuer(Map{'debug' => true});
        $status = 500;
        $next = function ($req, $res) use ($status) {
            throw new \RuntimeException("Weird stuff");
        };
        $server = ['HTTP_ACCEPT' => 'application/json'];
        $request = new \Zend\Diactoros\ServerRequest($server);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getReasonPhrase())->is('Internal Server Error');
        $assert->int($out->getStatusCode())->eq($status);
        $details = json_decode((string)$out->getBody(), true);
        $assert->int($details['status'])->eq($status);
        $assert->string($details['title'])->is("Internal Server Error");
        $assert->string($details['detail'])->is("It looks like we have a problem on our end! Our staff has been notified. Please try again later.");
    }
}
