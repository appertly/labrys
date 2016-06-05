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

class JobTest
{
    <<Test>>
    public async function testAssert(Assert $assert): Awaitable<void>
    {
        $subject = \Caridea\Acl\Subject::role('user');
        $aclService = new class($assert, $subject) extends \Caridea\Acl\Service {
            public $called = false;
            public function __construct(private Assert $assert, private \Caridea\Acl\Subject $subject)
            {
            }
            public function assert(array $subjects, string $verb, \Caridea\Acl\Target $target): void
            {
                $this->assert->string($verb)->is('write');
                $this->assert->container($subjects)->contains($this->subject);
                $this->assert->bool(in_array(\Caridea\Acl\Subject::principal('foobar@example.com'), $subjects))->is(true);
                $this->assert->string($target->getType())->is('foo');
                $this->assert->string($target->getId())->is('bar');
                $this->called = true;
            }
        };
        $resolver = new class($subject) implements SubjectResolver {
            public function __construct(private \Caridea\Acl\Subject $subject) {}
            public function getSubjects(\Caridea\Auth\Principal $principal): Traversable
            {
                return Vector{$this->subject};
            }
        };
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);
        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $object->assert('write', 'foo', 'bar');
        $assert->bool($aclService->called)->is(true);
    }

    <<Test>>
    public async function testCan(Assert $assert): Awaitable<void>
    {
        $subject = \Caridea\Acl\Subject::role('user');
        $aclService = new class($assert, $subject) extends \Caridea\Acl\Service {
            public $called = false;
            public function __construct(private Assert $assert, private \Caridea\Acl\Subject $subject)
            {
            }
            public function can(array $subjects, string $verb, \Caridea\Acl\Target $target): bool
            {
                return true;
            }
        };
        $resolver = new class($subject) implements SubjectResolver {
            public function __construct(private \Caridea\Acl\Subject $subject) {}
            public function getSubjects(\Caridea\Auth\Principal $principal): Traversable
            {
                return Vector{$this->subject};
            }
        };
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);
        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $assert->bool($object->can('write', 'foo', 'bar'))->is(true);
    }
}
