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
 * @copyright 2015-2017 Appertly
 * @license   Apache-2.0
 */
namespace Labrys\View;

use HackPack\HackUnit\Contract\Assert;

class AuthRuleTest
{
    <<Test>>
    public async function testBasic(Assert $assert): Awaitable<void>
    {
        $object = new BlockLayout();
        $assert->container($object->get('left'))->isEmpty();
        $object->add('left', 2, 'bar');
        $object->add('left', 1, 'foo');
        $object->add('right', 3, 'ghi');
        $object->add('right', 3, 'jkl');
        $object->add('right', 2, 'def');
        $object->add('right', 0, 'abc');
        $assert->mixed($object->get('left'))->looselyEquals(ImmVector{'foo', 'bar'});
        $assert->mixed($object->get('right'))->looselyEquals(ImmVector{'abc', 'def', 'ghi', 'jkl'});
    }

    <<Test>>
    public async function testMerge(Assert $assert): Awaitable<void>
    {
        $object = new BlockLayout();
        $object->add('left', 2, 'bar')
            ->add('left', 1, 'foo');

        $other = new BlockLayout();
        $other->add('right', 3, 'ghi')
            ->add('right', 3, 'jkl')
            ->add('right', 2, 'def')
            ->add('right', 0, 'abc');

        $object->merge($other);
        $assert->mixed($object->get('left'))->looselyEquals(ImmVector{'foo', 'bar'});
        $assert->mixed($object->get('right'))->looselyEquals(ImmVector{'abc', 'def', 'ghi', 'jkl'});
    }

    <<Test>>
    public async function testGetAll(Assert $assert): Awaitable<void>
    {
        $object = new BlockLayout();
        $object->add('left', 2, 'bar');
        $object->add('left', 1, 'foo');
        $object->add('right', 3, 'ghi');
        $object->add('right', 3, 'jkl');
        $object->add('right', 2, 'def');
        $object->add('right', 0, 'abc');
        $assert->mixed($object->getAll())->looselyEquals(ImmMap{
            'left' => ImmVector{'foo', 'bar'},
            'right' => ImmVector{'abc', 'def', 'ghi', 'jkl'},
        });
    }
}
