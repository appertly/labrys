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
namespace Labrys;

use HackPack\HackUnit\Contract\Assert;

class GetterTest
{
    <<Test>>
    public async function testGet(Assert $assert): Awaitable<void>
    {
        $array = ['foo' => 'bar'];
        $assert->mixed(Getter::get($array, 'foo'))->looselyEquals('bar');
        $assert->mixed(Getter::get($array, 'faz'))->identicalTo(null);

        $map = Map{'bar' => 'foo'};
        $assert->mixed(Getter::get($map, 'bar'))->looselyEquals('foo');
        $assert->mixed(Getter::get($map, 'faz'))->identicalTo(null);

        $object = (object)['abc' => 123];
        $assert->mixed(Getter::get($object, 'abc'))->looselyEquals(123);
        $assert->mixed(Getter::get($object, 'def'))->identicalTo(null);

        $assert->mixed(Getter::get($this, 'foo'))->looselyEquals('bar');
    }

    <<Test>>
    public async function testGetId(Assert $assert): Awaitable<void>
    {
        $assert->mixed(Getter::getId([]))->identicalTo(null);

        $array = ['_id' => 'bar'];
        $assert->mixed(Getter::getId($array))->looselyEquals('bar');

        $map = Map{'_id' => 'foo'};
        $assert->mixed(Getter::getId($map))->looselyEquals('foo');

        $object = (object)['_id' => 123];
        $assert->mixed(Getter::getId($object))->looselyEquals(123);

        $object2 = (object)['id' => 123];
        $assert->mixed(Getter::getId($object2))->looselyEquals(123);

        $assert->mixed(Getter::getId($this))->looselyEquals('foobar');
    }

    public function getFoo()
    {
        return 'bar';
    }

    public function getId()
    {
        return 'foobar';
    }
}
