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
namespace Labrys\Db;

use HackPack\HackUnit\Contract\Assert;

class CursorSubsetTest
{
    <<Test>>
    public async function testVector(Assert $assert): Awaitable<void>
    {
        $in = Vector{'foo', 'bar', 'baz'};
        $object = new CursorSubset($in, 5);
        $assert->int($object->getTotal())->eq(5);
        $assert->mixed($object->getInnerIterator())->looselyEquals($in->getIterator());
        $assert->container($object->toArray())->containsOnly(['foo', 'bar', 'baz']);
    }

    <<Test>>
    public async function testArrayObject(Assert $assert): Awaitable<void>
    {
        $in = new \ArrayObject(['foo', 'bar', 'baz']);
        $object = new CursorSubset($in, 4);
        $assert->int($object->getTotal())->eq(4);
        $assert->mixed($object->getInnerIterator())->looselyEquals($in->getIterator());
        $assert->container($object->toArray())->containsOnly(['foo', 'bar', 'baz']);
    }

    <<Test>>
    public async function testIterator(Assert $assert): Awaitable<void>
    {
        $in = new \ArrayIterator(['foo', 'bar', 'baz']);
        $object = new CursorSubset($in, 4);
        $assert->int($object->getTotal())->eq(4);
        $assert->mixed($object->getInnerIterator())->identicalTo($in);
        $assert->container($object->toArray())->containsOnly(['foo', 'bar', 'baz']);
    }

    <<Test>>
    public async function testIterator2(Assert $assert): Awaitable<void>
    {
        $in = new \EmptyIterator();
        $object = new CursorSubset($in, 4);
        $assert->int($object->getTotal())->eq(4);
        $assert->mixed($object->getInnerIterator())->identicalTo($in);
        $assert->container($object->toArray())->isEmpty();
    }

    <<Test>>
    public async function testException(Assert $assert): Awaitable<void>
    {
        $assert->whenCalled(function () {
            new CursorSubset(Vector{}, -1);
        })->willThrowClassWithMessage(\RangeException::class, "Total cannot be a negative number");
    }
}
