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
use Mockery as M;

class JsonHelperTest
{
    use JsonHelper;

    <<Test>>
    public async function testSendItems1(Assert $assert): Awaitable<void>
    {
        $response = new \Zend\Diactoros\Response();
        $pagination = new \Caridea\Http\Pagination(PHP_INT_MAX, 0);
        $items = Vector{'foo', 'bar', 'baz'};
        $output = $this->sendItems($response, $items, $pagination, 3);
        $assert->string((string)$output->getBody())->is(json_encode($items));
        $assert->string($output->getHeaderLine('Content-Range'))->is('items 0-2/3');
    }

    <<Test>>
    public async function testSendItems2(Assert $assert): Awaitable<void>
    {
        $response = new \Zend\Diactoros\Response();
        $pagination = new \Caridea\Http\Pagination(3, 2);
        $items = Vector{'foo', 'bar', 'baz'};
        $output = $this->sendItems($response, $items, $pagination, 5);
        $assert->string((string)$output->getBody())->is(json_encode($items));
        $assert->string($output->getHeaderLine('Content-Range'))->is('items 2-4/5');
    }

    <<Test>>
    public async function testSendItems3(Assert $assert): Awaitable<void>
    {
        $response = new \Zend\Diactoros\Response();
        $pagination = new \Caridea\Http\Pagination(PHP_INT_MAX, 0);
        $items = Vector{'foo', 'bar', 'baz'};
        $output = $this->sendItems($response, $items, $pagination, PHP_INT_MAX);
        $assert->string((string)$output->getBody())->is(json_encode($items));
        $assert->string($output->getHeaderLine('Content-Range'))->is('items 0-' . (PHP_INT_MAX - 1) . '/' . PHP_INT_MAX);
    }

    <<Test>>
    public async function testSendItems4(Assert $assert): Awaitable<void>
    {
        $response = new \Zend\Diactoros\Response();
        $pagination = new \Caridea\Http\Pagination(PHP_INT_MAX, 5);
        $items = Vector{'foo', 'bar', 'baz'};
        $output = $this->sendItems($response, $items, $pagination, PHP_INT_MAX);
        $assert->string((string)$output->getBody())->is(json_encode($items));
        $assert->string($output->getHeaderLine('Content-Range'))->is('items 5-' . (PHP_INT_MAX - 1) . '/' . PHP_INT_MAX);
    }

    <<Test>>
    public async function testSendItems5(Assert $assert): Awaitable<void>
    {
        $response = new \Zend\Diactoros\Response();
        $pagination = new \Caridea\Http\Pagination(PHP_INT_MAX, 0);
        $items = Vector{};
        $output = $this->sendItems($response, $items, $pagination);
        $assert->string((string)$output->getBody())->is(json_encode($items));
        $assert->string($output->getHeaderLine('Content-Range'))->is('items 0-0/0');
    }
}
