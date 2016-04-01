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
namespace Labrys\Acl;

/**
 * Generated by PHPUnit_SkeletonGenerator on 2015-08-04 at 18:14:26.
 */
class GatekeeperTest extends \PHPUnit_Framework_TestCase
{
    /**
     * @covers Labrys\Acl\Gatekeeper::assert
     */
    public function testAssert()
    {
        $aclService = $this->getMock(\Caridea\Acl\Service::class, ['assert'], [], '', false);
        $aclService->expects(self::once())
            ->method('assert');
        $role = \Caridea\Acl\Subject::role('user');
        $resolver = $this->getMock(SubjectResolver::class, ['getSubjects']);
        $resolver->expects(self::once())
            ->method('getSubjects')
            ->willReturn(Vector{$role});
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);

        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $object->assert('write', 'foo', 'bar');

        $this->verifyMockObjects();
    }

    /**
     * @covers Labrys\Acl\Gatekeeper::can
     */
    public function testCan()
    {
        $aclService = $this->getMock(\Caridea\Acl\Service::class, ['can'], [], '', false);
        $aclService->expects(self::once())
            ->method('can')
            ->willReturn(true);
        $role = \Caridea\Acl\Subject::role('user');
        $resolver = $this->getMock(SubjectResolver::class, ['getSubjects']);
        $resolver->expects(self::once())
            ->method('getSubjects')
            ->willReturn(Vector{$role});
        $principal = \Caridea\Auth\Principal::get('foobar@example.com', []);

        $object = new Gatekeeper($aclService, $principal, [$resolver]);
        $this->assertTrue($object->can('write', 'foo', 'bar'));

        $this->verifyMockObjects();
    }
}
