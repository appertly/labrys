<?hh // decl
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
namespace Labrys\Acl;

use HackPack\HackUnit\Contract\Assert;
use Mockery as M;

class JobTest
{
    <<Test>>
    public async function testAssert(Assert $assert): Awaitable<void>
    {
        $subject = \Caridea\Acl\Subject::role('user');
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);
        $psubject = \Caridea\Acl\Subject::principal('foobar@example.com');
        $resolver = M::mock(SubjectResolver::class);
        $resolver->shouldReceive('getSubjects')->withArgs([$principal])->andReturn(Vector{$subject});
        $target = new \Caridea\Acl\Target('foo', 'bar');
        $aclService = M::mock(\Caridea\Acl\Service::class);
        $aclService->shouldReceive('assert')->andReturnUsing(function ($a, $b, $c) use ($assert, $subject, $psubject, $target) {
            $assert->mixed($c)->looselyEquals($target);
            $assert->string($b)->is('write');
            $assert->container($a)->contains($subject);
            return true;
        })->once();
        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $object->assert('write', 'foo', 'bar');
        M::close();
    }

    <<Test>>
    public async function testCan(Assert $assert): Awaitable<void>
    {
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);
        $subject = \Caridea\Acl\Subject::role('user');
        $psubject = \Caridea\Acl\Subject::principal($principal->getUsername());
        $aclService = M::mock(\Caridea\Acl\Service::class);
        $target = new \Caridea\Acl\Target('foo', 'bar');
        $aclService->shouldReceive('can')->andReturnUsing(function ($a, $b, $c) use ($assert, $subject, $psubject, $target) {
            $assert->mixed($c)->looselyEquals($target);
            $assert->string($b)->is('write');
            $assert->container($a)->contains($subject);
            return true;
        });
        $resolver = M::mock(SubjectResolver::class);
        $resolver->shouldReceive('getSubjects')->withArgs([$principal])->andReturn(Vector{$subject});
        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $assert->bool($object->can('write', 'foo', 'bar'))->is(true);
        M::close();
    }
}
